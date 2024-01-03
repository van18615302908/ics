#pragma once

#include <coroutine>
#include <iterator>
#include <utility>
#include <optional>

namespace coro {

template <typename Ref, typename Value = std::remove_cvref_t<Ref>>
class generator {
public:
    struct promise_type {
        std::optional<Value> current_value;
        std::suspend_always yield_value(Value value) {
            current_value = std::move(value);
            return {};
        }
        generator get_return_object() {
            return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Value;
        using reference         = Ref;
        using pointer           = std::add_pointer_t<Ref>;

        explicit iterator(std::coroutine_handle<promise_type> coro = nullptr) : coro_(coro) {}

        iterator& operator++() {
            coro_.resume();
            if (coro_.done()) coro_ = nullptr;
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const iterator& other) const {
            return coro_ == other.coro_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return *coro_.promise().current_value;
        }

        pointer operator->() const {
            return &(operator*());
        }

    private:
        std::coroutine_handle<promise_type> coro_;
    };

    iterator begin() {
        if (coro_) coro_.resume();
        return coro_.done() ? end() : iterator{coro_};
    }

    iterator end() {
        return {};
    }

    generator(generator&& other) : coro_(other.coro_) {
        other.coro_ = nullptr;
    }

    generator& operator=(generator&& other) {
        if (this != &other) {
            coro_ = other.coro_;
            other.coro_ = nullptr;
        }
        return *this;
    }

    ~generator() {
        if (coro_) coro_.destroy();
    }

private:
    explicit generator(std::coroutine_handle<promise_type> coro) : coro_(coro) {}

    std::coroutine_handle<promise_type> coro_;
};

}  // namespace coro
