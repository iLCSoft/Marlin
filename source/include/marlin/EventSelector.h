#ifndef EventSelector_h
#define EventSelector_h 1

#include "marlin/Processor.h"
#include "marlin/EventModifier.h"
#include "lcio.h"
#include <string>
#include <set>
#include <map>  // pair

using namespace lcio ;
using namespace marlin ;


/** Simple event selector processor. Returns true if the given event 
 *  was specified in the EvenList parameter.
 * 
 *  <h4>Output</h4> 
 *  returns true or false
 * 
 * @param  EventList:   pairs of: EventNumber RunNumber
 * 
 * @author F. Gaede, DESY
 * @version $Id:$ 
 */

class EventSelector : public Processor, public marlin::EventModifier {
  
  typedef std::set< std::pair< int, int > > SET ;

 public:
  
  virtual Processor*  newProcessor() { return new EventSelector ; }
  
  
  EventSelector() ;
  
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
  
  
  virtual void modifyEvent( LCEvent *evt ) ;
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;
  
  virtual const std::string & name() const { return Processor::name() ; }
  
  
 protected:

  /** Input collection name.
   */
  IntVec _evtList{};
  SET _evtSet{};

  int _nRun=-1;
  int _nEvt=-1;
} ;

#endif



