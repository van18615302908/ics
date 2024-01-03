// //coro_ctx.h
// #pragma once

// #include <cstdlib>
// #include <cstdint>

// namespace coro {

// using func_t = void (*)(void *);

// struct context {
//     uintptr_t rsp;
//     uintptr_t rip;
//     uintptr_t rbx;
//     uintptr_t rbp;
//     uintptr_t r12;
//     uintptr_t r13;
//     uintptr_t r14;
//     uintptr_t r15;
// };

// void ctx_make(context *ctx, func_t coro_func, const void *arg);

// } // namespace coro

#pragma once


#include <stdlib.h>

namespace coro

{


    using func_t = void (*)(void *);



    struct context{
        void *regs[14];
        size_t sstack_size;
        char *sstack_rsp;
    };



    void ctx_make(context *ctx, func_t coro_func, const void *arg);



} // namespace coro