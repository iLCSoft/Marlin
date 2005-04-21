#include "marlin/ProcessorMgr.h"
#include "marlin/Global.h"
#include <iostream>
#include <algorithm>

#include "marlin/DataSourceProcessor.h"


namespace marlin{

  ProcessorMgr* ProcessorMgr::_me = 0 ;



  ProcessorMgr* ProcessorMgr::instance() {
  
    if( _me == 0 ) 
      _me = new ProcessorMgr ;

    return _me ;
  }  

  void ProcessorMgr::registerProcessor( Processor* processor ){

    const std::string& name = processor->type()  ;

    if( _map.find( name ) != _map.end() ){

      //     std::cerr << " ProcessorMgr::registerProcessor: processor " <<  name 
      // 	      << " already registered ! " 
      // 	      << std::endl   ;
    
      return ;
    }
    else
    
      _map[ name ] = processor ;

  }
  
  void ProcessorMgr::readDataSource() {

    for(  ProcessorList::iterator it = _list.begin() ;
	  it != _list.end() ; it++ ){

      DataSourceProcessor* dSP = dynamic_cast<DataSourceProcessor*>( *it ) ; 
      
      if( dSP != 0 )
	dSP->readDataSource() ;

    }
  }


  void  ProcessorMgr::dumpRegisteredProcessors() {

    typedef ProcessorMap::iterator MI ;
    
    std::cout  <<  std::endl 
	       << "   Available processors : " 
	       <<  std::endl 
	       <<  std::endl ;

    for(MI i=_map.begin() ; i!= _map.end() ; i++) {
      i->second->printDescription() ;
    }
  }

  

  Processor* ProcessorMgr::getProcessor( const std::string& type ){
    return _map[ type ] ;
  }

  Processor* ProcessorMgr::getActiveProcessor( const std::string& name ) {
    return _activeMap[ name ] ;
  }

  void ProcessorMgr::removeActiveProcessor(  const std::string& name ) {

  
    _list.remove( _activeMap[name] ) ;
    _activeMap.erase( name ) ;

  }
  
  bool ProcessorMgr::addActiveProcessor( const std::string& processorType , 
					 const std::string& processorName , 
					 StringParameters* parameters ) {

    Processor* processor = getProcessor( processorType ) ;

    if( processor == 0 ) {
      std::cerr << " ProcessorMgr::registerProcessor: unknown processor with type " 
		<<  processorType  << " ! " 	      
		<< std::endl   ;
      return false ;
    }
  
    if( _activeMap.find( processorName ) != _activeMap.end() ){
    
      std::cerr << " ProcessorMgr::addActiveProcessor: processor " <<  processorName 
		<< " already registered ! "
		<< std::endl ;
      return false ;
    
    } else {
    
      Processor* newProcessor = processor->newProcessor() ;
      newProcessor->setName( processorName ) ;
      _activeMap[ processorName ] = newProcessor ;
      _list.push_back( newProcessor ) ;

      if( parameters != 0 ){
	newProcessor->setParameters( parameters  ) ;
      }
    }

    return true ;
  }


  void ProcessorMgr::init(){ 

    for_each( _list.begin() , _list.end() , std::mem_fun( &Processor::baseInit ) ) ;
  }

  void ProcessorMgr::processRunHeader( LCRunHeader* run){ 

    for_each( _list.begin() , _list.end() ,  std::bind2nd(  std::mem_fun( &Processor::processRunHeader ) , run ) ) ;
  }   

  void ProcessorMgr::processEvent( LCEvent* evt ){ 

    static bool isFirstEvent = true ;

    for_each( _list.begin(), _list.end() , 
	      std::bind2nd( std::mem_fun( &Processor::setFirstEvent ),isFirstEvent )) ;

    isFirstEvent = false ;

    for_each( _list.begin() , _list.end() ,   std::bind2nd(  std::mem_fun( &Processor::processEvent ) , evt ) ) ;

    if( Global::parameters->getStringVal("SupressCheck") != "true" )

      for_each( _list.begin() , _list.end(), 
		std::bind2nd( std::mem_fun( &Processor::check ) , evt ) ) ;

  }

  void ProcessorMgr::end(){ 

    for_each( _list.begin() , _list.end() ,  std::mem_fun( &Processor::end ) ) ;
  }

 
} // namespace marlin
