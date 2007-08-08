#ifndef marlin_VerbosityLevels_h
#define marlin_VerbosityLevels_h

#include "streamlog/streamlog.h"

// import the log level classes into the marlin namespace 

namespace marlin{

  using  streamlog::DEBUG ;
  using  streamlog::DEBUG0 ;
  using  streamlog::DEBUG1 ;
  using  streamlog::DEBUG2 ;
  using  streamlog::DEBUG3 ;
  using  streamlog::DEBUG4 ;
  using  streamlog::MESSAGE ;
  using  streamlog::MESSAGE0 ;
  using  streamlog::MESSAGE1 ;
  using  streamlog::MESSAGE2 ;
  using  streamlog::MESSAGE3 ;
  using  streamlog::MESSAGE4 ;
  using  streamlog::WARNING ;
  using  streamlog::WARNING0 ;
  using  streamlog::WARNING1 ;
  using  streamlog::WARNING2 ;
  using  streamlog::WARNING3 ;
  using  streamlog::WARNING4 ;
  using  streamlog::ERROR ;
  using  streamlog::ERROR0 ;
  using  streamlog::ERROR1 ;
  using  streamlog::ERROR2 ;
  using  streamlog::ERROR3 ;
  using  streamlog::ERROR4 ;
  using  streamlog::SILENT ;

}

#endif


