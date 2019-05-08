
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Logging.h>

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
    Processor *newProcessor() { return new CPUCrunchingProcessor ; }
    void init() ;
    void processEvent( EVENT::LCEvent * evt ) ;

  private:
    // processor parameters
    int    _crunchTime {200} ; // unit ms
    float  _crunchSigma {0} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  CPUCrunchingProcessor::CPUCrunchingProcessor() : Processor("CPUCrunching") {

    // modify processor description
    _description = "CPUCrunchingProcessor crunch CPU time for n milliseconds" ;

    registerProcessorParameter("CrunchTime",
  			     "The crunching time (unit ms)",
  			     _crunchTime,
  			     _crunchTime ) ;

    registerProcessorParameter("CrunchSigma",
            "Smearing factor on the crunching time using a gaussian generator (unit ms)",
            _crunchSigma,
            _crunchSigma ) ;
  }

  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::init() {
    log<DEBUG>() << "CPUCrunchingProcessor::init() called" << std::endl ;
    // usually a good idea to
    printParameters() ;
    log<MESSAGE>() << "Will use total crunch time of " << _totalCrunchTime << " ms" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::processEvent( EVENT::LCEvent *) {
    std::default_random_engine generator(std::hash<void*>()(this));
    std::normal_distribution<clock::duration_rep> distribution(0, _crunchSigma);
    clock::duration_rep totalCrunchTime = _crunchTime + distribution(generator) ;
    // crunch for n milliseconds !
    clock::crunchFor<clock::milliseconds>(totalCrunchTime) ;
  }

  // processor declaration
  CPUCrunchingProcessor aCPUCrunchingProcessor ;

}
