#ifndef _DEBUG_ASSERT_H
#define _DEBUG_ASSERT_H

#ifdef DEBUG_ASSERT
extern void __assert(const char *expression, const char *file_name, const char *function_name,
        const char *message);

#define assert(Expression, Message) ({                                                      \
    if(Expression)                                                                          \
        ;                                                                                   \
    else                                                                                    \
        __assert(#Expression, __FILE__,  __func__, Message);                                \
})
#else
#define assert(Expression, Message) ({ Expression; })
#endif

#endif
