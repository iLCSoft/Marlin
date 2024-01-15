#ifndef Statusmonitor_h
#define Statusmonitor_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>

  /** Simple processor for writing out a status message every n-th event.
   *
   *  <h4>Input - Prerequisites</h4>
   *  none
   *  <h4>Output</h4> 
   *  none
   * @parameter HowOften  print run and event number for every HowOften-th event
   * 
   * @author A.Sailer CERN
   * @version $Id:$
   */
class Statusmonitor : public marlin::Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new Statusmonitor ; }
  
  
  Statusmonitor() ;

  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( EVENT::LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( EVENT::LCEvent * evt ) ; 
  
  
  virtual void check( EVENT::LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;
  
  
 protected:

  int _nRun=0;
  int _nEvt=0;
  int _howOften=10000;

} ;


#endif



