#ifndef LCIOOutputProcessor_h
#define LCIOOutputProcessor_h 1

#include "Processor.h"
#include "lcio.h"
#include "IO/LCWriter.h"


#define LCIOOUTPUTFILE "LCIOOutputFile"
#define LCIOWRITEMODE  "LCIOWriteMode"


using namespace lcio ;

namespace marlin{
/** Default output processor. If active every event is writen to the 
 *  specified LCIO file.  
 *  Make sure that the processor is the last one called.
 *
 * @parameter LCIOOutputFile  name of outputfile incl. path
 * @parameter LCIOWriteMode   WRITE_NEW, WRITE_APPEND  [optional]
 */
class LCIOOutputProcessor : public Processor {
  
public:  

  virtual Processor*  newProcessor() { return new LCIOOutputProcessor ; }
  
  
  LCIOOutputProcessor() ;

  /**Open the LCIO outputfile.
   */
  virtual void init() ;

  /** Write every run header.
   */
  virtual void processRunHeader( LCRunHeader* run) ;

  /** Write every event.
   */
  virtual void processEvent( LCEvent * evt ) ; 

  /** Close outputfile.
   */
  virtual void end() ;



protected:

  LCWriter* _lcWrt ;
  int _nRun ;
  int _nEvt ;
} ;

} // end namespace marlin 
#endif



