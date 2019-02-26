#ifndef MARLIN_CONCURRENCY_XMLPARSER_h
#define MARLIN_CONCURRENCY_XMLPARSER_h 1

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
      typedef std::map<std::string, std::string> StringMap ;

    public:
      XMLParser() = default ;

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
       *  @param  fallback fallback value if constant doesn't exists
       */
      template <typename T>
      T constantAs(const std::string &name, const T &fallback = T()) const ;

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
       *  @brief  Set whether to process <include> element while parsing
       *
       *  @param  process whether to process includes
       */
      void setProcessIncludes(bool process) ;

      /**
       *  @brief  Whether to process <include> elements
       */
      bool processIncludes() const ;

      /**
       *  @brief  Set whether to allow nested <include> elements, meaning include
       *          elements in included files
       *
       *  @param  allow whether to allow nested includes
       */
      void setAllowNestedIncludes( bool allow ) ;

      /**
       *  @brief  Whether to allow nested <include> elements meaning include
       *          elements in included files
       */
      bool allowNestedIncludes() const ;

      /**
       *  @brief  Set whether to process constants sections while parsing
       *
       *  @param  process whether to process constants
       */
      void setProcessConstants( bool process ) ;

      /**
       *  @brief  Whether to process constants sections while parsing
       */
      bool processConstants() const ;

      /**
       *  @brief  Set whether to allow the use of environment variable while resolving constants
       *
       *  @param  allow whether to allow the use of environment variable
       */
      void setAllowEnvVariables( bool allow ) ;

      /**
       *  @brief  Whether to allow the use of environment variable while resolving constants
       */
      bool allowEnvVariables() const ;

      /**
       *  @brief  Set whether to process XML for loop elements
       *
       *  @param  process whether to process for loops
       */
      void setProcessForLoops( bool process ) ;

      /**
       *  @brief  Whether to process XML for loop elements
       */
      bool processForLoops() const ;

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
      /// Whether to process includes elements
      bool             _processIncludes {true};
      /// Whether to allow nested include elements
      bool             _allowNestedIncludes {true};
      /// Whether to process constants elements
      bool             _processConstants {true};
      /// Whether to allow the use of environment variables
      bool             _allowEnvVariables {true};
      /// Whether to process for loops
      bool             _processForLoops {true};
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

  }

}

#endif  //  MARLIN_CONCURRENCY_XMLPARSER_h
