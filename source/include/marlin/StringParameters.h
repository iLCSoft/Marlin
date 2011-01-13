#ifndef StringParameters_h
#define StringParameters_h 1

#include "lcio.h"

#ifdef LCIO_MAJOR_VERSION 
 #if LCIO_VERSION_GE( 1,2)  
  #include "LCIOSTLTypes.h"
 #endif
#else
  #include "MarlinLCIOSTLTypes.h"
#endif


#include <vector>
#include <string>
#include <map>
#include <iostream>


typedef std::map< std::string ,  std::vector< std::string > >  ParametersMap ;


using namespace lcio ;

namespace marlin{

/** Simple parameters class for Marlin.
 *  Holds named parameters as string vectors.
 *  @author F. Gaede, DESY
 *  @version $Id: StringParameters.h,v 1.5 2006-11-10 11:56:07 engels Exp $ 
 */

class StringParameters {
  
  friend std::ostream& operator<< (  std::ostream& , StringParameters& ) ;

public:
  
  StringParameters() ; 
  StringParameters( const StringParameters &sp ) ; 

  void add( const std::string& key , const std::vector<std::string> & values ) ;
  void add( const std::vector<std::string> & tokens ) ;

  void erase( const std::string& key ) ;

  bool isParameterSet(  const std::string& key ) ;

  int   getIntVal( const std::string& key ) ;
  float getFloatVal( const std::string& key ) ;
  const std::string& getStringVal( const std::string& key ) ;

  IntVec& getIntVals( const std::string& key ,  IntVec& intVec ) ;
  FloatVec& getFloatVals( const std::string& key ,  FloatVec& floatVec ) ;
  StringVec & getStringVals( const std::string& key  , StringVec & stringVec ) ;

  StringVec& getStringKeys( StringVec& stringVec );
  
  virtual ~StringParameters() ; 
  
protected:
  ParametersMap _map ;

};

int intVal( const std::string& str) ;
float floatVal( const std::string& str) ;
std::ostream& operator<< (  std::ostream& s, StringParameters& p ) ;

} // end namespace marlin 
#endif
