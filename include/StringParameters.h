#ifndef StringParameters_h
#define StringParameters_h 1

#include "lcio.h"
#include "LCIOSTLTypes.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>


typedef std::map< std::string ,  std::vector< std::string > >  ParametersMap ;


using namespace lcio ;


/** Simple parameters class for Marlin.
 *  Holds named parameters as string vectors.
 */

class StringParameters {
  
  friend std::ostream& operator<< (  std::ostream& , StringParameters& ) ;

public:
  
  StringParameters() ; 

  void add( const std::string& key , const std::vector<std::string> & values ) ;
  void add( const std::vector<std::string> & tokens ) ;


  int   getIntVal( const std::string& key ) ;
  float getFloatVal( const std::string& key ) ;
  const std::string& getStringVal( const std::string& key ) ;

  IntVec& getIntVals( const std::string& key ,  IntVec& intVec ) ;
  FloatVec& getFloatVals( const std::string& key ,  FloatVec& floatVec ) ;
  StringVec & getStringVals( const std::string& key  , StringVec & stringVec ) ;
  
  virtual ~StringParameters() ; 
  
  
protected:
  ParametersMap _map ;

};

int intVal( const std::string& str) ;
float floatVal( const std::string& str) ;
std::ostream& operator<< (  std::ostream& s, StringParameters& p ) ;

#endif
