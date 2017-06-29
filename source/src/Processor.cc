#include "marlin/Processor.h"
#include "marlin/ProcessorMgr.h" 
#include "marlin/Global.h"
#include "marlin/VerbosityLevels.h"

using namespace lcio ;


namespace marlin{

  // set default verbosity level to MESSAGE
  //int Processor::Verbosity=Processor::MESSAGE;

  Processor::Processor(const std::string& typeName) :
    _description(" description not set by author ") ,
    _typeName( typeName ) ,
    _parameters(0) ,
    _isFirstEvent( true ),
    _logLevelName(""),
    _str(0) {
  
    //register processor in map
    ProcessorMgr::instance()->registerProcessor( this ) ;


    registerOptionalParameter( "Verbosity" , 
			       "verbosity level of this processor (\"DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT\")"  ,
			       _logLevelName ,  
			       std::string("DEBUG") ) ;
  
  }


  Processor::Processor() : _parameters(NULL), _isFirstEvent(false), _str(NULL) {}

  Processor::~Processor() {

    if( _str !=0 )
      delete _str ;
  
    typedef ProcParamMap::iterator PMI ;
  
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
    
      //     streamlog_out(DEBUG) << " deleting processor parameter " << i->second->name() 
      // 			 << " of processor [" << name() << "]" 
      // 			 << std::endl ;

      delete i->second ;
    }
  }


  void Processor::setParameters( std::shared_ptr<StringParameters> parameters) {

    _parameters = parameters ;

    updateParameters();

    // need to reset the log level name in case it has not been set by the user 
    if( !parameterSet("Verbosity") ){
      _logLevelName = "" ;
    }
  
    //  streamlog_out( MESSAGE)  << " setParameters :  Verbosity : " << _parameters->getStringVal("Verbosity") 
    // 			  << " parameterSet : " << parameterSet("Verbosity") 
    // 			  << " log level name : " << _logLevelName << std::endl ; 
    
  }
  
  std::stringstream& Processor::log() const {
    
    if( _str !=0 )
      delete _str ;

    _str = new std::stringstream ;

    return *_str ;
  }



  void Processor::printParameters() { printParameters<MESSAGE>() ;  }

  void Processor::printDescription() {
    
    std::cout << ".begin My"        <<  type()  << std::endl 
	      << "ProcessorType "   <<  type() << std::endl ;
    
    std::cout << "#---" << description() << std::endl ;
    
    typedef ProcParamMap::iterator PMI ;
    
    //     std::cout << std::endl  
    // 	      << "# processor parameters:" << std::endl ;
    
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
      
      ProcessorParameter* p = i->second ;

      std::cout << std::endl 
		<< "#\t"  << p->description() << std::endl 
		<< "#\t type: " << " [" <<  p->type() << "]" 
		<< std::endl ;

      if( p->isOptional() ) {
	
	std::cout << "#\t example: " << std::endl ;
	
	std::cout << "#\t"   << p->name()   
		  << "   "  << p->defaultValue() 
		  << std::endl 
		  << std::endl ;
	
      }else{

	std::cout << "#\t default: " << p->defaultValue() 
		  << std::endl ;
	
	std::cout << "\t"   << p->name()   
		  << "   "  << p->defaultValue() 
		  << std::endl 
		  << std::endl ;
      }
      
    }

    std::cout << ".end -------------------------------------------------" 
	      << std::endl 
	      << std::endl ;


  }

  void Processor::printDescriptionXML(std::ostream& stream) {

    if(&stream == &std::cout){
      stream << " <processor name=\"My" <<  type()  << "\"" 
	     << " type=\"" <<  type() << "\">" 
	     << std::endl ;
    }
    else{
      stream << " <processor name=\"" <<  name()  << "\"" 
	     << " type=\"" <<  type() << "\">" 
	     << std::endl ;

    }
    
    stream << " <!--" << description() << "-->" << std::endl ;
    
    typedef ProcParamMap::iterator PMI ;
    
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
      
      ProcessorParameter* p = i->second ;

      stream << "  <!--" << p->description() << "-->" << std::endl ;

      if( p->isOptional() ) {
	stream << "  <!--parameter name=\"" << p->name() << "\" " 
	       << "type=\"" << p->type() ;

	if ( isInputCollectionName( p->name() ) )
	  stream << "\" lcioInType=\"" << _inTypeMap[ p->name() ]  ;

	if ( isOutputCollectionName( p->name() ) )
	  stream << "\" lcioOutType=\"" << _outTypeMap[ p->name() ]  ;

	stream << "\">"
	       << p->defaultValue() 
	       << " </parameter-->"
	       << std::endl ;
      } else {
	stream << "  <parameter name=\"" << p->name() << "\" " 
	       << "type=\"" << p->type() ;

	if ( isInputCollectionName( p->name() ) )
	  stream << "\" lcioInType=\"" << _inTypeMap[ p->name() ]  ;

	if ( isOutputCollectionName( p->name() ) )
	  stream << "\" lcioOutType=\"" << _outTypeMap[ p->name() ]  ;

	stream  << "\">"
		<< p->defaultValue() 
		<< " </parameter>"
		<< std::endl ;
      }
    }
    
    stream << "</processor>" 
	   << std::endl 
	   << std::endl ;
    
  }

  //   ProcessorParameter* Processor::getProcessorParameter( const std::string name) {
  //     ProcParamMap::iterator it = _map.find(name) ;
  //     if( it != _map.end() )
  //       return it->second ;    
  //     else
  //       return 0 ;
  //   }

  bool Processor::parameterSet( const std::string& name ) {

    ProcParamMap::iterator it = _map.find(name) ;

    if( it != _map.end() )
      return it->second->valueSet() ;    
    else
      return false ;
  }
  
  void Processor::baseInit() {
    
    //fg: now in setParameters 
    // updateParameters();

    init() ;
  }
  
  void Processor::updateParameters() {

    typedef ProcParamMap::iterator PMI ;

    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {

      i->second->setValue( _parameters.get() ) ;
    }
  }

  void Processor::setReturnValue( bool val) {
    
    ProcessorMgr::instance()->setProcessorReturnValue(  this , val ) ;
  }
  
  void Processor::setLCIOInType(const std::string& collectionName,  const std::string& lcioInType) {
    _inTypeMap[ collectionName ] = lcioInType ;
  }

  std::string Processor::getLCIOInType( const std::string& colName ) {

    if( isInputCollectionName( colName )  )
      return _inTypeMap[ colName ] ;
    else
      return "" ;
  }


  std::string Processor::getLCIOOutType( const std::string& colName ) {

    if( isOutputCollectionName( colName )  )
      return _outTypeMap[ colName ] ;
    else
      return "" ;
  }


  bool Processor::isInputCollectionName( const std::string& pName  ) {
    return _inTypeMap.find( pName  ) != _inTypeMap.end() ;
  }
  
  void Processor::setLCIOOutType(const std::string& collectionName,  const std::string& lcioOutType) {
    _outTypeMap[ collectionName ] = lcioOutType ;
  }

  bool Processor::isOutputCollectionName( const std::string& pName  ) {
    return _outTypeMap.find( pName ) != _outTypeMap.end() ;
  }

  void Processor::setReturnValue( const std::string& name, bool val ){
  
    ProcessorMgr::instance()->setProcessorReturnValue(  this , val , name ) ;
  }


} // namespace marlin

