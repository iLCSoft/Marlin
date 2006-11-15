#ifdef USE_LCCD
#include "marlin/ConditionsProcessor.h"

//--- LCIO headers 
//#include <UTIL/LCTOOLS.h>   // debug
//#include <IMPL/LCEventImpl.h> // debug

//--- LCCD headers
#include "lccd.h"
#include "lccd/SimpleFileHandler.hh"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBFileHandler.hh"
#include "lccd/DataFileHandler.hh"

#include "lccd/LCConditionsMgr.hh"

//#include <iostream>

using namespace lcio ;


namespace marlin{
  
  
  ConditionsProcessor aConditionsProcessor ;
  
  
  bool ConditionsProcessor::registerChangeListener( lccd::IConditionsChangeListener* cl, const std::string&  name) {

    try{ 
      lccd::LCConditionsMgr::instance()->registerChangeListener( cl , name ) ;
      return true ;
    }
    catch(Exception){}
    
    return false ;
  }


  ConditionsProcessor::ConditionsProcessor() : Processor("ConditionsProcessor") {
    
    // modify processor description
    _description = "ConditionsProcessor provides access to conditions data " 
      " transparently from LCIO files or a databases, using LCCD" ;
    
    
    // register steering parameters: name, description, class-variable, default value
    
#ifdef USE_CONDDB
    registerProcessorParameter( "DBInit" , 
				"Initialization string for conditions database"  ,
				_dbInit ,
				std::string("localhost:lccd_test:calvin:hobbes")) ;
    
    
    StringVec dbcondExample ;
    dbcondExample.push_back("conditionsName");
    dbcondExample.push_back("/lccd/myfolder");
    dbcondExample.push_back("HEAD");
    
    
    registerOptionalParameter( "DBCondHandler" , 
			       "Initialization of a conditions database handler"  ,
			       _dbcondHandlerInit ,
			       dbcondExample ,
			       dbcondExample.size() ) ;
    
#endif
    
    StringVec simpleExample ;
    simpleExample.push_back("conditionsName");
    simpleExample.push_back("conditions.slcio");
    simpleExample.push_back("collectionName");
    
    registerOptionalParameter( "SimpleFileHandler" , 
			       "Initialization of a simple conditions file handler"  ,
			       _simpleHandlerInit ,
			       simpleExample ,
			       simpleExample.size() ) ;
    
    StringVec dbfileExample ;
    dbfileExample.push_back("conditionsName");
    dbfileExample.push_back("conditions.slcio");
    dbfileExample.push_back("collectionName");
    
    
    registerOptionalParameter( "DBFileHandler" , 
			       "Initialization of a conditions db file handler"  ,
			       _dbfileHandlerInit ,
			       dbfileExample ,
			       dbfileExample.size() ) ;
    
    
    StringVec datafileExample ;
    datafileExample.push_back("conditionsName");
    
    registerOptionalParameter( "DataFileHandler" , 
			       "Initialization of a data file handler"  ,
			       _datafileHandlerInit ,
			       datafileExample ,
			       datafileExample.size() ) ;
    
  }
  
  
  void ConditionsProcessor::init() { 
    
    // usually a good idea to
    printParameters() ;
    
    _nRun = 0 ;
    _nEvt = 0 ;
    
    // initialize the requested conditions handler 
    
    if( parameterSet( "SimpleFileHandler" ) ) {
      
      unsigned index = 0 ;
      while( index < _simpleHandlerInit.size() ){
	
	std::string condName( _simpleHandlerInit[ index++ ] ) ; 
	std::string fileName( _simpleHandlerInit[ index++ ] ) ; 
	std::string colName ( _simpleHandlerInit[ index++ ] ) ; 
	
	lccd::LCConditionsMgr::instance()->
	  registerHandler( condName ,  new lccd::SimpleFileHandler( fileName, condName, colName ) ) ;

	_condHandlerNames.push_back( condName ) ;
      }
    }
    
#ifdef USE_CONDDB
    
    if( parameterSet( "DBCondHandler" ) ) {
      
      unsigned index = 0 ;
      while( index < _dbcondHandlerInit.size() ){
	
	std::string condName( _dbcondHandlerInit[ index++ ] ) ; 
	std::string folder  ( _dbcondHandlerInit[ index++ ] ) ; 
	std::string tag     ( _dbcondHandlerInit[ index++ ] ) ; 
	
	// HEAD corresponds to no tag in database
	if( tag == "HEAD" ) tag = "" ;
	
	lccd::LCConditionsMgr::instance()->
	  registerHandler( condName, new lccd::DBCondHandler( _dbInit , folder , condName, tag ) ) ;

	_condHandlerNames.push_back( condName ) ;
      }
    }
    
#endif
    
    if( parameterSet( "DBFileHandler" ) ) {
      
      unsigned index = 0 ;
      while( index < _dbfileHandlerInit.size() ){
	
	std::string condName( _dbfileHandlerInit[ index++ ] ) ; 
	std::string fileName( _dbfileHandlerInit[ index++ ] ) ; 
	std::string colName ( _dbfileHandlerInit[ index++ ] ) ; 
	
	lccd::LCConditionsMgr::instance()->
	  registerHandler( condName ,  new lccd::DBFileHandler( fileName, condName, colName ) ) ;

	_condHandlerNames.push_back( condName ) ;
      }
    }
    
    
    if( parameterSet( "DataFileHandler" ) ) {
      
      unsigned index = 0 ;
      while( index < _datafileHandlerInit.size() ){
	
	std::string condName ( _datafileHandlerInit[ index++ ] ) ; 
	
	lccd::LCConditionsMgr::instance()->
	  registerHandler( condName ,  new lccd::DataFileHandler( condName ) ) ;

	_condHandlerNames.push_back( condName ) ;
      }
    }
    
  }


//   void ConditionsProcessor::processRunHeader( LCRunHeader* run) { 
//     _nRun++ ;
//   } 
  
  void ConditionsProcessor::processEvent( LCEvent * evt ) { 
    
    static int lastEvent = -1 ;
    static int lastRun   = -1 ;
    
    // update all condition handlers - if not yet done by previous conditions processor

    if( evt->getEventNumber() != lastEvent  || evt->getRunNumber() != lastRun ){

      lccd::LCConditionsMgr::instance()->updateEvent(  evt ) ;

      lastEvent = evt->getEventNumber() ;
      lastRun = evt->getRunNumber() ;
    }
  }
  
  //   void ConditionsProcessor::check( LCEvent * evt ) { 
  //     // nothing to check here - could be used to fill checkplots in reconstruction processor
  //   }
  
  
  void ConditionsProcessor::end(){ 

    // delete all handlers
    for(unsigned int i=0; i< _condHandlerNames.size()  ; ++i){

      lccd::LCConditionsMgr::instance()->removeHandler( _condHandlerNames[i] );
    }
  }
  
}
  
#endif // #ifdef USE_LCCD
