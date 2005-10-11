#ifndef IParser_h
#define IParser_h 1

#include <string>

namespace marlin{

  class StringParameters ;

  /** Interface for a parser of a steering file to be used with marlin 
   *
   *  @author F. Gaede, DESY
   *  @version $Id: IParser.h,v 1.2 2005-10-11 12:56:28 gaede Exp $ 
   */
  class IParser {

  public:
    
    virtual ~IParser() { /* no_op */ } 
    
    /** Parse the input file */
    virtual void parse() =0 ;

    /** Return the StringParameters defined for this section of the steering file */
    virtual StringParameters* getParameters( const std::string& sectionName ) const =0 ;
    
  };

}
#endif
