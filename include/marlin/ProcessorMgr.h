#ifndef ProcessorMgr_h
#define ProcessorMgr_h 1

#include "lcio.h"
#include "Processor.h"

#include "IO/LCRunListener.h"
#include "IO/LCEventListener.h"

#include "EVENT/LCEvent.h"
#include "EVENT/LCRunHeader.h"

#include <map>
#include <list>

using namespace lcio ;

namespace marlin{


typedef std::map< const std::string , Processor* > ProcessorMap ;
typedef std::list< Processor* > ProcessorList ;




/** Processor manager singleton class. Holds references to all registered Processors. 
 */
class ProcessorMgr : public LCRunListener, public LCEventListener {

friend class  Processor ;   

public:
  
  /** Return the instance of this manager.
   */
  static ProcessorMgr* instance() ;
  
  /** Add a processor of type processorType with name processorName to the list
   *  of active processors. Initializes the parameters (if != 0).
   */
  bool addActiveProcessor( const std::string& processorType , const std::string& processorName ,
			   StringParameters* parameters=0 ) ;
  
  /** Remove processor with name from list of active processors.
   */
  void removeActiveProcessor(  const std::string& name ) ;


  /** Return the active processor with the given name.
   * NULL if no processor exists. 
   */
  Processor* getActiveProcessor( const std::string& name ) ;

  /** Return the processor that has been registered with the given type.
   * NULL if no processor exists. 
   */
  Processor* getProcessor( const std::string& type ) ;
  
  
 
  virtual void init() ;
  virtual void processRunHeader( LCRunHeader* run) ;   
  virtual void processEvent( LCEvent* evt ) ;
  virtual void end() ;
  
  
  virtual void modifyRunHeader( LCRunHeader* run) { /*no_op*/; }   
  virtual void modifyEvent( LCEvent * evt ) { /*no_op*/ ;}


protected:
  /** Register a processor with the given name.
   */
  void registerProcessor( Processor* processor ) ;


private:
  static ProcessorMgr*  _me ;
  ProcessorMap _map ;
  ProcessorMap _activeMap ;
  ProcessorList _list ;
};
  
} // end namespace marlin 
#endif
