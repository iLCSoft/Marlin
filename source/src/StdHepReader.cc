
#include "marlin/StdHepReader.h"

#include "marlin/ProcessorMgr.h"

#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

#include "UTIL/LCStdHepRdr.h"
#include "UTIL/LCTOOLS.h"


namespace marlin{



  StdHepReader aStdHepReader ;
  
  
  StdHepReader::StdHepReader() : DataSourceProcessor("StdHepReader"),_fileName("") {
    
    _description = "Reads StdHep files as input and creates LCIO events with MCParticle collections."
      " Make sure to not specify any LCIOInputFiles in the steering in order to read StdHep files." ;
    
    registerProcessorParameter( "StdHepFileName" , 
				"input file"  ,
				_fileName ,
				std::string("input.stdhep") ) ;
    
  }
  
  StdHepReader*  StdHepReader::newProcessor() { 
    return new StdHepReader ;
  }
  
  void StdHepReader::init() {    
    printParameters() ;    
  }


  void StdHepReader::readDataSource( int numEvents ) {
    
    LCStdHepRdr* rdr = new  LCStdHepRdr( _fileName.c_str()  ) ;
    
    LCCollection* col ;
    LCEventImpl* evt ;

    int evtNum = 0 ;
    int runNum = 0 ;

    while( ( col = rdr->readEvent() ) != 0 ) {
      
      if ( numEvents > 0 && evtNum+1 > numEvents )
	{
	  delete col;
	  break;
	}
      
      if( isFirstEvent() ) {   // create run header
	
	LCRunHeaderImpl* rHdr = new LCRunHeaderImpl ;

	rHdr->setDescription( " Events read from stdhep input file: " + _fileName ) ; 
	rHdr->setRunNumber( runNum ) ;

	ProcessorMgr::instance()->processRunHeader( rHdr ) ;
	_isFirstEvent = false ;	
      }
      
      evt = new LCEventImpl ;
      evt->setRunNumber( runNum ) ;
      evt->setEventNumber( evtNum++ ) ;


      evt->addCollection(  col, "MCParticle"  ) ;
      
      ProcessorMgr::instance()->processEvent( evt ) ;

      delete evt ;
    }
    
    delete rdr ;
  }
  


  void StdHepReader::end() {

  }


}
