// -*- mode: c++;
#ifndef logstream_h
#define logstream_h

#include "streamlog/prefix.h"

#include <iostream>
#include <map>

namespace streamlog{

  class prefix_base ;
  class logbuffer ;
  class logscope ;


  /** Class defining a log stream that is used to print log messages depending 
   *  on current log level. Can be initialized with any std::ostream, typically either 
   *  std::cout or an std::ofstream file handler.
   *  There is one global instance of this class defined in the library: logstream::out  <br>
   *  Typically only this instance is needed, e.g.: <br>
   *  <pre>
   *    // in int main() :
   *    streamlog::out.init( std::cout, argv[0] ) ;
   * 
   *    //...
   *    
   *    if( streamlog::out.write< streamlog::DEBUG1 >() )
   *       streamlog::out() << " this message will only be printed if level >= DEBUG1::level " 
   *                        << std::endl ;
   * 
   *    // or the same, simply using a macro:
   * 
   *    streamlog_out( DEBUG )  << " this message will only be printed if level >= DEBUG1::level " 
   *                            << std::endl ;
   * 
   *    
   *  </pre>
   *  Note that with the above calling sequence or the macro no runtime overhead is created
   *  if streamlog::DEBUG1::active is false and else if the log level is smaller than 
   *  streamlog::DEBUG1::level no formatting of the message will happen, i.e. also very little 
   *  runtime cost is involved. <br>
   *  the behaviour of the logstream, i.e. the current log level and log scope name can be changed only
   *  through an object of streamlog::logscope.
   * 
   *  @see logstream::write() 
   *  @see logscope
   *
   *  @author F. Gaede, DESY
   *  @version $Id: logstream.h,v 1.3 2007-08-08 13:08:34 gaede Exp $
   */
  class logstream {

    friend class logscope ;
    friend class logbuffer ;


    typedef std::map< std::string,  unsigned > LevelMap ;

  public :

    logstream() ; 

    ~logstream() ;

    /** Initialize the logstream with an std::ostream, e.g. std::cout and 
     *  the main scope name, e.g. argv[0].
     *  Only first call to this method has an effect, subsequent calls are ignored.
     * 
     */
    void init( std::ostream& os , const std::string name ) ;

    /** True if next log message of the current level (class T ) will be written, i.e.
     *  the next call to std::ostream& operator()() will return a valid outstream.
     * 
     */
    template<class T>
    inline bool write() {
      
      // dont' call chek_level if T::active == false
      return (  T::active   &&    check_level<T>()  ) ;
    }

    /** Return the actual std::ostream where the message should be writen to - will return
     *  a nullstream unless prepended by a successfull call to write<MESSAGELEVEL>()
     */
    std::ostream& operator()() ;

    /** Adds a level name to the stream for setting the log level with a string through 
     *  a scope class. Only names added with this method will have an effect - other log 
     *  levels can only be activated with logstream::logscope::setLevel<T>().
     *  Useful for framework programs where the log level can be changed via a steering 
     *  parameter.
     *  
     *  @see logstream::logscope::setLevel(const std::string levelName )
     */
    template <class T>
    void addLevelName() {

      _map[ T::name() ] = T::level ;
    }


    // interface for friend classes: scope and logbuffer

  protected:

    /** Set the current level - user need to use a streamlog::logscope object 
     *  to do this.
     */
    void setLevel( unsigned level ) { _level = level ; } 

    /** Set the current level through its name - only level previously made known to
     *  the stream through addLevelName will have an effect.
     */
    unsigned setLevel( const std::string& levelName )  ; 

    /** Returns the prefix for the logbuffer object */
    prefix_base* prefix() { return _prefix ; }

    /** used internally by write<T> */
    template<class T>
    bool check_level() {
      
      if( T::level >= _level ){
	_active = true ;
	_prefix->_levelName = T::name() ;
      }
      return _active ;
    }


  private:

    /** Private helper class returned if message log level not reached */ 
    class nullstream :  public std::ostream {
    public:
      nullstream() : std::ios( 0 ), std::ostream( 0 ) {} ;
    } ;
  

    nullstream* _ns ;    // the nullstream
    std::ostream* _os ; // wrapper for actual ostream
    unsigned _level ;   // current log level 
    bool _active ;      // boolean helper 
    logbuffer* _lb ;        // log buffer adds prefix to everu log message
    prefix_base* _prefix ;  // prefix formatter
    LevelMap _map ;         // string map of level names
    
  } ;

  extern logstream out ;

}
#endif
