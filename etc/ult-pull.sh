#!/usr/bin/env bash
# C! the ultimate pull: git pull, then unpacks res-zipped.zip into res/ when res/ doesn't match res-list.txt (the list changed in the pull,
# C! or res/ is missing or empty, as after a fresh clone), so res/ matches what was pulled. Files that were deleted are deleted from res/ too
# C!   when the pull deletes res-list.txt and res-zipped.zip (res/ was emptied, see etc/res-pack.sh), it empties res/ too
# C! usage: etc/ult-pull.sh [git pull options...]
# C!   res/ is left alone, with a warning, when it's changed since it last matched git (tmp/res-synced-list.txt): it has changes of its own,
# C!   which unpacking would lose (pack them with etc/res-pack.sh and commit them first)
# C!   works on Windows (Git Bash), OSX, and Linux

set -u

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$( git -C "$script_dir" rev-parse --show-toplevel 2> /dev/null )"
if [ -z "$root" ]; then
    echo "ult-pull: not in a git repository" >&2
    exit 2
fi
cd "$root" || exit 2

# C! every folder (ending in /) and file in res/, the same way etc/res-pack.sh writes res-list.txt
list_res( )
{
    [ -d res ] || return 0
    ( cd res && find . -mindepth 1 | while IFS= read -r p; do p="${p#./}"; if [ -d "$p" ]; then printf '%s/\n' "$p"; else printf '%s\n' "$p"; fi; done ) | LC_ALL=C sort
}

list_before=""
[ -f res-list.txt ] && list_before="$( cat res-list.txt )"

# C! res/ as it last matched git (when it was last unpacked or packed), kept outside git; res-list.txt when there isn't one
synced_file="tmp/res-synced-list.txt"
synced_list="$list_before"
[ -f "$synced_file" ] && synced_list="$( cat "$synced_file" )"

# C! remembers res/ as it is now as matching git
mark_synced( ) { mkdir -p tmp; list_res > "$synced_file"; }

res_empty=0
if [ ! -d res ] || [ -z "$( find res -type f | head -n 1 )" ]; then res_empty=1; fi

res_changed_here=0
if [ "$res_empty" = 0 ] && [ -n "$synced_list" ] && [ "$( list_res )" != "$synced_list" ]; then res_changed_here=1; fi # C! it's changed since it last matched git

git pull "$@" || { echo "ult-pull: git pull didn't go through, so res/ is left as it is" >&2; exit 1; }

echo
if [ ! -f res-zipped.zip ]; then
    if [ -n "$list_before" ] && [ ! -f res-list.txt ] && [ -d res ]; then
        # C! the pull deleted both, which etc/res-pack.sh does when res/ was emptied, so it's emptied here too
        if [ "$res_changed_here" = 1 ]; then
            echo "ult-pull: res/ was emptied in what was pulled, but it has changes of its own here, so it's left as it is" >&2
            exit 1
        fi
        find res -mindepth 1 -delete
        mark_synced
        echo "res/ was emptied in what was pulled, so it's emptied here too"
        exit 0
    fi
    echo "no res-zipped.zip, so res/ is left as it is"
    exit 0
fi

list_after=""
[ -f res-list.txt ] && list_after="$( cat res-list.txt )"

if [ "$res_empty" = 0 ] && [ "$( list_res )" = "$list_after" ]; then
    mark_synced
    echo "res/ already matches res-list.txt, so it's left as it is"
    exit 0
fi

if [ "$res_changed_here" = 1 ]; then
    echo "ult-pull: res/ has changes of its own (it doesn't match res-list.txt from before the pull), so it wasn't unpacked;" >&2
    echo "ult-pull: pack and commit them with etc/res-pack.sh first, or move res/ aside, then run this again" >&2
    exit 1
fi

# C! unpacked next to res/ first, so a failed unzip leaves the old res/ as it was
fresh="res.unpacking"
rm -rf "$fresh"
mkdir -p "$fresh"

if command -v unzip > /dev/null 2>&1; then
    unzip -q res-zipped.zip -d "$fresh" || { rm -rf "$fresh"; echo "ult-pull: res-zipped.zip couldn't be unzipped" >&2; exit 1; }
elif [ -x /c/Windows/System32/tar.exe ]; then
    /c/Windows/System32/tar.exe -x -f "$( cygpath -w "$root/res-zipped.zip" )" -C "$( cygpath -w "$root/$fresh" )" || { rm -rf "$fresh"; echo "ult-pull: res-zipped.zip couldn't be unzipped" >&2; exit 1; }
elif tar --version 2> /dev/null | grep -q bsdtar; then
    tar -x -f res-zipped.zip -C "$fresh" || { rm -rf "$fresh"; echo "ult-pull: res-zipped.zip couldn't be unzipped" >&2; exit 1; }
else
    rm -rf "$fresh"
    echo "ult-pull: nothing to unzip res-zipped.zip with (unzip, or a bsdtar)" >&2
    exit 1
fi

rm -rf res
mv "$fresh" res
mark_synced
echo "unpacked res-zipped.zip into res/ ( $( find res -type f | wc -l | tr -d ' ' ) files )"
