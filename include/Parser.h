#ifndef Parser_h
#define Parser_h 1

//#include "lcio.h"
#include <fstream>
#include <string>
#include <iostream>
#include "StringParameters.h"

class LCTokenizer ;

/** Simple parser class for Marlin.
 *  Creates LCParameter objects for all sections in a steering file
 *  defined by enclosing <br>
 *  .begin SectionName <br>  
 *  .end  <br>
 *  Parameters are defined for every line that doesn't start with #.
 *  The first string is the name of the parameter (key) the rest of the line
 *  is interpreted as the list of values separated by whitespace.
 *  Values from multiple lines starting with the same name/key are appended
 *  to the corresponding list.
 */


typedef std::map< std::string ,  StringParameters* > StringParametersMap ;

class Parser {
  

public:
  

  //  Parser(const std::string[] & namespaces ) ;
  //   void parse( const std::string& fileName )
  
  Parser( const std::string& fileName ) ;
  virtual ~Parser() ; 

  StringParameters* getParameters( const std::string& sectionName ) ;
  

protected:

  /** Helper method that reads the next line from a stream
   * that is not empty or starts with a '#' 
   */
  int readNextValidLine( std::string& str , std::istream& stream) ;

  /**Parse the input file
   */
  void parse(std::ifstream & inFile ) ;


  StringParametersMap _map ;
  StringParameters* _current ;

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


#endif
