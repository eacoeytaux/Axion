#ifndef OS_hpp
#define OS_hpp

#ifdef _WIN32
#define OS_WINDOWS
#elifdef __APPLE__
#define OS_APPLE
#elifdef __linux__
#define OS_LINUX
#else
#define OS_INVALID
#error "operating system not supported!"
#endif

#endif /* OS_hpp */
