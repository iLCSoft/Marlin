#include "marlin/ProcessorEventSeeder.h"
#include "marlin/Processor.h"
#include "marlin/Scheduler.h"
#include "marlin/RandomSeedGenerator.h"

namespace marlin{

  void ProcessorEventSeeder::registerProcessor( Processor* proc ) {
    proc->_scheduler->randomSeedGenerator()->addEntry( proc ) ;
  }
  
  //--------------------------------------------------------------------------

  void ProcessorEventSeeder::refreshSeeds( EVENT::LCEvent * /*evt*/ ) {
    throw Exception( "ProcessorEventSeeder::refreshSeeds: deprecated method, should not be called" ) ;    
  }
  
  //--------------------------------------------------------------------------
  
  unsigned int ProcessorEventSeeder::getSeed( Processor* proc ) {
    return proc->_scheduler->randomSeedGenerator()->getSeed( proc ) ;
  }

} // namespace marlin
