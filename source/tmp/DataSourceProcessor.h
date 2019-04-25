#ifndef DataSourceProcessor_h
#define DataSourceProcessor_h 1

#include "marlin/Processor.h"

using namespace lcio ;


namespace marlin{
  
  /** Base class for data source handlers that can read arbitrary non-LCIO input files and 
   *  create LCIO events and run headers from that. If a user subclass is defined as active 
   *  processor in the steering file _and_ there are no LCIO input files defined than this 
   *  class is called to generate the LCIO data stream.
   *
   *  @author F. Gaede, DESY
   *  @version $Id: DataSourceProcessor.h,v 1.3 2005-10-11 12:56:28 gaede Exp $ 
   */
  
  class DataSourceProcessor : public Processor {
    
  public:
    
    DataSourceProcessor( const std::string& typeName ) ;


    /** User call back to read the data source and create the LCIO event and run header.
     *  The user needs to call ProcessMgr::processRunHeader(LCRunHeader* rhdr) and 
     *  ProcessMgr::processEvent(LCEvent* evt) in order to call the relevant Processor methods in 
     *  readDataSource().<br>
     *  \param numEvents The number of events the DataSourceProcessor should generate.
     */
    virtual void readDataSource( int numEvents ) = 0 ;

  };
 
} // end namespace marlin 

#endif
