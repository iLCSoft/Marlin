#ifndef marlin_VerbosityLevels_h
#define marlin_VerbosityLevels_h


namespace marlin{

#ifdef MARLINDEBUG
  typedef DEBUG_on< true > DEBUG ;
#else
  typedef DEBUG_on< false >   DEBUG ;
#endif
  
  typedef MESSAGE_on< true > MESSAGE ;
  
  typedef WARNING_on< true > WARNING ;
  
  typedef ERROR_on< true >   ERROR ;
  
}

#endif


