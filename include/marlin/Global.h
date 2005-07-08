#ifndef Global_h
#define Global_h 1

#ifdef USE_GEAR
#include "gear/GearMgr.h"
#endif 



namespace marlin{

  class StringParameters ;

  /** Simple global class for Marlin.
   *  Holds global parameters.
   */
  class Global{
    
  public:
    
    static StringParameters* parameters ;
    

#ifdef USE_GEAR
    static gear::GearMgr* GEAR ;
#endif 


  };
  
  
} // end namespace marlin 
#endif
