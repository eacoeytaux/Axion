#!/usr/bin/env bash
# C! checks the src files against the style guide in .claude/CLAUDE.md and lists every violation, changing nothing
# C! usage: etc/format/format-check.sh [--overrides] [src files...]
# C!   with no files it checks every src file ( .cpp / .hpp / .h under src/, except the third-party Catch.hpp ),
# C!   and reports any comments in the Visual Studio and Xcode project setting files
# C!   --overrides also compiles each .cpp with clang to find functions that override without saying override
# C! exits 1 when it finds violations, 0 when the code is clean

set -u

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$script_dir"
while [ ! -d "$root/src" ] && [ "$root" != "/" ]; do root="$( dirname "$root" )"; done
cd "$root" || exit 2

if ! command -v perl > /dev/null 2>&1; then
    echo "format-check: perl is needed (it comes with Git for Windows and with OSX)" >&2
    exit 2
fi

overrides=0
if [ "${1:-}" = "--overrides" ]; then overrides=1; shift; fi

perl "$script_dir/format-rules.pl" check "$@"
status=$?

if [ "$overrides" = 1 ]; then
    echo
    echo "checking for overrides missing 'override' (compiling with clang)..."
    clang_cl="/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe"
    all=""
    for f in $( find src -name '*.cpp' | sort ); do
        case "$f" in *EngineSDL.cpp) continue ;; esac
        if [ -x "$clang_cl" ]; then
            out="$( "$clang_cl" /nologo /std:c++20 /EHsc /Zs /DAXN_DEBUG /D_DEBUG /D_CONSOLE /Isrc /Ilib/win/glfw/include /FIaxn/Precompiled.hpp -Wno-everything -Wsuggest-override -Winconsistent-missing-override "$f" 2>&1 )"
        elif command -v clang++ > /dev/null 2>&1; then
            out="$( clang++ -std=c++20 -fsyntax-only -DAXN_DEBUG -Isrc -include src/axn/Precompiled.hpp -Wno-everything -Wsuggest-override -Winconsistent-missing-override "$f" 2>&1 )"
        else
            echo "format-check: no clang found, skipping the override check" >&2
            break
        fi
        all="$all
$( echo "$out" | grep -E "overrides a member function but is not marked 'override'" )"
    done
    missing="$( echo "$all" | grep -v '^$' | sort -u )"
    if [ -n "$missing" ]; then
        echo "$missing"
        echo "$( echo "$missing" | wc -l | tr -d ' ' ) override(s) missing 'override'"
        status=1
    else
        echo "no missing overrides found"
    fi
fi

exit $status
