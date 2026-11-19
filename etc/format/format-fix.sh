#!/usr/bin/env bash
# C! fixes the style guide violations in the src files that can be fixed safely, then lists what's left to fix by hand
# C! usage: etc/format/format-fix.sh [--clang-format] [src files...]
# C!   with no files it fixes every src file ( .cpp / .hpp / .h under src/, except the third-party Catch.hpp )
# C!   --clang-format first runs clang-format with format-clang.yaml over the files, then the style fixes;
# C!   clang-format can't follow the whole style guide, so this also changes correct code (see format-clang.yaml), review it with git diff
# C! it only ever changes src files, and keeps each file's line endings
# C! commit or stash first, so the changes are easy to review with git diff

set -u

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$script_dir"
while [ ! -d "$root/src" ] && [ "$root" != "/" ]; do root="$( dirname "$root" )"; done
cd "$root" || exit 2

if ! command -v perl > /dev/null 2>&1; then
    echo "format-fix: perl is needed (it comes with Git for Windows and with OSX)" >&2
    exit 2
fi

run_clang_format=0
if [ "${1:-}" = "--clang-format" ]; then run_clang_format=1; shift; fi

if [ "$run_clang_format" = 1 ]; then
    cf=""
    for c in clang-format "/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-format.exe" /opt/homebrew/bin/clang-format /usr/local/bin/clang-format; do
        if command -v "$c" > /dev/null 2>&1 || [ -x "$c" ]; then cf="$c"; break; fi
    done
    if [ -z "$cf" ]; then
        echo "format-fix: clang-format not found (on OSX: brew install clang-format)" >&2
        exit 2
    fi

    style="$script_dir/format-clang.yaml"
    # C! a Windows clang-format.exe needs Windows paths
    if command -v cygpath > /dev/null 2>&1; then style="$( cygpath -w "$style" )"; fi

    if [ $# -gt 0 ]; then files="$*"; else files="$( find src \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) ! -name 'Catch.hpp' | sort )"; fi
    for f in $files; do
        case "$f" in *Catch.hpp) continue ;; src/*.cpp|src/*.hpp|src/*.h) ;; *) echo "skipping $f: not a src file" >&2; continue ;; esac
        "$cf" -i --style=file:"$style" "$f"
    done
    echo "format-fix: ran clang-format"
fi

perl "$script_dir/format-rules.pl" fix "$@"
