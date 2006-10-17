
#include "marlin/XMLFixCollTypes.h"
#include "marlin/Exceptions.h"
#include "marlin/tinyxml.h"
#include "marlin/ProcessorMgr.h"

#include <algorithm>
#include <set>

#include <sstream>

namespace marlin{

  // open steering file with processor names 
  XMLFixCollTypes::XMLFixCollTypes( const std::string&  fileName) :
    _fileName( fileName ) {
  }
  
  XMLFixCollTypes::~XMLFixCollTypes(){
  }
  
  void XMLFixCollTypes::parse(const std::string& outFileName ){
    
    
    _doc = new TiXmlDocument ;
    bool loadOkay = _doc->LoadFile(_fileName  ) ;
    
    if( !loadOkay ) {
      
      std::stringstream str ;
      
      str  << "XMLFixCollTypes::parse error in file [" << _fileName 
	   << ", row: " << _doc->ErrorRow() << ", col: " << _doc->ErrorCol() << "] : "
	   << _doc->ErrorDesc() ;
      
      throw ParseException( str.str() ) ;
    }
    
    //     TiXmlHandle docHandle( _doc );
    
    TiXmlElement* root = _doc->RootElement();
    if( root==0 ){
      
      throw ParseException(std::string( "XMLFixCollTypes::parse : no root tag <marlin>...</marlin> found in  ") 
			   + _fileName  ) ;
    }
    

    std::set<std::string> collectionNames ;

    TiXmlNode* section = 0 ;
    while( (section = root->IterateChildren("processor",  section ) )  != 0  ){
      

      std::string name( getAttribute( section, "name") ) ;

      std::string type=getAttribute( section, "type")  ;      

      
      Processor* processor =  ProcessorMgr::instance()->getProcessor( type )  ;
      
      std::cout << " processor : " << name << " ( type: " << type << " ) "  ; 

      if( processor != 0 ) {

	std::cout << std::endl ;

	// loop over parameters ------
	TiXmlNode* par = 0 ;
	while( ( par = section->IterateChildren( "parameter", par ) )  != 0  ){
	  
	  
	  //        std::cout << " parameter found : " << par->ToElement()->Attribute("name") << std::endl ;
	  
	  
	  std::string name( getAttribute( par, "name" )  ) ;
	  
	  
	  if( processor->isInputCollectionName( name ) ){

	    std::string inType("")  ;

	    try{  
	      
	      inType = getAttribute( par , "lcioInType" )  ; 
	      
// 	      std::cout << " ------ found lcioInType : " << inType  
// 			<< "   name : " << name 
// 			<< std::endl ;
	      
	      
	    }
	    
	    catch( ParseException ) { }
	    
	    
	    if( inType != processor->getLCIOInType( name )  ) {
	      
	      std::cout << "  -- fixing wrong lcioInType : " << inType 
			<< " to " << processor->getLCIOInType( name ) 
			<< std::endl ;          
	      
	      
	      par->ToElement()->SetAttribute( "lcioInType" , processor->getLCIOInType( name )  );
	    }
	    
	    collectionNames.insert( name ) ;

	  }

	  if( processor->isOutputCollectionName( name ) ){
	    
	    std::string outType("")  ;

	    try{  
	      
	      
	      outType = getAttribute( par , "lcioOutType" )  ; 
// 	      std::cout << " ------ found lcioOutType : " << outType
// 			<< "   name : " << name 
// 			<< std::endl ;
	      
	    }      
	    catch( ParseException ) { }
	    
	    
	    if( outType != processor->getLCIOOutType( name )  ) {
	      
	      std::cout << "  -- fixing wrong lcioOutType : " << outType 
			<< " to " << processor->getLCIOOutType( name ) 
			<< std::endl ;
	      
	      
	      par->ToElement()->SetAttribute( "lcioOutType" , processor->getLCIOOutType( name )  );
	    }

	    collectionNames.insert( name ) ;
	  }
	}
	// now add all new in and out collections:

	typedef ProcParamMap::const_iterator PMI ;
    
	for( PMI i = processor->procMap().begin() ; i != processor->procMap().end() ; i ++ ) {
	    
	  ProcessorParameter* p = i->second ;
	    
	  if ( ( processor->isInputCollectionName(  p->name() )     ||
		 processor->isOutputCollectionName( p->name() )  ) &&
	       collectionNames.find( p->name() ) == collectionNames.end() ) {
	      

	    TiXmlElement newParam( "parameter" ) ;

	    newParam.SetAttribute( "name" , p->name() ) ;

	    newParam.SetAttribute( "type" , p->type() ) ;

	    if( processor->isInputCollectionName(  p->name() ) ) 
	      newParam.SetAttribute( "lcioInType" , processor->getLCIOInType( p->name() ) ) ;
	    else
	      newParam.SetAttribute( "lcioOutType" , processor->getLCIOOutType( p->name() ) ) ;

	    newParam.SetAttribute( "value" , p->defaultValue() ) ;
	    

	    TiXmlNode* position = section->InsertEndChild( newParam ) ;

	    TiXmlComment comment ;
	    comment.SetValue(  p->description() ) ;
	    
	    section->InsertBeforeChild( position  , comment ) ;
	    
	    std::cout << "    --- adding new collection parameter '" << p->name() 
		      << " to processor " << std::endl ; 


// 	    std::cout << "  <!--" << p->description() << "-->" << std::endl ;
// 	    if( p->isOptional() ) {
// 	      std::cout << "  <!--parameter name=\"" << p->name() << "\" " 
// 			<< "type=\"" << p->type() ;
// 	      if ( processor->isInputCollectionName( p->name() ) )
// 		std::cout << "\" lcioInType=\"" << processor->getLCIOInType( p->name() )  ;
// 	      if ( processor->isOutputCollectionName( p->name() ) )
// 		std::cout << "\" lcioOutType=\"" <<  processor->getLCIOOutType( p->name() )  ;
// 	      std::cout << "\">"
// 			<< p->defaultValue() 
// 			<< " </parameter-->"
// 			<< std::endl ;
// 	    } else {
// 	      std::cout << "  <parameter name=\"" << p->name() << "\" " 
// 			<< "type=\"" << p->type() ;
// 	      if ( processor->isInputCollectionName( p->name() ) )
// 		std::cout << "\" lcioInType=\"" << processor->getLCIOInType( p->name() )  ;
// 	      if ( processor->isOutputCollectionName( p->name() ) )
// 		std::cout << "\" lcioOutType=\"" <<  processor->getLCIOOutType( p->name() )  ;
// 	      std::cout  << "\">"
// 			 << p->defaultValue() 
// 			 << " </parameter>"
// 			 << std::endl ;
// 	    }
	  }
	}


      } else {

	std::cout << " UNKNOWN ! " << std::endl ;
      }
      
    }

    _doc->SaveFile( outFileName ) ;
      
  }


  
  const char* XMLFixCollTypes::getAttribute( TiXmlNode* node , const std::string& name ){
    
    TiXmlElement* el = node->ToElement() ;
    if( el == 0 ) 
      throw ParseException("XMLFixCollTypes::getAttribute not an XMLElement " ) ;
    
    const char* at = el->Attribute( name.c_str() )  ;
    
    if( at == 0 ){

      std::stringstream str ;
      str  << "XMLFixCollTypes::getAttribute missing attribute \"" << name 
	   << "\" in element <" << el->Value() << "/> in file " << _fileName  ;
      throw ParseException( str.str() ) ;
    }

    return at ;

  }  


  
  TiXmlNode* XMLFixCollTypes::findElement( TiXmlNode* node , const std::string& type, 
			  const std::string& attribute, const std::string& value ) {
    
    TiXmlNode* child = 0 ;
    bool elementFound  = false ;
    
    while( (child = node->IterateChildren( type , child ) )  != 0  ){
      
      if( std::string( *child->ToElement()->Attribute( attribute ) ) == value ) { 
	elementFound = true ;
	break ;
      }
    }
    if( ! elementFound ) 
      child = 0 ;
    
    return child ;
  }  
  
}  // namespace marlin


