#include "coroutine.h"

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <valgrind/valgrind.h>


static void coroutine_mainfunc(void *data);


coroutine_t *
coroutine_create(coroutine_pt fn, void *ud, size_t stack_size)
{
    coroutine_t  *co;
    size_t        size;

    if (stack_size == 0) {
        stack_size = SIGSTKSZ;
    }

    size = sizeof(*co) + stack_size;

    co = malloc(size);
    if (co == NULL) {
        return NULL;
    }

    memset(co, 0, sizeof(*co));

    co->func = fn;
    co->ud = ud;

    co->stack = co + 1;
    co->stack_size = stack_size;

    co->status = COROUTINE_READY;

    co->stack_id = VALGRIND_STACK_REGISTER(co, (void *) co + size);

    return co;
}


void
coroutine_destroy(coroutine_t *co)
{
    if (co == NULL) {
        return;
    }

    VALGRIND_STACK_DEREGISTER(co->stack_id);

    free(co);
}


int
coroutine_resume(coroutine_t *co)
{
    switch (co->status) {
    case COROUTINE_READY:
        if (getcontext(&co->ctx) == -1) {
            return CO_ERROR;
        }

        co->status = COROUTINE_RUNNING;

        co->ctx.uc_stack.ss_sp = co->stack;
        co->ctx.uc_stack.ss_size = co->stack_size;
        co->ctx.uc_stack.ss_flags = 0;
        co->ctx.uc_link = &co->main;

        makecontext(&co->ctx, (void (*)(void)) coroutine_mainfunc, 1, co);

        if (swapcontext(&co->main, &co->ctx) == -1) {
            return CO_ERROR;
        }

        break;

    case COROUTINE_SUSPEND:
        co->status = COROUTINE_RUNNING;

        if (swapcontext(&co->main, &co->ctx) == -1) {
            return CO_ERROR;
        }

        break;

    default:
        /* unreachable */
        assert(0);
    }


    if (co->done) {
        coroutine_destroy(co);
    }

    return CO_OK;
}


int
coroutine_yield(coroutine_t *co)
{
    co->status = COROUTINE_SUSPEND;

    if (swapcontext(&co->ctx, &co->main) == -1) {
        return CO_ERROR;
    }

    return CO_OK;
}


static void
coroutine_mainfunc(void *data)
{
    coroutine_t  *co = data;

    co->func(co->ud);

    co->done = 1;
}
