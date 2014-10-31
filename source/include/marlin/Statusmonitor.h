#ifndef Statusmonitor_h
#define Statusmonitor_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>

using namespace lcio ;
using namespace marlin ;

class Statusmonitor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new Statusmonitor ; }
  
  
  Statusmonitor() ;

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
  int _howOften;

} ;


#endif



