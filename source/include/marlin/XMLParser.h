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

  typedef std::map< std::string ,  StringParameters* > StringParametersMap ;

  /** XML parser for Marlin steering files.
   *  Marlin XML steering files have the following form (use <b>Marlin -x > example.xml</b> to 
   *  generate a template steering file):<br>
   * 
   *  <pre>
   *  &lt;marlin&gt;
   *   &lt;execute&gt;   [1]
   *      ...  // the processors and processor groups to be executed
   *   &lt;/execute&gt;
   *   &lt;global&gt;    [1]
   *      ...  // global parameter section
   *   &lt;/global&gt;
   *   &lt;processor&gt; [n]
   *      ...  // definition of the processor and its parameters
   *   &lt;/processor&gt;
   *   &lt;group&gt; [m]   
   *      ...    // a group of processors
   *     &lt;processor&gt; [k]
   *      ...  // definition of the processor and its parameters
   *     &lt;/processor&gt;
   *   &lt;/group&gt; 
   *  &lt;/marlin&gt;
   * </pre>
   * where the numbers enclosed in "[]" denote how many elements of the given type are allowed 
   * or required, n,m,k = [0,1,2,...]. 
   * <p>
   * The &lt;execute/&gt; section defines the processors that are to be called in the order specified. 
   * The Processor::processEvent() method is only called if the relevant condition (&lt;if condition="A"/&gt;)
   * is fullfilled. Conditions can be arbitrary logical expressions formed of [!,(,&&,||,),value], e.g.<br>
   *  ( A && ( B || !C ) ) || ( !B && D ), where the keys A,B,C,D can be either procesor names 
   * (Processor::setReturnValue(bool val) ) or processor names followed by a string 
   * ( Processor::setReturnValue(const std::string &name, bool val) ). <br>
   * In the following example the Pflow processor is only called for events where the EventSelection processor
   * has returnd true and the TwoJetAnalysis is is then in turn only called for events identified as 
   * having tow jets by the Pflow processor
   * 
   * <pre>
   *  &lt;execute&gt;
   *   &lt;processor name="MyAIDAProcessor"/&gt;
   *   &lt;processor name="EventSelection"/&gt;  
   *   &lt;if condition="EventSelection"&gt;
   *     &lt;processor name="Pflow"/&gt;  
   *     &lt;if condition="Pflow.TwoJet"&gt;
   *       &lt;group name="TwoJetAnalysis"/&gt;
   *     &lt;/if&gt;
   *   &lt;/if&gt;
   *   &lt;processor name="MyLCIOOutputProcessor"/&gt;  
   *  &lt;/execute&gt;   
   *  </pre>
   * The &lt;global&gt; section defines the global paramters:
   * <pre>
   *  &lt;global&gt;
   *    &lt;parameter name="LCIOInputFiles"&gt;dd_rec.slcio &lt;/parameter&gt;
   *    &lt;parameter name="LCIOInputFiles"&gt;../tt500_all_set1_12.slcio &lt;/parameter&gt;
   *    &lt;parameter name="MaxRecordNumber" value="5001" /&gt;  
   *    &lt;parameter name="SupressCheck" value="false" /&gt;  
   *  &lt;/global&gt;
   * </pre>
   * <b>Parameters can be either specified as the content of the &lt;parameter/&gt; tag or in the 
   * value-attribute of the tag !</b>
   * <p>
   * The &lt;processor name="..." type="..." &gt; section defines the processor and its parameters,
   * where name and type are required attibutes, e.g.
   * <pre>
   *  &lt;processor name="EventSelection" type="SelectionProcessor"&gt;
   *    &lt;parameter name="EnergyCut" type="float"&gt;50.0&lt;/parameter&gt;
   *  &lt;/processor&gt;
   * </pre>
   * Note: the parameter's type-attribute is optional.
   * <p>
   * Processor sections can be enclosed in a  &lt;group/&gt; tag, where parameters defined outside any
   * &lt;processor/&gt; tag are group parameters valid for all processors in the group, .e.g.
   * <pre>
   *  &lt;group&gt;
   *    &lt;parameter name="PtCut" value="0.03"&gt;
   *    &lt;processor name="TrackFinding" type="TrackFinder"/&gt;
   *    &lt;processor name="TrackFitting" type="KalmanProcessor"&gt;
   *      &lt;parameter name="UseDAF" value="true"&gt;
   *    &lt;processor&gt;
   *  &lt;/group&gt;
   * </pre>
   * 
   * @author F. Gaede, DESY
   * @version $Id: XMLParser.h,v 1.7 2007-07-18 12:43:09 engels Exp $ 
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
  

    XMLParser( const std::string& fileName, bool forCCheck=false ) ;
    virtual ~XMLParser() ; 


    /** Parse the input file */
    void parse() ;

    /** Return the StringParameters for the section as read from the xml file */
    StringParameters* getParameters( const std::string& sectionName ) const ;
  

  protected:

    /** Extracts all parameters from the given node and adss them to the current StringParameters object
     */
    void parametersFromNode(TiXmlNode* section, std::pair<unsigned,unsigned>* typeCount=0) ;

    /** Return named attribute - throws ParseException if attribute doesn't exist */
    const char* getAttribute( TiXmlNode* node , const std::string& name ) ;

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
    bool _forCCheck; //boolean variable set to true if parser is used for consistency checking

  };

} // end namespace marlin 
#endif
/**
 * <p>
 * The &lt;processor name="..." type="..." &gt; section defines the processor and its parameters,
 * where name and type are required attibutes.
 * 
 */
 
