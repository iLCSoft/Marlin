#ifndef CaloMapProcessor_h
#define CaloMapProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>

#include "ChannelToCellID.h"

#include "lccd/ConditionsMap.hh"


using namespace lcio ;
using namespace marlin ;



  typedef lccd::ConditionsMap<int,ChannelToCellID> ChMap ;


/** Test processor for conditions data with channel to cell ID mapping.
 */
class CaloMapProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new CaloMapProcessor ; }
  
  
  CaloMapProcessor() ;
  
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

  /** Input collection name. */
  std::string _colName ;

  /** the map */
  ChMap* _chMap ;

  int _nRun ;
  int _nEvt ;
} ;

#endif



