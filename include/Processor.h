#ifndef Processor_h
#define Processor_h 1

#include "lcio.h"

#include "IO/LCRunListener.h"
#include "IO/LCEventListener.h"
#include "IO/LCReader.h"

#include "EVENT/LCEvent.h"
#include "EVENT/LCRunHeader.h"

#include "StringParameters.h"

using namespace lcio ;


 class ProcessorMgr ;


/** Base class for Marlin processors.
 * Users can optionaly overwrite the following methods:<br>
 * @see init 
 * @see processRun
 * @see processEvent
 * @see end
 */

class Processor {
  
  friend class ProcessorMgr ;

public:
  

  Processor(const std::string& typeName) ; 

  virtual ~Processor() ; 
  
  
  /**Return a new instance of the processor.
   * Has to be implemented by subclasses.
   */
  virtual Processor*  newProcessor() = 0 ;
  

  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() { }

  /** Called for every run, e.g. overwrite to initialize run dependent 
   *  histograms.
   */
  virtual void processRunHeader( LCRunHeader* run) { } 

  /** Called for every event - the working horse. 
   */
  virtual void processEvent( LCEvent * evt ) { }

  /** Called after data processing for clean up.
   */
  virtual void end(){ }
  

  /** Return type name for the processor (as set in constructor).
   */
  virtual const std::string & type() const { return _typeName ; } 

  /** Return  name of this  processor.
   */
  virtual const std::string & name() const { return _processorName ; } 

  virtual void setName( const std::string & name) { _processorName = name ; }
  

  /** Return parameters defined for this Processor.
   */
  virtual StringParameters* parameters() { return _parameters ; } 

  virtual void setParameters( StringParameters* parameters) ; 


//   virtual void modifyRunHeader( LCRunHeader* run) { /*no_op*/; }   
//   virtual void modifyEvent( LCEvent * evt ) { /*no_op*/ ;}

protected:

  std::string _typeName  ;
  std::string _processorName ;
  StringParameters* _parameters ;

private:
  Processor() ; 

};

#endif
