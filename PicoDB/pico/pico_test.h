//
//  pico_test.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/17/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_pico_test_h
#define PicoDB_pico_test_h
 namespace pico
{
    class pico_test{
    public:
        
    static string smallKey0 ;
    static string smallKey1 ;
    static string smallKey2 ;
    static string smallKey3 ;
    static string smallValue0;
    static string smallValue1;
    static string smallValue2;
    static string smallValue3;
    static string bigValue0;
    static string bigValue1;
    
//        void test_reading_from_collection()
//        {
//            std::ifstream is (filename, std::ifstream::binary);
//            if (is) {
//                // get length of file:
//                is.seekg (0, is.end);
//                size_t length = is.tellg();
//                is.seekg (0, is.beg);
//                
//                char * buffer = new char [length];
//                
//                std::cout << "Reading " << length << " characters... ";
//                // read data as a block:
//                is.read (buffer,length);
//                
//                if (is)
//                    std::cout << "all characters read successfully.";
//                else
//                    std::cout << "error: only " << is.gcount() << " could be read";
//                is.close();
//                
//                // ...buffer contains the entire file...
//                
//                delete[] buffer;
//            }
//        }

    };
}


#endif
