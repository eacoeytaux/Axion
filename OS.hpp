#ifndef OS_hpp
#define OS_hpp

#if defined ( _WIN32 )
#define OS_WINDOWS
#elif defined ( __APPLE__ )
#define OS_APPLE
#elif defined ( __linux__ )
#define OS_LINUX
#else
#define OS_INVALID
#error "operating system not supported!"
#endif

#endif /* OS_hpp */
