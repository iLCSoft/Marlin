#include "marlin/ProcessorMgr.h"
#include "marlin/Global.h"
#include "marlin/Exceptions.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>

#include "marlin/DataSourceProcessor.h"
#include "marlin/EventModifier.h"
#include "streamlog/streamlog.h"
#include "streamlog/logbuffer.h"

namespace marlin{

  ProcessorMgr* ProcessorMgr::_me = 0 ;



  struct ProcMgrStopProcessing : public StopProcessingException {
    ProcMgrStopProcessing(const std::string m){
      StopProcessingException::message = m  ; 
    }
  };


  // create a dummy streamlog stream for std::cout 
  streamlog::logstream my_cout ;


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
  
  void ProcessorMgr::readDataSource( int numEvents ) {

    for(  ProcessorList::iterator it = _list.begin() ;
	  it != _list.end() ; it++ ){

      DataSourceProcessor* dSP = dynamic_cast<DataSourceProcessor*>( *it ) ; 
      
      if( dSP != 0 )
	dSP->readDataSource( numEvents ) ;

    }
  }


  void  ProcessorMgr::dumpRegisteredProcessors() {

    typedef ProcessorMap::iterator MI ;
    
    std::cout  << "  ##########################################" << std::endl
	       << "  #                                        #" << std::endl
	       << "  #     Example steering file for marlin   #" << std::endl
	       << "  #                                        #" << std::endl
	       << "  ##########################################" << std::endl
	       <<  std::endl ;
    
    std::cout  << ".begin Global  ---------------------------------------" << std::endl
	       << "   LCIOInputFiles simjob.slcio " << std::endl 
	       << std::endl
	       << "  # the active processors that are called in the given order" << std::endl
	       << "   ActiveProcessors MyAIDAProcessor" << std::endl
	       << "   ActiveProcessors MyTestProcessor" << std::endl
	       << "   ActiveProcessors MyLCIOOutputProcessor" << std::endl
	       << std::endl
	       << "  # limit the number of processed records (run+evt):" << std::endl
	       << "   MaxRecordNumber 5001" << std::endl
	       << std::endl
	       << "  # skip the first  n events  " << std::endl
	       << "  SkipNEvents  0 " << std::endl
	       << "  # don't call the check method of the processors if \"true\"" << std::endl
	       << "   SupressCheck false" << std::endl
	       << ".end   -----------------------------------------------" << std::endl
	       <<  std::endl 
	       <<  std::endl ;
    
    
    for(MI i=_map.begin() ; i!= _map.end() ; i++) {
      i->second->printDescription() ;
    }
  }
  void  ProcessorMgr::dumpRegisteredProcessorsXML() {

    typedef ProcessorMap::iterator MI ;
    
    std::cout  << "<!--##########################################" << std::endl
	       << "    #                                        #" << std::endl
	       << "    #     Example steering file for marlin   #" << std::endl
	       << "    #                                        #" << std::endl
	       << "    ##########################################-->" << std::endl
	       <<  std::endl ;

    std::cout  <<  std::endl 
	       << "<marlin xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
           << "xsi:noNamespaceSchemaLocation=\"http://ilcsoft.desy.de/marlin/marlin.xsd\">"
	       <<  std::endl ;

    std::cout  <<  " <execute>" << std::endl 
	       <<  "  <processor name=\"MyAIDAProcessor\"/>" << std::endl
	       <<  "  <processor name=\"MyTestProcessor\"/>  " << std::endl
	       <<  "  <processor name=\"MyLCIOOutputProcessor\"/>  " << std::endl
	       <<  " </execute>" << std::endl
	       << std::endl ;

    std::cout  <<  " <global>" << std::endl 
	       <<  "  <parameter name=\"LCIOInputFiles\"> simjob.slcio </parameter>" << std::endl
	       <<  "  <!-- limit the number of processed records (run+evt): -->  " << std::endl
	       <<  "  <parameter name=\"MaxRecordNumber\" value=\"5001\" />  " << std::endl
	       <<  "  <parameter name=\"SkipNEvents\" value=\"0\" />  " << std::endl
	       <<  "  <parameter name=\"SupressCheck\" value=\"false\" />  " << std::endl
#ifdef USE_GEAR
	       <<  "  <parameter name=\"GearXMLFile\"> gear_ldc.xml </parameter>  " << std::endl
#endif
	       <<  "  <parameter name=\"Verbosity\" options=\"DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT\"> DEBUG  </parameter> " << std::endl
	       <<  " </global>" << std::endl
	       << std::endl ;

    for(MI i=_map.begin() ; i!= _map.end() ; i++) {
      i->second->printDescriptionXML() ;
    }

    std::cout  <<  std::endl 
	       << "</marlin>" 
	       <<  std::endl ;

  }

  std::set< std::string > ProcessorMgr::getAvailableProcessorTypes(){
      
     std::set< std::string > ptypes;
     
     for(ProcessorMap::iterator i=_map.begin() ; i!= _map.end() ; i++) {
       ptypes.insert(i->first);
     }
    return ptypes;
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
					 StringParameters* parameters ,
					 const std::string condition) {

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
      _conditions.addCondition( processorName, condition ) ;

      if( parameters != 0 ){
	newProcessor->setParameters( parameters  ) ;
      }
//       // keep a copy of the output processor
//       if( processorType == "LCIOOutputProcessor" ){
// 	_outputProcessor = dynamic_cast<LCIOOutputProcessor*>( newProcessor ) ;
//       }
    }

    return true ;
  }


  void ProcessorMgr::init(){ 

    streamlog::logbuffer* lb = new streamlog::logbuffer( std::cout.rdbuf() ,  &my_cout ) ;
    std::cout.rdbuf(  lb ) ;

//     for_each( _list.begin() , _list.end() , std::mem_fun( &Processor::baseInit ) ) ;
    
    for( ProcessorList::iterator it = _list.begin() ; it != _list.end() ; ++it ) {

      streamlog::logscope scope( streamlog::out ) ; scope.setName(  (*it)->name()  ) ;
      streamlog::logscope scope1(  my_cout ) ; scope1.setName(  (*it)->name()  ) ;

      (*it)->baseInit() ;
    }

  }

  void ProcessorMgr::processRunHeader( LCRunHeader* run){ 


#ifdef USE_GEAR
    // check if gear file is consistent with detector model in lcio run header 
    std::string lcioDetName = run->getDetectorName() ;

    
    std::string gearDetName("unknwon_gear_detector") ;

    bool doConsistencyCheck = true ;

    try{

      gearDetName = Global::GEAR->getDetectorName()  ; 

    }
    catch( gear::UnknownParameterException ){ 

      streamlog_out( WARNING ) << std::endl
			       << " ======================================================== " << std::endl
			       << "   Detector name  not found in Gear file     " << std::endl
			       << "    - can't check consistency with lcio file " << std::endl
			       << " ======================================================== " << std::endl
			       << std::endl ;

      doConsistencyCheck = false ;
    }



    if( doConsistencyCheck  && lcioDetName != gearDetName ) {

      std::stringstream sstr ;

      sstr  << std::endl
	    << " ============================================================= " << std::endl
	    << " ProcessorMgr::processRunHeader : inconsistent detector models : " << std::endl 
	    << " in lcio : " << lcioDetName << " <=> in gear file : "  << gearDetName << std::endl
	    << " ============================================================= " << std::endl
	    << std::endl ;
      
      //throw lcio::Exception( sstr.str() )  ;

      throw ProcMgrStopProcessing( sstr.str() ) ;
    }
      


#endif
    
//     for_each( _list.begin() , _list.end() ,  std::bind2nd(  std::mem_fun( &Processor::processRunHeader ) , run ) ) ;
    for( ProcessorList::iterator it = _list.begin() ; it != _list.end() ; ++it ) {

      streamlog::logscope scope( streamlog::out ) ; scope.setName(  (*it)->name()  ) ;
      streamlog::logscope scope1(  my_cout ) ; scope1.setName(  (*it)->name()  ) ;
      
      (*it)->processRunHeader( run ) ;
    }
  }   


//   void ProcessorMgr::modifyEvent( LCEvent * evt ) { 
//     if( _outputProcessor != 0 )
//       _outputProcessor->dropCollections( evt ) ;
//   }
  


  void ProcessorMgr::modifyEvent( LCEvent* evt ){ 

    static bool first = true  ;
    typedef std::vector<EventModifier*> EMVec ;
    static  EMVec  emv ;

    if( first ) {

      for(  ProcessorList::iterator it = _list.begin() ;
	    it != _list.end() ; it++ ){
	
	EventModifier* em = dynamic_cast<EventModifier*>( *it ) ; 
	
	if( em != 0 ) {
	  emv.push_back( em ) ;	
	  streamlog_out( WARNING4 ) << " -----------   " << std::endl
				    << " the following processor will modify the LCIO event :  "
				    << (*it)->name()  << " !! " <<  std::endl
				    << " ------------  "   << std::endl ; 
	}
      }

      first = false ;
    }

    for( EMVec::iterator it = emv.begin();  it !=  emv.end()  ; ++ it) {

      streamlog::logscope scope( streamlog::out ) ; scope.setName(  (*it)->name()  ) ;

      streamlog::logscope scope1(  my_cout ) ; scope1.setName(  (*it)->name()  ) ;

      (*it)->modifyEvent( evt ) ;
    }
    //    for_each( emv.begin() , emv.end() ,   std::bind2nd(  std::mem_fun( &EventModifier::modifyEvent ) , evt ) ) ;

  }


  void ProcessorMgr::processEvent( LCEvent* evt ){ 

//     static bool isFirstEvent = true ;

//     for_each( _list.begin() , _list.end() ,   std::bind2nd(  std::mem_fun( &Processor::processEvent ) , evt ) ) ;

//     if( Global::parameters->getStringVal("SupressCheck") != "true" ) {
      
//       for_each( _list.begin() , _list.end(), 
// 		std::bind2nd( std::mem_fun( &Processor::check ) , evt ) ) ;
//     }
    
//     if ( isFirstEvent ) {
//       isFirstEvent = false;
//       for_each( _list.begin(), _list.end() , 
// 		std::bind2nd( std::mem_fun( &Processor::setFirstEvent ),isFirstEvent )) ;
//     }

    _conditions.clear() ;

    bool check = ( Global::parameters->getStringVal("SupressCheck") != "true" ) ;


    try{ 

      for( ProcessorList::iterator it = _list.begin() ; it != _list.end() ; ++it ) {
	
	if( _conditions.conditionIsTrue( (*it)->name() ) ) {
	  
	  streamlog::logscope scope( streamlog::out ) ; scope.setName(  (*it)->name()  ) ;

	  streamlog::logscope scope1(  my_cout ) ; scope1.setName(  (*it)->name()  ) ;



	  (*it)->processEvent( evt ) ; 
	  
	  if( check )  (*it)->check( evt ) ;
	  
	  (*it)->setFirstEvent( false ) ;
	}       
      }    
    } catch( SkipEventException& e){

      ++ _skipMap[ e.what() ] ;
    }  
  }
  

  void ProcessorMgr::setProcessorReturnValue( Processor* proc, bool val ) {

    _conditions.setValue( proc->name() , val ) ;

  }
  void ProcessorMgr::setProcessorReturnValue( Processor* proc, bool val, 
					      const std::string& name){
    
    std::string valName = proc->name() + "." + name ;
    _conditions.setValue( valName , val ) ;
  }
  
  void ProcessorMgr::end(){ 

//     for_each( _list.begin() , _list.end() ,  std::mem_fun( &Processor::end ) ) ;

//    for_each( _list.rbegin() , _list.rend() ,  std::mem_fun( &Processor::end ) ) ;

    for( ProcessorList::reverse_iterator it = _list.rbegin() ; it != _list.rend() ; ++it ) {

      streamlog::logscope scope( streamlog::out ) ; scope.setName(  (*it)->name()  ) ;
      streamlog::logscope scope1(  my_cout ) ; scope1.setName(  (*it)->name()  ) ;
      
      (*it)->end() ;
    }
//     if( _skipMap.size() > 0 ) {
      std::cout  << " --------------------------------------------------------- " << std::endl
		 << "  Events skipped by processors : " << std::endl ;

      unsigned nSkipped = 0 ;
      for( SkippedEventMap::iterator it = _skipMap.begin() ; it != _skipMap.end() ; it++) {

	std::cout << "       " << it->first << ": \t" <<  it->second << std::endl ;

	nSkipped += it->second ;	
      }
      std::cout  << "  Total: " << nSkipped  << std::endl ;
      std::cout  << " --------------------------------------------------------- "  
		 << std::endl
		 << std::endl ;
//     }

  }

 
} // namespace marlin
