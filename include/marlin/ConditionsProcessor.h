#ifndef ConditionsProcessor_h
#define ConditionsProcessor_h 1

#ifdef USE_LCCD

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>


using namespace lcio ;
using namespace marlin ;



/** Processor that provides access to conditions data in a Marlin application.
 *  Uses LCCD ConditionsHandler.
 */
class ConditionsProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new ConditionsProcessor ; }
  
  
  ConditionsProcessor() ;
  
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
  
  /** Default database init string */
  std::string _dbInit ;

  /** initialization of SimpleFileHandlers */
  StringVec _simpleHandlerInit ;

  /** initialization of DBCondHandlers */
  StringVec _dbcondHandlerInit ;

  /** initialization of DBFileHandlers */
  StringVec _dbfileHandlerInit ;

  /** initialization of DataFileHandlers */
  StringVec _datafileHandlerInit ;

  int _nRun ;
  int _nEvt ;

} ;

#endif

#endif // #ifdef USE_LCCD


