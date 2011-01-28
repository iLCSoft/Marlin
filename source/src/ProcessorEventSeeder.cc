#include "marlin/ProcessorEventSeeder.h"
#include "marlin/Processor.h"

#include "jenkinsHash.h"

#include <stdlib.h>
#include <limits>

namespace marlin{

  ProcessorEventSeeder::ProcessorEventSeeder( int global_seed ) : _global_seed(global_seed) , _eventProcessingStarted(false) {
    
    if( _global_seed < 0 )
      {
	throw Exception("ProcessorEventSeeder: Marlin's global seed, must not be less than zero");
      }

    // at this point we don't know which runs and events will be processed so all we can do is use the global random seed set in the steering file.
    srand( _global_seed );
    streamlog_out(DEBUG) << "ProcessorEventSeeder: srand initialised with global seed " << _global_seed << std::endl; 

  } 

  
  void ProcessorEventSeeder::registerProcessor( Processor* proc ) {

    
    if ( _eventProcessingStarted ) { // event processing started, so disallow any more calls to registerProcessor
      streamlog_out(ERROR) << "ProcessorEventSeeder:registerProcessor( Processor* proc ) called from Processor: " 
			   << proc->name() << std::endl << "The method registerProcessor( Processor* proc ) must be called in the init() method of the Processor" 
			   << std::endl;

      throw Exception("ProcessorEventSeeder: Event Processing has already started. registerProcessor( Processor* proc ) must be called in the init() method");
    }

    // just in case any body has called rand since we set the seed 
    srand( _global_seed );
    streamlog_out(DEBUG) << "ProcessorEventSeeder: srand initialised with global seed " << _global_seed << std::endl; 

    _seed_map[proc] = rand();
    streamlog_out(DEBUG) << "ProcessorEventSeeder: Processor " << proc->name() << " registered for random seed service. Allocated " <<  _seed_map[proc] << " as initial seed." << std::endl; 

  }

  void ProcessorEventSeeder::refreshSeeds( LCEvent * evt ) {

    _eventProcessingStarted = true; // event processing started so disallow any more calls to registerProcessor

    // get hashed seed using jenkins_hash
    unsigned int seed = 0 ; // initial state
    unsigned int eventNumber = evt->getEventNumber() ;
    unsigned int runNumber = evt->getRunNumber() ;

    unsigned char * c = (unsigned char *) &eventNumber ;
    seed = jenkins_hash( c, sizeof eventNumber, seed) ;

    c = (unsigned char *) &runNumber ;
    seed = jenkins_hash( c, sizeof runNumber, seed) ;

    c = (unsigned char *) &_global_seed ;
    seed = jenkins_hash( c, sizeof _global_seed, seed) ;

    // set the seed for rand() for this event
    if ( seed == 1 ) seed = 123456789 ; // can't used a seed value of 1 as srand(1) sets rand() back the state of the last call to srand( seed ).

    streamlog_out(DEBUG) << "ProcessorEventSeeder: Refresh Seeds using " << seed << " as seed for srand( seed )" << std::endl; 
    srand( seed );

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
