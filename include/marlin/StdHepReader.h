#ifndef StdHepReader_h
#define StdHepReader_h 1

#include "marlin/DataSourceProcessor.h"

using namespace lcio ;


namespace marlin{
  
  /** Example processor for reading non-LCIO input files - creates events with
   *  MCParticle collections from binary StdHep files.
   */
  
  class StdHepReader : public DataSourceProcessor {
    
  public:

    StdHepReader() ;

    virtual StdHepReader*  newProcessor() ;


    /** Creates events with MCParticle collections from the StdHep input file and
     *  calls all active processors' processEvent() and processRunHeader Method.
     */
    virtual void readDataSource( int numEvents ) ;
    
    
    virtual void init() ;
    virtual void end() ;
    
  protected:
    
    std::string _fileName ;

  };
 
} // end namespace marlin 

#endif
