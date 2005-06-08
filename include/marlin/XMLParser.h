#ifndef XMLParser_h
#define XMLParser_h 1

#include <fstream>
#include <string>
#include <iostream>
#include "StringParameters.h"

class TiXmlNode ;
class TiXmlDocument ;

namespace marlin{

// class LCTokenizer ;


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
 */

class XMLParser {
  

public:


/** Helper class for XMLParser
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
  

  //  XMLParser(const std::string[] & namespaces ) ;
  //   void parse( const std::string& fileName )
  
  XMLParser( const std::string& fileName ) ;
  virtual ~XMLParser() ; 

  StringParameters* getParameters( const std::string& sectionName ) ;
  

protected:

  /** Helper method that reads the next line from a stream
   * that is not empty or starts with a '#' 
   */
//   int readNextValidLine( std::string& str , std::istream& stream) ;

  /**Parse the input file
   */
  void parse(const std::string& fileName ) ;

  /** Extracts all parameters from the given node and adss them to the current StringParameters object
   */
  void parametersFromNode(TiXmlNode* section) ;


  /** Helper method - replaces all <group/> tag with corresponding <processor/> tags 
   */
  void replacegroups(TiXmlNode* section) ;
    
  /** Helper method - finds child element of node with given type and attribute value. */
  TiXmlNode* findElement( TiXmlNode* node , const std::string& type, 
			  const std::string& attribute, const std::string& value ) ;



  /** Helper method - recursively moves processors from <if/> tags to top level (<execute/>) and
   *  adds corresponding conditions attribute 
   */
  void processconditions( TiXmlNode* current , const std::string& conditions ) ;




  StringParametersMap _map ;
  StringParameters* _current ;
  TiXmlDocument* _doc ;

private:
  XMLParser() ;

};

} // end namespace marlin 
#endif
