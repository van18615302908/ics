#pragma once
#include <cassert>

#include <cstdlib>

#include <vector>

#include "coro_ctx.h"

namespace coro {

struct share_stack;

struct coroutine_attr {

  int stack_size = 128 * 1024;

  share_stack* sstack = nullptr;

};

struct coroutine;

coroutine *create(func_t coro_func, void *arg, const coroutine_attr *attr = nullptr);

void release(coroutine *co);

int resume(coroutine *co, int param = 0);

int yield(int ret = 0);



struct stack_mem {//管理栈内存结构体

  coroutine* last_user = nullptr;

  size_t stack_size = 0;

  bool in_use = false;

  char *stack_start = nullptr; // 栈的起始地址

  char *stack_end = nullptr;

  stack_mem(size_t size) : stack_size(size) {

    stack_start =new char[size];

  }

  ~stack_mem() {

    free(stack_start);

  }

};


struct share_stack {

  stack_mem** stack_array;//指向栈内存块的指针数组

  int count = 0;//共享栈大小

  size_t stack_size = 0;//每个栈的大小

  int position = 0;//如果共享栈全部被占用，需要进行保存、覆盖操作的计数器


  share_stack(int count, size_t stack_size) : count(count), stack_size(stack_size) {

    stack_array = new stack_mem*[count];

    for (int i = 0; i < count; ++i) {

      stack_array[i] = new stack_mem(stack_size); 

    }

  }


  ~share_stack() {

    for (int i = 0; i < count; ++i) {
      delete stack_array[i];
    }

    delete[] stack_array;

  }



  stack_mem* get_stackmem() {
    for (int i = 0; i < count; ++i) {
      if (!stack_array[i]->in_use) {

        stack_array[i]->in_use = true;
        return stack_array[i]; 

      }
    }
    position = (++position)%count;

    return stack_array[position];
  }

};




struct coroutine {
  bool started = false; 
  bool end = false;
  int data;
  bool is_shared = false;
  func_t coro_func = nullptr;
  void* arg = nullptr;



  context ctx; 
  stack_mem * stack;//在内存上运行时候的信息
  size_t stack_size = 0;
  char *stack_sp = nullptr;


//在共享栈被清走时，备份的在共享栈上的内存信息
  char *stack_mem_sp = nullptr;
  size_t stack_mem_size = 0;

  coroutine(func_t func=nullptr, void *args =nullptr) : coro_func(func), arg(args){}

};



class coroutine_env {

public:
  std::vector<coroutine*> coroutines;

  coroutine_env() {
    coroutine* main = new coroutine(); // Main coroutine
    coroutines.push_back(main);
  }

  ~coroutine_env() {
  }


  void push(coroutine* co) {

    coroutines.push_back(co);

  }


  void pop() {

    coroutines.pop_back();

  }

  coroutine* get_coro(int idx) {

    return coroutines[idx];

  }

};





}  // namespace coro


