#pragma once

#include <coroutine>
#include <iterator>
#include <utility>

namespace coro {

template <typename Ref, typename Value = std::remove_cvref_t<Ref>>
class generator {
public:
    class promise_type {
    public:
        promise_type() : root_(this) {}



        // 处理未捕获异常
        void unhandled_exception() {
            if (exception_ == nullptr)
                throw;
            std::rethrow_exception(std::current_exception());
        }


        // 协程终止（无返回值）
        void return_void() noexcept {}

        // 初始挂起点
        std::suspend_always initial_suspend() noexcept { return {}; }

        // 最终挂起点
        struct final_awaiter {
            bool await_ready() noexcept {
                return false;
            }

            // 将控制权转移给父协程（如果存在）
            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                auto& promise = h.promise();
                auto parent = h.promise().parent_;

                if (parent) {
                    promise.root_->leaf_ = parent;
                    return std::noop_coroutine();
                }

                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };


        final_awaiter final_suspend() noexcept { return {}; }
        // 获取协程对象
        generator get_return_object() noexcept {
            return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        // 从协程中产生值
        std::suspend_always yield_value(Ref&& x) noexcept {
            root_->value_ = std::addressof(x);
            return {};
        }

        std::suspend_always yield_value(Ref& x) noexcept {
            root_->value_ = std::addressof(x);
            return {};
        }

        // 用于产生另一个generator的Awaitable对象
        struct yield_sequence_awaiter {
            generator gen_;
            std::exception_ptr exception_;

            explicit yield_sequence_awaiter(generator&& g) noexcept
                : gen_(std::move(g)) {}

            bool await_ready() noexcept {
                return !gen_.coro_;
            }

            // 将控制权转移到嵌套generator
            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                auto& current = h.promise();
                auto& nested = gen_.coro_.promise();
                auto& root = current.root_;

                nested.root_ = root;
                root->leaf_ = &nested;
                nested.parent_ = &current;
                nested.exception_ = &exception_;

                return gen_.coro_;
            }

            // 重新抛出在嵌套generator执行期间捕获的任何异常
            void await_resume() {
                if (exception_)
                    std::rethrow_exception(std::move(exception_));
            }
        };

        yield_sequence_awaiter yield_value(generator&& g) noexcept {
            return yield_sequence_awaiter{std::move(g)};
        }

        // 从父协程中恢复
        void resume() {
            std::coroutine_handle<promise_type>::from_promise(*leaf_).resume();
        }

        // 禁止await_transform
        void await_transform() = delete;

    private:
        friend generator;

        union {
            promise_type* root_;
            promise_type* leaf_;
        };

        promise_type* parent_ = nullptr;
        std::exception_ptr* exception_ = nullptr;
        std::add_pointer_t<Ref> value_;
    };

    // 默认构造函数
    generator() noexcept = default;

    // 移动构造函数
    generator(generator&& other) noexcept {
        // 使用 std::exchange 函数交换两个指针的值，并将原来的值返回
        coro_ = std::exchange(other.coro_, nullptr);
    }


    // 析构函数
    ~generator() noexcept {
    }

    // 用于迭代的结束标志
    struct sentinel {};

    // generator的迭代器
    class iterator {
        using coroutine_handle = std::coroutine_handle<promise_type>;
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Value;
        using reference = Ref;
        using pointer = std::add_pointer_t<Ref>;

        // 默认构造函数
        iterator() noexcept = default;

        // 移动构造函数
        iterator(iterator&& o) {
            std::swap(coro_, o.coro_);
        }

        // 移动赋值运算符
        iterator& operator=(iterator&& o) {
            coro_ = std::exchange(o.coro_, nullptr);
            return *this;
        }

        // 析构函数
        ~iterator() {}

        // 与结束标志比较相等性
        friend bool operator==(const iterator &it, sentinel) noexcept {
            return it.coro_ == nullptr || it.coro_.done();
        }


        // 与结束标志比较不等性
        friend bool operator!=(const iterator &it, sentinel) noexcept {
            return it.coro_ && !it.coro_.done();
        }


        // 前缀递增运算符
        iterator &operator++() {
            if (coro_) {
                coro_.promise().resume();
                if (coro_.done()) {
                    coro_ = nullptr;  // 当协程完成时，将 coro_ 设置为 nullptr，表示结束
                }
            }
            return *this;
        }


        // 后缀递增运算符
        iterator operator++(int) {
            iterator tmp(*this);  // 创建副本
            ++(*this);            // 递增原始对象
            return tmp;           // 返回副本
        }


        // 解引用运算符
        reference operator*() const noexcept {
            return *coro_.promise().value_;
        }

        // 用于引用类型的箭头运算符
        pointer operator->() const noexcept {
            return std::addressof(operator*());
        }

    private:
        friend generator;

        // 从协程句柄显式构造的迭代器
        explicit iterator(coroutine_handle coro) noexcept
            : coro_(coro) {}
        coroutine_handle coro_;
    };

    // 迭代的起始函数
    iterator begin() {
        if (coro_)
            coro_.resume();
        return iterator{coro_};
    }

    // 迭代的结束函数
    sentinel end() noexcept {
        return {};
    }

private:
    // 从协程句柄显式构造的generator
    explicit generator(std::coroutine_handle<promise_type> coro) noexcept
        : coro_(coro) {}

    std::coroutine_handle<promise_type> coro_;
};

}  // namespace coro
