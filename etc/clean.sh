#!/usr/bin/env bash
# C! removes what building, testing, and running leave behind, so the next build starts fresh
# C! usage: etc/clean.sh [--dry-run] [--ide]
# C!   always removes:
# C!     - everything in bin/ (the built programs) and tmp/ (the coverage report, screenshots, format-fix.diff), keeping the folders
# C!     - the intermediate files Visual Studio builds into %USERPROFILE%/AxionBuild/tmp (Windows)
# C!     - the coverage build in ~/AxionBuild/coverage (etc/coverage/coverage.sh)
# C!     - Xcode's DerivedData for Axion (OSX)
# C!     - .DS_Store files in the repo
# C!   --ide also clears Visual Studio's caches in .vs/ (IntelliSense databases, precompiled headers, the file index: several gigabytes),
# C!     keeping its .suo / .wsuo / DocumentLayout files (breakpoints, open documents, window layout). Close Visual Studio first: it rebuilds
# C!     the caches the next time the solution is opened, which takes a while
# C!   --dry-run lists what would be removed and removes nothing
# C! works in Git Bash on Windows, and on OSX and Linux

set -u
shopt -s nullglob dotglob # C! a pattern that matches nothing expands to nothing, and * includes hidden files

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$script_dir"
while [ ! -d "$root/src" ] && [ "$root" != "/" ]; do root="$( dirname "$root" )"; done
[ -d "$root/src" ] || { echo "clean: can't find the repo (no src/ above $script_dir)" >&2; exit 2; }
cd "$root" || exit 2

dry_run=0
ide=0
for arg in "$@"; do
    case "$arg" in
        --dry-run) dry_run=1 ;;
        --ide) ide=1 ;;
        *) echo "usage: etc/clean.sh [--dry-run] [--ide]" >&2; exit 2 ;;
    esac
done

failed=0

# C! removes each path that exists, and says what it did
remove( )
{
    for path in "$@"; do
        [ -e "$path" ] || [ -L "$path" ] || continue
        if [ "$dry_run" = 1 ]; then
            echo "would remove $path"
        elif rm -rf -- "$path"; then
            echo "removed $path"
        else
            echo "clean: couldn't remove $path (is something using it?)" >&2
            failed=1
        fi
    done
}

# C! the folder Visual Studio and the coverage build put their files under: %USERPROFILE% on Windows, ~ elsewhere
case "$( uname -s )" in
    MINGW*|MSYS*|CYGWIN*) os=windows ;;
    Darwin) os=osx ;;
    *) os=linux ;;
esac

user_dir="$HOME"
if [ "$os" = windows ] && [ -n "${USERPROFILE:-}" ] && command -v cygpath > /dev/null 2>&1; then
    user_dir="$( cygpath -u "$USERPROFILE" )"
fi

# C! -- the repo's own build output -- #

remove bin/* tmp/*

# C! -- intermediate files outside the repo (kept out of OneDrive, see the build notes in .claude/CLAUDE.md) -- #

remove "$user_dir/AxionBuild/tmp" "$HOME/AxionBuild/coverage"

# C! -- Xcode -- #

if [ "$os" = osx ]; then
    remove "$HOME"/Library/Developer/Xcode/DerivedData/Axion-*
fi

# C! -- OSX's folder settings files -- #

while IFS= read -r ds_store; do
    remove "$ds_store"
done < <( find . -name .DS_Store -not -path "./.git/*" 2> /dev/null )

# C! -- Visual Studio's caches, only with --ide -- #

if [ "$ide" = 1 ]; then
    for vs in .vs/*/; do
        vs="${vs%/}"
        remove "$vs/FileContentIndex" "$vs"/v*/ipch "$vs"/v*/Browse.VC* "$vs"/v*/Solution.VC* "$vs"/v*/fileList*.bin
    done
fi

if [ "$dry_run" = 1 ]; then
    echo "clean: nothing removed (dry run)"
elif [ "$failed" = 1 ]; then
    echo "clean: some files couldn't be removed, see above" >&2
    exit 1
else
    echo "clean: done"
fi
