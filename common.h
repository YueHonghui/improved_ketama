#ifndef COMMON_H_
#define COMMON_H_
#include <stdlib.h>

#define COUNT_OF(array) (sizeof(array)/sizeof((array)[0]))
#define ATTR_PRINTF(n, m) __attribute__((format(printf, n, m)))
#define UNUSED(a) ((void)(a));

#define MIN(x, y) ({							\
			typeof(x) min1__ = (x);					\
			typeof(y) min2__ = (y);					\
			(void) (&min1__ == &min2__);			\
			min1__ < min2__ ? min1__ : min2__; })

#define MAX(x, y) ({							\
			typeof(x) max1__ = (x);					\
			typeof(y) max2__ = (y);					\
			(void) (&max1__ == &max2__);			\
			max1__ > max2__ ? max1__ : max2__; })

#ifndef container_of
#define container_of(ptr, type, member) ({			\
		(type *)( (char *)ptr - offsetof(type,member) );})
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef likely
#ifdef __builtin_expect
#define likely(x) __builtin_expect((x),1)
#else
#define likely(x) (x)
#endif
#endif

#ifndef unlikely
#ifdef __builtin_expect
#define unlikely(x) __builtin_expect((x),0)
#else
#define unlikely(x) (x)
#endif
#endif

#define ASSERT(exp)\
    do{\
        if(unlikely(0 == (exp))){\
            abort();\
        }\
    }while(0);

#ifdef __cplusplus
extern "C"
{
#endif

typedef char boolean;
typedef struct s_keyvalue
{
    const char* key;
    const char* value;
}s_keyvalue;

#ifdef __cplusplus
}
#endif

#endif
