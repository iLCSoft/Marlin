#ifndef TestProcessor_h
#define TestProcessor_h 1

#include "Processor.h"
#include "lcio.h"

using namespace lcio ;


namespace marlin {
  
  
  /** Simple processor for testing. 
   *  Writes something to stdout for every callbackmethod.
   * 
   *  <h4>Input - Prerequisites</h4>
   *  none
   *
   *  <h4>Output</h4> 
   *  none
   * 
   * @author F. Gaede, DESY
   * @version $Id: TestProcessor.h,v 1.7 2007-05-23 13:12:21 gaede Exp $ 
   */

  class TestProcessor : public Processor {
    
  public:
    
    virtual Processor*  newProcessor() { return new TestProcessor ; }
    
    
    TestProcessor() ;
    
    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() ;
    
    /** Called for every run.
     */
    virtual void processRunHeader( LCRunHeader* run ) ;
    
    /** Called for every event - the working horse.
     */
    virtual void processEvent( LCEvent * evt ) ; 
    
    
    virtual void check( LCEvent * evt ) ; 
    
    
    /** Called after data processing for clean up.
     */
    virtual void end() ;
    
    
  protected:

    /**Test method for const.*/
    void printEndMessage() const ;

    int _nRun ;
    int _nEvt ;

    bool _doCalibration ;
    int _nLoops ;
  } ;
  
} // end namespace marlin 
#endif



