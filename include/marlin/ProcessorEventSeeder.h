#ifndef PROCESSOREVENTSEEDER_H
#define PROCESSOREVENTSEEDER_H 1

#include "lcio.h"
#include "EVENT/LCEvent.h"
#include <map>

using namespace lcio ;

namespace marlin{
  
  class Processor;
  
  /** Processor event seeder - provides independent pseudo-randomly generated seeds 
   *  for registered processors on an event by event basis.   
   * 
   *  The seeds to be supplied are generated using rand() from stdlib. 
   *  The pseudo-random number generator is initialized using srand( unsigned int seed )
   *  for each event, where "seed" is determined from the Marlin Global seed, as well as 
   *  the run number and event number via:
   *  
   *     global_seed ^ evt->getRunNumber() ^ evt->getEventNumber()
   *
   *  note: the value of the global seed must be a positive integer, with max value 2,147,483,647
   *
   *  If a call is made to getSeed( Processor* ) preceededing a call to registerProcessor( Processor* )
   *  an exception will be thrown.
   *
   *  @author S.J. Aplin, DESY
   */
  class ProcessorEventSeeder {
    
  public:

    /** Constructor */
    ProcessorEventSeeder( int global_seed ) ;

    /** Destructor */
    ~ProcessorEventSeeder() { } ;
    
    /** Called by Processors to register themselves for the seeding service. 
     */
    void registerProcessor( Processor* proc) ;

    /** Create new set of seeds for registered Processors for the given event.
     */
    void refreshSeeds(LCEvent * evt) ;

    /** Called by Processors to obtain seed assigned to it for the current event.
     */
    EVENT::long64 getSeed( Processor* proc ) ;
 
  private:
    
    ProcessorEventSeeder(const ProcessorEventSeeder&);	// prevent copying
    ProcessorEventSeeder& operator=(const ProcessorEventSeeder&);

    /** Global seed for current Job. Set in steering file.
     */
    int _global_seed ;

    /** Map to hold pointers to the registered processors and their assigned seeds
     */
    std::map<Processor*, EVENT::long64> _seed_map ;

  } ;

} // end namespace marlin 
#endif
