// Derived from code with this license:
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_GETTID
#include <unistd.h>
#else

#ifdef _WIN32

#    include <processthreadsapi.h>

#else // unix

#    ifdef __linux__
#        include <sys/syscall.h> //Use gettid() syscall under linux to get thread id

#    elif defined(_AIX)
#        include <pthread.h> // for pthread_getthrds_np

#    elif defined(__DragonFly__) || defined(__FreeBSD__)
#        include <pthread_np.h> // for pthread_getthreadid_np

#    elif defined(__NetBSD__)
#        include <lwp.h> // for _lwp_self

#    elif defined(__sun)
#        include <thread.h> // for thr_self
#    endif

#endif // unix



#undef gettid

// Return current thread id as pid_t
static size_t gettid()
{
#ifdef _WIN32
    return (size_t)(GetCurrentThreadId());
#elif defined(__linux__)
#    if defined(__NR_gettid)
#        define SYS_gettid __NR_gettid
#    endif
    return (size_t)(syscall(SYS_gettid));
#elif defined(_AIX)
    struct __pthrdsinfo buf;
    int reg_size = 0;
    pthread_t pt = pthread_self();
    int retval = pthread_getthrds_np(&pt, PTHRDSINFO_QUERY_TID, &buf, sizeof(buf), NULL, &reg_size);
    int tid = (!retval) ? buf.__pi_tid : 0;
    return (size_t)(tid);
#elif defined(__DragonFly__) || defined(__FreeBSD__)
    return (size_t)(pthread_getthreadid_np());
#elif defined(__NetBSD__)
    return (size_t)(_lwp_self());
#elif defined(__OpenBSD__)
    return (size_t)(getthrid());
#elif defined(__sun)
    return (size_t)(thr_self());
#elif __APPLE__
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return (size_t)(tid);
#else
    // Either avoid usage of it through this macro or port this to the new platform
#define YALIBCT_LIBC_GETTID_NOT_USABLE
    abort(); //static_cast<size_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}

#endif
