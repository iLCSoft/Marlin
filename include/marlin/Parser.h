#ifndef Parser_h
#define Parser_h 1

//#include "lcio.h"
#include "IParser.h"
#include "StringParameters.h"

#include <fstream>
#include <string>
#include <iostream>

namespace marlin{

class LCTokenizer ;


typedef std::map< std::string ,  StringParameters* > StringParametersMap ;


/** Simple parser class for Marlin.
 *  Creates Parameter objects for all sections in a steering file
 *  defined by enclosing <br>
 *  .begin SectionName <br>  
 *  .end  <br>
 *  Parameters are defined for every line that doesn't start with #.
 *  The first string is the name of the parameter (key) the rest of the line
 *  is interpreted as the list of values separated by whitespace.
 *  Values from multiple lines starting with the same name/key are appended
 *  to the corresponding list.
 *
 *  @author F. Gaede, DESY
 *  @version $Id: Parser.h,v 1.3 2005-10-11 12:56:28 gaede Exp $ 
 */

class Parser : public IParser {
  

public:
  

  //  Parser(const std::string[] & namespaces ) ;
  //   void parse( const std::string& fileName )
  
  Parser( const std::string& fileName ) ;
  virtual ~Parser() ; 

  StringParameters* getParameters( const std::string& sectionName ) const ;
  
  /** Parse the input file
   */
  void parse() ;


protected:

  /** Helper method that reads the next line from a stream
   * that is not empty or starts with a '#' 
   */
  int readNextValidLine( std::string& str , std::istream& stream) ;


  mutable StringParametersMap _map ;
  StringParameters* _current ;

  std::string _fileName ;

private:
  Parser() ;

};





/** Helper class for Parser
 */
class LCTokenizer{

  std::vector< std::string >& _tokens ;
  char _del ;
  char _last ;
 public:

  LCTokenizer( std::vector< std::string >& tokens, char del ) : _tokens(tokens) , _del(del), _last(del) {
  }


  void operator()(const char& c) { 

    if( c != _del  ) {

	if( _last == _del  ) {
	  _tokens.push_back("") ; 
	}
      _tokens.back() += c ;
      result() ;
    }
    _last = c ;

  } 

  ~LCTokenizer(){
  }
  
  std::vector<std::string> & result()  { 
    
    return _tokens ; 
    
  }
};


} // end namespace marlin 
#endif
