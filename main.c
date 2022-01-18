#include "coroutine.h"
#include <stdio.h>


static void
foo(void *data)
{
    const char *name = data;

    printf("hello %s\n", name);
}


int
main(void)
{
    coroutine_t *co;

    co = coroutine_create(foo, "coroutine", 0);
    coroutine_resume(co);
    coroutine_destroy(co);

    return 0;
}
