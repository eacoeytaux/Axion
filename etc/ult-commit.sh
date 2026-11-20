#!/usr/bin/env bash
# C! the ultimate commit: packs res/ with etc/res-pack.sh (res-zipped.zip and res-list.txt, staged), fixes the style of the src files with
# C! etc/format/format-fix.sh, stops if anything is left to fix by hand, then commits what's staged with the message given
# C! usage: etc/ult-commit.sh "commit message" [more git commit options...]
# C!   files that were fully staged get the style fixes staged along with them. A file that was only partly staged (git add -p)
# C!   and that the fixes change stops the commit instead, since staging the fixes would stage the rest of the file too
# C!   works on Windows (Git Bash), OSX, and Linux

set -u

if [ $# -lt 1 ] || [ -z "${1// /}" ]; then
    echo "usage: etc/ult-commit.sh \"commit message\" [more git commit options...]" >&2
    exit 2
fi

message="$1"
shift

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$( git -C "$script_dir" rev-parse --show-toplevel 2> /dev/null )"
if [ -z "$root" ]; then
    echo "ult-commit: not in a git repository" >&2
    exit 2
fi
cd "$root" || exit 2

# C! res/ isn't in git: res-pack.sh zips it into res-zipped.zip and lists it in res-list.txt, which are staged with the rest
bash "$script_dir/res-pack.sh" || { echo "ult-commit: res/ couldn't be packed, so nothing was committed" >&2; exit 1; }
for f in res-zipped.zip res-list.txt; do
    if [ -f "$f" ] || git ls-files --error-unmatch -- "$f" > /dev/null 2>&1; then git add -A -- "$f"; fi # C! stages a deletion too, when res-pack.sh deleted it
done
echo

staged="$( git diff --cached --name-only )"
if [ -z "$staged" ]; then
    echo "ult-commit: nothing is staged, stage what to commit with git add first" >&2
    exit 1
fi

unstaged_before="$( git diff --name-only )" # C! files with changes that aren't staged, before the fixes

echo "fixing the style ..."
bash "$script_dir/format/format-fix.sh"

echo
echo "checking the style ..."
if ! bash "$script_dir/format/format-check.sh" > /dev/null; then
    bash "$script_dir/format/format-check.sh"
    echo
    echo "ult-commit: style violations are left to fix by hand, so nothing was committed" >&2
    exit 1
fi
echo "no style violations found"

# C! the fixes changed files in the working tree only: stage them in the files that are being committed
restage=""
while IFS= read -r f; do
    [ -z "$f" ] && continue
    [ -f "$f" ] || continue
    git diff --quiet -- "$f" && continue # C! the fixes didn't change it
    if printf '%s\n' "$unstaged_before" | grep -qxF -- "$f"; then
        echo "ult-commit: $f is only partly staged and the style fixes changed it; stage it as it should be and run this again" >&2
        exit 1
    fi
    restage="$restage$f"$'\n'
done <<< "$staged"

if [ -n "$restage" ]; then
    while IFS= read -r f; do
        [ -n "$f" ] && git add -- "$f"
    done <<< "$restage"
    echo
    echo "staged the style fixes in:"
    printf '%s' "$restage" | sed 's/^/  /'
fi

echo
git commit -m "$message" "$@"
