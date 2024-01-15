#ifndef TestProcessorEventSeeder_h
#define TestProcessorEventSeeder_h 1

#include "marlin/Processor.h"
#include "marlin/EventModifier.h"

#include "lcio.h"
#include <string>

#include <map>
#include <vector>


/**  test processor for testing the uniquie event seeding functionality of the ProcessorEventSeeder service in Marlin.
 * 
 * 
 * @author S. J. Aplin, DESY
 */

class TestProcessorEventSeeder : public marlin::Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new TestProcessorEventSeeder ; }
  
  
  TestProcessorEventSeeder() ;
    
  virtual const std::string & name() const { 
    static std::string myName("TestProcessorEventSeeder") ; 
    return myName ;  
  } 


 /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
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

  /** Input collection name.
   */
  std::string _colName="";

  std::map< unsigned long long, unsigned int>  _seeds{};

  int _nRun=-1;
  int _nEvt=-1;
} ;

#endif



