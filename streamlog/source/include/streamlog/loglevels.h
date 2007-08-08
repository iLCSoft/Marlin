// -*- mode: c++;

/** loglevels.h:
 *  defines default log levels for streamlog
 *  
 *  Default log levels for streamlog::logstream:<br>
 *  <b><ul> 
 *   <li>DEBUG/DEBUG0, DEBUG1, DEBUG2, DEBUG3, DEBUG4 </li>
 *   <li>MESSAGE/MESSAGE0,...  MESSAGE4</li>
 *   <li> WARNING/WARNING0,... WARNING4</li>
 *   <li>  ERROR/ERROR0,... ERROR4</li>
 *  </ul></b>
 *  By default all groups are active, except DEBUG when compiled with
 *  -DNDEBUG, i.e. in release mode.
 *  Through -DSTREAMLOG_LEVEL=N, where N=0,1,2,3,4 this behaviour can be changed, e.g.
 *  if -DSTREAMLOG_LEVEL=2 is specified all compile time all messages of the groups
 *  DEBUG and MESSAGE are suppressed (no overhead in space or time) and only WARNING
 *  and ERROR messages will be visible if the current log level is reached.
 * 
 *  Users can define their own log levels or additional loglevels with the macro:
 *  <pre>  DEFINE_STREAMLOG_LEVEL( classname,  "LogString",  loglevel , active_flag )  </pre>
 *  if active flag is false corresponding code will have no effect.
 * 
 *  
 *  @author F. Gaede, DESY
 *  @version $Id: loglevels.h,v 1.3 2007-08-08 13:58:33 gaede Exp $
 */

#ifndef loglevels_h
#define loglevels_h

#include "streamlog/baselevels.h"

namespace streamlog{


  enum log_level_enum{
    debug_base_level=0,
    message_base_level=100,
    warning_base_level=200,
    error_base_level=300
  } ;

  //  DEFINE_STREAMLOG_LEVEL( classname,  "LogString",  loglevel , active_flag ) 

  DEFINE_STREAMLOG_LEVEL( DEBUG,   "DEBUG",  debug_base_level + 0 , STREAMLOG_DEBUG_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( DEBUG0,  "DEBUG0",  debug_base_level + 0 , STREAMLOG_DEBUG_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( DEBUG1,  "DEBUG1",  debug_base_level + 1 , STREAMLOG_DEBUG_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( DEBUG2,  "DEBUG2",  debug_base_level + 2 , STREAMLOG_DEBUG_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( DEBUG3,  "DEBUG3",  debug_base_level + 3 , STREAMLOG_DEBUG_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( DEBUG4,  "DEBUG4",  debug_base_level + 4 , STREAMLOG_DEBUG_ACTIVE ) 

  DEFINE_STREAMLOG_LEVEL( MESSAGE,  "MESSAGE", message_base_level + 0 , STREAMLOG_MESSAGE_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( MESSAGE0, "MESSAGE0", message_base_level + 0 , STREAMLOG_MESSAGE_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( MESSAGE1, "MESSAGE1", message_base_level + 1 , STREAMLOG_MESSAGE_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( MESSAGE2, "MESSAGE2", message_base_level + 2 , STREAMLOG_MESSAGE_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( MESSAGE3, "MESSAGE3", message_base_level + 3 , STREAMLOG_MESSAGE_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( MESSAGE4, "MESSAGE4", message_base_level + 4 , STREAMLOG_MESSAGE_ACTIVE ) 

  DEFINE_STREAMLOG_LEVEL( WARNING,  "WARNING", warning_base_level + 0 , STREAMLOG_WARNING_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( WARNING0, "WARNING0", warning_base_level + 0 , STREAMLOG_WARNING_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( WARNING1, "WARNING1", warning_base_level + 1 , STREAMLOG_WARNING_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( WARNING2, "WARNING2", warning_base_level + 2 , STREAMLOG_WARNING_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( WARNING3, "WARNING3", warning_base_level + 3 , STREAMLOG_WARNING_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( WARNING4, "WARNING4", warning_base_level + 4 , STREAMLOG_WARNING_ACTIVE ) 

  DEFINE_STREAMLOG_LEVEL( ERROR,  "ERROR", error_base_level + 0 , STREAMLOG_ERROR_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( ERROR0, "ERROR0", error_base_level + 0 , STREAMLOG_ERROR_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( ERROR1, "ERROR1", error_base_level + 1 , STREAMLOG_ERROR_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( ERROR2, "ERROR2", error_base_level + 2 , STREAMLOG_ERROR_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( ERROR3, "ERROR3", error_base_level + 3 , STREAMLOG_ERROR_ACTIVE ) 
  DEFINE_STREAMLOG_LEVEL( ERROR4, "ERROR4", error_base_level + 4 , STREAMLOG_ERROR_ACTIVE ) 


    // use this to turn of all logging messages
  DEFINE_STREAMLOG_LEVEL( SILENT, "SILENT" ,  STREAMLOG_MAX_LEVEL    , false ) 

}

#endif


