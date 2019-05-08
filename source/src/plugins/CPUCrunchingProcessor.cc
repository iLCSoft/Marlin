
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Logging.h>

// -- lcio headers
#include <lcio.h>

// -- std headers
#include <cmath>
#include <ctime>

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
    
    float  _totalCrunchTime {200} ;
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
    std::default_random_engine generator(std::hash<void*>()(this));
    std::normal_distribution<float> distribution(0, _crunchSigma);
    _totalCrunchTime = _crunchTime + distribution(generator) ;
    log<MESSAGE>() << "Will use total crunch time of " << _totalCrunchTime << " ms" << std::endl ;
  }
  
  //--------------------------------------------------------------------------

  void CPUCrunchingProcessor::processEvent( EVENT::LCEvent *) {
    auto start = clock() ;
    auto now = clock() ;
    // crunch for n milliseconds !
    while ( (now - start)*1000. / static_cast<double>(CLOCKS_PER_SEC) < _totalCrunchTime) {
      // crunch !
      (void)std::sqrt(2) ;
      // update time
      now = clock() ;
    }
  }
  
  // processor declaration
  CPUCrunchingProcessor aCPUCrunchingProcessor ;
  
}

