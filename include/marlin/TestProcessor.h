#ifndef TestProcessor_h
#define TestProcessor_h 1

#include "Processor.h"
#include "lcio.h"

using namespace lcio ;


namespace marlin {


  /** Simple processor for testing - writes something to stdout for every callback
   *  method.
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

  int _nRun ;
  int _nEvt ;
} ;

} // end namespace marlin 
#endif



