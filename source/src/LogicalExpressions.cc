#include "marlin/LogicalExpressions.h"
#include "marlin/Exceptions.h"
#include "marlin/Logging.h"

#include <iostream>
#include <sstream>
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
  }

  void LogicalExpressions::clear() {
    for( ResultMap::iterator it = _resultMap.begin() ; it != _resultMap.end() ; it++){
      it->second = false ;
    }
    setValue("true",true);
    setValue("True",true);
    setValue("false",false);
    setValue("False",false);
  }

  bool LogicalExpressions::conditionIsTrue( const std::string& name ) const {
    auto iter = _condMap.find( name ) ;
    // RE: This method is now const. The old logic of
    // a condition not in the map was to return true.
    // Keep this logic here
    if( _condMap.end() == iter ) {
      return true ;
    }
    return expressionIsTrue( iter->second ) ;
  }

  bool LogicalExpressions::expressionIsTrue( const std::string& expression ) const {
    std::vector<Expression> tokens ;
    Tokenizer t( tokens ) ;
    std::for_each( expression.begin(),expression.end(), t ) ;

    // atomic expression
    if( tokens.size() == 1
	   && tokens[0].Value.find('&') == std::string::npos
	   && tokens[0].Value.find('|') == std::string::npos ) {
      if( tokens[0].isNot ) {
        return not getValue( tokens[0].Value ) ;
      }
      else {
        return  getValue( tokens[0].Value ) ;
      }
    }
    bool returnVal = true ;
    for( auto it = tokens.begin() ; it != tokens.end() ; it++ ) {
      bool tokenValue = expressionIsTrue( it->Value ) ;
      if( it->isNot ) {
        tokenValue = ! tokenValue ;
      }
      if( it->Operation == Expression::AND ) {
        returnVal &= tokenValue ;
      }
      else {
	       returnVal |= tokenValue ;
       }
    }
    return returnVal ;
  }

  void LogicalExpressions::setValue( const std::string& key, bool val ) {
    _resultMap[ key ] = val ;
  }

  bool LogicalExpressions::getValue( const std::string& key) const {
    auto it = _resultMap.find( key ) ;
    if (it == _resultMap.end() ) {
      std::ostringstream error;
      error << "LogicalExpressions::getValue():  key \"" << key << "\" not found. Bad processor condition?\n";
      //fg: debug:
      for( auto iter = _resultMap.begin() ; iter != _resultMap.end() ; ++iter ){
        streamlog_out( DEBUG ) << " key : " << iter->first << " val: " << iter->second << std::endl ;
      }
      throw marlin::ParseException( error.str() );
    }
    return it->second;
  }

}
