cd $( dirname $0 )

ls | grep -e \.h -e \.hpp -e \.cpp | xargs -I % sh -c 'echo clang-format -i % && clang-format -i %'
