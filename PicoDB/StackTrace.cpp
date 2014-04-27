//
//  StackTrace.cpp
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/26/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#include "StackTrace.h"

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <cxxabi.h>

struct empty { };

template <typename T, int N>
struct bar { };


int main()
{
    int     status;
    char   *realname="hjjjhjh";
    
    // exception classes not in <stdexcept>, thrown by the implementation
    // instead of the user
    std::bad_exception  e;
    realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
    //std::cout << e.what() << "\t=> " << realname << "\t: " << status << '\n';
    free(realname);
    
    
    // typeid
    bar<empty,17>          u;
    const std::type_info  &ti = typeid(u);
    
    realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
    free(realname);
     //
    return 0;
}

void handler(int sig) {
    void *array[10];
    size_t size;
    
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);
    
    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

void baz() {
    int *foo = (int*)-1; // make a bad pointer
    //printf("%d\n", *foo);
    // causes segfault
    handler(11);
}

//void bar() { baz(); }
//void foo() { bar(); }


int main3(int argc, char **argv) {
    signal(SIGSEGV, handler);   // install our handler
  //  foo(); // this will call foo, bar, and baz.  baz segfaults.
    return 1;
}