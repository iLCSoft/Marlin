#ifndef XMLParser_h
#define XMLParser_h 1

#include "IParser.h"
#include "StringParameters.h"

#include <fstream>
#include <string>
#include <iostream>

class TiXmlNode ;
class TiXmlDocument ;

namespace marlin{

  // class LCTokenizer ;


  typedef std::map< std::string ,  StringParameters* > StringParametersMap ;


  /** XML parser for Marlin steering files.
   *  See Marlin main page documentation for details.
   */

  class XMLParser : public IParser {
  

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
  

    XMLParser( const std::string& fileName ) ;
    virtual ~XMLParser() ; 


    /** Parse the input file */
    void parse() ;

    /** Return the StringParameters for the section as read from the xml file */
    StringParameters* getParameters( const std::string& sectionName ) const ;
  

  protected:

    /** Extracts all parameters from the given node and adss them to the current StringParameters object
     */
    void parametersFromNode(TiXmlNode* section) ;

    /** Return named attribute - throws ParseException if attribute doesn't exist */
    const char* XMLParser::getAttribute( TiXmlNode* node , const std::string& name ) ;

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


    mutable StringParametersMap _map ;
    StringParameters* _current ;
    TiXmlDocument* _doc ;

    std::string _fileName ;

  private:
    XMLParser() ;

  };

} // end namespace marlin 
#endif
