
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/ProcessorApi.h>
#include <marlin/Logging.h>
#include <marlin/PluginManager.h>

// -- lcio headers
#include <lcio.h>

// -- std headers
#include <cmath>
#include <ctime>
#include <random>

namespace marlin {

  /** Simple processor crunching CPU time for n milliseconds.
   *
   *  <h4>Input - Prerequisites</h4>
   *  none
   *  <h4>Output</h4>
   *  none
   * @parameter CrunchTime the time in millisecond to crunch CPU
   *
   * @author R.Ete, DESY
   */
  class CPUCrunchingProcessor : public Processor {

   public:
    CPUCrunchingProcessor() ;
    void init() ;
    void processEvent( EVENT::LCEvent * evt ) ;

  private:
    Property<int> _crunchTime {this, "CrunchTime",
             "The crunching time (unit ms)", 200 } ;

    Property<float> _crunchSigma {this, "CrunchSigma",
             "Smearing factor on the crunching time using a gaussian generator (unit ms)", 0 } ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  CPUCrunchingProcessor::CPUCrunchingProcessor() : 
    Processor("CPUCrunching") {
    // modify processor description
    _description = "CPUCrunchingProcessor crunch CPU time for n milliseconds" ;
  }

  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::init() {
    log<DEBUG>() << "CPUCrunchingProcessor::init() called" << std::endl ;
    // usually a good idea to
    printParameters() ;
    ProcessorApi::registerForRandomSeeds( this ) ;
  }

  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::processEvent( EVENT::LCEvent *event ) {
    auto randomSeed = ProcessorApi::getRandomSeed( this, event ) ;
    std::default_random_engine generator( randomSeed );
    std::normal_distribution<clock::duration_rep> distribution(0, _crunchSigma);
    clock::duration_rep totalCrunchTime = _crunchTime + distribution(generator) ;
    log<MESSAGE>() << "Will use total crunch time of " << totalCrunchTime << " ms" << std::endl ;
    // crunch for n milliseconds !
    clock::crunchFor<clock::milliseconds>(totalCrunchTime) ;
  }

  // processor declaration
  MARLIN_DECLARE_PROCESSOR( CPUCrunchingProcessor )
}
