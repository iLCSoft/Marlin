#include "marlin/LogicalExpressions.h"
#include <iostream>

 
#include <algorithm>


namespace marlin{
  
  std::ostream& operator<< (  std::ostream& s,  Expression& e ) {
    
    if( e.Operation == Expression::AND ) s << " && "  ;
    else s << " || " ;
    if( e.isNot ) s << " ! " ;
    s << " [ " <<  e.Value << " ] "  << std::endl ;
    return s ;
  }

  LogicalExpressions::LogicalExpressions() {
    setValue("true",true);
    setValue("True",true);
    setValue("false",false);
    setValue("False",false);
  }
  
  
  void LogicalExpressions::addCondition( const std::string& name, const std::string& expression ) {
    _condMap[ name ] = expression ;

//     std::cout << " LogicalExpressions::addCondition( " << name << ", " << expression << " ) " << std::endl ;
  }
  
  void LogicalExpressions::clear() {

    for( ResultMap::iterator it = _resultMap.begin() ; it != _resultMap.end() ; it++){
      it->second = false ;
    }
    setValue("true",true);
    setValue("True",true);
    setValue("false",false);
    setValue("False",false);
//     std::cout << " LogicalExpressions::clear() "  << std::endl ;
  }
  
  bool LogicalExpressions::conditionIsTrue( const std::string& name ) {

    return expressionIsTrue( _condMap[ name ] ) ;
  }
  
  bool LogicalExpressions::expressionIsTrue( const std::string& expression ) {

    std::vector<Expression> tokens ;
    Tokenizer t( tokens ) ;
    
    std::for_each( expression.begin(),expression.end(), t ) ; 
    
    // atomic expression
    if( tokens.size() == 1 
	&& tokens[0].Value.find('&') == std::string::npos 
	&& tokens[0].Value.find('|') == std::string::npos ) { 
      
      if( tokens[0].isNot ) {
// 	std::cout << " evaluated !"<< tokens[0].Value << " to "  << ! _resultMap[ tokens[0].Value ] << std::endl ;
	return ! _resultMap[ tokens[0].Value ] ;
      }
      else {
// 	if(  _resultMap.find( tokens[0].Value ) == _resultMap.end() ) 
// 	  std::cout << " error in result map : " <<  tokens[0].Value << " not found !"  << std::endl ;
// 	std::cout << " evaluated "<< tokens[0].Value << " to "  <<  _resultMap[ tokens[0].Value ] << std::endl ;
	return _resultMap[ tokens[0].Value ] ;
      }
    }	
	  
    bool returnVal = true ;

    for(   std::vector<Expression>::iterator it = tokens.begin() ; it != tokens.end() ; it++ ){

      bool tokenValue = expressionIsTrue( it->Value ) ;

      if( it->isNot ) 
	tokenValue = ! tokenValue ;

      if( it->Operation == Expression::AND ) 
	returnVal &= tokenValue ;
      else
	returnVal |= tokenValue ;
    }

//     std::cout << " expression given : " << expression << std::endl ;
//     std::cout << " evaluated by parser:" <<  std::endl ;
//     for(   std::vector<Expression>::iterator it = tokens.begin() ; it != tokens.end() ; it++ ){
//       std::cout << *it ;
//     }
//     std::cout << " Result " << returnVal << std::endl << std::endl ;

    return returnVal ;
  }
  
  void LogicalExpressions::setValue( const std::string& key, bool val ) {

//     std::cout << " LogicalExpressions::setValue() "  << key << " - " << val << std::endl ;

    _resultMap[ key ] = val ;
  }
  
//     ConditionsMap _condMap ;
//     ResultMap _resultMap ;

}

