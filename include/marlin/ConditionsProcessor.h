#ifndef ConditionsProcessor_h
#define ConditionsProcessor_h 1

#ifdef USE_LCCD

#include "lccd/IConditionsChangeListener.hh"

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>



using namespace lcio ;
using namespace marlin ;



/** Processor that provides access to conditions data in a Marlin application.
 *  Uses LCCD ConditionsHandler. For every ConditionsHandler you have to specify a 
 *  line in the steering file. Use MyMarlin -l to get examples for the steering
 *  parameters.<br>
 *  The conditions data are added to the event as LCCollections with their name.
 *  Optionally IConditionsChangeListener subclasses, e.g. ConditionsMaps can be  
 *  registered for conditions data 'name', these are updated whenever the 
 *  data changes.
 */

class ConditionsProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new ConditionsProcessor ; }
  
  
  ConditionsProcessor() ;
  

  /** Registers an IConditionsChangeListener, e.g. a ConditionsMap with the conditions handler 'name'.
   *  Returns true if successfull.
   */
  static bool registerChangeListener( lccd::IConditionsChangeListener* cl, const std::string&  name) ;


  /** Initializes conditions handlers as defined in the steering file
   */
  virtual void init() ;
  
//   /** Called for every run.
//    */
//   virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Updates all registered conditions handlers and adds the data to the event.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
//   virtual void check( LCEvent * evt ) ; 
//   /** Called after data processing for clean up.
//    */
//   virtual void end() ;
  
  
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


