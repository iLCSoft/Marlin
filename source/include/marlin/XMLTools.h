#ifndef MARLIN_XMLTOOLS_h
#define MARLIN_XMLTOOLS_h 1

// -- std headers
#include <ostream>
#include <string>

namespace marlin {

  /**
   *  @brief  XMLTools
   *  Various helper function for XML files
   */
  class XMLTools {
  public:
    /**
     *  @brief  Dump the registered processors in the ostream
     *
     *  @param  stream the output stream
     */
    static void dumpRegisteredProcessors( std::ostream &stream ) ;

    /**
     *  @brief  Dump the registered processors in the XML file
     *
     *  @param  fname the output steering file name
     */
    static void dumpRegisteredProcessors( const std::string &fname ) ;
  };

}

#endif
