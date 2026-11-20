#!/usr/bin/env bash
# C! packs res/ for git, which doesn't keep res/ itself (see .gitignore): zips every file in it into res-zipped.zip, and writes
# C! res-list.txt, every folder and file in it (nested ones too), one path per line. Both go in the project folder. It stages nothing
# C!   when res/ is there but empty, it deletes both; when there's no res/ folder at all, it leaves them alone
# C! usage: etc/res-pack.sh
# C!   the zip is only made again when a file in res/ has changed (zipping the same files twice doesn't give the same bytes)
# C!   works on Windows (Git Bash), OSX, and Linux
# C! exits 1 when res/ can't be zipped

set -u

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$script_dir"
while [ ! -d "$root/src" ] && [ "$root" != "/" ]; do root="$( dirname "$root" )"; done
cd "$root" || exit 2

if [ ! -d res ]; then
    # C! not there at all, which is also how a fresh clone starts (before etc/ult-pull.sh unpacks it), so the zip may be the only copy
    echo "no res/ folder, so res-zipped.zip and res-list.txt are left as they are"
    exit 0
fi

if [ -z "$( find res -type f | head -n 1 )" ]; then
    rm -f res-zipped.zip res-list.txt
    mkdir -p tmp && : > tmp/res-synced-list.txt # C! for etc/ult-pull.sh: res/ as it last matched git
    echo "res/ is empty, so res-zipped.zip and res-list.txt are deleted"
    exit 0
fi

# C! every folder (ending in /) and file in res/, by its path inside it, in order
( cd res && find . -mindepth 1 | while IFS= read -r p; do p="${p#./}"; if [ -d "$p" ]; then printf '%s/\n' "$p"; else printf '%s\n' "$p"; fi; done ) | LC_ALL=C sort > res-list.txt # C! no find -printf, which OSX doesn't have
mkdir -p tmp && cp res-list.txt tmp/res-synced-list.txt # C! for etc/ult-pull.sh: res/ as it last matched git
echo "listed res/ in res-list.txt ( $( grep -c -v '/$' res-list.txt ) files )"

# C! every file in the folder in_1, as its content hash and its path, one per line, in order
fingerprint( )
{
    ( cd "$1" && find . -type f | LC_ALL=C sort | while IFS= read -r p; do printf '%s %s\n' "$( git hash-object -- "$p" )" "$p"; done )
}

tmp="$( mktemp -d )"
trap 'rm -rf "$tmp"' EXIT
out="$tmp/res-zipped.zip"

# C! compared by each file's path and contents, since the zip's bytes change with the files' times
if [ -f res-zipped.zip ] && command -v unzip > /dev/null 2>&1; then
    mkdir -p "$tmp/unzipped"
    if unzip -q res-zipped.zip -d "$tmp/unzipped" 2> /dev/null && [ "$( fingerprint res )" = "$( fingerprint "$tmp/unzipped" )" ]; then
        echo "res/ hasn't changed since res-zipped.zip was made"
        exit 0
    fi
fi

if command -v zip > /dev/null 2>&1; then
    ( cd res && zip -q -r -X "$out" . ) || exit 1
elif [ -x /c/Windows/System32/tar.exe ]; then
    # C! Git Bash has no zip, but Windows' own tar (bsdtar) writes zips; it takes Windows paths
    ( cd res && find . -mindepth 1 -maxdepth 1 -printf '%P\0' | /c/Windows/System32/tar.exe --null -a -c -f "$( cygpath -w "$out" )" -T - ) || exit 1
elif tar --version 2> /dev/null | grep -q bsdtar; then
    ( cd res && find . -mindepth 1 -maxdepth 1 -print0 | tar --null -a -c -f "$out" -T - ) || exit 1
else
    echo "res-pack: nothing to zip res/ with (zip, or a bsdtar)" >&2
    exit 1
fi

cp "$out" res-zipped.zip
echo "zipped res/ into res-zipped.zip"
