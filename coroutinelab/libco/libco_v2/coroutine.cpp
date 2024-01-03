
#include "coroutine.h"

#include <cstring>
#include <functional>

namespace coro {

static coroutine_env g_coro_env;

extern "C" {
extern void coro_ctx_swap(context*, context*) asm("coro_ctx_swap");
};


coroutine *create(func_t coro_func, void *arg, const coroutine_attr *attr)
{
    coroutine_attr at;
    if (attr != nullptr)
    {
        at = *attr;
    }

    
    stack_mem *main_stack_mem = nullptr;
    coroutine *co = new coroutine(coro_func, arg);

    if (at.sstack != nullptr)//采用共享栈
    {
        main_stack_mem = at.sstack->get_stackmem();
        co->is_shared = true;

        //at.stack_size = at.sstack->stack_size;
    }else{//不采用共享栈
        main_stack_mem = new stack_mem(8 * 1024);}
        co->stack = main_stack_mem;
        co->ctx.sstack_rsp = main_stack_mem->stack_start;
        co->ctx.sstack_size = main_stack_mem->stack_size;

        return co;
    
}


void release(coroutine *co)
{
    // TODO: implement your code here
    if (!co->is_shared)
    {
        free(co->stack->stack_start);
        free(co->stack);
    }
    else
    {
        if (co->stack_mem_sp)
            free(co->stack_mem_sp);

        if (co->stack->last_user == co)
            co->stack->last_user = nullptr;
    }
    free(co);
}


void save_stack(coroutine* co) {//旧的栈被清走的时候，保存旧的结构体的信息

    int len = co->stack->stack_start +co->stack->stack_size - co->stack_sp;
    if(co-> stack_mem_sp ){
        free(co-> stack_mem_sp);
        co->stack_mem_sp = nullptr;
    }
    co->stack_mem_sp = (char *)malloc(len);
    co->stack_mem_size = len;
    memcpy(co->stack_mem_sp,co->stack_sp, len);
}


void swap(coroutine *caller, coroutine *callee)
{

    char c;
    caller->stack_sp = &c;

    if (callee->is_shared)
    {

        coroutine *occupy = callee->stack->last_user;
        callee->stack->last_user = callee;

        // last_member = occupy;
        if (occupy && occupy != callee)
        {
            save_stack(occupy);
            memcpy( callee->stack_sp,  callee->stack_mem_sp,  callee->stack_mem_size);
        }
    }
    coro_ctx_swap(&(caller->ctx), &(callee->ctx));
    
}

static void func_wrap(coroutine* co) {
    if (co->coro_func) {
        co->coro_func(co->arg);
    }
    co->end = true;
    yield(-1);
}

int resume(coroutine* co, int param) { 
    if(co->started == false){    
        co->started = true;
        ctx_make(&co->ctx, (func_t)func_wrap, co);
    }
    coroutine* cur = g_coro_env.get_coro(g_coro_env.coroutines.size()-1);
    g_coro_env.push(co); // 将协程推入栈中
    co->data = param;
    swap(cur, co);  
    return co->data; // 返回yield时的数据
}


int yield(int ret ) {
    coroutine* cur = g_coro_env.get_coro(g_coro_env.coroutines.size()-1); // 获取当前协程
    coroutine* last = g_coro_env.get_coro(g_coro_env.coroutines.size()-2);
    cur->data = ret; // 保存返回值
    g_coro_env.pop(); // 弹出栈顶协程    
    swap(cur, last); // 切换到调用者
    return  cur->data; // 返回resume时传入的数据
}

}
// namespace coro
