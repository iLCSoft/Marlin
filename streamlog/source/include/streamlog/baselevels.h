// -*- mode: c++;

#ifndef baselevels_h
#define baselevels_h
/* 
 *  Define base log level classes for streamlog::logstream. There are the following
 *  groups of log level classes: DEBUG, WARNING, MESSAGE and ERROR. 
 *  By default all groups are active, except DEBUG when compiled with
 *  -DNDEBUG, i.e. in release mode.
 *  Through -DSTREAMLOG_LEVEL=N, where N=0,1,2,3,4 this behaviour can be changed, e.g.
 *  if -DSTREAMLOG_LEVEL=2 is specified at compile time all messages of the groups
 *  DEBUG and MESSAGE are suppressed (no overhead in space or time) and only WARNING
 *  and ERROR messages will be visible if the current log level is reached.
 *  
 *  @author F. Gaede, DESY
 *  @version $Id: baselevels.h,v 1.2 2007-07-13 11:09:04 gaede Exp $
 */


namespace streamlog{

#define STREAMLOG_MAX_LEVEL 0xFFFFFFFF 
    
#ifndef STREAMLOG_LEVEL
 #ifndef NDEBUG
  #define STREAMLOG_LEVEL 0
 #else
  #define STREAMLOG_LEVEL 1
 #endif
#endif

#if STREAMLOG_LEVEL > 3
  #define STREAMLOG_ERROR_ACTIVE false
#else
  #define STREAMLOG_ERROR_ACTIVE true
#endif

#if STREAMLOG_LEVEL > 2
  #define STREAMLOG_WARNING_ACTIVE false
#else
  #define STREAMLOG_WARNING_ACTIVE true
#endif

#if STREAMLOG_LEVEL > 1
  #define STREAMLOG_MESSAGE_ACTIVE false
#else
  #define STREAMLOG_MESSAGE_ACTIVE true
#endif

#if STREAMLOG_LEVEL > 0
  #define STREAMLOG_DEBUG_ACTIVE false
#else
  #define STREAMLOG_DEBUG_ACTIVE true
#endif

  
  template <unsigned LEVEL, bool ON>
  struct Verbosity{
    static const unsigned level = LEVEL ; 
    static const bool active = ON ;
  } ;



  /** macro for registering a new loglevel class */
#define DEFINE_STREAMLOG_LEVEL( ClassName, String, LOG_Level, Active )\
  template <bool ON>\
  struct  active_##ClassName : public Verbosity< (LOG_Level) , ON >{\
    static const char* name() { return (String) ; }\
  };\
  typedef  active_##ClassName< Active > ClassName ; \

}
#endif
