#ifndef MARLIN_RANDOMSEEDMANAGER_h
#define MARLIN_RANDOMSEEDMANAGER_h 1

// -- std headers
#include <map>
#include <random>
#include <memory>
#include <functional>
#include <unordered_set>

namespace EVENT {
  class LCEvent ;
}

namespace marlin {

  /**
   *  @brief  RandomSeedManager class
   */
  class RandomSeedManager {
  public:
    RandomSeedManager(const RandomSeedManager&) = delete ;
    RandomSeedManager& operator=(const RandomSeedManager&) = delete ;
    ~RandomSeedManager() = default ;

  public:
    // typedefs
    typedef unsigned int                                        SeedType ;
    typedef std::hash<const void*>                              HashFunction ;
    typedef HashFunction::argument_type                         HashArgument ;
    typedef HashFunction::result_type                           HashResult ;
    typedef std::unordered_set<HashResult>                      EntryList ;
    typedef std::map<HashResult, SeedType>                      RandomSeedMap ;
    typedef std::mt19937                                        RandomGenerator ;
    typedef std::uniform_int_distribution<SeedType>             RandomDistribution ;
    // constants
    static const SeedType MinSeed = 0 ;
    static const SeedType MaxSeed = std::numeric_limits<SeedType>::max() ;

  public:
    /**
     *  @brief  Constructor with global seed
     *
     *  @param  globalSeed the global seed
     */
    RandomSeedManager( SeedType globalSeed = time(nullptr) ) ;

    /**
     *  @brief  Add an entry to the random seed manager
     *
     *  @param  entry a hash ideintifying the entry
     */
    void addEntry( HashResult entry ) ;

    /**
     *  @brief  Add an entry to the random seed manager
     *
     *  @param  entry a hash ideintifying the entry
     */
    void addEntry( HashArgument arg ) ;

    /**
     *  @brief  Generate a random seed map.
     *  Mainly used by the whiteboard to get random seeds
     *  which are local for a given event context
     *
     *  @param  evt the event source
     */
    std::unique_ptr<RandomSeedMap> generateRandomSeeds( const EVENT::LCEvent * const evt ) ;

  private:
    /**
     *  @brief  Get a new random number from the internal generator
     */
    SeedType getRandom() ;

  private:
    /// The global random seed, if set
    SeedType                _globalSeed {0} ;
    /// The entry list
    EntryList               _entryList {} ;
    /// The random generator engine
    RandomGenerator         _generator {_globalSeed} ;
    /// The random number distribution
    RandomDistribution      _rdmDistribution {MinSeed, MaxSeed} ;
  };

} // end namespace marlin

#endif
