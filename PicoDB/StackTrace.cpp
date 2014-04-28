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


//
//int main()
//{
//    int     status;
//    char   *realname="hjjjhjh";
//    
//    // exception classes not in <stdexcept>, thrown by the implementation
//    // instead of the user
//    std::bad_exception  e;
//    realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
//    //std::cout << e.what() << "\t=> " << realname << "\t: " << status << '\n';
//    free(realname);
//    
//    
//    // typeid
//    bar<empty,17>          u;
//    const std::type_info  &ti = typeid(u);
//    
//    realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
//    std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
//    free(realname);
//     //
//    return 0;
//}

