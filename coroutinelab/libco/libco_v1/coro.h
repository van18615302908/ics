#pragma once

#include <ucontext.h>
#include <cassert>
#include <stack>
#include <vector>

namespace coro {

class coroutine;
using func_t = void (*)(void*);

coroutine* create(func_t func, void* args);
void release(coroutine* co);
int resume(coroutine* co, int param = 0);
int yield(int ret = 0);

struct coroutine {
    bool started = false;
    bool end = false;
    int data;
    func_t coro_func = nullptr;
    void* args = nullptr;

    ucontext_t ctx;
    char stack[8192];  // 设置一个足够大的栈空间

    coroutine(func_t func, void* args) : coro_func(func), args(args) {
        
        ctx.uc_stack.ss_sp = stack;
        ctx.uc_stack.ss_size = sizeof(stack);
        ctx.uc_link = nullptr;
    }

    ~coroutine() {
        // 析构函数中暂时不需要特别操作
    }
};

class coroutine_env {
private:
    std::vector<coroutine*> coro_stack;
    coroutine* main_coro;

public:
    coroutine_env() {
        main_coro = new coroutine(nullptr, nullptr); // 创建主协程
        coro_stack.push_back(main_coro);
    }

    ~coroutine_env() {
        delete main_coro; // 清理主协程
    }

    coroutine* get_coro(int idx) {
        return coro_stack[idx];

    }


    void push(coroutine* co) {
        coro_stack.push_back(co);
    }

    void pop() {
    if (!coro_stack.empty()) {
        coro_stack.pop_back();
        }
    }
};

}  // namespace coro
