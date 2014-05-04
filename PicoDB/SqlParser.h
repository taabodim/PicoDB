//
//  SqlParser.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/3/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_SqlParser_h
#define PicoDB_SqlParser_h
//#include "YourLexer.h"
//#include "YourParser.h"
//
//int main()
//{
//    
//    uint8_t * bufferData;     // Some memory with text in it
//    uint32_t bufferSize;      // Size of said memory
//    pANTLR3_UINT8 bufferName; // Name of buffer. ANTLR uses this for some default
//    // error messages
//    
//    //Creates an input stream. If you want to parse once from multiple sources
//    // you can switch among these during lexing
//    pANTLR3_INPUT_STREAM input = antlr3StringStreamNew(
//                                                       bufferData,
//                                                       ANTLR3_ENC_8BIT,
//                                                       bufferSize,
//                                                       bufferName);
//    assert(input != NULL);
//    
//    //Creates the lexer. Doesn't do anything until the parser(or you) tells it to.
//    pYourLexer lxr = YourLexerNew(input);
//    assert(lxr != NULL);
//    
//    //Creates an empty token stream.
//    pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(
//                                                                           ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
//    assert(tstream != NULL);
//    
//    //Creates a parser.
//    pYourParser psr = YourParserNew(tstream);
//    assert(psr != NULL);
//    
//    //Run the parser rule. This also runs the lexer to create the token stream.
//    psr->some_parser_rule(psr);


#endif
