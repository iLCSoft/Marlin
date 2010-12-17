#include "marlin/ProcessorEventSeeder.h"
#include "marlin/Processor.h"

#include <stdlib.h>
#include <limits>

namespace marlin{

  ProcessorEventSeeder::ProcessorEventSeeder( int global_seed ) : _global_seed(global_seed) {
    
    if( _global_seed < 0 )
      {
	throw Exception("Marlin's global seed, must be greater than zero");
      }


    //    if( _gobal_seed > numeric_limits<int>::max()

    srand( _global_seed );

  } 

  
  void ProcessorEventSeeder::registerProcessor( Processor* proc ) {

    _seed_map[proc] = rand();

  }

  void ProcessorEventSeeder::refreshSeeds( LCEvent * evt ) {

    // set the seed for rand() for this event
    srand( _global_seed ^ evt->getRunNumber() ^ evt->getEventNumber() );

    // fill map with seeds for each registered processor using rand() 
    std::map<Processor*, EVENT::long64>::iterator it = _seed_map.begin();
    for( /* it initialised above */; it != _seed_map.end(); ++it) 
      {
	it->second = rand();
      } 
    
  }

  EVENT::long64 ProcessorEventSeeder::getSeed( Processor* proc ) {

    std::map<Processor*, EVENT::long64>::iterator it = _seed_map.find(proc);
    
    return ( it != _seed_map.end() ? it->second : throw ) ;

  }

} // namespace marlin
