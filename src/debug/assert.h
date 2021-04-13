#ifndef _DEBUG_ASSERT_H
#define _DEBUG_ASSERT_H

#ifdef DEBUG_ASSERT
extern void __assert(const char *expression, const char *file_name, const char *function_name,
        const char *message);

#define assert(expression, message) ({                              \
    if(expression)                                                  \
        ;                                                           \
    else                                                            \
        __assert(#expression, __FILE__,  __func__, message);        \
})
#else
#define assert(expression, message) ({ expression; })
#endif

#endif
