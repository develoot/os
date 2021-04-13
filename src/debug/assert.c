#include <kernel/print.h>

#ifdef DEBUG_ASSERT
void __assert(const char *expression, const char *file_name, const char *function_name,
        const char *message)
{
    print_format("Assert:\"%s\",File:%s,Func:%s,%s", expression, file_name, function_name, message);
    while (1) {}
}
#endif
