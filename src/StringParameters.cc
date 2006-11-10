#include "marlin/StringParameters.h"

#include <sstream>
#include <algorithm>

namespace marlin{

StringParameters::StringParameters(){

}

StringParameters::StringParameters( const StringParameters& sp ){
    _map=sp._map;
}

StringParameters::~StringParameters(){

}


void StringParameters::add( const std::string& key , const std::vector<std::string> & values ) {

  for(unsigned int i=0 ; i< values.size() ; i++){
    
    _map[ key ].push_back( values[i] ) ;
  }
}

void StringParameters::add( const std::vector<std::string> & tokens ) {
  
  for(unsigned int i= 1 ; i< tokens.size() ; i++){
    
    _map[ tokens[0]  ].push_back( tokens[i] ) ;
  }
}

void StringParameters::erase( const std::string& key ) {
    
  _map.erase( key );
}

bool StringParameters::isParameterSet(  const std::string& key ) {

  return _map[ key ].size() > 0 ;
}

int StringParameters::getIntVal( const std::string& key ) {
  
  if( _map[ key ].size() == 0 ) return 0 ;

  return intVal( _map[ key ][0] ) ;
}

float StringParameters::getFloatVal( const std::string& key ) {

  if( _map[ key ].size() == 0 ) return 0 ;
  
  return floatVal(  _map[ key ][0] ) ;
}


const std::string& StringParameters::getStringVal( const std::string& key )  {

  static std::string empty("")  ;
  if( _map[ key ].size() == 0 ) return empty ;
  return  _map[ key ][0] ;
}



IntVec& StringParameters::getIntVals( const std::string& key ,  IntVec& intVec ) {

  transform(  _map[ key ].begin() , _map[ key ].end()  , back_inserter(intVec) , intVal ) ;
  
  return intVec ;
}


FloatVec&  StringParameters::getFloatVals( const std::string& key  ,  FloatVec& floatVec ){

  transform(  _map[ key ].begin() , _map[ key ].end()  , back_inserter(floatVec) , floatVal ) ;

  return floatVec ;
}


StringVec& StringParameters::getStringVals( const std::string& key  , StringVec& stringVec ) {

  copy( _map[ key ].begin() , _map[ key ].end()  , back_inserter(stringVec) ) ;

  return  stringVec ;
}

StringVec& StringParameters::getStringKeys( StringVec& stringVec ) {

  for(ParametersMap::iterator m = _map.begin() ; m != _map.end() ; m++){
    stringVec.push_back( m->first ); 
  }
  
  return  stringVec ;
}

int intVal(  const std::string& str ){
  // FIXME - add support for hex numbers 
  return atoi( str.c_str() )  ; 
}

float floatVal( const std::string& str ){
  return atof( str.c_str() )  ; 
}

std::ostream& operator<< (  std::ostream& s,  StringParameters& p ) {

  for(ParametersMap::iterator m = p._map.begin() ; m != p._map.end() ; m++){

    s << "      " << m->first << ": " ;  

    for(unsigned int i=0; i < m->second.size(); i++ ){

      s << "[" << m->second[i] << "]  " ;
      
    }
    s << std::endl ;
    
  }
  return s ;
}

} //namsepace marlin
