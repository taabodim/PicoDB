//
//  ShellClient.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/29/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_ShellClient_h
#define PicoDB_ShellClient_h
#include <PonocoDriver.h>
namespace  pic  {
    
    class ShellClient{
    
        void startTheShell(std::shared_ptr<DriverType> ptr) {
            std::string quitCmd("quit");
            
            cout << "Wlecome to the Pico Shell  " << endl;
            std::string shellCommand; // declare one variable
            do {
                
                cin >> shellCommand;
                try{
                    //		queueType msg ( new pico_message(shellCommand));
                    //		ptr->queueCommand(msg);
                }catch(...)
                {
                    cout<<("error in parsing command");
                }
                
            } while (shellCommand.compare(quitCmd) != 0);

    };
    <#declarations#>
}

#endif
