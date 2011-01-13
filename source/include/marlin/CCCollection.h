#ifndef CCCOLLECTION_H
#define CCCOLLECTION_H

#include <string>

namespace marlin {

  class CCProcessor;
 
/**handles information about LCIO collections
 * needed by MarlinSteerCheck
 *
 * @author Jan Engels, Benjamin Eberhardt, DESY
 * @version $Id: CCCollection.h,v 1.8 2007-01-11 16:56:37 engels Exp $
 */

  class CCCollection{
    
  public:

    //constructor
    CCCollection( const std::string& value="", const std::string& type="", const std::string& name="", CCProcessor* srcProc=NULL );
    
    //copy constructor
    CCCollection( const CCCollection &c );
    
    //destructor
    ~CCCollection();
    
    const std::string& getValue() const { return _value; }
    const std::string& getType() const { return _type; }
    const std::string& getName() const { return _name; }

    /** Returns the source CCProcessor associated to this collection */
    CCProcessor* getSrcProc(){ return _srcProc; } 
    
    void setValue( const std::string& value );
    void setType( const std::string& type );
    void setName( const std::string& name );
    
    /** Sets the source CCProcessor associated to this collection */
    void setSrcProc( CCProcessor* srcProc );
    
  private:
    
    std::string _value;	    // lcio collection name
    std::string _type;	    // lcio collection type
    std::string _name;	    // parameter "name" of the collection in the xml file
    CCProcessor* _srcProc;  // CCProcessor associated to this collection
  };

} // namespace
#endif
