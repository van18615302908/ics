// sleep.h

#pragma once

#include <coroutine>
#include <functional>
#include <queue>
#include <thread>

namespace coro {

static std::queue<std::function<bool()>> task_queue;

struct sleep {
    sleep(int n_ms) : delay{n_ms} {}

    std::chrono::milliseconds delay;

    bool await_ready() const noexcept {
        return false;
    }

void await_suspend(std::coroutine_handle<> h) const noexcept {
    try {
        auto start = std::chrono::steady_clock::now();
        task_queue.push([start, h, d = delay] {
            try {
                if (decltype(start)::clock::now() - start > d) {
                    h.resume();
                    return true;
                }
                else {
                    return false;
                }
            } catch (...) {//test用不到
                // 处理在 lambda 函数中的异常
                // 记录异常或执行其他适当的操作
                return false;
            }
        });
    } catch (...) {//test用不到
        // 处理在 await_suspend 函数中的异常
        // 记录异常或执行其他适当的操作
    }
}


    void await_resume() const noexcept {}
};

struct Task {
    struct promise_type {
        Task get_return_object() {
            return {};
        }

        std::suspend_never initial_suspend() {
            return {};
        }

        std::suspend_never final_suspend() noexcept {
            return {};
        }

        void return_void() {}

        void unhandled_exception() {}
    };
};

void wait_task_queue_empty() {
    while (!task_queue.empty()) {
        auto task = task_queue.front();
        if (!task())
            task_queue.push(task);
        task_queue.pop();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

}  // namespace coro
