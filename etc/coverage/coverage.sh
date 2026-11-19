#!/usr/bin/env bash
# C! builds Axion with clang's source-based coverage, runs the Catch tests, and writes an HTML coverage report for the src files
# C! usage: etc/coverage/coverage.sh [--tests-only] [--open]
# C!   it runs the tests, then the game, so the report covers what you play too;
# C!   quit with Esc or by closing the window when done (the coverage data is only written when the game exits normally)
# C!   --tests-only skips the game: the program exits right after the tests
# C!   the report goes to tmp/coverage/index.html, replacing the last one (tmp/ is made if it doesn't exist, and is git-ignored)
# C!   --open opens the report when it's done
# C! the build itself goes to ~/AxionBuild/coverage, outside the repo (and outside OneDrive, see the build notes in .claude/CLAUDE.md)
# C! the report only covers the src files, not lib/ or the third-party Catch.hpp
# C! Windows (Git Bash): needs Visual Studio 2022 with its "C++ Clang tools" component (clang-cl, lld-link, llvm-profdata, llvm-cov)
# C! OSX: needs Xcode's command line tools; this path hasn't been tried yet

set -euo pipefail

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
root="$script_dir"
while [ ! -d "$root/src" ] && [ "$root" != "/" ]; do root="$( dirname "$root" )"; done
cd "$root"

open_report=0
play=1
for arg in "$@"; do
    case "$arg" in
        --open) open_report=1 ;;
        --tests-only) play=0 ;;
        *) echo "usage: etc/coverage/coverage.sh [--tests-only] [--open]" >&2; exit 2 ;;
    esac
done

report="$root/tmp/coverage"
build="$HOME/AxionBuild/coverage"
mkdir -p "$build/bin" "$build/obj" "$build/profile"
rm -f "$build/profile/"*.profraw

die( ) { echo "coverage: $*" >&2; exit 1; }

case "$( uname -s )" in
    MINGW*|MSYS*|CYGWIN*)
        # C! -- Windows: MSBuild with clang-cl and lld-link swapped in, plus coverage.props -- #
        vswhere="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
        vs=""
        if [ -x "$vswhere" ]; then vs="$( "$vswhere" -latest -products '*' -requires Microsoft.Component.MSBuild -property installationPath | tr -d '\r' )"; fi
        [ -n "$vs" ] || vs='C:\Program Files\Microsoft Visual Studio\2022\Community'
        vs_unix="$( cygpath -u "$vs" )"
        msbuild="$vs_unix/MSBuild/Current/Bin/MSBuild.exe"
        llvm_bin="$vs_unix/VC/Tools/Llvm/x64/bin"
        clang_ver="$( ls "$vs_unix/VC/Tools/Llvm/x64/lib/clang" | sort -n | tail -1 )"
        llvm_lib="$vs_unix/VC/Tools/Llvm/x64/lib/clang/$clang_ver/lib/windows"
        for tool in "$msbuild" "$llvm_bin/clang-cl.exe" "$llvm_bin/lld-link.exe" "$llvm_bin/llvm-profdata.exe" "$llvm_bin/llvm-cov.exe" "$llvm_lib/clang_rt.profile-x86_64.lib"; do
            [ -e "$tool" ] || die "missing $tool (install the C++ Clang tools component in the Visual Studio Installer)"
        done

        echo "building the coverage build into $build ..."
        "$msbuild" "$( cygpath -w "$root/Axion.vcxproj" )" -nologo -v:minimal -m \
            -p:Configuration=Debug -p:Platform=x64 \
            -p:CLToolExe=clang-cl.exe -p:CLToolPath="$( cygpath -w "$llvm_bin" )" \
            -p:LinkToolExe=lld-link.exe -p:LinkToolPath="$( cygpath -w "$llvm_bin" )" -p:LinkIncremental=false \
            -p:AxnCoveragePlay="$( [ "$play" = 1 ] && echo true || echo false )" \
            -p:ForceImportBeforeCppTargets="$( cygpath -w "$script_dir/coverage.props" )" \
            -p:AxnLlvmLib="$( cygpath -w "$llvm_lib" )" \
            -p:OutDir="$( cygpath -w "$build/bin" )\\" -p:IntDir="$( cygpath -w "$build/obj" )\\" \
            || die "the coverage build failed"
        exe="$build/bin/Axion.exe"
        profile_file="$( cygpath -w "$build/profile" )\\axion-%p.profraw"
        profdata( ) { "$llvm_bin/llvm-profdata.exe" "$@"; }
        llvm_cov( ) { "$llvm_bin/llvm-cov.exe" "$@"; }
        to_tool_path( ) { cygpath -w "$1"; }
        ;;
    Darwin)
        # C! -- OSX: build the sources directly with Apple clang (untested) -- #
        command -v xcrun > /dev/null || die "Xcode's command line tools are needed (xcode-select --install)"
        echo "building the coverage build into $build ..."
        objs=( )
        tests_only=( -DAXN_COVERAGE )
        if [ "$play" = 1 ]; then tests_only=( ); fi
        for f in $( find src -name '*.cpp' ! -name 'EngineSDL.cpp' | sort ); do
            o="$build/obj/$( echo "$f" | tr '/' '_' ).o"
            xcrun clang++ -std=c++20 -c -g -O0 -DAXN_DEBUG "${tests_only[@]}" -Isrc -include src/axn/Precompiled.hpp \
                -F/Library/Frameworks -fprofile-instr-generate -fcoverage-mapping -w "$f" -o "$o" || die "failed to compile $f"
            objs+=( "$o" )
        done
        xcrun clang++ -fprofile-instr-generate "${objs[@]}" -F/Library/Frameworks -framework SDL2 -framework OpenGL \
            -Wl,-rpath,/Library/Frameworks -o "$build/bin/Axion" || die "the coverage link failed"
        exe="$build/bin/Axion"
        profile_file="$build/profile/axion-%p.profraw"
        profdata( ) { xcrun llvm-profdata "$@"; }
        llvm_cov( ) { xcrun llvm-cov "$@"; }
        to_tool_path( ) { echo "$1"; }
        ;;
    *)
        die "unsupported system $( uname -s )"
        ;;
esac

# C! -- run the tests ( an AXN_COVERAGE build exits after them, which writes the coverage data ), and unless --tests-only the game too -- #

if [ "$play" = 1 ]; then
    echo "running the tests, then the game: play, then quit with Esc or by closing the window ..."
    ( cd "$root" && LLVM_PROFILE_FILE="$profile_file" "$exe" 2>&1 | tee "$build/tests.txt" | grep -E --line-buffered 'test cases|TESTS (PASSED|FAILED)' ) || true
else
    echo "running the tests ..."
    ( cd "$root" && LLVM_PROFILE_FILE="$profile_file" "$exe" > "$build/tests.txt" 2>&1 ) || true
    grep -E 'test cases|TESTS (PASSED|FAILED)' "$build/tests.txt" || true
fi
ls "$build/profile/"*.profraw > /dev/null 2>&1 || die "no coverage data was written, the game has to exit normally (Esc or closing the window), not be killed (see $build/tests.txt)"

# C! -- the report, src files only -- #

echo "writing the report ..."
raws=( )
for r in "$build/profile/"*.profraw; do raws+=( "$( to_tool_path "$r" )" ); done
profdata merge -sparse "${raws[@]}" -o "$( to_tool_path "$build/axion.profdata" )" || die "llvm-profdata failed"

rm -rf "$report"
mkdir -p "$report"
ignore='Catch\.hpp$'
llvm_cov show "$( to_tool_path "$exe" )" -instr-profile="$( to_tool_path "$build/axion.profdata" )" -format=html \
    -output-dir="$( to_tool_path "$report" )" -show-line-counts-or-regions -ignore-filename-regex="$ignore" \
    "$( to_tool_path "$root/src" )" || die "llvm-cov show failed"
llvm_cov report "$( to_tool_path "$exe" )" -instr-profile="$( to_tool_path "$build/axion.profdata" )" \
    -ignore-filename-regex="$ignore" "$( to_tool_path "$root/src" )" > "$report/summary.txt" || die "llvm-cov report failed"
cp "$build/tests.txt" "$report/tests.txt"

echo
tail -1 "$report/summary.txt"
echo "report: $report/index.html"
if [ "$open_report" = 1 ]; then
    case "$( uname -s )" in
        Darwin) open "$report/index.html" ;;
        *) start "" "$( to_tool_path "$report/index.html" )" ;;
    esac
fi
