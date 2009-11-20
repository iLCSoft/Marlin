// -*- mode: c++;
#ifndef logbuffer_h
#define logbuffer_h

#include <sstream>
#include <cstdio>
#include "streamlog/logstream.h"

namespace streamlog{


  /** Helper class that adds a prefix to every new line that is written to its
   *  std::ostream.
   * 
   *  @author F. Gaede, DESY
   *  @version $Id: logbuffer.h,v 1.1.1.1 2007-07-12 17:14:48 gaede Exp $
   */
  class logbuffer : public std::streambuf {
    
    std::streambuf* _sbuf ;
    logstream* _ls ;

    logbuffer();


  public:

    logbuffer( std::streambuf* sbuf, logstream* logstream ) : _sbuf( sbuf ), _ls(logstream) {} 
  
    ~logbuffer() {
    
    }

    
  /** This is where the logstream's current prefix is  added to every
   *  new line written to the std::ostream that has this logbuffer.
   *  Idea taken from J.Samson, DESY.
   */
    inline virtual int overflow( int c = EOF ) {
      
      static bool hasNewLine = true ;
      
      if ( c == EOF ) 
	return EOF ;
    
      bool success = true;
    
      if ( hasNewLine == true ) {
	
	std::string pre = (* _ls->prefix() )() ;

	success &= (  (unsigned) _sbuf->sputn( pre.c_str() , pre.size() ) == pre.size()  ) ;

	hasNewLine = false;
      }

      if ( c == '\n' )
	hasNewLine = true;
    
      if ( success )
	success &= ( _sbuf->sputc(c) != EOF ) ;
    
      if( success ) 
	return 0 ;

      return EOF ;
    }

  } ;

}
#endif
