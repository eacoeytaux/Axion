#!/usr/bin/env perl
# C! shared rule engine for etc/format-check.sh and etc/format-fix.sh, following .claude/CLAUDE.md
# C! usage: perl etc/format-rules.pl check|fix [--root DIR] [src files...]
# C! with no files it works on every src file: the .cpp / .hpp / .h files under src/, except the third-party Catch.hpp
# C! it only ever writes src files, anything else passed in is refused
# C! with no files it also reads the project setting files (never writing them) to report any comments in them
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
if( @ARGV && $ARGV[0] eq '--root' ) { shift @ARGV; $root = abs_path( shift @ARGV ); }
chdir $root or die "can't enter $root\n";

# C! -- which files -- #

sub is_src_file
{
    my( $path ) = @_;
    $path =~ s{\\}{/}g;
    $path =~ s{^\./}{};
    return 0 unless $path =~ m{^src/.+\.(cpp|hpp|h)$};
    return 0 if $path =~ m{(^|/)Catch\.hpp$};
    return 0 if $path =~ m{(^|/)\.\.(/|$)};
    return 1;
}

my @files;
my $all_files = !@ARGV;
if( @ARGV )
{
    for my $f ( @ARGV )
    {
        ( my $p = $f ) =~ s{\\}{/}g;
        if( !is_src_file( $p ) ) { warn "skipping $f: not a src file (only .cpp / .hpp / .h under src/ are allowed)\n"; next; }
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

sub issue { my( $f, $n, $rule, $msg ) = @_; push @issues, [ $f, $n, $rule, $msg ]; }
sub fixed { my( $rule, $count ) = @_; $fixed{$rule} += ( $count // 1 ); }

# C! -- masking strings and comments so rules only see code -- #

# C! returns ( code with strings / comments replaced by placeholders, saved pieces, comment text )
# C! placeholders are \x01N\x02 for strings / chars / inline block comments and \x03N\x04 for a trailing // comment
sub mask
{
    my( $line, $in_block ) = @_;
    my @saved;
    my $out = '';
    my $i = 0;
    my $n = length $line;
    while( $i < $n )
    {
        if( $$in_block )
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
        if( $c2 eq '//' ) { push @saved, substr( $line, $i ); $out .= "\x03$#saved\x04"; last; }
        if( $c2 eq '/*' ) { $$in_block = 1; next; }
        if( $c eq '"' || ( $c eq "'" && !( $i > 0 && substr( $line, $i - 1, 1 ) =~ /[0-9A-Fa-f]/ ) ) )
        {
            my $j = $i + 1;
            while( $j < $n )
            {
                my $d = substr( $line, $j, 1 );
                if( $d eq '\\' ) { $j += 2; next; }
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
    return( $out, \@saved );
}

sub unmask
{
    my( $s, $saved ) = @_;
    $s =~ s/[\x01\x03](\d+)[\x02\x04]/$saved->[$1]/g;
    return $s;
}

# C! same length as the line, with string and comment contents blanked to 'x', for column based checks
sub lmask
{
    my( $line, $in_block ) = @_;
    my( $m, $saved ) = mask( $line, $in_block );
    $m =~ s/[\x01\x03](\d+)[\x02\x04]/'x' x length( $saved->[$1] )/ge;
    return $m;
}

# C! drops the inner pair wherever a pair of parentheses holds nothing but another pair: ( ( x ) ) is ( x ), whatever x holds
# C! the inner pair stays around an assignment ( if( ( a = b ) ) says the = is meant ), around a comma at x's top level (a macro argument
# C! needs the pair to stay one argument), and around && or || inside a Catch check( ) (Catch can't take those apart)
# C! returns ( the masked line, how many pairs were dropped )
sub drop_double_parens
{
    my( $m ) = @_;
    my $dropped = 0;

    while( 1 )
    {
        my %close;
        my @open;
        for my $i ( 0 .. length( $m ) - 1 )
        {
            my $c = substr( $m, $i, 1 );
            if( $c eq '(' ) { push @open, $i; }
            elsif( $c eq ')' && @open ) { $close{ pop @open } = $i; }
        }

        my $done = 1;
        for my $outer ( sort { $a <=> $b } keys %close )
        {
            my $inside = substr( $m, $outer + 1, $close{$outer} - $outer - 1 );
            next unless $inside =~ /^(\s*)\(.*\)(\s*)$/s;
            my $inner = $outer + 1 + length( $1 );
            next unless defined $close{$inner} && $close{$inner} == ( $close{$outer} - 1 - length( $2 ) );

            my $x = substr( $m, $inner + 1, $close{$inner} - $inner - 1 );
            next unless $x =~ /\S/;

            ( my $top = $x ) =~ s/\x01\d+\x02/s/g;
            1 while $top =~ s/\([^()]*\)/ /;
            next if $top =~ /(?<![=!<>])=(?!=)/;
            next if $top =~ /,/;
            next if $top =~ /&&|\|\|/ && substr( $m, 0, $outer ) =~ /\b(?:check|check_not|CHECK|CHECK_FALSE|REQUIRE|REQUIRE_FALSE)\s*$/;

            # C! the inner pair's own spaces go with it: ( ( x ) ) becomes ( x )
            my $end = $close{$inner};
            my $start = $inner;
            $end-- if substr( $m, $end - 1, 1 ) eq ' ';
            my $after_open = ( substr( $m, $start + 1, 1 ) eq ' ' ) ? 2 : 1;
            substr( $m, $end, $close{$inner} - $end + 1, '' );
            substr( $m, $start, $after_open, '' );

            $dropped++;
            $done = 0;
            last;
        }

        last if $done;
    }

    return( $m, $dropped );
}

# C! whether the whole of in_s is one pair of parentheses: ( ... )
sub wrapped_in_parens
{
    my( $s ) = @_;
    return 0 unless $s =~ /^\(/;
    my $depth = 0;
    for my $j ( 0 .. length( $s ) - 1 )
    {
        my $c = substr( $s, $j, 1 );
        if( $c eq '(' ) { $depth++; }
        elsif( $c eq ')' ) { $depth--; return( $j == length( $s ) - 1 ) if $depth == 0; }
    }
    return 0;
}

# C! an Assert's condition (and initAssert's) sits in its own parens when a message follows it: Assert( ( x ), "..." ), not Assert( x, "..." ); alone it's just Assert( x )
# C! a string first isn't a condition (Assert( "message" ) always fails), and #define lines aren't calls
# C! returns ( the masked line with conditions wrapped, how many were wrapped, how many need wrapping by hand )
sub assert_parens
{
    my( $m ) = @_;
    my $out = '';
    my $pos = 0;
    my $wrapped = 0;
    my $by_hand = 0;
    while( $m =~ /\b(?:init)?Assert\( /g )
    {
        my $start = pos( $m );
        next if substr( $m, 0, $-[0] ) =~ /#\s*define\s+$/;
        my $depth = 0;
        my $end = -1;
        for my $j ( $start .. length( $m ) - 1 )
        {
            my $c = substr( $m, $j, 1 );
            if( $c =~ /[(\[{]/ ) { $depth++; }
            elsif( $c =~ /[)\]}]/ ) { if( $depth == 0 ) { $end = $j; last; } $depth--; }
            elsif( $c eq ',' && $depth == 0 ) { $end = $j; last; }
        }
        my $arg = ( $end < 0 ) ? substr( $m, $start ) : substr( $m, $start, $end - $start );
        $arg =~ s/\s+$//;
        next if $arg eq '' || $arg =~ /^\x01\d+\x02$/;
        if( $end < 0 ) { $by_hand++ unless $arg =~ /^\(/; next; } # C! carries on to the next line, so only checked when it doesn't start with a paren
        next if substr( $m, $end, 1 ) eq ')'; # C! the condition is all there is: Assert( x ), its own pair would be redundant
        next if wrapped_in_parens( $arg );
        $out .= substr( $m, $pos, $start - $pos ) . "( $arg )";
        $pos = $start + length( $arg );
        $wrapped++;
    }
    $out .= substr( $m, $pos );
    return( $out, $wrapped, $by_hand );
}

# C! -- line rules -- #

sub line_rules
{
    my( $f ) = @_;
    my $d = $file{$f};
    my $L = $d->{lines};
    my $in_block = 0;
    my $is_hpp = ( $f =~ /\.(hpp|h)$/ );

    # C! var_consts come before var_statics in the same scope (file level, a section, a class). A var_const in an #if block opened after
    # C! the var_statics is left where it is: it belongs with the rest of that block (like the debug-only REPLAY_INPUTS in Engine.cpp)
    {
        my @scope = ( [ 0, 0 ] ); # C! for each open {, the line of its first var_static and how many #ifs were open there
        my $ifs = 0;
        for my $i ( 0 .. $#$L )
        {
            my $s = $L->[$i];
            $s =~ s{//.*}{};
            if( $s =~ /^\s*#\s*if/ ) { $ifs++; next; }
            if( $s =~ /^\s*#\s*endif/ ) { $ifs-- if $ifs; next; }
            if( $s =~ /^\s*var_static\(/ ) { @{ $scope[-1] } = ( $i + 1, $ifs ) unless $scope[-1][0]; }
            elsif( $s =~ /^\s*var_const\(/ && $scope[-1][0] && $ifs <= $scope[-1][1] )
            {
                issue( $f, $i + 1, 'const-order', "var_consts come before var_statics ( a var_static is at line $scope[-1][0] )" );
            }
            my $o = () = $s =~ /\{/g;
            my $c = () = $s =~ /\}/g;
            push @scope, [ 0, 0 ] for 1 .. $o;
            for( 1 .. $c ) { pop @scope if @scope > 1; }
        }
    }

    # C! never two blank lines in a row
    for( my $i = $#$L; $i > 0; $i-- )
    {
        next unless $L->[$i] !~ /\S/ && $L->[ $i - 1 ] !~ /\S/;
        if( $fix ) { splice @$L, $i, 1; $d->{changed} = 1; fixed( 'double-blank' ); }
        else { issue( $f, $i + 1, 'double-blank', 'only one blank line in a row' ); }
    }

    # C! the file ends in exactly one newline: none missing, and no blank lines after the last one
    if( @$L && ( !$d->{final_nl} || $L->[-1] !~ /\S/ ) )
    {
        if( $fix )
        {
            pop @$L while @$L && $L->[-1] !~ /\S/;
            $d->{final_nl} = 1;
            $d->{changed} = 1;
            fixed( 'final-newline' );
        }
        else { issue( $f, scalar( @$L ), 'final-newline', 'end the file in exactly one newline' ); }
    }
    my $in_stdc = ( $f =~ m{system/STDc\.hpp$} );
    my $in_defines = ( $f =~ m{system/Defines\.hpp$} );
    my $std_allowed = ( $f =~ m{(system/(Defines|STDc|STDptr)\.hpp|system/Clock\.cpp)$} ); # C! the only files that already used std::
    my $in_assert_hpp = ( $f =~ m{system/Assert\.hpp$} ); # C! Assert's own constructors take a bool first, they aren't calls

    for my $i ( 0 .. $#$L )
    {
        my $raw = $L->[$i];
        my $n = $i + 1;
        my $was_in_block = $in_block;
        my( $m, $saved ) = mask( $raw, \$in_block );
        my $orig_m = $m;
        my $pp = ( $m =~ /^\s*#/ );
        my $define = ( $m =~ /^\s*#\s*define\b/ );
        my $include = ( $m =~ /^\s*#\s*(include|pragma|error|warning|import)\b/ );
        my $code_line = !( $was_in_block && $m =~ /^\x01\d+\x02$/ );

        # C! trailing whitespace
        if( $raw =~ /[ \t]+$/ )
        {
            if( $fix ) { $raw =~ s/[ \t]+$//; ( $m, $saved ) = mask( $raw, \( my $tmp = $was_in_block ) ); fixed( 'trailing-whitespace' ); }
            else { issue( $f, $n, 'trailing-whitespace', 'line ends in spaces or tabs' ); }
        }

        # C! tabs: leading ones become 4 spaces, others are reported
        if( $m =~ /^\t/ || $m =~ /^ *\t/ )
        {
            if( $fix ) { my( $lead ) = $m =~ /^([ \t]*)/; ( my $sp = $lead ) =~ s/\t/    /g; $m =~ s/^[ \t]*/$sp/; fixed( 'tab' ); }
            else { issue( $f, $n, 'tab', 'tab used for indentation' ); }
        }
        issue( $f, $n, 'tab', 'tab inside a line' ) if $m =~ /\S\t/;

        next unless $code_line;

        # C! doc comments
        issue( $f, $n, 'doc-comment', 'doc comment (/** */ or ///), use a plain // comment' ) if $raw =~ m{^\s*(///|/\*\*)};

        # C! #ifdef / #ifndef ( header guards are allowed their #ifndef )
        if( $m =~ /^(\s*)#\s*ifdef\s+(\w+)\s*$/ )
        {
            if( $fix ) { $m = "$1#if defined( $2 )"; fixed( 'ifdef' ); }
            else { issue( $f, $n, 'ifdef', "use #if defined( $2 ) instead of #ifdef" ); }
        }
        elsif( $m =~ /^(\s*)#\s*ifndef\s+(\w+)\s*$/ && !( $is_hpp && $2 =~ /_hpp$/ ) )
        {
            if( $fix ) { $m = "$1#if !defined( $2 )"; fixed( 'ifdef' ); }
            else { issue( $f, $n, 'ifdef', "use #if !defined( $2 ) instead of #ifndef" ); }
        }

        unless( $include )
        {
            # C! empty (), [ ] and { } get one space
            if( $m =~ /\(\)|\[\](?!\])|\{\}/ )
            {
                if( $fix ) { $m =~ s/\(\)/( )/g; $m =~ s/\[\](?!\])/[ ]/g; $m =~ s/\{\}/{ }/g; fixed( 'empty-brackets' ); }
                else { issue( $f, $n, 'empty-brackets', 'empty (), [] or {} needs one space inside' ); }
            }

            # C! no space between a keyword and its (
            if( $m =~ /\b(if|for|while|switch|return|sizeof|catch)\s+\(/ )
            {
                if( $fix ) { $m =~ s/\b(if|for|while|switch|return|sizeof|catch)\s+\(/$1(/g; fixed( 'keyword-paren' ); }
                else { issue( $f, $n, 'keyword-paren', "no space between '$1' and (" ); }
            }

            # C! spaces inside parentheses
            if( $m =~ /\((?=[^ )])/ || $m =~ /(?<=[^ (])\)/ )
            {
                if( $fix ) { $m =~ s/\((?=[^ )])/( /g; $m =~ s/(?<=[^ (])\)/ )/g; fixed( 'paren-spaces' ); }
                else { issue( $f, $n, 'paren-spaces', 'needs a space inside the parentheses' ); }
            }

            # C! an attribute gets one space inside its double brackets: [[ maybe_unused ]], not [[maybe_unused]]
            if( $m =~ /\[\[(?! \S)|(?<!\S )\]\]|\[\[ {2,}| {2,}\]\]/ )
            {
                if( $fix ) { $m =~ s/\[\[\s*(.*?)\s*\]\]/[[ $1 ]]/g; fixed( 'attribute-spaces' ); }
                else { issue( $f, $n, 'attribute-spaces', 'an attribute needs one space inside its brackets: [[ maybe_unused ]]' ); }
            }

            # C! spaces inside square brackets ( attributes [[ ]] are dealt with above )
            if( $m !~ /\[\[|\]\]/ && ( $m =~ /\[(?=[^ \]])/ || $m =~ /(?<=[^ \[])\]/ ) )
            {
                if( $fix ) { $m =~ s/\[(?=[^ \]])/[ /g; $m =~ s/(?<=[^ \[])\]/ ]/g; fixed( 'bracket-spaces' ); }
                else { issue( $f, $n, 'bracket-spaces', 'needs a space inside the square brackets' ); }
            }
        }

        # C! an Assert's condition in its own parens ( #define lines are checked too, for the Assert inside initAssert )
        if( !$in_assert_hpp && !$include && $m =~ /\b(?:init)?Assert\( / )
        {
            my( $new, $wrapped, $by_hand ) = assert_parens( $m );
            if( $wrapped )
            {
                if( $fix ) { $m = $new; fixed( 'assert-parens', $wrapped ); }
                else { issue( $f, $n, 'assert-parens', "wrap the Assert's condition in its own parens: Assert( ( x ), ... )" ) for 1 .. $wrapped; }
            }
            issue( $f, $n, 'assert-parens', "the Assert's condition carries on to the next line: wrap it in its own parens by hand" ) for 1 .. $by_hand;
        }

        unless( $define )
        {
            # C! template<...> is written with the template( ... ) macro (Defines.hpp, which defines it, is left alone)
            my $raw_template = qr/\btemplate<((?:[^<>]|<[^<>]*>)*)>/;
            if( !$in_defines && $m =~ $raw_template )
            {
                if( $fix ) { $m =~ s/$raw_template/my $x = $1; $x =~ s{^\s+|\s+$}{}g; ( $x eq '' ) ? 'template( )' : "template( $x )"/ge; fixed( 'template-macro' ); }
                else { issue( $f, $n, 'template-macro', 'use the template( ... ) macro instead of template<...>' ); }
            }

            # C! return( *this ) is rethis
            if( $m =~ /\breturn\(\s*\*this\s*\)/ )
            {
                if( $fix ) { $m =~ s/\breturn\(\s*\*this\s*\)/rethis/g; fixed( 'rethis' ); }
                else { issue( $f, $n, 'rethis', 'use rethis instead of return( *this )' ); }
            }

            # C! *this = x; rethis; is rethis = x;
            if( $m =~ /\*this\s*=(?!=)\s*([^;]+?)\s*;\s*rethis\s*;/ )
            {
                if( $fix ) { $m =~ s/\*this\s*=(?!=)\s*([^;]+?)\s*;\s*rethis\s*;/rethis = $1;/g; fixed( 'rethis-assign' ); }
                else { issue( $f, $n, 'rethis-assign', 'use rethis = x; instead of *this = x; rethis;' ); }
            }

            # C! return values are wrapped in parens
            if( $m =~ /\breturn\s+(?![\s(;{])/ )
            {
                if( $fix && $m =~ /\breturn\s+([^;{}]+?)\s*;/ ) { $m =~ s/\breturn\s+(?![\s(;{])([^;{}]+?)\s*;/return( $1 );/g; fixed( 'return-parens' ); }
                else { issue( $f, $n, 'return-parens', 'wrap the return value in parens: return( x );' ); }
            }

            # C! ( ( x ) ) is ( x ), whatever x holds (see drop_double_parens( ) for the exceptions)
            my( $undoubled, $doubled ) = drop_double_parens( $m );
            if( $doubled )
            {
                if( $fix ) { $m = $undoubled; fixed( 'double-parens', $doubled ); }
                else { issue( $f, $n, 'double-parens', 'redundant parentheses: a pair holding nothing but another pair, ( ( x ) ) is ( x )' ) for 1 .. $doubled; }
            }

            # C! "" is nostring
            if( !$include && grep { $_ eq '""' } map { $saved->[$_] } ( $m =~ /\x01(\d+)\x02/g ) )
            {
                if( $fix ) { $m =~ s/\x01(\d+)\x02/$saved->[$1] eq '""' ? 'nostring' : "\x01$1\x02"/ge; fixed( 'nostring' ); }
                else { issue( $f, $n, 'nostring', 'use nostring instead of ""' ); }
            }

            # C! overrides are explicit: virtual and override
            if( $m =~ /\boverride\b/ && $m !~ /\bvirtual\b/ )
            {
                if( $fix ) { $m =~ s/^(\s*(?:(?:public|protected|private)\s*:\s*)?)/$1virtual /; fixed( 'virtual-override' ); }
                else { issue( $f, $n, 'virtual-override', 'override needs virtual too' ); }
            }

            # C! var_const / var_static values in their own parens
            if( $m =~ /\bvar_(const|static)\(\s*[^=;]+=\s*+(?![({])/ )
            {
                if( $fix && $m =~ /\bvar_(?:const|static)\(\s*[^=;]+=\s*+(?![({]).*\s\);\s*(?:\x03\d+\x04)?$/ )
                {
                    $m =~ s/(\bvar_(?:const|static)\(\s*[^=;]+=\s*+)(?![({])(.*?)(\s\);\s*(?:\x03\d+\x04)?)$/$1( $2 )$3/;
                    fixed( 'var-const-parens' );
                }
                else { issue( $f, $n, 'var-const-parens', 'wrap the constant\'s value in its own parens' ); }
            }

            # C! float literals are written with .0
            if( $m =~ /(?<![\w.])\d+\.(?![\d\w])/ || $m =~ /(?<![\w.\d])\.\d/ )
            {
                if( $fix ) { $m =~ s/(?<![\w.])(\d+)\.(?![\d\w])/$1.0/g; $m =~ s/(?<![\w.\d])\.(\d)/0.$1/g; fixed( 'float-literal' ); }
                else { issue( $f, $n, 'float-literal', 'write float literals with digits on both sides of the point, like 1.0 or 0.5' ); }
            }
            issue( $f, $n, 'float-literal', 'dec literals have no f suffix' ) if $m =~ /(?<![\w.])\d+(\.\d+)?[fF]\b/;

            # C! always braces, a one statement if / else / loop body gets them too
            if( $m =~ /^(\s*(?:\}\s*)?(?:else\s+)?(?:if|while|for_range|for_each|for_each_const|do_count|while_max)\s*\()/ )
            {
                my $start = length $1;
                my( $depth, $end ) = ( 1, -1 );
                for my $k ( $start .. length( $m ) - 1 )
                {
                    my $ch = substr( $m, $k, 1 );
                    $depth++ if $ch eq '(';
                    $depth-- if $ch eq ')';
                    if( $depth == 0 ) { $end = $k; last; }
                }
                if( $end > 0 )
                {
                    my $rest = substr( $m, $end + 1 );
                    if( $rest =~ /^\s*([^\s{\x03].*?;)\s*(\x03\d+\x04)?$/ && $rest !~ /^\s*;/ )
                    {
                        if( $fix ) { my( $body, $c ) = ( $1, $2 // '' ); substr( $m, $end + 1 ) = " { $body }" . ( $c ne '' ? " $c" : '' ); fixed( 'braces' ); }
                        else { issue( $f, $n, 'braces', 'one-statement body needs braces: { ... }' ); }
                    }
                }
            }
            if( $m =~ /^(\s*(?:\}\s*)?else)\s+(?!if\b)([^\s{\x03].*?;)\s*(\x03\d+\x04)?$/ )
            {
                if( $fix ) { my $c = $3 // ''; $m = "$1 { $2 }" . ( $c ne '' ? " $c" : '' ); fixed( 'braces' ); }
                else { issue( $f, $n, 'braces', 'one-statement else needs braces: { ... }' ); }
            }

            # C! a comment after code has a space before it
            if( $m =~ /\S\x03\d+\x04/ )
            {
                if( $fix ) { $m =~ s/(\S)(\x03\d+\x04)/$1 $2/; fixed( 'comment-space' ); }
                else { issue( $f, $n, 'comment-space', 'leave a space between code and a comment after it' ); }
            }

            # C! report only: rules that need a person to decide the fix
            issue( $f, $n, 'std-vector', 'use varray instead of std::vector' ) if !$in_stdc && $m =~ /\bstd::vector\b/;
            issue( $f, $n, 'auto', 'write the type out instead of auto (a lambda goes in lambdaf( ))' ) if !$in_defines && $m =~ /\bauto\b/;
            issue( $f, $n, 'shape', 'call a polygon a polygon, not a shape (in names and comments)' ) if $raw =~ /shape/i;
            issue( $f, $n, 'std-prefix', 'leave out std::, bring the name in with use_std( ) in STD.hpp' ) if !$std_allowed && $m =~ /\bstd::/ && $m !~ /\bstd::vector\b/;
            issue( $f, $n, 'namespace-prefix', "leave out the namespace prefix ( $1:: ), the name is in scope already" ) if $f !~ m{system/(?:Namespaces|STDptr)\.hpp$} && $m !~ /\bfriend\b/ && $m =~ /\b(axn|audio|geometry|graphics|physics|reality|utility|mtmercy)::/; # C! needed only where it's ambiguous or has to be qualified (a friend, a std:: specialization)
            issue( $f, $n, 'friend', 'avoid friend: hand the data over through a function instead' ) if $m =~ /\bfriend\b/;
            issue( $f, $n, 'dec-time', "a time or duration is ticks, not dec ( $1 )" ) if $m =~ /\bdec\s+((?:in_|m_|c_)?(?:time|duration|delay)\w*)\b/;
            issue( $f, $n, 'zero-compare', 'compare with 0 through is_pos( ) / is_neg( ) / is_zero( ) (with ! for the opposites)' ) if $f !~ m{system/Utility.hpp$|/tests/} && ( $m =~ /(?<![<>])(?:[<>]=?|[!=]=)\s*\+?0(?:\.0)?(?![\w.])/ || $m =~ /(?<![\w.])0(?:\.0)?\s*(?:[<>]=?|[!=]=)(?![=<>])/ ); # C! not a shift (>> 0); Utility.hpp defines the helpers, and needs exact comparisons
            issue( $f, $n, 'using-base', 'no using Base::name; write the function out (passing through to the base), or rename the one that hides it' ) if $m =~ /^\s*using\s+\w+(?:<[^>]*>)?::\w+\s*;/;
            issue( $f, $n, 'pair', 'avoid pair<>: a small struct with named members says what each half is' ) if $m =~ /\bpair\s*</;
            issue( $f, $n, 'filename', "a string holding a file's name is named filename, not file ( $1 )" ) if $m =~ /\bstring(?:\s+(?:cref|ref))?\s+((?:in_|m_|c_)?(?:\w+_)?file)\b/;
            {
                my %lambdas = map { $_ => 1 } $m =~ /\blambdaf\(\s*(\w+)\s*\)/g; # C! a lambda's name is a function's
                my @names = $m =~ /\b([A-Za-z_][A-Za-z0-9_]*)\b(?!\s*\()/g;
                for my $name ( @names )
                {
                    next if $name =~ /^GL_/ || $lambdas{$name} || $name !~ /(?:^|_)(?:points?|polygons?|POINTS?|POLYGONS?)(?:_|$)/; # C! function names keep the long words; OpenGL's GL_ constants are its own
                    issue( $f, $n, 'short-name', "in a variable or constant's name, point is pt, points pts, polygon poly, and polygons polys ( $name )" );
                }
            }
            if( $m =~ /\bvar_const\(\s*.*?\b([A-Za-z_]\w*)\s*(?:=|\(|{)/ && $m !~ /\bvar_const\(\s*Error\b/ ) { my $name = $1; issue( $f, $n, 'const-name', "a constant (dev param) is named in UPPER_SNAKE ( $name )" ) if $name =~ /[a-z]/; } # C! Error values (no_error, ...) are exempt
            issue( $f, $n, 'raw-for', 'use for_range / for_each instead of a raw for loop' ) if $m =~ /\bfor\s*\(/;
            issue( $f, $n, 'get-set', "getters and setters share a name, no get_ / set_ prefix ($1)" ) if $m =~ /\b((?:get|set)_\w+)\s*\(/;
            issue( $f, $n, 'c-cast', 'use a function-style cast like dec( x ) instead of a C-style cast' ) if $m =~ /\(\s*(?:const\s+)?(?:int|uint|uint64|dec|double|float|char|bool|long|short|unsigned|size_t)\s*\*?\s*\)\s*[\w(\x01]/;
            issue( $f, $n, 'raw-ref', 'use ref / cref instead of & / const & in signatures' ) if !$in_defines && ( $m =~ /\bconst\s*&\s*\w/ || $m =~ /\w\s*&\s+in_\w/ );
        }

        # C! Allman braces, except one-liners and brace-init lists
        if( !$pp && $m =~ /\S\s*\{\s*(\x03\d+\x04)?$/ && $m !~ /[=,(]\s*\{\s*(\x03\d+\x04)?$/ && $m !~ /\\\s*$/ && $m !~ /\breturn\s*\{/ )
        {
            issue( $f, $n, 'allman', 'opening brace goes on its own line' );
        }

        # C! stray double spaces ( lining things up across lines is fine )
        my $lm = lmask( unmask( $m, $saved ), \( my $t2 = $was_in_block ) );
        if( !$define && $lm =~ /\S {2,}\S/ )
        {
            my $prev = $i > 0 ? $L->[ $i - 1 ] : '';
            my $next = $i < $#$L ? $L->[ $i + 1 ] : '';
            my $bad = 0;
            my $new = $lm;
            while( $lm =~ /(?<=\S)( {2,})(?=\S)/g )
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
            if( $bad )
            {
                if( $fix )
                {
                    my $full = unmask( $m, $saved );
                    my $lm2 = $lm;
                    my $out = '';
                    my $pos = 0;
                    while( $lm2 =~ /(?<=\S)( {2,})(?=\S)/g )
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
        if( $new ne $L->[$i] ) { $L->[$i] = $new; $d->{changed} = 1 if $fix; }
    }
}

# C! -- structural rules -- #

sub indent_of { my( $s ) = @_; my( $w ) = $s =~ /^(\s*)/; return $w; }

# C! brace depth at the start of every line, from masked code
sub depths
{
    my( $L ) = @_;
    my @depth;
    my $d = 0;
    my $in_block = 0;
    for my $i ( 0 .. $#$L )
    {
        $depth[$i] = $d;
        my( $m ) = mask( $L->[$i], \$in_block );
        next if $m =~ /^\s*#/;
        for my $ch ( split //, $m ) { $d++ if $ch eq '{'; $d-- if $ch eq '}'; }
        $d = 0 if $d < 0;
    }
    return \@depth;
}

sub structure_rules
{
    my( $f ) = @_;
    my $d = $file{$f};
    my $L = $d->{lines};
    my $is_hpp = ( $f =~ /\.(hpp|h)$/ );

    # C! no empty destructors ( { } or = default ), unless it's the virtual one a class hierarchy needs (see empty_destructor_allowed( )); reported only
    for my $i ( 0 .. $#$L )
    {
        my $s = $L->[$i];
        my $class;
        if( $is_hpp && $s =~ /^\s*(?:virtual\s+)?~(\w+)\s*\(\s*\)\s*(?:override\s*)?(?:\{\s*\}|=\s*default\s*;)/ ) { $class = $1; }
        elsif( !$is_hpp && $s =~ /^(?:\w+::)*(\w+)::~\1\s*\(\s*\)\s*(?:\{\s*\}\s*)?$/ )
        {
            my $c = $1;
            if( $s =~ /\{\s*\}/ ) { $class = $c; }
            elsif( $i + 2 <= $#$L && $L->[ $i + 1 ] =~ /^\s*\{\s*$/ && $L->[ $i + 2 ] =~ /^\s*\}\s*$/ ) { $class = $c; } # C! { and } on lines of their own
        }
        next unless defined $class;
        next if empty_destructor_allowed( $class );
        issue( $f, $i + 1, 'empty-destructor', "$class\'s destructor is empty: take it out (only a virtual one is kept, in a class none of whose bases already has a virtual destructor)" );
    }

    # C! a class's destructor comes before its constructors, where it's declared (header) and where it's defined (.cpp); reported only
    {
        my %first_destructor;
        my %first_constructor;
        my %classes;
        my %pops; # C! classes whose destructor pops (calls a pop... function)
        my %pushes; # C! and whose constructor pushes: a scope guard, which reads push then pop, so it's left as it is
        for my $i ( 0 .. $#$L )
        {
            my $s = $L->[$i];
            next if $s =~ /^\s*(#|\/\/)/;
            my $class;
            my $is_destructor = 0;
            if( $is_hpp )
            {
                $classes{$1} = 1 if $s =~ /^\s*(?:class|struct|classT\w*|structT\w*)\s+(\w+)/;
                if( $s =~ /^\s*(?:virtual\s+)?~(\w+)\s*\(/ ) { $class = $1; $is_destructor = 1; }
                elsif( $s =~ /^\s*(?:explicit\s+)?(\w+)\s*\(/ && $classes{$1} ) { $class = $1; }
            }
            else
            {
                if( $s =~ /^(?:\w+::)*(\w+)::~\1\s*\(/ ) { $class = $1; $is_destructor = 1; }
                elsif( $s =~ /^(?:\w+::)*(\w+)::\1\s*\(/ ) { $class = $1; }
            }
            next unless defined $class;
            if( $is_destructor ) { $first_destructor{$class} //= $i; $pops{$class} = 1 if $s =~ /\bpop\w*\s*\(/; }
            else { $first_constructor{$class} //= $i; $pushes{$class} = 1 if $s =~ /\bpush\w*\s*\(/; }
        }
        for my $class ( sort keys %first_destructor )
        {
            next unless defined $first_constructor{$class} && $first_constructor{$class} < $first_destructor{$class};
            next if $pops{$class} && $pushes{$class}; # C! a push/pop scope guard
            issue( $f, $first_destructor{$class} + 1, 'destructor-first', "$class\'s destructor comes before its constructors" );
        }
    }

    # C! no empty // comments, unless the line is part of a block of // lines (one straight above or below it); // C! isn't empty
    {
        my @comment; # C! per line: [ code before the comment, comment text ] when the line has a // comment
        my $in_block = 0;
        for my $i ( 0 .. $#$L )
        {
            my( $m, $saved ) = mask( $L->[$i], \$in_block );
            if( $m =~ /^(.*?)\x03(\d+)\x04\s*$/ ) { $comment[$i] = [ $1, $saved->[$2] ]; }
        }
        my $own_line = sub { my( $k ) = @_; return( $k >= 0 && $k <= $#$L && $comment[$k] && $comment[$k][0] !~ /\S/ ); };
        for( my $i = $#$L; $i >= 0; $i-- )
        {
            next unless $comment[$i] && $comment[$i][1] =~ m{^//\s*$};
            my $alone = ( $comment[$i][0] !~ /\S/ );
            next if $alone && ( $own_line->( $i - 1 ) || $own_line->( $i + 1 ) );
            if( $fix )
            {
                if( $alone ) { splice @$L, $i, 1; }
                else { $L->[$i] =~ s{\s*//\s*$}{}; }
                $d->{changed} = 1;
                fixed( 'empty-comment' );
            }
            else { issue( $f, $i + 1, 'empty-comment', 'an empty // comment, outside a block of // lines: give it some text or remove it' ); }
        }
    }

    # C! macro_end on a multi-line macro lines up with the lines above it
    for my $i ( 1 .. $#$L )
    {
        next unless $L->[$i] =~ /^(\s*)macro_end\s*$/;
        my $have = $1;
        next unless $L->[ $i - 1 ] =~ /\\\s*$/;
        my $want = indent_of( $L->[ $i - 1 ] );
        next if $have eq $want;
        if( $fix ) { $L->[$i] = "${want}macro_end"; $d->{changed} = 1; fixed( 'macro-end' ); }
        else { issue( $f, $i + 1, 'macro-end', 'macro_end should line up with the macro lines above it' ); }
    }

    # C! header guards: #ifndef Name_hpp / #define Name_hpp ... #endif /* Name_hpp */
    if( $is_hpp )
    {
        ( my $base = basename( $f ) ) =~ s/\.(hpp|h)$//;
        my $guard = "${base}_hpp";
        my( $first ) = grep { $L->[$_] =~ /\S/ } 0 .. $#$L;
        my( $last ) = reverse grep { $L->[$_] =~ /\S/ } 0 .. $#$L;
        if( defined $first && $L->[$first] =~ /^#\s*ifndef\s+(\w+)/ && $first < $#$L && $L->[ $first + 1 ] =~ /^#\s*define\s+(\w+)/ )
        {
            my $have = $1;
            my $endif_ok = ( $L->[$last] =~ m{^#endif /\* \Q$guard\E \*/\s*$} );
            if( $have ne $guard || $L->[$first] !~ m{^#ifndef \Q$guard\E\s*(//.*)?$} || !$endif_ok )
            {
                if( $fix && $L->[$last] =~ /^#\s*endif/ )
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
        elsif( defined $first && grep { $L->[$_] =~ /\S/ && $L->[$_] !~ m{^\s*//} } 0 .. $#$L )
        {
            if( $fix )
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
    for( my $i = 1; $i <= $#$L; $i++ )
    {
        next unless $L->[$i] =~ m{^\}\s*//\s*namespace};
        my $prev = $L->[ $i - 1 ];
        if( $prev =~ m{^\}\s*//\s*namespace} ) { next; }
        if( $prev !~ /\S/ && $i > 1 && $L->[ $i - 2 ] =~ m{^\}\s*//\s*namespace} )
        {
            if( $fix ) { splice @$L, $i - 1, 1; $d->{changed} = 1; fixed( 'namespace-close' ); $i--; }
            else { issue( $f, $i + 1, 'namespace-close', 'no blank line between consecutive namespace closers' ); }
            next;
        }
        if( $prev =~ /\S/ )
        {
            if( $fix ) { splice @$L, $i, 0, ''; $d->{changed} = 1; fixed( 'namespace-close' ); $i++; }
            else { issue( $f, $i + 1, 'namespace-close', 'leave one blank line before the closing namespace bracket' ); }
        }
    }

    # C! classes and structs: blank line after { and before };, access specifiers lined up with the class with a blank line on each side
    my $depth = depths( $L );
    for( my $i = 0; $i < $#$L; $i++ )
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
            my( $m ) = mask( $L->[$j], \$in_block );
            next if $m =~ /^\s*#/;
            for my $ch ( split //, $m ) { $dd++ if $ch eq '{'; $dd-- if $ch eq '}'; }
            if( $dd == 0 ) { $close = $j; last; }
        }
        next unless defined $close && $L->[$close] =~ /^\s*\};\s*$/;
        if( $L->[ $open + 1 ] =~ /\S/ )
        {
            if( $fix ) { splice @$L, $open + 1, 0, ''; $close++; $d->{changed} = 1; fixed( 'class-blank-lines' ); }
            else { issue( $f, $open + 2, 'class-blank-lines', 'leave a blank line after the class\'s opening {' ); }
        }
        if( $L->[ $close - 1 ] =~ /\S/ )
        {
            if( $fix ) { splice @$L, $close, 0, ''; $close++; $d->{changed} = 1; fixed( 'class-blank-lines' ); }
            else { issue( $f, $close + 1, 'class-blank-lines', 'leave a blank line before the class\'s closing };' ); }
        }
        # C! access specifiers directly inside this class
        my $dep = depths( $L );
        my $inner = $dep->[ $open + 1 ];
        for( my $j = $open + 1; $j < $close; $j++ )
        {
            next unless $dep->[$j] == $inner && $L->[$j] =~ /^(\s*)(public|private|protected)\s*:\s*(\/\/.*)?$/;
            if( $1 ne $cls_indent )
            {
                if( $fix ) { $L->[$j] =~ s/^\s*/$cls_indent/; $d->{changed} = 1; fixed( 'access-specifier' ); }
                else { issue( $f, $j + 1, 'access-specifier', 'access specifier lines up with its class (no indent)' ); }
            }
            if( $L->[ $j - 1 ] =~ /\S/ && $L->[ $j - 1 ] !~ /^\s*\{\s*$/ )
            {
                if( $fix ) { splice @$L, $j, 0, ''; $j++; $close++; $d->{changed} = 1; fixed( 'access-specifier' ); }
                else { issue( $f, $j + 1, 'access-specifier', 'leave a blank line before the access specifier' ); }
            }
            if( $j < $#$L && $L->[ $j + 1 ] =~ /\S/ )
            {
                if( $fix ) { splice @$L, $j + 1, 0, ''; $close++; $d->{changed} = 1; fixed( 'access-specifier' ); }
                else { issue( $f, $j + 1, 'access-specifier', 'leave a blank line after the access specifier' ); }
            }
            $dep = depths( $L ) if $fix;
        }
    }

    # C! no blank line straight after a function's opening {
    for( my $i = 1; $i < $#$L; $i++ )
    {
        next unless $L->[$i] =~ /^\{\s*$/ && $L->[ $i + 1 ] !~ /\S/;
        my( $p ) = grep { $L->[$_] =~ /\S/ } reverse 0 .. $i - 1;
        next unless defined $p;
        my $sig = $L->[$p];
        next if $sig =~ /^\s*(namespace|class|struct|enum|union|extern|section|static_once)\b/ || $sig =~ /^\s*(classT|structT)\w*\b/ || $sig =~ /=\s*$/;
        next unless $sig =~ /\)\s*(const)?\s*(override)?\s*$/ || $sig =~ /^\S.*::.*\(/;
        if( $fix ) { splice @$L, $i + 1, 1; $d->{changed} = 1; fixed( 'function-blank-line' ); }
        else { issue( $f, $i + 2, 'function-blank-line', 'no blank line after a function\'s opening {' ); }
    }

    # C! preprocessor lines inside a function or class are indented the way Visual Studio indents them, so its formatter and this
    # C! agree: like the code line after them would be (before a closing } or an access specifier, like the block's contents), so an
    # C! #endif between an unbraced if and its statement takes the statement's indent. An #if that splits a declaration (the code
    # C! line above it isn't finished) takes that declaration's indent instead, and so does the rest of its group
    $depth = depths( $L );
    my @group; # C! for each open #if, the declaration's indent when it splits one
    my $next_indent = sub
    {
        my( $i ) = @_;
        my( $next ) = grep { $L->[$_] =~ /\S/ && $L->[$_] !~ /^\s*#/ } $i + 1 .. $#$L;
        return( undef ) unless defined $next;
        my $indent = indent_of( $L->[$next] );
        $indent .= '    ' if ( $L->[$next] =~ /^\s*\}/ && $L->[$next] !~ m{\}\s*//\s*namespace} ) || $L->[$next] =~ /^\s*(public|private|protected)\s*:/; # C! the block's contents (a namespace's aren't indented)
        return( $indent );
    };
    for my $i ( 0 .. $#$L )
    {
        next unless $L->[$i] =~ /^(\s*)#\s*(if|ifdef|ifndef|else|elif|endif)\b/;
        my $have = $1;
        my $dir = $2;
        my $want;
        if( $dir =~ /^if/ )
        {
            my $split;
            if( $depth->[$i] > 0 )
            {
                my( $prev ) = grep { $L->[$_] =~ /\S/ && $L->[$_] !~ /^\s*#/ } reverse 0 .. $i - 1;
                my $prev_code = defined $prev ? $L->[$prev] : '';
                $prev_code =~ s/\s*\/\/.*$//;
                if( defined $prev && $prev_code =~ /\S/ && $prev_code !~ /[;{}:,\\]\s*$/ && $prev_code !~ /^\s*(public|private|protected)\s*:/ && $prev_code !~ /^\s*(?:\}\s*)?(?:if|else|for|while|switch)\b/ ) { $split = indent_of( $L->[$prev] ); }
            }
            $want = $split // $next_indent->( $i ) if $depth->[$i] > 0;
            push @group, [ $split, $depth->[$i] > 0 ]; # C! a group outside any function or class is left alone
        }
        else
        {
            my( $split, $inside ) = @group ? @{ $group[-1] } : ( undef, 0 );
            $want = $split // $next_indent->( $i ) if $inside;
            pop @group if $dir eq 'endif';
        }
        next unless defined $want && $depth->[$i] > 0;
        next if $have eq $want;
        if( $fix ) { $L->[$i] =~ s/^\s*/$want/; $d->{changed} = 1; fixed( 'preprocessor-indent' ); }
        else { issue( $f, $i + 1, 'preprocessor-indent', 'preprocessor line inside a function or class is indented like the code line after it, as Visual Studio does' ); }
    }
}

# C! -- declaration order -- #

my %skipkw = map { $_ => 1 } qw( if for while switch return sizeof catch equal_default equal_never equal_always strinfo STATE var_const var_static static_assert decltype Assert Log for_range for_each for_each_const do_count while_max return_if rethis_if break_if continue_if return_true_if return_false_if );

# C! function declarations in a header, in order, as "Class::Nested::name" keys, plus each class's own list
sub header_decls
{
    my( $L ) = @_;
    my @stack;
    my $pending;
    my %idx;
    my $n = 0;
    my %by_class;
    my $in_block = 0;
    for my $i ( 0 .. $#$L )
    {
        my( $s ) = mask( $L->[$i], \$in_block );
        $s =~ s/[\x01\x03]\d+[\x02\x04]/""/g;
        next if $s =~ /^\s*#/;
        my $in_fn = grep { $_->[0] eq 'fn' } @stack;
        if( !$in_fn )
        {
            if( $s =~ /^\s*(?:template\s*<[^>]*>\s*)?(?:class|struct)\s+(\w+)\b[^;]*$/ || $s =~ /^\s*(?:classT\w*|structT\w*)\s+(\w+)\b[^;]*$/ ) { $pending = [ 'class', $1 ]; }
            elsif( $s =~ /^\s*namespace\b/ ) { $pending = [ 'ns', '' ]; }
            elsif( $s =~ /^\s*(?:(?:public|private|protected)\s*:\s*)?(?:[\w:<>,*&\s]+?\s+)?(~?\w+|operator\s*[^\s(]+)\s*\(/ && !$skipkw{$1} && $s !~ /^\s*(?:public|private|protected)\s*:\s*$/ )
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
            if( $ch eq '{' ) { push @stack, ( $pending // [ 'blk', '' ] ); $pending = undef; }
            elsif( $ch eq '}' ) { pop @stack; }
        }
        $pending = undef if $s =~ /;\s*$/;
    }
    return( \%idx, \%by_class );
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
        while( my $line = <$fh> )
        {
            next unless $line =~ /^\s*(?:class|struct)\s+(\w+)\s*:\s*(.+?)\s*$/;
            my $cls = $1;
            for my $base ( split /,/, $2 ) { $base =~ s/^\s*(public|protected|private)\s+//; $base =~ s/.*:://; $base =~ s/\W.*//; push @{ $parents{$cls} }, $base if $base ne ''; }
        }
        close $fh;
    }
    my %is = ( Object => 1 );
    my $grew = 1;
    while( $grew ) { $grew = 0; for my $c ( keys %parents ) { next if $is{$c}; if( grep { $is{$_} } @{ $parents{$c} } ) { $is{$c} = 1; $grew = 1; } } }
    return \%is;
}

sub order_rules
{
    my( $order, $derived ) = @_;
    my %rank;
    @rank{@$order} = 0 .. $#$order;

    # C! headers: Object's functions in Object.hpp's order, render before update, debug_overlay after render
    for my $f ( grep { /\.(hpp|h)$/ } @files )
    {
        my( undef, $by_class ) = header_decls( $file{$f}{lines} );
        for my $q ( sort keys %$by_class )
        {
            my $cls = ( split /::/, $q )[-1];
            my @decls = @{ $by_class->{$q} };
            my %first;
            for my $dcl ( @decls ) { $first{ $dcl->[0] } //= $dcl->[1]; }
            if( defined $first{debug_overlay} && defined $first{render} && $first{debug_overlay} < $first{render} )
            {
                issue( $f, $first{debug_overlay}, 'member-order', "$q: debug_overlay( ) should come after render( )" );
            }
            next unless $derived->{$cls};
            if( defined $first{render} && defined $first{update} && $first{update} < $first{render} )
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
        ( $hpp = $cpp ) =~ s{^(.*/[A-Z][a-z]+)[A-Z]\w*\.cpp$}{$1.hpp} unless -f $hpp; # C! a file with no header of its own, like EngineSDL.cpp, follows the one it's named after (Engine.hpp)
        next unless -f $hpp;
        my $HL = $file{$hpp} ? $file{$hpp}{lines} : do { open my $h, '<:raw', $hpp or next; my @l = map { s/\r?\n$//r } <$h>; \@l };
        my( $idx ) = header_decls( $HL );
        reorder_cpp( $cpp, $idx );
        lift_helpers( $cpp, $idx );
    }
}

# C! a free function (not one of the header's) used by a definition above it is moved up to just above the first definition that uses it,
# C! with its comments, so it's declared before it's used. Reported only, unless fixing
sub lift_helpers
{
    my( $cpp, $idx ) = @_;
    my $d = $file{$cpp};
    my $L = $d->{lines};

    for my $pass ( 1 .. 20 ) # C! each pass moves at most one helper, and a file has only a few
    {
        my @ms;
        { my $ib = 0; @ms = map { my( $m ) = mask( $_, \$ib ); $m =~ s/[\x01\x03]\d+[\x02\x04]/""/g; $m } @$L; }

        # C! every function definition at file level: its name, where its comments start, where its body ends, and whether it's the header's
        my @defs;
        my $depth = 0;
        my $i = 0;
        while( $i <= $#$L )
        {
            my $s = $ms[$i];
            if( $depth == 0 && $s !~ /^\s*#/ && $s !~ /;\s*$/ && $s =~ /^(?:[\w:<>,*&\s]+?\s+)?((?:\w+::)*(?:~?\w+|operator\s*[^\s(]+))\s*\(/ && $s !~ /^\s*(?:var_const|var_static|section|namespace|return|if|for|while|switch|macro_\w+)\b/ )
            {
                my $name = $1;
                $name =~ s/\s+//g;
                my( $dd, $seen, $e ) = ( 0, 0, undef );
                for my $j ( $i .. $#$L )
                {
                    for my $ch ( split //, $ms[$j] ) { if( $ch eq '{' ) { $dd++; $seen = 1; } elsif( $ch eq '}' ) { $dd--; } }
                    if( $seen && $dd == 0 ) { $e = $j; last; }
                    last if !$seen && $ms[$j] =~ /;\s*$/;
                }
                if( defined $e )
                {
                    my $st = $i;
                    while( $st > 0 && $L->[ $st - 1 ] =~ m{^\s*//} && $L->[ $st - 1 ] !~ m{^\s*//\s*(C!\s*)?-+} ) { $st--; }
                    push @defs, { name => $name, s => $st, head => $i, e => $e, mine => exists $idx->{$name} };
                    $i = $e + 1;
                    next;
                }
            }
            for my $ch ( split //, $s ) { $depth++ if $ch eq '{'; $depth-- if $ch eq '}'; }
            $depth = 0 if $depth < 0;
            $i++;
        }

        # C! the first helper used by a definition above it
        my( $helper, $user );
        for my $h ( grep { !$_->{mine} && $_->{name} !~ /::/ } @defs )
        {
            for my $u ( grep { $_->{s} < $h->{s} } @defs )
            {
                next unless grep { $ms[$_] =~ /\b\Q$h->{name}\E\b/ } $u->{head} .. $u->{e};
                ( $helper, $user ) = ( $h, $u );
                last;
            }
            last if $helper;
        }
        return unless $helper;

        if( !$fix )
        {
            issue( $cpp, $helper->{head} + 1, 'helper-order', "$helper->{name}( ) is used above it, by $user->{name}( ): define it before" );
            return;
        }

        my @moved = @{$L}[ $helper->{s} .. $helper->{e} ];
        my( $cut_s, $cut_e ) = ( $helper->{s}, $helper->{e} );
        $cut_e++ if $cut_e < $#$L && $L->[ $cut_e + 1 ] !~ /\S/; # C! and the blank line after it
        splice( @$L, $cut_s, $cut_e - $cut_s + 1 );
        splice( @$L, $user->{s}, 0, @moved, '' );
        $d->{changed} = 1;
        fixed( 'helper-order' );
    }
}

# C! definitions are gathered, sorted into the header's order, and put where the last one was, so helpers, macros and constants stay above them
# C! only definitions in the same #if region move together, and each keeps its comments, a divider straight above it, and an #if wrapped around only it
sub reorder_cpp
{
    my( $cpp, $idx ) = @_;
    my $d = $file{$cpp};
    my $L = $d->{lines};
    my @region;
    my @pp;
    for my $i ( 0 .. $#$L )
    {
        if( $L->[$i] =~ /^\s*#\s*if/ ) { $region[$i] = join( ',', @pp ); push @pp, $i; }
        elsif( $L->[$i] =~ /^\s*#\s*endif/ ) { pop @pp; $region[$i] = join( ',', @pp ); }
        elsif( $L->[$i] =~ /^\s*#\s*el/ ) { $pp[-1] = "$pp[-1]e$i" if @pp; $region[$i] = join( ',', @pp ); }
        else { $region[$i] = join( ',', @pp ); }
    }
    my @units;
    my $depth = 0;
    my $in_block = 0;
    my $i = 0;
    my @ms;
    { my $ib = 0; @ms = map { my( $m ) = mask( $_, \$ib ); $m =~ s/[\x01\x03]\d+[\x02\x04]/""/g; $m } @$L; }
    while( $i <= $#$L )
    {
        my $s = $ms[$i];
        if( $depth == 0 && $s !~ /^\s*#/ && $s !~ /;\s*$/ && $s =~ /^\s*(?:[\w:<>,*&\s]+?\s+)?((?:\w+::)*(?:~?\w+|operator\s*[^\s(]+))\s*\(/ )
        {
            my $key = $1;
            $key =~ s/\s+//g;
            if( exists $idx->{$key} )
            {
                my( $dd, $seen, $e ) = ( 0, 0, $i );
                for my $j ( $i .. $#$L )
                {
                    for my $ch ( split //, $ms[$j] ) { if( $ch eq '{' ) { $dd++; $seen = 1; } elsif( $ch eq '}' ) { $dd--; } }
                    if( $seen && $dd == 0 ) { $e = $j; last; }
                }
                my $st = $i;
                while( $st > 0 && $L->[ $st - 1 ] =~ m{^\s*//} && $L->[ $st - 1 ] !~ m{^\s*//\s*(C!\s*)?-+} ) { $st--; }
                my( $us, $ue ) = ( $st, $e );
                $us -= 2 if $us > 1 && $L->[ $us - 1 ] !~ /\S/ && $L->[ $us - 2 ] =~ m{^\s*//\s*(C!\s*)?-+.*-+\s*//\s*$};
                if( $us > 0 && $L->[ $us - 1 ] =~ /^\s*#\s*if\b/ && $ue < $#$L && $L->[ $ue + 1 ] =~ /^\s*#\s*endif/ && !grep { $L->[$_] =~ /^\s*#/ } $us .. $ue ) { $us--; $ue++; }
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
    my( %drop, %replace );
    my $moved = 0;
    for my $r ( sort keys %by )
    {
        my @slots = @{ $by{$r} };
        my @sorted = sort { $a->{rank} <=> $b->{rank} || $a->{s} <=> $b->{s} } @slots;
        next if join( ',', map { $_->{s} } @slots ) eq join( ',', map { $_->{s} } @sorted );
        if( !$fix )
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
            if( $u->{e} + 1 <= $#$L && $L->[ $u->{e} + 1 ] !~ /\S/ && !$drop{ $u->{e} + 1 } ) { $drop{ $u->{e} + 1 } = 1; }
            elsif( $u->{s} > 0 && $L->[ $u->{s} - 1 ] !~ /\S/ && !$drop{ $u->{s} - 1 } ) { $drop{ $u->{s} - 1 } = 1; }
        }
        my @block;
        for my $k ( 0 .. $#sorted )
        {
            my $u = $sorted[$k];
            if( $k ) { my $p = $sorted[ $k - 1 ]; push @block, '' unless $u->{s} == $p->{e} + 1; }
            push @block, @{$L}[ $u->{s} .. $u->{e} ];
        }
        $replace{ $last->{s} } = [ $last->{e}, \@block ];
    }
    return unless $fix && $moved;
    my @out;
    my $p = 0;
    while( $p <= $#$L )
    {
        if( my $rep = $replace{$p} ) { push @out, @{ $rep->[1] }; $p = $rep->[0] + 1; next; }
        push @out, $L->[$p] unless $drop{$p};
        $p++;
    }
    @$L = @out;
    $d->{changed} = 1;
    fixed( 'cpp-order', $moved );
}

# C! -- project setting files: no comments at all, report only (they're read here, never written) -- #

sub project_files
{
    my @p = grep { -f $_ } ( 'Axion.vcxproj', 'Axion.vcxproj.filters' );
    if( -d 'Axion.xcodeproj' )
    {
        find( { no_chdir => 1, wanted => sub { my $p = $File::Find::name; $p =~ s{^\./}{}; push @p, $p if -f $p && $p =~ m{(/project\.pbxproj|\.xcscheme|\.xcsettings)$}; } }, 'Axion.xcodeproj' );
    }
    return sort @p;
}

sub project_rules
{
    for my $f ( project_files( ) )
    {
        open my $h, '<:raw', $f or next;
        my $n = 0;
        while( my $line = <$h> )
        {
            $n++;
            if( $f =~ /\.pbxproj$/ )
            {
                # C! Xcode writes its own /* name */ comments in here, so only // comments and C! ones can be told apart
                # C! the first line, // !$*UTF8*$!, is Xcode's own
                next if $n == 1 && $line =~ m{^// !\$\*UTF8\*\$!};
                ( my $code = $line ) =~ s/"(?:[^"\\]|\\.)*"//g;
                issue( $f, $n, 'project-comment', 'comment in a project setting file' ) if $code =~ m{//} || $code =~ m{/\*\s*C!};
            }
            elsif( $line =~ /<!--/ )
            {
                issue( $f, $n, 'project-comment', 'comment in a project setting file' );
            }
        }
        close $h;
    }
}

# C! -- overrides call their parent, or say they purposely don't -- #

# C! a file's code as one string, comments and strings blanked out (each character kept, so positions and lines still line up)
# C! and preprocessor lines blanked (macro definitions like strinfo( ) hold overrides that aren't written out anywhere)
my %code_cache;
sub code_of
{
    my( $f ) = @_;
    return $code_cache{$f} if exists $code_cache{$f};
    my @L;
    if( $file{$f} ) { @L = @{ $file{$f}{lines} }; }
    else { open my $h, '<:raw', $f or return( $code_cache{$f} = '' ); @L = map { s/\r?\n$//r } <$h>; close $h; }
    my $in_block = 0;
    my $continued = 0;
    my @out;
    for my $line ( @L )
    {
        my( $m, $saved ) = mask( $line, \$in_block );
        $m =~ s/([\x01\x03])(\d+)([\x02\x04])/' ' x length( $saved->[$2] )/ge;
        my $directive = $continued || $m =~ /^\s*#/;
        $continued = $directive && $line =~ /\\\s*$/;
        push @out, $directive ? ( ' ' x length( $m ) ) : $m;
    }
    return( $code_cache{$f} = join( "\n", @out ) );
}

sub line_at { my( $s, $pos ) = @_; return 1 + ( () = substr( $s, 0, $pos ) =~ /\n/g ); }

# C! the position of the bracket closing the one at in_pos
sub close_of
{
    my( $s, $pos ) = @_;
    my( $open, $close ) = ( substr( $s, $pos, 1 ), ( substr( $s, $pos, 1 ) eq '(' ) ? ')' : '}' );
    my $depth = 0;
    for my $i ( $pos .. length( $s ) - 1 )
    {
        my $c = substr( $s, $i, 1 );
        if( $c eq $open ) { $depth++; }
        elsif( $c eq $close ) { $depth--; return $i if !$depth; }
    }
    return -1;
}

# C! a parameter list without names or default values, so a declaration and its definition compare equal
sub param_types
{
    my( $p ) = @_;
    my @params;
    my( $depth, $cur ) = ( 0, '' );
    for my $c ( split //, $p )
    {
        $depth++ if $c =~ /[(<\[{]/;
        $depth-- if $c =~ /[)>\]}]/;
        if( $c eq ',' && !$depth ) { push @params, $cur; $cur = ''; } else { $cur .= $c; }
    }
    push @params, $cur;
    my @types;
    for my $x ( @params )
    {
        $x =~ s/=.*$//s;
        $x =~ s/\s+/ /g;
        $x =~ s/^\s+|\s+$//g;
        next if $x eq '';
        $x =~ s/\s+\w+$// if $x =~ /\S\s+\w+$/ && $x !~ /^(?:const\s+)?\w+$/;
        $x =~ s/\s+//g;
        push @types, $x;
    }
    return join( ',', @types );
}

# C! the body of in_class::in_name( in_params ) in the source files, as [ file, line, body ]
sub find_definition
{
    my( $class, $name, $params, $const, $header ) = @_;
    ( my $own_cpp = $header ) =~ s/\.(hpp|h)$/.cpp/;
    my @cpps = ( ( -f $own_cpp ? $own_cpp : () ), grep { $_ ne $own_cpp } @{ all_cpps( ) } );
    for my $cpp ( @cpps )
    {
        my $s = code_of( $cpp );
        while( $s =~ /\b\Q$class\E::\Q$name\E\s*\(/g )
        {
            my $open = $+[0] - 1;
            my $close = close_of( $s, $open );
            next if $close < 0;
            next unless param_types( substr( $s, $open + 1, $close - $open - 1 ) ) eq $params;
            my $rest = substr( $s, $close + 1, 200 );
            next unless $rest =~ /^(\s*const\b)?[^;{]*?\{/;
            next if( defined $1 ? 1 : 0 ) != $const;
            my $body_open = $close + 1 + $-[0] + length( $& ) - 1;
            my $body_close = close_of( $s, $body_open );
            next if $body_close < 0;
            return [ $cpp, line_at( $s, $body_open ), substr( $s, $body_open, $body_close - $body_open + 1 ) ];
        }
    }
    return undef;
}

my $all_cpps;
sub all_cpps
{
    return $all_cpps if $all_cpps;
    my @c;
    find( { no_chdir => 1, wanted => sub { my $p = $File::Find::name; $p =~ s{^\./}{}; push @c, $p if $p =~ /\.cpp$/ && is_src_file( $p ); } }, 'src' );
    return( $all_cpps = [ sort @c ] );
}

# C! every class in the headers: its base classes, and the text of its body, by its own name (nested classes by their innermost name,
# C! so two classes with the same name both count)
my %class_bases;
my %class_bodies;
sub read_classes
{
    return if %class_bodies;
    my @headers;
    find( { no_chdir => 1, wanted => sub { my $p = $File::Find::name; $p =~ s{^\./}{}; push @headers, $p if $p =~ /\.(hpp|h)$/ && is_src_file( $p ) && $p !~ /Catch\.hpp$/; } }, 'src' );
    for my $h ( @headers )
    {
        my $s = code_of( $h );
        while( $s =~ /\b(?:class|struct|classT\w*|structT\w*)\s+(\w+)\s*(:[^;{]*)?\{/g )
        {
            my( $name, $bases ) = ( $1, $2 // '' );
            my $open = $+[0] - 1;
            my $close = close_of( $s, $open );
            next if $close < 0;
            push @{ $class_bodies{$name} }, substr( $s, $open, $close - $open + 1 );
            $bases =~ s/^://;
            for my $base ( split /,/, $bases )
            {
                $base =~ s/<.*//; $base =~ s/^\s*(?:public|protected|private|virtual)\s+//g; $base =~ s/.*:://; $base =~ s/\W//g;
                push @{ $class_bases{$name} }, $base if $base ne '';
            }
        }
    }
}

# C! whether in_class may have an empty destructor: only a virtual one, and only when none of its base classes (all the way up) already has a
# C! virtual destructor. That one is needed so deleting through a pointer to the class runs the whole object's destructor; any other empty
# C! destructor does nothing the compiler wouldn't (a class whose base has a virtual one gets a virtual one anyway)
sub empty_destructor_allowed
{
    my( $class ) = @_;
    read_classes( );
    return 0 unless grep { /\bvirtual\s+~\Q$class\E\s*\(/ } @{ $class_bodies{$class} // [ ] };
    my @queue = @{ $class_bases{$class} // [ ] };
    my %seen;
    while( @queue )
    {
        my $base = shift @queue;
        next if $seen{$base}++;
        return 0 if grep { /\bvirtual\s+~\Q$base\E\s*\(/ } @{ $class_bodies{$base} // [ ] };
        push @queue, @{ $class_bases{$base} // [ ] };
    }
    return 1;
}

# C! whether the nearest base class of in_class that declares in_name declares it pure virtual ( = 0; )
sub parent_is_pure
{
    my( $class, $name ) = @_;
    read_classes( );
    my @queue = @{ $class_bases{$class} // [ ] };
    my %seen;
    while( @queue )
    {
        my $base = shift @queue;
        next if $seen{$base}++;
        my @bodies = @{ $class_bodies{$base} // [ ] };
        return 1 if grep { /\b\Q$name\E\s*\((?:[^()]|\([^()]*\))*\)\s*(?:const\s*)?(?:override\s*)?(?:final\s*)?=\s*0\s*;/ } @bodies;
        next if grep { /\b\Q$name\E\s*\(/ } @bodies; # C! declared with a body: that's the parent, and it isn't pure
        push @queue, @{ $class_bases{$base} // [ ] };
    }
    return 0;
}

# C! takes skip_override; out of the body that starts at in_line of in_file
sub remove_skip_override
{
    my( $f, $line ) = @_;
    my $L = $file{$f}{lines} or return;
    for my $i ( $line - 1 .. ( ( $line + 3 < $#$L ) ? $line + 3 : $#$L ) )
    {
        if( $L->[$i] =~ /^\s*skip_override;.*$/ )
        {
            splice @$L, $i, ( $i < $#$L && $L->[ $i + 1 ] !~ /\S/ ) ? 2 : 1; # C! and the blank line after it
        }
        elsif( $L->[$i] =~ s/\{ skip_override; (?:\/\/[^\n]*?(?=\}))?/{ / ) { }
        else { return if $L->[$i] =~ /}/; next; } # C! past the end of this body: it isn't there
        $file{$f}{changed} = 1;
        fixed( 'skip-override-unneeded' );
        return;
    }
}

sub override_rules
{
    %code_cache = ( ); # C! read again, the files may have been fixed since
    %class_bodies = ( );
    %class_bases = ( );
    my @skip_removals;

    my %checking = map { $_ => 1 } @files;
    my @headers;
    find( { no_chdir => 1, wanted => sub { my $p = $File::Find::name; $p =~ s{^\./}{}; push @headers, $p if $p =~ /\.(hpp|h)$/ && is_src_file( $p ) && $p !~ /Catch\.hpp$/; } }, 'src' );

    for my $h ( sort @headers )
    {
        my $s = code_of( $h );
        my @stack; # C! [ 'class', name ] or [ 'other' ] for each open {
        my $pending;
        my $i = 0;
        my $n = length $s;
        while( $i < $n )
        {
            my $rest = substr( $s, $i, 64 );
            if( $rest =~ /^(?:class|struct|classT\w*|structT\w*)\s+(\w+)/ && ( $i == 0 || substr( $s, $i - 1, 1 ) !~ /\w/ ) )
            {
                my $name = $1;
                my $after = substr( $s, $i, 400 );
                $pending = $name if $after =~ /^[^;{]*\{/;
            }
            my $c = substr( $s, $i, 1 );
            if( $c eq '{' ) { push @stack, defined $pending ? [ 'class', $pending ] : [ 'other' ]; $pending = undef; }
            elsif( $c eq '}' ) { pop @stack; }
            elsif( $c eq ';' ) { $pending = undef; }
            elsif( $rest =~ /^override\b/ && substr( $s, $i - 1, 1 ) !~ /\w/ && @stack && $stack[-1][0] eq 'class' )
            {
                # C! back from override to the parameter list's closing ), past any const or & qualifiers
                my $before = substr( $s, 0, $i );
                if( $before =~ /\)\s*(const\s*)?(?:&&?\s*)?(?:noexcept\s*)?$/ )
                {
                    my $const = defined $1 ? 1 : 0;
                    my $close = $-[0];
                    my $depth = 0;
                    my $open = -1;
                    for( my $k = $close; $k >= 0; $k-- )
                    {
                        my $d = substr( $s, $k, 1 );
                        if( $d eq ')' ) { $depth++; } elsif( $d eq '(' ) { $depth--; if( !$depth ) { $open = $k; last; } }
                    }
                    if( $open > 0 && substr( $s, 0, $open ) =~ /(~?\w+)\s*$/ && $1 !~ /^operator/ && substr( $s, 0, $open ) !~ /operator\s*\S*\s*$/ )
                    {
                        my $name = $1;
                        my $class = $stack[-1][1];
                        my $params = param_types( substr( $s, $open + 1, $close - $open - 1 ) );
                        my $after = substr( $s, $i + 8, 50 );
                        my $where;
                        if( $after =~ /^\s*\{/ )
                        {
                            my $body_open = $i + 8 + $-[0] + length( $& ) - 1;
                            my $body_close = close_of( $s, $body_open );
                            $where = [ $h, line_at( $s, $i ), substr( $s, $body_open, $body_close - $body_open + 1 ) ] if $body_close > 0;
                        }
                        elsif( $after =~ /^\s*;/ )
                        {
                            $where = find_definition( $class, $name, $params, $const, $h );
                        }
                        if( $where && $checking{ $where->[0] } )
                        {
                            # C! a parent that's pure virtual ( = 0; ) has nothing to call, and a const function only gives something back (calling the
                            # C! parent would change nothing), so either override needs neither the call nor skip_override
                            if( $const || parent_is_pure( $class, $name ) )
                            {
                                if( $where->[2] =~ /\bskip_override\b/ )
                                {
                                    if( $fix ) { push @skip_removals, [ $where->[0], $where->[1] ]; } # C! made after the scan, bottom up, so removing lines doesn't move the ones still to come
                                    else { issue( $where->[0], $where->[1], 'skip-override-unneeded', "$class\::$name( ) " . ( $const ? "is const" : "overrides a pure virtual function" ) . ", so it doesn't need skip_override" ); }
                                }
                            }
                            elsif( $where->[2] !~ /\bskip_override\b/ && $where->[2] !~ /\b\w+(?:<[^<>;]*>)?::\Q$name\E\s*\(/ )
                            {
                                issue( $where->[0], $where->[1], 'override-parent', "$class\::$name( ) overrides without calling its parent's $name( ): call it, or put skip_override; in it when that's on purpose" );
                            }
                        }
                    }
                }
            }
            $i++;
        }
    }

    remove_skip_override( @$_ ) for sort { $a->[0] cmp $b->[0] || $b->[1] <=> $a->[1] } @skip_removals; # C! bottom up within each file
}

# C! -- run -- #

for my $f ( @files ) { line_rules( $f ); structure_rules( $f ); }
order_rules( object_order( ), object_classes( ) );
override_rules( );

if( $fix )
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
    override_rules( );
    project_rules( ) if $all_files;
    if( @issues ) { print "\nleft to fix by hand:\n"; }
}
else
{
    project_rules( ) if $all_files;
}

my %count;
for my $x ( sort { $a->[0] cmp $b->[0] || $a->[1] <=> $b->[1] } @issues ) { print "$x->[0]:$x->[1]: [$x->[2]] $x->[3]\n"; $count{ $x->[2] }++; }
if( @issues )
{
    print "\n", scalar( @issues ), " style violation(s)\n";
    printf "  %-22s %d\n", $_, $count{$_} for sort { $count{$b} <=> $count{$a} } keys %count;
    exit 1;
}
print "no style violations found\n" unless $mode eq 'fix' && %fixed;
exit 0;
