#ifndef XMLFixCollTypes_h
#define XMLFixCollTypes_h 1


#include <fstream>
#include <string>
#include <iostream>

class TiXmlNode ;
class TiXmlDocument ;

namespace marlin{



  /** Internal helper class that creates a new xml steering file 
   *  with the parameter attributes lcioInType and lcioOutType for the
   *  processors as defined by the Processor subclasses.
   *   This is used by Marlin -f.
   *
   * @author F. Gaede, DESY
   * @version $Id: XMLFixCollTypes.h,v 1.2 2007-03-06 08:05:07 gaede Exp $ 
   */

  class XMLFixCollTypes {
    
    
  public:
    
    

    XMLFixCollTypes( const std::string& fileName ) ;
    virtual ~XMLFixCollTypes() ; 


    /** Parse the input file and write the fixed outputfile to filename */
    void parse(const std::string& filename ) ;
    
  protected:


    /** Return named attribute - throws ParseException if attribute doesn't exist */
    const char* getAttribute( TiXmlNode* node , const std::string& name ) ;

    
    /** Helper method - finds child element of node with given type and attribute value. */
    TiXmlNode* findElement( TiXmlNode* node , const std::string& type, 
			    const std::string& attribute, const std::string& value ) ;


    TiXmlDocument* _doc ;

    std::string _fileName ;

  private:
    XMLFixCollTypes() ;

  };

} // end namespace marlin 
#endif
/**
 * <p>
 * The &lt;processor name="..." type="..." &gt; section defines the processor and its parameters,
 * where name and type are required attibutes.
 * 
 */
 
