cd $( dirname $0 )

ls | grep \.cpp | xargs -I % sh -c 'clang-format -i %'
ls | grep \.hpp | xargs -I % sh -c 'clang-format -i %'
ls | grep \.h   | xargs -I % sh -c 'clang-format -i %'

#exts=( cpp c h hpp )
#if [[ ${ #exts[ @ ] } -gt 0 ]]; then
#    if [[ ${ #exts[ @ ] } -gt 1 ]]; then
#        find . -type f $( for i in $( seq 1 $(( ${ #exts[ @ ] } - 1 ) ) ); do printf -- "-iname *.${ exts[ i ] } -o "; done) -iname "*.${ exts[ 0 ] }" | sort | cut -c3- | xargs -I % sh -c 'echo clang-format -i % && clang-format -i %'
#    else
#        find . -type f -iname "*.${ exts[ 0 ] }" | sort | cut -c3- | xargs -I % sh -c 'echo clang-format -i % && clang-format -i %'
#    fi
#fi
