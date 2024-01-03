#include "coro.h"

namespace coro {

static coroutine_env g_coro_env;

static void func_wrap(coroutine* co) {
    if (co->coro_func) {
        co->coro_func(co->args);
    }
    co->end = true;
    yield(-1);
}

coroutine* create(func_t func, void* args) {
    coroutine* co = new coroutine(func, args);
    getcontext(&(co->ctx));
    return co;
}

void release(coroutine* co) {
    delete co;
}


int resume(coroutine* co, int param) { 
    co->data = param; // 设置传入参数
    if(co->started == false){    
        co->started = true;
        makecontext(&(co->ctx), (void(*)())func_wrap, 1, co);
    }

    g_coro_env.push(co); // 将协程推入栈中
    swapcontext(&(g_coro_env.get_coro(0)->ctx), &(co->ctx));
    
    return g_coro_env.get_coro(0)->data; // 返回yield时的数据
}


int yield(int ret ) {
    coroutine* current = g_coro_env.get_coro(1); // 获取当前协程
    int par = current->data;
    g_coro_env.get_coro(0)->data = ret; // 保存返回值
    g_coro_env.pop(); // 弹出栈顶协程    
    swapcontext(&(current->ctx), &(g_coro_env.get_coro(0)->ctx)); // 切换到调用者
    return par; // 返回resume时传入的数据
}


}  // namespace coro
