#!/usr/bin/env perl
# C! shared rule engine for etc/format-check.sh and etc/format-fix.sh, following .claude/CLAUDE.md
# C! usage: perl etc/format-rules.pl check|fix [--root DIR] [src files...]
# C! with no files it works on every src file: the .cpp / .hpp / .h files under src/, except the third-party Catch.hpp
# C! it only ever reads and writes src files, anything else passed in is refused
use strict;
use warnings;
use File::Find;
use File::Basename qw( dirname basename );
use Cwd qw( abs_path );

my $mode = shift @ARGV // '';
die "usage: format-rules.pl check|fix [--root DIR] [src files...]\n" unless $mode eq 'check' || $mode eq 'fix';
my $fix = ( $mode eq 'fix' );

# C! the repo root is the nearest folder above this script that has a src/ folder
my $root = dirname( abs_path( $0 ) );
$root = dirname( $root ) while !-d "$root/src" && dirname( $root ) ne $root;
if ( @ARGV && $ARGV[0] eq '--root' ) { shift @ARGV; $root = abs_path( shift @ARGV ); }
chdir $root or die "can't enter $root\n";

# C! -- which files -- #

sub is_src_file
{
    my ( $path ) = @_;
    $path =~ s{\\}{/}g;
    $path =~ s{^\./}{};
    return 0 unless $path =~ m{^src/.+\.(cpp|hpp|h)$};
    return 0 if $path =~ m{(^|/)Catch\.hpp$};
    return 0 if $path =~ m{(^|/)\.\.(/|$)};
    return 1;
}

my @files;
if ( @ARGV )
{
    for my $f ( @ARGV )
    {
        ( my $p = $f ) =~ s{\\}{/}g;
        if ( !is_src_file( $p ) ) { warn "skipping $f: not a src file (only .cpp / .hpp / .h under src/ are allowed)\n"; next; }
        push @files, $p if -f $p;
    }
}
else
{
    find( { no_chdir => 1, wanted => sub { my $p = $File::Find::name; $p =~ s{^\./}{}; push @files, $p if -f $p && is_src_file( $p ); } }, 'src' );
}
@files = sort @files;

# C! -- reading and writing, keeping each file's line endings and final newline -- #

my %file;
for my $f ( @files )
{
    open my $h, '<:raw', $f or die "can't read $f\n";
    local $/;
    my $text = <$h> // '';
    close $h;
    my $eol = ( $text =~ /\r\n/ ) ? "\r\n" : "\n";
    my $final_nl = ( $text eq '' || $text =~ /\n\z/ ) ? 1 : 0;
    my @lines = split /\r?\n/, $text, -1;
    pop @lines if $final_nl && @lines && $lines[-1] eq '';
    $file{$f} = { lines => \@lines, eol => $eol, final_nl => $final_nl, changed => 0 };
}

sub save_all
{
    my $n = 0;
    for my $f ( @files )
    {
        my $d = $file{$f};
        next unless $d->{changed};
        die "refusing to write $f: not a src file\n" unless is_src_file( $f );
        my $text = join( $d->{eol}, @{ $d->{lines} } );
        $text .= $d->{eol} if $d->{final_nl} && @{ $d->{lines} };
        open my $h, '>:raw', $f or die "can't write $f\n";
        print $h $text;
        close $h;
        $n++;
    }
    return $n;
}

# C! -- reporting -- #

my @issues;   # C! [ file, line, rule, message ] still to fix
my %fixed;    # C! rule => count fixed

sub issue { my ( $f, $n, $rule, $msg ) = @_; push @issues, [ $f, $n, $rule, $msg ]; }
sub fixed { my ( $rule, $count ) = @_; $fixed{$rule} += ( $count // 1 ); }

# C! -- masking strings and comments so rules only see code -- #

# C! returns ( code with strings / comments replaced by placeholders, saved pieces, comment text )
# C! placeholders are \x01N\x02 for strings / chars / inline block comments and \x03N\x04 for a trailing // comment
sub mask
{
    my ( $line, $in_block ) = @_;
    my @saved;
    my $out = '';
    my $i = 0;
    my $n = length $line;
    while ( $i < $n )
    {
        if ( $$in_block )
        {
            my $end = index( $line, '*/', $i );
            my $seg = ( $end < 0 ) ? substr( $line, $i ) : substr( $line, $i, $end + 2 - $i );
            push @saved, $seg;
            $out .= "\x01$#saved\x02";
            $i += length $seg;
            $$in_block = 0 if $end >= 0;
            next;
        }
        my $c = substr( $line, $i, 1 );
        my $c2 = substr( $line, $i, 2 );
        if ( $c2 eq '//' ) { push @saved, substr( $line, $i ); $out .= "\x03$#saved\x04"; last; }
        if ( $c2 eq '/*' ) { $$in_block = 1; next; }
        if ( $c eq '"' || ( $c eq "'" && !( $i > 0 && substr( $line, $i - 1, 1 ) =~ /[0-9A-Fa-f]/ ) ) )
        {
            my $j = $i + 1;
            while ( $j < $n )
            {
                my $d = substr( $line, $j, 1 );
                if ( $d eq '\\' ) { $j += 2; next; }
                last if $d eq $c;
                $j++;
            }
            my $seg = substr( $line, $i, $j - $i + 1 );
            push @saved, $seg;
            $out .= "\x01$#saved\x02";
            $i = $j + 1;
            next;
        }
        $out .= $c;
        $i++;
    }
    return ( $out, \@saved );
}

sub unmask
{
    my ( $s, $saved ) = @_;
    $s =~ s/[\x01\x03](\d+)[\x02\x04]/$saved->[$1]/g;
    return $s;
}

# C! same length as the line, with string and comment contents blanked to 'x', for column based checks
sub lmask
{
    my ( $line, $in_block ) = @_;
    my ( $m, $saved ) = mask( $line, $in_block );
    $m =~ s/[\x01\x03](\d+)[\x02\x04]/'x' x length( $saved->[$1] )/ge;
    return $m;
}

# C! -- line rules -- #

sub line_rules
{
    my ( $f ) = @_;
    my $d = $file{$f};
    my $L = $d->{lines};
    my $in_block = 0;
    my $is_hpp = ( $f =~ /\.(hpp|h)$/ );
    my $in_stdc = ( $f =~ m{system/STDc\.hpp$} );
    my $in_defines = ( $f =~ m{system/Defines\.hpp$} );

    for my $i ( 0 .. $#$L )
    {
        my $raw = $L->[$i];
        my $n = $i + 1;
        my $was_in_block = $in_block;
        my ( $m, $saved ) = mask( $raw, \$in_block );
        my $orig_m = $m;
        my $pp = ( $m =~ /^\s*#/ );
        my $define = ( $m =~ /^\s*#\s*define\b/ );
        my $include = ( $m =~ /^\s*#\s*(include|pragma|error|warning|import)\b/ );
        my $code_line = !( $was_in_block && $m =~ /^\x01\d+\x02$/ );

        # C! trailing whitespace
        if ( $raw =~ /[ \t]+$/ )
        {
            if ( $fix ) { $raw =~ s/[ \t]+$//; ( $m, $saved ) = mask( $raw, \( my $tmp = $was_in_block ) ); fixed( 'trailing-whitespace' ); }
            else { issue( $f, $n, 'trailing-whitespace', 'line ends in spaces or tabs' ); }
        }

        # C! tabs: leading ones become 4 spaces, others are reported
        if ( $m =~ /^\t/ || $m =~ /^ *\t/ )
        {
            if ( $fix ) { my ( $lead ) = $m =~ /^([ \t]*)/; ( my $sp = $lead ) =~ s/\t/    /g; $m =~ s/^[ \t]*/$sp/; fixed( 'tab' ); }
            else { issue( $f, $n, 'tab', 'tab used for indentation' ); }
        }
        issue( $f, $n, 'tab', 'tab inside a line' ) if $m =~ /\S\t/;

        next unless $code_line;

        # C! doc comments
        issue( $f, $n, 'doc-comment', 'doc comment (/** */ or ///), use a plain // comment' ) if $raw =~ m{^\s*(///|/\*\*)};

        # C! #ifdef / #ifndef ( header guards are allowed their #ifndef )
        if ( $m =~ /^(\s*)#\s*ifdef\s+(\w+)\s*$/ )
        {
            if ( $fix ) { $m = "$1#if defined( $2 )"; fixed( 'ifdef' ); }
            else { issue( $f, $n, 'ifdef', "use #if defined( $2 ) instead of #ifdef" ); }
        }
        elsif ( $m =~ /^(\s*)#\s*ifndef\s+(\w+)\s*$/ && !( $is_hpp && $2 =~ /_hpp$/ ) )
        {
            if ( $fix ) { $m = "$1#if !defined( $2 )"; fixed( 'ifdef' ); }
            else { issue( $f, $n, 'ifdef', "use #if !defined( $2 ) instead of #ifndef" ); }
        }

        unless ( $include )
        {
            # C! empty (), [ ] and { } get one space
            if ( $m =~ /\(\)|\[\](?!\])|\{\}/ )
            {
                if ( $fix ) { $m =~ s/\(\)/( )/g; $m =~ s/\[\](?!\])/[ ]/g; $m =~ s/\{\}/{ }/g; fixed( 'empty-brackets' ); }
                else { issue( $f, $n, 'empty-brackets', 'empty (), [] or {} needs one space inside' ); }
            }

            # C! no space between a keyword and its (
            if ( $m =~ /\b(if|for|while|switch|return|sizeof|catch)\s+\(/ )
            {
                if ( $fix ) { $m =~ s/\b(if|for|while|switch|return|sizeof|catch)\s+\(/$1(/g; fixed( 'keyword-paren' ); }
                else { issue( $f, $n, 'keyword-paren', "no space between '$1' and (" ); }
            }

            # C! spaces inside parentheses
            if ( $m =~ /\((?=[^ )])/ || $m =~ /(?<=[^ (])\)/ )
            {
                if ( $fix ) { $m =~ s/\((?=[^ )])/( /g; $m =~ s/(?<=[^ (])\)/ )/g; fixed( 'paren-spaces' ); }
                else { issue( $f, $n, 'paren-spaces', 'needs a space inside the parentheses' ); }
            }

            # C! spaces inside square brackets ( attributes [[ ]] are left alone )
            if ( $m !~ /\[\[|\]\]/ && ( $m =~ /\[(?=[^ \]])/ || $m =~ /(?<=[^ \[])\]/ ) )
            {
                if ( $fix ) { $m =~ s/\[(?=[^ \]])/[ /g; $m =~ s/(?<=[^ \[])\]/ ]/g; fixed( 'bracket-spaces' ); }
                else { issue( $f, $n, 'bracket-spaces', 'needs a space inside the square brackets' ); }
            }
        }

        unless ( $define )
        {
            # C! return( *this ) is rethis
            if ( $m =~ /\breturn\(\s*\*this\s*\)/ )
            {
                if ( $fix ) { $m =~ s/\breturn\(\s*\*this\s*\)/rethis/g; fixed( 'rethis' ); }
                else { issue( $f, $n, 'rethis', 'use rethis instead of return( *this )' ); }
            }

            # C! *this = x; rethis; is rethis = x;
            if ( $m =~ /\*this\s*=(?!=)\s*([^;]+?)\s*;\s*rethis\s*;/ )
            {
                if ( $fix ) { $m =~ s/\*this\s*=(?!=)\s*([^;]+?)\s*;\s*rethis\s*;/rethis = $1;/g; fixed( 'rethis-assign' ); }
                else { issue( $f, $n, 'rethis-assign', 'use rethis = x; instead of *this = x; rethis;' ); }
            }

            # C! return values are wrapped in parens
            if ( $m =~ /\breturn\s+(?![\s(;{])/ )
            {
                if ( $fix && $m =~ /\breturn\s+([^;{}]+?)\s*;/ ) { $m =~ s/\breturn\s+(?![\s(;{])([^;{}]+?)\s*;/return( $1 );/g; fixed( 'return-parens' ); }
                else { issue( $f, $n, 'return-parens', 'wrap the return value in parens: return( x );' ); }
            }

            # C! "" is nostring
            if ( !$include && grep { $_ eq '""' } map { $saved->[$_] } ( $m =~ /\x01(\d+)\x02/g ) )
            {
                if ( $fix ) { $m =~ s/\x01(\d+)\x02/$saved->[$1] eq '""' ? 'nostring' : "\x01$1\x02"/ge; fixed( 'nostring' ); }
                else { issue( $f, $n, 'nostring', 'use nostring instead of ""' ); }
            }

            # C! overrides are explicit: virtual and override
            if ( $m =~ /\boverride\b/ && $m !~ /\bvirtual\b/ )
            {
                if ( $fix ) { $m =~ s/^(\s*(?:(?:public|protected|private)\s*:\s*)?)/$1virtual /; fixed( 'virtual-override' ); }
                else { issue( $f, $n, 'virtual-override', 'override needs virtual too' ); }
            }

            # C! var_const / var_static values in their own parens
            if ( $m =~ /\bvar_(const|static)\(\s*[^=;]+=\s*+(?![({])/ )
            {
                if ( $fix && $m =~ /\bvar_(?:const|static)\(\s*[^=;]+=\s*+(?![({]).*\s\);\s*(?:\x03\d+\x04)?$/ )
                {
                    $m =~ s/(\bvar_(?:const|static)\(\s*[^=;]+=\s*+)(?![({])(.*?)(\s\);\s*(?:\x03\d+\x04)?)$/$1( $2 )$3/;
                    fixed( 'var-const-parens' );
                }
                else { issue( $f, $n, 'var-const-parens', 'wrap the constant\'s value in its own parens' ); }
            }

            # C! float literals are written with .0
            if ( $m =~ /(?<![\w.])\d+\.(?![\d\w])/ || $m =~ /(?<![\w.\d])\.\d/ )
            {
                if ( $fix ) { $m =~ s/(?<![\w.])(\d+)\.(?![\d\w])/$1.0/g; $m =~ s/(?<![\w.\d])\.(\d)/0.$1/g; fixed( 'float-literal' ); }
                else { issue( $f, $n, 'float-literal', 'write float literals with digits on both sides of the point, like 1.0 or 0.5' ); }
            }
            issue( $f, $n, 'float-literal', 'dec literals have no f suffix' ) if $m =~ /(?<![\w.])\d+(\.\d+)?[fF]\b/;

            # C! always braces, a one statement if / else / loop body gets them too
            if ( $m =~ /^(\s*(?:\}\s*)?(?:else\s+)?(?:if|while|for_range|for_each|for_each_const|do_count|while_max)\s*\()/ )
            {
                my $start = length $1;
                my ( $depth, $end ) = ( 1, -1 );
                for my $k ( $start .. length( $m ) - 1 )
                {
                    my $ch = substr( $m, $k, 1 );
                    $depth++ if $ch eq '(';
                    $depth-- if $ch eq ')';
                    if ( $depth == 0 ) { $end = $k; last; }
                }
                if ( $end > 0 )
                {
                    my $rest = substr( $m, $end + 1 );
                    if ( $rest =~ /^\s*([^\s{\x03].*?;)\s*(\x03\d+\x04)?$/ && $rest !~ /^\s*;/ )
                    {
                        if ( $fix ) { my ( $body, $c ) = ( $1, $2 // '' ); substr( $m, $end + 1 ) = " { $body }" . ( $c ne '' ? " $c" : '' ); fixed( 'braces' ); }
                        else { issue( $f, $n, 'braces', 'one-statement body needs braces: { ... }' ); }
                    }
                }
            }
            if ( $m =~ /^(\s*(?:\}\s*)?else)\s+(?!if\b)([^\s{\x03].*?;)\s*(\x03\d+\x04)?$/ )
            {
                if ( $fix ) { my $c = $3 // ''; $m = "$1 { $2 }" . ( $c ne '' ? " $c" : '' ); fixed( 'braces' ); }
                else { issue( $f, $n, 'braces', 'one-statement else needs braces: { ... }' ); }
            }

            # C! report only: rules that need a person to decide the fix
            issue( $f, $n, 'std-vector', 'use varray instead of std::vector' ) if !$in_stdc && $m =~ /\bstd::vector\b/;
            issue( $f, $n, 'raw-for', 'use for_range / for_each instead of a raw for loop' ) if $m =~ /\bfor\s*\(/;
            issue( $f, $n, 'get-set', "getters and setters share a name, no get_ / set_ prefix ($1)" ) if $m =~ /\b((?:get|set)_\w+)\s*\(/;
            issue( $f, $n, 'c-cast', 'use a function-style cast like dec( x ) instead of a C-style cast' ) if $m =~ /\(\s*(?:const\s+)?(?:int|uint|uint64|dec|double|float|char|bool|long|short|unsigned|size_t)\s*\*?\s*\)\s*[\w(\x01]/;
            issue( $f, $n, 'raw-ref', 'use ref / cref instead of & / const & in signatures' ) if !$in_defines && ( $m =~ /\bconst\s*&\s*\w/ || $m =~ /\w\s*&\s+in_\w/ );
        }

        # C! Allman braces, except one-liners and brace-init lists
        if ( !$pp && $m =~ /\S\s*\{\s*(\x03\d+\x04)?$/ && $m !~ /[=,(]\s*\{\s*(\x03\d+\x04)?$/ && $m !~ /\\\s*$/ && $m !~ /\breturn\s*\{/ )
        {
            issue( $f, $n, 'allman', 'opening brace goes on its own line' );
        }

        # C! stray double spaces ( lining things up across lines is fine )
        my $lm = lmask( unmask( $m, $saved ), \( my $t2 = $was_in_block ) );
        if ( !$define && $lm =~ /\S {2,}\S/ )
        {
            my $prev = $i > 0 ? $L->[ $i - 1 ] : '';
            my $next = $i < $#$L ? $L->[ $i + 1 ] : '';
            my $bad = 0;
            my $new = $lm;
            while ( $lm =~ /(?<=\S)( {2,})(?=\S)/g )
            {
                my $col = pos( $lm );
                my $aligned = 0;
                for my $o ( $prev, $next )
                {
                    next unless length( $o ) > $col;
                    $aligned = 1 if substr( $o, $col, 1 ) =~ /\S/ && substr( $o, $col - 1, 1 ) eq ' ';
                }
                $aligned = 1 if substr( $lm, $col ) =~ /^\\\s*$/;
                $bad = 1 unless $aligned;
            }
            if ( $bad )
            {
                if ( $fix )
                {
                    my $full = unmask( $m, $saved );
                    my $lm2 = $lm;
                    my $out = '';
                    my $pos = 0;
                    while ( $lm2 =~ /(?<=\S)( {2,})(?=\S)/g )
                    {
                        my $gs = $-[1];
                        my $ge = $+[1];
                        my $aligned = 0;
                        for my $o ( $prev, $next ) { next unless length( $o ) > $ge; $aligned = 1 if substr( $o, $ge, 1 ) =~ /\S/ && substr( $o, $ge - 1, 1 ) eq ' '; }
                        next if $aligned;
                        $out .= substr( $full, $pos, $gs - $pos ) . ' ';
                        $pos = $ge;
                    }
                    $out .= substr( $full, $pos );
                    ( $m, $saved ) = mask( $out, \( my $t3 = $was_in_block ) );
                    fixed( 'double-space' );
                }
                else { issue( $f, $n, 'double-space', 'stray double space' ); }
            }
        }

        my $new = unmask( $m, $saved );
        if ( $new ne $L->[$i] ) { $L->[$i] = $new; $d->{changed} = 1 if $fix; }
    }
}

# C! -- structural rules -- #

sub indent_of { my ( $s ) = @_; my ( $w ) = $s =~ /^(\s*)/; return $w; }

# C! brace depth at the start of every line, from masked code
sub depths
{
    my ( $L ) = @_;
    my @depth;
    my $d = 0;
    my $in_block = 0;
    for my $i ( 0 .. $#$L )
    {
        $depth[$i] = $d;
        my ( $m ) = mask( $L->[$i], \$in_block );
        next if $m =~ /^\s*#/;
        for my $ch ( split //, $m ) { $d++ if $ch eq '{'; $d-- if $ch eq '}'; }
        $d = 0 if $d < 0;
    }
    return \@depth;
}

sub structure_rules
{
    my ( $f ) = @_;
    my $d = $file{$f};
    my $L = $d->{lines};
    my $is_hpp = ( $f =~ /\.(hpp|h)$/ );

    # C! macro_end on a multi-line macro lines up with the lines above it
    for my $i ( 1 .. $#$L )
    {
        next unless $L->[$i] =~ /^(\s*)macro_end\s*$/;
        my $have = $1;
        next unless $L->[ $i - 1 ] =~ /\\\s*$/;
        my $want = indent_of( $L->[ $i - 1 ] );
        next if $have eq $want;
        if ( $fix ) { $L->[$i] = "${want}macro_end"; $d->{changed} = 1; fixed( 'macro-end' ); }
        else { issue( $f, $i + 1, 'macro-end', 'macro_end should line up with the macro lines above it' ); }
    }

    # C! header guards: #ifndef Name_hpp / #define Name_hpp ... #endif /* Name_hpp */
    if ( $is_hpp )
    {
        ( my $base = basename( $f ) ) =~ s/\.(hpp|h)$//;
        my $guard = "${base}_hpp";
        my ( $first ) = grep { $L->[$_] =~ /\S/ } 0 .. $#$L;
        my ( $last ) = reverse grep { $L->[$_] =~ /\S/ } 0 .. $#$L;
        if ( defined $first && $L->[$first] =~ /^#\s*ifndef\s+(\w+)/ && $first < $#$L && $L->[ $first + 1 ] =~ /^#\s*define\s+(\w+)/ )
        {
            my $have = $1;
            my $endif_ok = ( $L->[$last] =~ m{^#endif /\* \Q$guard\E \*/\s*$} );
            if ( $have ne $guard || $L->[$first] !~ m{^#ifndef \Q$guard\E\s*(//.*)?$} || !$endif_ok )
            {
                if ( $fix && $L->[$last] =~ /^#\s*endif/ )
                {
                    $L->[$first] = "#ifndef $guard";
                    $L->[ $first + 1 ] = "#define $guard";
                    $L->[$last] = "#endif /* $guard */";
                    $d->{changed} = 1;
                    fixed( 'header-guard' );
                }
                else { issue( $f, $first + 1, 'header-guard', "header guard should be $guard, ending with #endif /* $guard */" ); }
            }
        }
        elsif ( defined $first && grep { $L->[$_] =~ /\S/ && $L->[$_] !~ m{^\s*//} } 0 .. $#$L )
        {
            if ( $fix )
            {
                unshift @$L, "#ifndef $guard", "#define $guard", '';
                pop @$L while @$L && $L->[-1] !~ /\S/;
                push @$L, '', "#endif /* $guard */";
                $d->{final_nl} = 1;
                $d->{changed} = 1;
                fixed( 'header-guard' );
            }
            else { issue( $f, 1, 'header-guard', "missing header guard $guard" ); }
        }
    }

    # C! one blank line before the closing namespace brackets, none between consecutive closers
    for ( my $i = 1; $i <= $#$L; $i++ )
    {
        next unless $L->[$i] =~ m{^\}\s*//\s*namespace};
        my $prev = $L->[ $i - 1 ];
        if ( $prev =~ m{^\}\s*//\s*namespace} ) { next; }
        if ( $prev !~ /\S/ && $i > 1 && $L->[ $i - 2 ] =~ m{^\}\s*//\s*namespace} )
        {
            if ( $fix ) { splice @$L, $i - 1, 1; $d->{changed} = 1; fixed( 'namespace-close' ); $i--; }
            else { issue( $f, $i + 1, 'namespace-close', 'no blank line between consecutive namespace closers' ); }
            next;
        }
        if ( $prev =~ /\S/ )
        {
            if ( $fix ) { splice @$L, $i, 0, ''; $d->{changed} = 1; fixed( 'namespace-close' ); $i++; }
            else { issue( $f, $i + 1, 'namespace-close', 'leave one blank line before the closing namespace bracket' ); }
        }
    }

    # C! classes and structs: blank line after { and before };, access specifiers lined up with the class with a blank line on each side
    my $depth = depths( $L );
    for ( my $i = 0; $i < $#$L; $i++ )
    {
        next unless $L->[$i] =~ /^(\s*)(?:(?:class|struct)|classT\w*|structT\w*)\s+\w+[^;{(]*$/;
        my $cls_indent = $1;
        next unless $L->[$i] !~ /\benum\b/ && $L->[ $i + 1 ] =~ /^\s*\{\s*$/;
        my $open = $i + 1;
        my $close;
        my $dd = 0;
        my $in_block = 0;
        for my $j ( $open .. $#$L )
        {
            my ( $m ) = mask( $L->[$j], \$in_block );
            next if $m =~ /^\s*#/;
            for my $ch ( split //, $m ) { $dd++ if $ch eq '{'; $dd-- if $ch eq '}'; }
            if ( $dd == 0 ) { $close = $j; last; }
        }
        next unless defined $close && $L->[$close] =~ /^\s*\};\s*$/;
        if ( $L->[ $open + 1 ] =~ /\S/ )
        {
            if ( $fix ) { splice @$L, $open + 1, 0, ''; $close++; $d->{changed} = 1; fixed( 'class-blank-lines' ); }
            else { issue( $f, $open + 2, 'class-blank-lines', 'leave a blank line after the class\'s opening {' ); }
        }
        if ( $L->[ $close - 1 ] =~ /\S/ )
        {
            if ( $fix ) { splice @$L, $close, 0, ''; $close++; $d->{changed} = 1; fixed( 'class-blank-lines' ); }
            else { issue( $f, $close + 1, 'class-blank-lines', 'leave a blank line before the class\'s closing };' ); }
        }
        # C! access specifiers directly inside this class
        my $dep = depths( $L );
        my $inner = $dep->[ $open + 1 ];
        for ( my $j = $open + 1; $j < $close; $j++ )
        {
            next unless $dep->[$j] == $inner && $L->[$j] =~ /^(\s*)(public|private|protected)\s*:\s*(\/\/.*)?$/;
            if ( $1 ne $cls_indent )
            {
                if ( $fix ) { $L->[$j] =~ s/^\s*/$cls_indent/; $d->{changed} = 1; fixed( 'access-specifier' ); }
                else { issue( $f, $j + 1, 'access-specifier', 'access specifier lines up with its class (no indent)' ); }
            }
            if ( $L->[ $j - 1 ] =~ /\S/ && $L->[ $j - 1 ] !~ /^\s*\{\s*$/ )
            {
                if ( $fix ) { splice @$L, $j, 0, ''; $j++; $close++; $d->{changed} = 1; fixed( 'access-specifier' ); }
                else { issue( $f, $j + 1, 'access-specifier', 'leave a blank line before the access specifier' ); }
            }
            if ( $j < $#$L && $L->[ $j + 1 ] =~ /\S/ )
            {
                if ( $fix ) { splice @$L, $j + 1, 0, ''; $close++; $d->{changed} = 1; fixed( 'access-specifier' ); }
                else { issue( $f, $j + 1, 'access-specifier', 'leave a blank line after the access specifier' ); }
            }
            $dep = depths( $L ) if $fix;
        }
    }

    # C! no blank line straight after a function's opening {
    for ( my $i = 1; $i < $#$L; $i++ )
    {
        next unless $L->[$i] =~ /^\{\s*$/ && $L->[ $i + 1 ] !~ /\S/;
        my ( $p ) = grep { $L->[$_] =~ /\S/ } reverse 0 .. $i - 1;
        next unless defined $p;
        my $sig = $L->[$p];
        next if $sig =~ /^\s*(namespace|class|struct|enum|union|extern|section|static_once)\b/ || $sig =~ /^\s*(classT|structT)\w*\b/ || $sig =~ /=\s*$/;
        next unless $sig =~ /\)\s*(const)?\s*(override)?\s*$/ || $sig =~ /^\S.*::.*\(/;
        if ( $fix ) { splice @$L, $i + 1, 1; $d->{changed} = 1; fixed( 'function-blank-line' ); }
        else { issue( $f, $i + 2, 'function-blank-line', 'no blank line after a function\'s opening {' ); }
    }

    # C! preprocessor lines inside a function or class are indented to the code level
    $depth = depths( $L );
    for my $i ( 0 .. $#$L )
    {
        next unless $L->[$i] =~ /^(\s*)#\s*(if|ifdef|ifndef|else|elif|endif)\b/ && $depth->[$i] > 0;
        my $have = $1;
        my $dir = $2;
        my $ref;
        if ( $dir eq 'endif' ) { ( $ref ) = grep { $L->[$_] =~ /\S/ && $L->[$_] !~ /^\s*#/ } reverse 0 .. $i - 1; }
        else { ( $ref ) = grep { $L->[$_] =~ /\S/ && $L->[$_] !~ /^\s*#/ } $i + 1 .. $#$L; }
        next unless defined $ref;
        my $want = indent_of( $L->[$ref] );
        $want = indent_of( $L->[$ref] ) if $L->[$ref] =~ /^\s*\}/ && $dir ne 'endif';
        next if $have eq $want || $have ne '';
        if ( $fix ) { $L->[$i] =~ s/^\s*/$want/; $d->{changed} = 1; fixed( 'preprocessor-indent' ); }
        else { issue( $f, $i + 1, 'preprocessor-indent', 'preprocessor line inside a function or class is indented to the code level' ); }
    }
}

# C! -- declaration order -- #

my %skipkw = map { $_ => 1 } qw( if for while switch return sizeof catch equal_default equal_never equal_always strinfo STATE var_const var_static static_assert decltype Assert Log for_range for_each for_each_const do_count while_max return_if rethis_if break_if continue_if return_true_if return_false_if );

# C! function declarations in a header, in order, as "Class::Nested::name" keys, plus each class's own list
sub header_decls
{
    my ( $L ) = @_;
    my @stack;
    my $pending;
    my %idx;
    my $n = 0;
    my %by_class;
    my $in_block = 0;
    for my $i ( 0 .. $#$L )
    {
        my ( $s ) = mask( $L->[$i], \$in_block );
        $s =~ s/[\x01\x03]\d+[\x02\x04]/""/g;
        next if $s =~ /^\s*#/;
        my $in_fn = grep { $_->[0] eq 'fn' } @stack;
        if ( !$in_fn )
        {
            if ( $s =~ /^\s*(?:template\s*<[^>]*>\s*)?(?:class|struct)\s+(\w+)\b[^;]*$/ || $s =~ /^\s*(?:classT\w*|structT\w*)\s+(\w+)\b[^;]*$/ ) { $pending = [ 'class', $1 ]; }
            elsif ( $s =~ /^\s*namespace\b/ ) { $pending = [ 'ns', '' ]; }
            elsif ( $s =~ /^\s*(?:(?:public|private|protected)\s*:\s*)?(?:[\w:<>,*&\s]+?\s+)?(~?\w+|operator\s*[^\s(]+)\s*\(/ && !$skipkw{$1} && $s !~ /^\s*(?:public|private|protected)\s*:\s*$/ )
            {
                my $name = $1;
                $name =~ s/\s+//g;
                my @cls = map { $_->[1] } grep { $_->[0] eq 'class' } @stack;
                my $q = join( '::', @cls );
                my $key = ( $q ne '' ? "$q\::" : '' ) . $name;
                $idx{$key} //= $n++;
                push @{ $by_class{$q} }, [ $name, $i + 1, ( $s =~ /\boverride\b/ ? 1 : 0 ), ( $s =~ /\bvirtual\b/ ? 1 : 0 ) ] if $q ne '' && @stack && $stack[-1][0] eq 'class';
                $pending = [ 'fn', '' ] if $s =~ /\{/ || $s !~ /;\s*$/;
            }
        }
        for my $ch ( split //, $s )
        {
            if ( $ch eq '{' ) { push @stack, ( $pending // [ 'blk', '' ] ); $pending = undef; }
            elsif ( $ch eq '}' ) { pop @stack; }
        }
        $pending = undef if $s =~ /;\s*$/;
    }
    return ( \%idx, \%by_class );
}

# C! Object.hpp's virtual functions, in order
sub object_order
{
    my $o = 'src/axn/world/Object.hpp';
    return [] unless -f $o;
    my $L = $file{$o} ? $file{$o}{lines} : do { open my $h, '<:raw', $o or return []; my @l = map { s/\r?\n$//r } <$h>; \@l };
    my @order;
    my %seen;
    for my $line ( @$L )
    {
        next unless $line =~ /\bvirtual\b[^(]*?\b(~?\w+)\s*\(/;
        my $name = $1;
        next if $name =~ /^~/ || $name eq 'operator' || $seen{$name}++;
        push @order, $name;
    }
    return \@order;
}

# C! every class derived from Object, from all the headers
sub object_classes
{
    my %parents;
    my @hdrs;
    find( { no_chdir => 1, wanted => sub { push @hdrs, $File::Find::name if /\.(hpp|h)$/ && !/Catch\.hpp$/; } }, 'src' );
    for my $h ( @hdrs )
    {
        open my $fh, '<:raw', $h or next;
        while ( my $line = <$fh> )
        {
            next unless $line =~ /^\s*(?:class|struct)\s+(\w+)\s*:\s*(.+?)\s*$/;
            my $cls = $1;
            for my $base ( split /,/, $2 ) { $base =~ s/^\s*(public|protected|private)\s+//; $base =~ s/.*:://; $base =~ s/\W.*//; push @{ $parents{$cls} }, $base if $base ne ''; }
        }
        close $fh;
    }
    my %is = ( Object => 1 );
    my $grew = 1;
    while ( $grew ) { $grew = 0; for my $c ( keys %parents ) { next if $is{$c}; if ( grep { $is{$_} } @{ $parents{$c} } ) { $is{$c} = 1; $grew = 1; } } }
    return \%is;
}

sub order_rules
{
    my ( $order, $derived ) = @_;
    my %rank;
    @rank{@$order} = 0 .. $#$order;

    # C! headers: Object's functions in Object.hpp's order, render before update, debug_overlay after render
    for my $f ( grep { /\.(hpp|h)$/ } @files )
    {
        my ( undef, $by_class ) = header_decls( $file{$f}{lines} );
        for my $q ( sort keys %$by_class )
        {
            my $cls = ( split /::/, $q )[-1];
            my @decls = @{ $by_class->{$q} };
            my %first;
            for my $dcl ( @decls ) { $first{ $dcl->[0] } //= $dcl->[1]; }
            if ( defined $first{debug_overlay} && defined $first{render} && $first{debug_overlay} < $first{render} )
            {
                issue( $f, $first{debug_overlay}, 'member-order', "$q: debug_overlay( ) should come after render( )" );
            }
            next unless $derived->{$cls};
            if ( defined $first{render} && defined $first{update} && $first{update} < $first{render} )
            {
                issue( $f, $first{update}, 'member-order', "$q: render( ) should come before update( )" );
            }
            my @ov = grep { exists $rank{ $_->[0] } && ( $_->[2] || ( $cls eq 'Object' && $_->[3] ) ) } @decls;
            my %seen;
            @ov = grep { !$seen{ $_->[0] }++ } @ov;
            for my $k ( 1 .. $#ov )
            {
                next unless $rank{ $ov[$k][0] } < $rank{ $ov[ $k - 1 ][0] };
                issue( $f, $ov[$k][1], 'member-order', "$q: $ov[$k][0]( ) should come before $ov[$k-1][0]( ), following Object.hpp's order" );
            }
        }
    }

    # C! .cpp definitions follow their header's order
    for my $cpp ( grep { /\.cpp$/ } @files )
    {
        ( my $hpp = $cpp ) =~ s/\.cpp$/.hpp/;
        next unless -f $hpp;
        my $HL = $file{$hpp} ? $file{$hpp}{lines} : do { open my $h, '<:raw', $hpp or next; my @l = map { s/\r?\n$//r } <$h>; \@l };
        my ( $idx ) = header_decls( $HL );
        reorder_cpp( $cpp, $idx );
    }
}

# C! definitions are gathered, sorted into the header's order, and put where the last one was, so helpers, macros and constants stay above them
# C! only definitions in the same #if region move together, and each keeps its comments, a divider straight above it, and an #if wrapped around only it
sub reorder_cpp
{
    my ( $cpp, $idx ) = @_;
    my $d = $file{$cpp};
    my $L = $d->{lines};
    my @region;
    my @pp;
    for my $i ( 0 .. $#$L )
    {
        if ( $L->[$i] =~ /^\s*#\s*if/ ) { $region[$i] = join( ',', @pp ); push @pp, $i; }
        elsif ( $L->[$i] =~ /^\s*#\s*endif/ ) { pop @pp; $region[$i] = join( ',', @pp ); }
        elsif ( $L->[$i] =~ /^\s*#\s*el/ ) { $pp[-1] = "$pp[-1]e$i" if @pp; $region[$i] = join( ',', @pp ); }
        else { $region[$i] = join( ',', @pp ); }
    }
    my @units;
    my $depth = 0;
    my $in_block = 0;
    my $i = 0;
    my @ms;
    { my $ib = 0; @ms = map { my ( $m ) = mask( $_, \$ib ); $m =~ s/[\x01\x03]\d+[\x02\x04]/""/g; $m } @$L; }
    while ( $i <= $#$L )
    {
        my $s = $ms[$i];
        if ( $depth == 0 && $s !~ /^\s*#/ && $s !~ /;\s*$/ && $s =~ /^\s*(?:[\w:<>,*&\s]+?\s+)?((?:\w+::)*(?:~?\w+|operator\s*[^\s(]+))\s*\(/ )
        {
            my $key = $1;
            $key =~ s/\s+//g;
            if ( exists $idx->{$key} )
            {
                my ( $dd, $seen, $e ) = ( 0, 0, $i );
                for my $j ( $i .. $#$L )
                {
                    for my $ch ( split //, $ms[$j] ) { if ( $ch eq '{' ) { $dd++; $seen = 1; } elsif ( $ch eq '}' ) { $dd--; } }
                    if ( $seen && $dd == 0 ) { $e = $j; last; }
                }
                my $st = $i;
                while ( $st > 0 && $L->[ $st - 1 ] =~ m{^\s*//} && $L->[ $st - 1 ] !~ m{^\s*//\s*(C!\s*)?-+} ) { $st--; }
                my ( $us, $ue ) = ( $st, $e );
                $us -= 2 if $us > 1 && $L->[ $us - 1 ] !~ /\S/ && $L->[ $us - 2 ] =~ m{^\s*//\s*(C!\s*)?-+.*-+\s*//\s*$};
                if ( $us > 0 && $L->[ $us - 1 ] =~ /^\s*#\s*if\b/ && $ue < $#$L && $L->[ $ue + 1 ] =~ /^\s*#\s*endif/ && !grep { $L->[$_] =~ /^\s*#/ } $us .. $ue ) { $us--; $ue++; }
                push @units, { key => $key, rank => $idx->{$key}, s => $us, e => $ue, region => $region[$us], line => $i + 1 };
                $i = $e + 1;
                next;
            }
        }
        for my $ch ( split //, $s ) { $depth++ if $ch eq '{'; $depth-- if $ch eq '}'; }
        $depth = 0 if $depth < 0;
        $i++;
    }
    my %by;
    push @{ $by{ $_->{region} } }, $_ for @units;
    my ( %drop, %replace );
    my $moved = 0;
    for my $r ( sort keys %by )
    {
        my @slots = @{ $by{$r} };
        my @sorted = sort { $a->{rank} <=> $b->{rank} || $a->{s} <=> $b->{s} } @slots;
        next if join( ',', map { $_->{s} } @slots ) eq join( ',', map { $_->{s} } @sorted );
        if ( !$fix )
        {
            my %pos;
            @pos{ map { $_->{s} } @sorted } = 0 .. $#sorted;
            for my $k ( 1 .. $#slots ) { issue( $cpp, $slots[$k]{line}, 'cpp-order', "$slots[$k]{key} is defined out of the header's order" ) if $slots[$k]{rank} < $slots[ $k - 1 ]{rank}; }
            next;
        }
        $moved += grep { $slots[$_] != $sorted[$_] } 0 .. $#slots;
        my $last = $slots[-1];
        for my $u ( @slots[ 0 .. $#slots - 1 ] )
        {
            $drop{$_} = 1 for $u->{s} .. $u->{e};
            if ( $u->{e} + 1 <= $#$L && $L->[ $u->{e} + 1 ] !~ /\S/ && !$drop{ $u->{e} + 1 } ) { $drop{ $u->{e} + 1 } = 1; }
            elsif ( $u->{s} > 0 && $L->[ $u->{s} - 1 ] !~ /\S/ && !$drop{ $u->{s} - 1 } ) { $drop{ $u->{s} - 1 } = 1; }
        }
        my @block;
        for my $k ( 0 .. $#sorted )
        {
            my $u = $sorted[$k];
            if ( $k ) { my $p = $sorted[ $k - 1 ]; push @block, '' unless $u->{s} == $p->{e} + 1; }
            push @block, @{$L}[ $u->{s} .. $u->{e} ];
        }
        $replace{ $last->{s} } = [ $last->{e}, \@block ];
    }
    return unless $fix && $moved;
    my @out;
    my $p = 0;
    while ( $p <= $#$L )
    {
        if ( my $rep = $replace{$p} ) { push @out, @{ $rep->[1] }; $p = $rep->[0] + 1; next; }
        push @out, $L->[$p] unless $drop{$p};
        $p++;
    }
    @$L = @out;
    $d->{changed} = 1;
    fixed( 'cpp-order', $moved );
}

# C! -- run -- #

for my $f ( @files ) { line_rules( $f ); structure_rules( $f ); }
order_rules( object_order( ), object_classes( ) );

if ( $fix )
{
    my $n = save_all( );
    my $total = 0;
    $total += $_ for values %fixed;
    print "format-fix: fixed $total violation(s) in $n file(s)\n";
    printf "  %-22s %d\n", $_, $fixed{$_} for sort keys %fixed;
    # C! check again, so what's left is what needs a person
    @issues = ();
    $fix = 0;
    for my $f ( @files ) { line_rules( $f ); structure_rules( $f ); }
    order_rules( object_order( ), object_classes( ) );
    if ( @issues ) { print "\nleft to fix by hand:\n"; }
}

my %count;
for my $x ( sort { $a->[0] cmp $b->[0] || $a->[1] <=> $b->[1] } @issues ) { print "$x->[0]:$x->[1]: [$x->[2]] $x->[3]\n"; $count{ $x->[2] }++; }
if ( @issues )
{
    print "\n", scalar( @issues ), " style violation(s)\n";
    printf "  %-22s %d\n", $_, $count{$_} for sort { $count{$b} <=> $count{$a} } keys %count;
    exit 1;
}
print "no style violations found\n" unless $mode eq 'fix' && %fixed;
exit 0;
