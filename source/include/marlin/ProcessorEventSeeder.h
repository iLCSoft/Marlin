#ifndef PROCESSOREVENTSEEDER_H
#define PROCESSOREVENTSEEDER_H 1

namespace EVENT {
  class LCEvent ;
}

namespace marlin {

  class ProcessorMgr;  
  class Processor;

  /** Processor event seeder - provides independent pseudo-randomly generated seeds 
   *  for registered processors on an event by event basis.   
   * 
   *      This Class is used to provide individual pseudo-random numbers to Processors on
   *      an event-by-event and run-by-run basis. These may be used by Processors to 
   *	  seed their random number generator of choice. In order to use this service 
   *	  a Processor must register itself in the init method using:
   *
   *             Global::EVENTSEEDER->registerProcessor(this);
   *
   *      and should retrieve its allocated event seed during processEvent using:
   *
   *             int eventSeed = Global::EVENTSEEDER->getSeed(this);
   *
   *	  and include the header file:
   *
   *	  	#include "marlin/ProcessorEventSeeder.h"	      	
   *
   *      The global seed is used for a complete job and is set in the Global steering parameters thus:
   *     
   *             <parameter name="RandomSeed" value="1234567890"/>
   *
   *      Note that the value must be a positive integer, with max value 2,147,483,647
   *      A pseudo-random event seed is generated using a three step hashing function of unsigned ints,
   *	  in the following order: event_number, run_number, RandomSeed. The hashed int from each step 
   *	  in the above order is used as input to the next hash step. This is used to ensure that in 
   *	  the likely event of similar values of event_number, run_number and RandomSeed, different 
   *	  event seeds will be generated. 
   *    
   *	  The event seed is then used to seed rand via srand(seed) and then rand is used to 
   *	  generate one seed per registered processor.
   *
   *	  This mechanism ensures reproducible results for every event, regardless of the sequence 
   *	  in which the event is processed in a Marlin job, whilst maintaining the full 32bit range 
   *	  for event and run numbers.
   *   
   *      If a call is made to getSeed( Processor* ) preceededing a call to registerProcessor( Processor* )
   *      an exception will be thrown.
   *
   *  @author S.J. Aplin, DESY
   *  @deprecated
   */
  class ProcessorEventSeeder {
    friend class ProcessorMgr ; // for creation
    ProcessorEventSeeder(const ProcessorEventSeeder&) = delete ;	// prevent copying
    ProcessorEventSeeder& operator=(const ProcessorEventSeeder&) = delete ; // prevent assignment
    
  public:
    /** Destructor */
    ~ProcessorEventSeeder() = default ;
    
    /** Called by Processors to register themselves for the seeding service. 
     */
    void registerProcessor( Processor* proc ) ;
 
    /** Called by Processors to obtain seed assigned to it for the current event.
     */
    unsigned int getSeed( Processor* proc ) ;
 
  private:

    /** Constructor */
    ProcessorEventSeeder() = default ;

    /** Create new set of seeds for registered Processors for the given event.
     *  This method should only be called from ProcessorMgr::processEvent
     */
    void refreshSeeds( EVENT::LCEvent * evt ) ;
  } ;

} // end namespace marlin 
#endif
