#include "marlin/XMLParser.h"

#include <algorithm>

#include "tinyxml.h"
#include <sstream>

namespace marlin{

  // open steering file with processor names 
  XMLParser::XMLParser( const std::string&  fileName) :
    _current(0) 
  {
    
    parse(  fileName ) ;
    
  }
  
  XMLParser::~XMLParser(){
  }
  
  void XMLParser::parse( const std::string&  fileName ){
    
    
//     TiXmlDocument _doc( fileName );
    _doc = new TiXmlDocument ;
    bool loadOkay = _doc->LoadFile(fileName  ) ;
    
    if( !loadOkay ) {
      std::cout << "XMLParser::parse : Could not open file: " << fileName << std::endl ;
      return ;
    }
    
    TiXmlHandle docHandle( _doc );
    
//     TiXmlElement* root = _doc->FirstChildElement("marlin");
    TiXmlElement* root = _doc->RootElement();
    if( root==0 ){
      std::cout << "XMLParser::parse : no root tag <marlin>...</marlin> found in  " << fileName << std::endl ;
      return ;
    }
    
    TiXmlNode* section = 0 ;
    

    _map[ "Global" ] = new StringParameters() ;

    section = root->FirstChild("global")  ;
    if( section != 0  ){
      
//       section->InsertEndChild( activeProcessorItem );
//       std::cout << " global: \n" << *section << std::endl ;
      _current =  _map[ "Global" ] ;
//       _current = new StringParameters() ;
//       _map[ "Global" ] = _current ;
      parametersFromNode( section ) ;
    } 

// create global parameter ActiveProcessors from <execute> section
//     TiXmlElement activeProcessorItem( "parameter" );
//     activeProcessorItem.SetAttribute( "name", "ActiveProcessors" );
    
    std::vector<std::string> activeProcs ;
    activeProcs.push_back("ActiveProcessors") ;

    std::vector<std::string> procConds ;
    procConds.push_back("ProcessorConditions") ;

    section = root->FirstChild("execute")  ;
    if( section != 0  ){
      
      // preprocess groups: replace with <processor/> tags
      replacegroups(  section )  ;

      // process processor conditions:
      processconditions( section , "" ) ;

//       std::cout <<  " execute after group replacement : " << *section << std::

      TiXmlNode* proc = 0 ;
      while( ( proc = section->IterateChildren( "processor", proc ) )  != 0  ){

	activeProcs.push_back( proc->ToElement()->Attribute("name") ) ;

	std::string condition(  proc->ToElement()->Attribute("condition")  ) ;

	if( condition.size() == 0 ) 
	  condition += "true" ;

	procConds.push_back( condition  ) ;

      }

    } else {

      std::cout << "XMLParser::parse : couldn't find section <execute> in file " << fileName << std::endl ;
    }

    _current->add( activeProcs ) ;
    _current->add( procConds ) ;



    // preprocess groups: ---------------------------------------------------------------------------------
    // simply copy all group parameters to the processors
    // and then copy the processors to the root node <Marlin>
    while( (section = root->IterateChildren( "group", section ) )  != 0  ){
      
      std::vector<TiXmlNode*> groupParams ;

      TiXmlNode* param = 0 ;
      while( ( param = section->IterateChildren( "parameter" , param ) )  != 0  ){
	groupParams.push_back( param->Clone() ) ;
      }

      TiXmlNode* proc = 0 ;
      while( ( proc = section->IterateChildren( "processor" , proc ) )  != 0  ){
	
	for( std::vector<TiXmlNode*>::iterator it = groupParams.begin() ; it != groupParams.end() ; it++){
	  proc->InsertEndChild( **it ) ;
	}
	std::auto_ptr<TiXmlNode> clone( proc->Clone() ) ;
	root->InsertBeforeChild( section , *clone  ) ;   // FIXME: memory mngmt. ?
      }

      root->RemoveChild( section ) ;

      for( std::vector<TiXmlNode*>::iterator it = groupParams.begin() ; it != groupParams.end() ; it++){
	delete *it ;
      }
    }
    
    
    // process processor sections -------------------------------------------------------------------------
    while( (section = root->IterateChildren("processor",  section ) )  != 0  ){
      
//       std::cout << " processor found: " << section->Value() << std::endl ;
      
      _current = new StringParameters() ;
      
      _map[ section->ToElement()->Attribute("name") ] =  _current ;
      

//       TiXmlElement item( "parameter" );
//       item.SetAttribute( "name", "ProcessorType" );
//       TiXmlText text(  section->ToElement()->Attribute("type") );
//       item.InsertEndChild( text );
//       section->InsertEndChild( item );

      // add ProcessorType to processor parameters
      std::vector<std::string> procType(2)  ;
      procType[0] = "ProcessorType" ;
      procType[1] =  section->ToElement()->Attribute("type")  ;
      _current->add( procType ) ;

      parametersFromNode( section ) ;
    }




    // DEBUG:

    _doc->SaveFile( "debug.xml" ) ;

  }
  
  void XMLParser::parametersFromNode(TiXmlNode* section) { 
    
    TiXmlNode* par = 0 ;
    while( ( par = section->IterateChildren( "parameter", par ) )  != 0  ){
      
      
//        std::cout << " parameter found : " << par->ToElement()->Attribute("name") << std::endl ;
      
      std::vector<std::string> tokens ;
      tokens.push_back( std::string(par->ToElement()->Attribute("name"))  ) ; // first token is parameter name 
      LCTokenizer t( tokens ,' ') ;
      
      std::string inputLine("") ;
      
      if( par->ToElement()->Attribute("value") != 0 ) {
	inputLine = par->ToElement()->Attribute("value") ;
      }
      else if( par->FirstChild() ) {
	inputLine =  par->FirstChild()->Value() ;
      }
      
//        std::cout << " values : " << inputLine << std::endl ;
    
       std::for_each( inputLine.begin(),inputLine.end(), t ) ; 

//        for( StringVec::iterator it = tokens.begin() ; it != tokens.end() ; it++ ){
//  	std::cout << "  " << *it ;
//        } 
//        std::cout << std::endl ;
      
      _current->add( tokens ) ;

    }
  }
  
  //   TiXmlElement* child2 = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).Child( "Child", 1 ).Element();
  //   if ( child2 ){}

  StringParameters* XMLParser::getParameters( const std::string& sectionName ) {
    
//     for( StringParametersMap::iterator iter = _map.begin() ; iter != _map.end() ; iter++){
//       //     std::cout << " parameter section " << iter->first 
//       // 	      << std::endl 
//       // 	      << *iter->second 
//       // 	      << std::endl ;
//     }
    
    return _map[ sectionName ] ;
  }
  

  void XMLParser::processconditions( TiXmlNode* current , const std::string& aCondition ) {
    
    std::string condition ;
    // put parentheses around compound expressions 
    if( aCondition.find('&') != std::string::npos  ||  aCondition.find('|') != std::string::npos ) 
      condition = "(" + aCondition + ")" ;
    else
      condition = aCondition ;

    TiXmlNode* child = 0 ;
    while( ( child = current->IterateChildren( "if" , child )  )  != 0  ){
      
      processconditions( child , child->ToElement()->Attribute("condition") ) ;  
    }

    while( ( child = current->IterateChildren( "processor" , child )  )  != 0  ) {
      

      if(  child->ToElement()->Attribute("condition") == 0 ) {
	
	child->ToElement()->SetAttribute("condition" ,  condition ) ;

      } else {

	std::string cond( child->ToElement()->Attribute("condition") ) ; 

	if( cond.size() > 0 && condition.size() ) 
	  cond += " && " ;

	cond += condition ;
	
	child->ToElement()->SetAttribute("condition" ,  cond ) ;

      }
       

      if( std::string( current->Value() ) != "execute" ) {

	// unless we are already in the top node (<execute/>) we have to move all processors up

	TiXmlNode* parent = current->Parent() ;

	std::auto_ptr<TiXmlNode> clone( child->Clone() ) ;

	parent->InsertBeforeChild(  current , *clone ) ;  

      }

    }
    // remove the current <if/> node
    if( std::string( current->Value() ) != "execute" ) {
      TiXmlNode* parent = current->Parent() ;
      parent->RemoveChild( current ) ;
    }    
    
  }



  void XMLParser::replacegroups(TiXmlNode* section) {
    
    TiXmlElement* root = _doc->RootElement()  ;
    if( root==0 ){
      std::cout << "XMLParser::parse : no root tag <marlin>...</marlin> found ! " << std::endl ;
      return ;
    }

    TiXmlNode* child = 0 ;
    while( ( child = section->IterateChildren( child ) )  != 0  ){
      
      if( std::string( child->Value() )  == "group" ) {
	
	// find group definition in root node 
	TiXmlNode* group = findElement( root, "group", "name" , child->ToElement()->Attribute("name") ) ;
	
	if( group != 0 ) {
	  
	  TiXmlNode* sub = 0 ;
	  while( ( sub = group->IterateChildren( "processor" , sub ) )  != 0  ){
	    
	    // insert <processor/> tag
	    TiXmlElement item( "processor" );
	    item.SetAttribute( "name",  sub->ToElement()->Attribute("name") ) ;

	    section->InsertBeforeChild( child , item ) ;
	      
// 	      std::cout << " inserting processor tag for group : " << item.Value() << ", " 
// 			<< item.Attribute("name") << std::endl ;
	  }

	  section->RemoveChild( child ) ; 

	} else
	  
	  std::cout << " XMLParser::parse - group not found : " <<  child->ToElement()->Attribute("name") << std::endl ;

      } else  if( std::string( child->Value() )  == "if" )  {  // other element, e.g. <if></if>

	replacegroups( child ) ;

      }
    }
  }
  
  
  TiXmlNode* XMLParser::findElement( TiXmlNode* node , const std::string& type, 
			  const std::string& attribute, const std::string& value ) {
    
    TiXmlNode* child = 0 ;
    bool elementFound  = false ;
    
    while( (child = node->IterateChildren( type , child ) )  != 0  ){
      
      if( std::string( child->ToElement()->Attribute( attribute ) ) == value ) { 
	elementFound = true ;
	break ;
      }
    }
    if( ! elementFound ) 
      child = 0 ;
    
    return child ;
  }  
  
}  // namespace marlin


