#include <cassert>
#include <iostream>

#define CO_BEGIN switch (started) { case 0:
#define CO_END   }
#define CO_YIELD(...) do { started = __LINE__; return __VA_ARGS__; case __LINE__:; } while (0)
#define CO_RETURN(...) do { started = -1; return __VA_ARGS__; } while (0)

struct coroutine_base {
    int started = 0;
};

class fib : public coroutine_base {
private:
    int a = 0, b = 1;

public:
    int operator()() {
        CO_BEGIN
        while (true) {
            CO_YIELD(a);
            int tmp = a;
            a = b;
            b = tmp + b;
        }
        CO_END
        return -1;
    }
};


int main() {
    int ans[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    fib foo;
    for (int i = 0; i < 10; i++)
        assert(foo() == ans[i]);
    std::cout << "libco_v3 test passed!" << std::endl;
}
