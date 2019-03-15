#ifndef MARLIN_RANDOMSEEDGENERATOR_h
#define MARLIN_RANDOMSEEDGENERATOR_h 1

// -- lcio headers
#include "EVENT/LCEvent.h"

// -- marlin headers
#include <marlin/Exceptions.h>
#include "jenkinsHash.h"

// -- std headers
#include <iostream>
#include <map>
#include <random>
#include <atomic>

namespace marlin {

  /** 
   *  @brief  RandomSeedGenerator template class
   *  
   *  Random seed generator helper - provides independent pseudo-randomly generated seeds 
   *  for registered objects on an event by event basis.   
   * 
   *  This class is mainly used to provide individual pseudo-random numbers to processors on
   *  an event-by-event and run-by-run basis. These may be used by processors to 
   *	seed their random number generator of choice. In order to use this service 
   *	a Processor must register itself in the init method using:
   *
   *  // TODO define the processor API. Keep the old documentation for the moment. Must be updated !
   *             Global::EVENTSEEDER->registerProcessor(this);
   *
   *  and should retrieve its allocated event seed during processEvent using:
   *
   *             int eventSeed = Global::EVENTSEEDER->getSeed(this);
   *
   *	and include the header file:
   *
   *	  	#include "marlin/ProcessorEventSeeder.h"	      	
   *
   *  The global seed is used for a complete job and is set in the Global steering parameters thus:
   *     
   *             <parameter name="RandomSeed" value="1234567890"/>
   *
   *  Note that the value must be a positive integer, with max value std::numeric_limits<unsigned int>::max()
   *  A pseudo-random event seed is generated using a three step hashing function of unsigned ints,
   *  in the following order: event_number, run_number, RandomSeed. The hashed int from each step 
   *	in the above order is used as input to the next hash step. This is used to ensure that in 
   *	the likely event of similar values of event_number, run_number and RandomSeed, different 
   *	event seeds will be generated. 
   *    
   *  The event seed is then used to seed rand via std::mersenne_twister_engine::seed(seed) and then 
   *  a std::mersenne_twister_engine with a uniform distribution (std::uniform_int_distribution) is used 
   *  to generate one seed per registered object.
   *
   *	This mechanism ensures reproducible results for every event, regardless of the sequence 
   *	in which the event is processed in a Marlin job, whilst maintaining the full 32bit range 
   *	for event and run numbers.
   *   
   *  If a call is made to getSeed( obj ) preceededing a call to addEntry( obj )
   *  an exception will be thrown.
   *
   *  @author S.J. Aplin, DESY
   *  @author R. Ete, DESY
   */
  template <typename T>
  class RandomSeedGenerator {
  public:
    // no copy
    RandomSeedGenerator(const RandomSeedGenerator<T>&) = delete ;
    RandomSeedGenerator& operator=(const RandomSeedGenerator<T>&) = delete ;
    ~RandomSeedGenerator() = default ;
    
  public:
    // typedefs
    typedef unsigned int                                        SeedType ;
    typedef std::map<const T*, SeedType>                        RandomSeedMap ;
    typedef std::mt19937                                        RandomGenerator ;
    typedef std::uniform_int_distribution<SeedType>             RandomDistribution ;
    // constants
    static const SeedType MinSeed = 0 ;
    static const SeedType MaxSeed = std::numeric_limits<SeedType>::max() ;
        
  public:
    /** 
     *  @brief  Constructor
     */
    RandomSeedGenerator() = default ;
    
    /**
     *  @brief  Constructor with global seed
     * 
     *  @param  globalSeed the global seed
     */
    RandomSeedGenerator( SeedType globalSeed ) ;
    
    /**
     *  @brief  Add an entry to the random seed generator
     *  
     *  @param  instance a new instance of object
     */
    void addEntry( const T *instance ) ;
 
    /** Called by Processors to obtain seed assigned to it for the current event.
     */
    /**
     *  @brief  Get the seed associated to the registered object instance
     *  
     *  @param  instance a registered instance
     */
    SeedType getSeed( const T* instance ) const ;
    
    /** 
     *  @brief  Create new set of seeds for registered instance for the given event.
     */
    void refreshSeeds( const EVENT::LCEvent * const evt ) ;
    
  private:
    /**
     *  @brief  Get a new random number from the internal generator
     */
    SeedType getRandom() ;
 
  private:
    /// The global random seed, if set
    SeedType                _globalSeed {0} ;
    /// Whether the global seed has been set
    bool                    _globalSeedSet {false} ;
    /// Whether the event processing has been started
    bool                    _eventProcessingStarted {false} ;
    /// The random seed mapping
    RandomSeedMap           _randomSeeds {} ;
    /// The random generator engine
    RandomGenerator         _generator {_globalSeed} ;
    /// The random number distribution
    RandomDistribution      _rdmDistribution{MinSeed, MaxSeed} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline RandomSeedGenerator<T>::RandomSeedGenerator( unsigned int globalSeed ) :
    _globalSeed( globalSeed ), 
    _globalSeedSet( true ),
    _generator( _globalSeed ) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline void RandomSeedGenerator<T>::addEntry( const T* entry ) {
    // event processing started, so disallow any more calls to addEntry
    if ( _eventProcessingStarted ) { 
      std::cerr << "RandomSeedGenerator<T>::addEntry called from object: " 
			   << (void*)entry 
         << std::endl 
         << "The method addEntry must be called at initialization time" 
         << std::endl ;
      throw Exception("RandomSeedGenerator: Event processing has already started. 'addEntry' must be called at initialization time") ;
    }
    // just in case any body has called rand since we set the seed 
    _generator.seed( _globalSeed );
    std::cout << "ProcessorEventSeeder: generator initialised with global seed " << _globalSeed << std::endl ;
    auto iter = _randomSeeds.find( entry ) ;
    if ( _randomSeeds.end() != iter ) {
      std::cerr << "RandomSeedGenerator<T>::addEntry called from object: " 
         << (void*)entry 
         << std::endl 
         << "Instance already registered !" 
         << std::endl ;
      throw Exception("RandomSeedGenerator: Instance already registered !") ;
    }
    auto seed = getRandom() ;
    typename RandomSeedMap::value_type mapEntry( entry, seed ) ;
    _randomSeeds.insert( mapEntry ) ;
    std::cout << "RandomSeedGenerator<T>: instance " << (void*)entry
                         << " registered for random seed service. Allocated "
                         <<  seed << " as initial seed." << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline typename RandomSeedGenerator<T>::SeedType RandomSeedGenerator<T>::getSeed( const T* entry ) const {
    auto iter = _randomSeeds.find( entry ) ;
    if ( _randomSeeds.end() == iter ) {
      std::cerr << "RandomSeedGenerator<T>::getSeed called from object: " 
         << (void*)entry 
         << std::endl 
         << "No such instance registered !" 
         << std::endl ;
      throw Exception("RandomSeedGenerator: Instance not registered !") ;
    }
    return iter->second.load() ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void RandomSeedGenerator<T>::refreshSeeds( const EVENT::LCEvent * const evt ) {
    // event processing started so disallow any more calls to addEntry
    _eventProcessingStarted = true ;
    // get hashed seed using jenkins_hash
    unsigned int seed = 0 ; // initial state
    unsigned int eventNumber = evt->getEventNumber() ;
    unsigned int runNumber = evt->getRunNumber() ;
    unsigned char * c = (unsigned char *) &eventNumber ;
    seed = jenkins_hash( c, sizeof eventNumber, seed) ;
    c = (unsigned char *) &runNumber ;
    seed = jenkins_hash( c, sizeof runNumber, seed) ;
    c = (unsigned char *) &_globalSeed ;
    seed = jenkins_hash( c, sizeof _globalSeed, seed) ;
    // refresh the seed
    std::cout << "RandomSeedGenerator: Refresh seeds using " << seed << " as new random seed" << std::endl ;
    _generator.seed( seed ) ;
    // fill map with seeds for each registered instance using random number generator 
    for (auto iter : _randomSeeds ) {
      iter.second = getRandom() ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline typename RandomSeedGenerator<T>::SeedType RandomSeedGenerator<T>::getRandom() {
    return static_cast<SeedType>(_rdmDistribution(_generator)) ;
  }

} // end namespace marlin 

#endif
