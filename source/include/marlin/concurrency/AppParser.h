#ifndef MARLIN_CONCURRENCY_APPPARSER_h
#define MARLIN_CONCURRENCY_APPPARSER_h 1

// -- marlin headers
#include "marlin/concurrency/XMLParser.h"
#include "marlin/concurrency/Parameters.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  AppParser class
     *  Does Marlin application specific steering file parsing
     *  and provides access to steering file content
     */
    class AppParser {
    public:
      AppParser() = default ;

      /**
       *  @brief  Set/unset an XML parser option
       *
       *  @param  opt the XML parser option to set/unset
       *  @param  set whether to set/unset the option
       */
      void setParserOption( XMLParser::Option opt, bool set = true ) ;

      /**
       *  @brief  Parse the XML file
       *
       *  @param  fname the file name
       */
      void parse( const std::string &fname ) ;

      /**
       *  @brief  Get the global parameter value
       *
       *  @param  name the parameter name
       */
      template <typename T>
      T global( const std::string &name ) const ;

      /**
       *  @brief  Get the global parameter value
       *
       *  @param  name the parameter name
       *  @param  fallback the fallback value if the global parameter doesn't exists
       */
      template <typename T>
      T global( const std::string &name , const T &fallback ) const ;

      /**
       *  @brief  Check whether the processor configuration exists
       *
       *  @param  name the procesor name
       */
      bool processorExists( const std::string &name ) const ;

      /**
       *  @brief  Get the processor parameters
       *
       *  @param  name the procesor name
       *  @param  parameters the parameters to populate
       */
      void processorParameters( const std::string &name, Parameters &parameters ) const ;

      /**
       *  @brief  Get the constant names
       */
      std::vector<std::string> constantNames() const ;

      /**
       *  @brief  Get a specific constant
       *
       *  @param  name the constant name
       */
      template <typename T>
      T constant( const std::string &name ) const ;

      /**
       *  @brief  Get a specific constant
       *
       *  @param  name the constant name
       *  @param  fallback the fallback value if the constant doesn't exists
       */
      template <typename T>
      T constant( const std::string &name , const T &fallback ) const ;

      /**
       *  @brief  Get the number of concurrent tasks to run from steering file
       *  Returns std::thread::hardware_concurrency() if not found
       */
      unsigned int concurrency() const ;

      /**
       *  @brief  Clear the parser content
       */
      void clear();

    private:
      std::string globalParameter( const std::string &name ) const ;
      const TiXmlElement *processorElement( const std::string &name ) const ;

    private:
      /// The XML parser instance
      XMLParser           _parser {} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline T AppParser::constant( const std::string &name ) const {
      return _parser.constantAs<T>( name );
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T AppParser::constant( const std::string &name , const T &fallback ) const {
      return _parser.constantAs<T>( name, fallback );
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T AppParser::global( const std::string &name ) const {
      return StringUtil::stringToType<T>( globalParameter( name ) ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T AppParser::global( const std::string &name , const T &fallback ) const {
      try {
        return global<T>( name );
      }
      catch(...) {
      }
      return fallback ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
