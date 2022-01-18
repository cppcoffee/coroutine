#ifndef COROUTINE_H
#define COROUTINE_H


#include <ucontext.h>


#define CO_OK       0
#define CO_ERROR    1


typedef void (*coroutine_pt)(void *ud);


typedef enum {
    COROUTINE_READY,
    COROUTINE_RUNNING,
    COROUTINE_SUSPEND,
    COROUTINE_DEAD,
} coroutine_status_e;


typedef struct {
    ucontext_t              main;
    ucontext_t              ctx;

    coroutine_pt            func;
    void                   *ud;

    void                   *stack;
    size_t                  stack_size;

    coroutine_status_e      status;

    int                     stack_id;

    unsigned                done:1;
    unsigned                recycle:1;
} coroutine_t;


coroutine_t *coroutine_create(coroutine_pt fn, void *ud, size_t stack_size);
void coroutine_destroy(coroutine_t *co);

int coroutine_yield(coroutine_t *co);
int coroutine_resume(coroutine_t *co);


#endif /* COROUTINE_H */
