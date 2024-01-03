#include "coro_ctx.h"


#include <cstdint>

#include <cstring>



constexpr int kRDI = 7;

constexpr int kRSI = 8;

constexpr int kRCX = 11;

constexpr int kRETURN = 9;

constexpr int kRSP = 13;


namespace coro

{



    void ctx_make(context *ctx, func_t coro_func, const void *arg)

    {

        char *rsp = ctx->sstack_rsp + ctx->sstack_size - 8;//栈顶指针下移，防止返回地址覆盖


        memset(ctx->regs, 0, sizeof(ctx->regs));

        void **return_address = (void **)(rsp);

        *return_address = (void *)coro_func;


        ctx->regs[kRSP] = (char *)rsp;
//104


        ctx->regs[kRETURN] = (char *)coro_func;
//72


        ctx->regs[kRDI] = (char *)arg;

    }

} // namespace coro

