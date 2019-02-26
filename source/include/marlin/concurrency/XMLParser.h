#ifndef MARLIN_CONCURRENCY_XMLPARSER_h
#define MARLIN_CONCURRENCY_XMLPARSER_h 1

// -- std headers
#include <bitset>

// -- marlin headers
#include "marlin/concurrency/Internal.h"
#include "marlin/tinyxml.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  XMLParser class.
     */
    class XMLParser {
    public:
      /**
       *  @brief  The XML parser options
       */
      enum class Option {
        INCLUDES = 0,    ///< whether to process XML includes
        NESTED_INCLUDES, ///< whether to process nested XML includes
        CONSTANTS,       ///< whether to perform replacement of constants
        ENV_VARS,        ///< whether to perform replacement of environment variables
        FOR_LOOPS,       ///< whether to process for loops
        N_OPTS           ///< the number of XMLParser options
      };

      typedef std::map<std::string, std::string> StringMap ;
      typedef std::bitset<static_cast<int>(Option::N_OPTS)> Options ;

    public:
      /**
       *  @brief  Constructor
       */
      XMLParser() ;

      /**
       *  @brief  Parse the xml file. See the different parsing options below
       *
       *  @param fname the xml file name
       */
      void parse(const std::string &fname) ;

      /**
       *  @brief  Get the last parsed file name
       */
      const std::string &fileName() const ;

      /**
       *  @brief  Get the constants map.
       */
      const StringMap &constants() const ;

      /**
       *  @brief  Get a constant value to specified type
       *
       *  @param  name the constant name
       */
      template <typename T>
      T constantAs(const std::string &name) const ;

      /**
       *  @brief  Get a constant value to specified type
       *
       *  @param  name the constant name
       *  @param  fallback fallback value if constant doesn't exists
       */
      template <typename T>
      T constantAs(const std::string &name, const T &fallback) const ;

      /**
       *  @brief  Get the xml document
       */
      const TiXmlDocument &document() const ;

      /**
       *  @brief  Get the xml document
       */
      TiXmlDocument &document() ;

      /**
       *  @brief  Reset the parser: clear the constants and the xml document
       */
      void reset() ;

      /**
       *  @brief  Set an XML parser option
       *
       *  @param  opt the XML parser option
       *  @param  set whether to set/unset this option
       */
      void setOption( Option opt, bool set = true ) ;

      /**
       *  @brief  Whether specified option is set
       *
       *  @param  opt the option to test
       */
      bool option( Option opt ) const ;

    private:
      // parsing functions
      void resolveIncludes() ;
      void resolveForLoops() ;
      void resolveConstants() ;

      // include related functions
      void resolveIncludes(const std::string &referencePath, TiXmlNode *node) ;
      void resolveInclude(const std::string &referencePath, TiXmlElement *element, TiXmlDocument &document) ;
      void getFilePath(const std::string &fileName, std::string &filePath) ;
      void getRelativeFileName(const std::string &fileName, const std::string &relativeTo,
                               std::string &relativeFileName) ;

      // constants related functions
      void readConstants() ;
      void replaceAllConstants() ;
      void readConstantsSection(TiXmlElement *constants) ;
      void resolveConstantsInElement(TiXmlElement *element) ;
      void performConstantReplacement(std::string &value) ;
      void performEnvVariableReplacement(std::string &value) ;

      // for loop related
      void resolveForLoops(TiXmlElement *element) ;
      void resolveForLoop(TiXmlNode *node, const std::string &id, int value) ;

    private:
      /// The XML parser options
      Options          _options {} ;
      /// The file name of the last parsed file
      std::string      _fileName {};
      /// The xml document
      TiXmlDocument    _document {};
      /// The constants map
      StringMap        _constants {};
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLParser::constantAs(const std::string &name, const T &fallback) const {
      T value(fallback);
      auto iter = _constants.find( name ) ;
      if (_constants.end() != iter) {
        value = StringUtil::stringToType<T>( iter->second );
      }
      return value;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLParser::constantAs(const std::string &name) const {
      auto iter = _constants.find( name ) ;
      if (_constants.end() != iter) {
        return StringUtil::stringToType<T>( iter->second );
      }
      throw Exception( "XMLParser::constantAs<T>: constant '" + name + "' doesn't exists" );
    }

  }

}

#endif  //  MARLIN_CONCURRENCY_XMLPARSER_h
