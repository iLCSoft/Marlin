// -*- mode: c++;
#ifndef streamlog_h
#define streamlog_h

  /**
   *  Main header file for the streamlog library. 
   *  Includes necessary header files and defines macros <br>
   *      streamlog_out( MLEVEL )  <br>
   *      streamlog_message( MLEVEL , CODE_BLOCK , OUT_MESSAGE) <br>
   * 
   *  @author F. Gaede, DESY
   *  @version $Id: streamlog.h,v 1.3 2007-09-12 11:59:53 gaede Exp $
   */

#ifndef DONT_USE_STREAMLOG
#define USE_STREAMLOG 
#endif

#ifdef  USE_STREAMLOG

#include "logstream.h"
#include "logscope.h"
#include "loglevels.h"


#define  streamlog_out( MLEVEL ) if(streamlog::out.write< streamlog::MLEVEL >() ) streamlog::out()  


#define  streamlog_message( MLEVEL , CODE_BLOCK , OUT_MESSAGE)\
   if( streamlog::out.write< streamlog::MLEVEL >() ) { \
      CODE_BLOCK \
    streamlog::out() << OUT_MESSAGE }

//for use in templated classes with gcc 3.2 use these macros:
#define  streamlog_out_T( MLEVEL ) if(streamlog::out.template write< streamlog::MLEVEL >() ) streamlog::out() 

#define  streamlog_message_T( MLEVEL , CODE_BLOCK , OUT_MESSAGE)\
   if( streamlog::out.template write< streamlog::MLEVEL >() ) { \
      CODE_BLOCK \
    streamlog::out() << OUT_MESSAGE }

#else

#define  streamlog_out( MLEVEL ) std::cout

#define  streamlog_message( MLEVEL , CODE_BLOCK , OUT_MESSAGE)\
   if( true ) { \
      CODE_BLOCK \
      std::out << OUT_MESSAGE }

#endif


#endif
