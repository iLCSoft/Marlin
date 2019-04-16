#include <marlin/RandomSeedManager.h>

// -- marlin headers
#include <jenkinsHash.h>
#include <marlin/Exceptions.h>

// -- lcio headers
#include <EVENT/LCEvent.h>

namespace marlin {

  RandomSeedManager::RandomSeedManager( SeedType globalSeed ) :
    _globalSeed( globalSeed ),
    _generator( _globalSeed ) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  void RandomSeedManager::addEntry( HashResult entry ) {
    bool inserted = _entryList.insert( entry ).second ;
    if ( not inserted ) {
      throw Exception("RandomSeedManager: Entry '" + std::to_string(entry) + "' already registered !") ;
    }
  }

  //--------------------------------------------------------------------------

  void RandomSeedManager::addEntry( HashArgument arg ) {
    HashFunction hashf ;
    addEntry( hashf(arg) ) ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<RandomSeedManager::RandomSeedMap>
  RandomSeedManager::generateRandomSeeds( const EVENT::LCEvent * const evt ) {
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
    _generator.seed( seed ) ;
    std::unique_ptr<RandomSeedMap> seedMap( new RandomSeedMap() ) ;
    // fill map with seeds for each entry using random number generator
    for (auto iter : _entryList ) {
      (*seedMap)[iter] = getRandom() ;
    }
    return seedMap ;
  }

  //--------------------------------------------------------------------------

  RandomSeedManager::SeedType RandomSeedManager::getRandom() {
    return static_cast<SeedType>(_rdmDistribution(_generator)) ;
  }

}
