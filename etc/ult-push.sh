#!/usr/bin/env bash
# C! the ultimate push: etc/ult-commit.sh (packs res/, fixes the style, commits), then git push, only when the commit went through
# C! usage: etc/ult-push.sh "commit message" [more git commit options...]
# C!   pushes the current branch to where it's set to push, like a plain git push
# C!   works on Windows (Git Bash), OSX, and Linux

set -u

if [ $# -lt 1 ] || [ -z "${1// /}" ]; then
    echo "usage: etc/ult-push.sh \"commit message\" [more git commit options...]" >&2
    exit 2
fi

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

bash "$script_dir/ult-commit.sh" "$@" || { echo "ult-push: the commit didn't go through, so nothing was pushed" >&2; exit 1; }

echo
echo "pushing ..."
git -C "$script_dir" push
