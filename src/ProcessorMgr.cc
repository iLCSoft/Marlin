#include "ProcessorMgr.h"
#include <iostream>
#include <algorithm>



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
  
bool ProcessorMgr::addActiveProcessor( const std::string& processorType , const std::string& processorName ) {

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
  }

  return true ;
}


void ProcessorMgr::init(){ 

  for_each( _list.begin() , _list.end() , std::mem_fun( &Processor::init ) ) ;
}

void ProcessorMgr::processRunHeader( LCRunHeader* run){ 

  for_each( _list.begin() , _list.end() ,  std::bind2nd(  std::mem_fun( &Processor::processRunHeader ) , run ) ) ;
}   

void ProcessorMgr::processEvent( LCEvent* evt ){ 

  for_each( _list.begin() , _list.end() ,   std::bind2nd(  std::mem_fun( &Processor::processEvent ) , evt ) ) ;
}

void ProcessorMgr::end(){ 

  for_each( _list.begin() , _list.end() ,  std::mem_fun( &Processor::end ) ) ;
}

 
