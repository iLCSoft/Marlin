#include "marlin/Global.h"
#include "marlin/StringParameters.h"

namespace marlin{
  
  StringParameters* Global::parameters = 0 ;
  
#ifdef USE_GEAR
  gear::GearMgr* Global::GEAR = 0 ;
#endif 
 

}
