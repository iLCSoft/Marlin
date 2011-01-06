#include "marlin/Global.h"
#include "marlin/StringParameters.h"
#include "marlin/ProcessorEventSeeder.h"

namespace marlin{
  
  StringParameters* Global::parameters = 0 ;
  
//#ifdef USE_GEAR
  gear::GearMgr* Global::GEAR = 0 ;
//#endif 

  ProcessorEventSeeder* Global::EVENTSEEDER = 0 ;

}
