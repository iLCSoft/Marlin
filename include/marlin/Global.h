#ifndef Global_h
#define Global_h 1

#ifdef USE_GEAR
#include "gear/GearMgr.h"
#endif 



namespace marlin{

  class StringParameters ;

  /** Simple global class for Marlin.
   *  Holds global parameters.
   *
   *  @author F. Gaede, DESY
   *  @version $Id: Global.h,v 1.4 2005-10-11 12:56:28 gaede Exp $ 
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
