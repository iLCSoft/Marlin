#include "marlin/ProcessorEventSeeder.h"
#include "marlin/Processor.h"
#include "marlin/Global.h"

#include "jenkinsHash.h"

#include <stdlib.h>
#include <limits>
#include <algorithm>

namespace marlin{


  ProcessorEventSeeder::ProcessorEventSeeder() : _global_seed(0), _global_seed_set(false), _eventProcessingStarted(false), _vector_pair_proc_seed()
  {
  } 

  
  void ProcessorEventSeeder::registerProcessor( Processor* proc ) {

    
    if( _global_seed_set == false ) {
      _global_seed = Global::parameters->getIntVal("RandomSeed" ) ;
      _global_seed_set = true;
    }

    if ( _eventProcessingStarted ) { // event processing started, so disallow any more calls to registerProcessor
      streamlog_out(ERROR) << "ProcessorEventSeeder:registerProcessor( Processor* proc ) called from Processor: " 
			   << proc->name() << std::endl << "The method registerProcessor( Processor* proc ) must be called in the init() method of the Processor" 
			   << std::endl;

      throw Exception("ProcessorEventSeeder: Event Processing has already started. registerProcessor( Processor* proc ) must be called in the init() method");
    }

    // just in case any body has called rand since we set the seed 
    srand( _global_seed );
    streamlog_out(DEBUG) << "ProcessorEventSeeder: srand initialised with global seed " << _global_seed << std::endl; 

    _vector_pair_proc_seed.push_back( std::make_pair( proc, rand() ) );
    streamlog_out(DEBUG) << "ProcessorEventSeeder: Processor " << proc->name()
                         << " registered for random seed service. Allocated "
                         <<  _vector_pair_proc_seed.back().second << " as initial seed." << std::endl;

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
    for (auto& pairProcSeed : _vector_pair_proc_seed ) {
      pairProcSeed.second = rand();
    }
    
  }
  
  unsigned int ProcessorEventSeeder::getSeed( Processor* proc ) {
    
    typedef std::pair<Processor*, unsigned int> Pair;
    
    auto it = find_if( _vector_pair_proc_seed.begin(), _vector_pair_proc_seed.end(), [&](Pair const& pair){ return pair.first == proc; } );

    return ( it != _vector_pair_proc_seed.end() ? it->second : throw ) ;

  }

} // namespace marlin
