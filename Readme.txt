//
//  Readme.txt
//  PicoDB
//
//  Created by Mahmoud Taabodi on 3/26/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//
install xcode and command line dools and skd from hard drive

install brew using this command
ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"



how to install valgrind.
1. brew install automake
2. go to source dir, then do ./autogen
3. then do ./configure
4. then make
5. make install

adding the thread pool
replace it with the one in the project
 its a bug for 1.55 version

 or then change the permission on this folder and file, and modify it according 
 to the git hub that removed all the =0 cases.


sudo chmod 777 /usr/local/include/boost/atomic/detail
sudo chmod 777 /usr/local/include/boost/atomic/detail/gcc-atomic.hpp
brew install boost

install boost using brew and link brew

command : brew install boost
command : brew link --overwrite boost







learn how to use cmake for this project




mongoclient
boost_filesystem
qpidmessaging
json
PocoNet
boost_thread-mt
PocoUtil
PocoXML
boost_system
PocoFoundation
boost_program_options
pthread

linking for xcode 
-lboost_filesystem -lboost_thread-mt -lboost_system -lboost_program_options -lboost_serialization -lboost_iostreams 

/usr/lib
/usr/local/lib

