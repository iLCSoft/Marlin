#ifndef MARLIN_CONCURRENCY_XMLHELPER_h
#define MARLIN_CONCURRENCY_XMLHELPER_h 1

// -- std headers
#include <vector>
#include <string>
#include <functional>

// -- marlin headers
#include "marlin/Exceptions.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  XMLHelper class
     *  Helper class to extract information from XML nodes
     */
    class XMLHelper {
    public:
      typedef std::function<bool(const TiXmlElement *const, const std::string&, const std::string&)> IterateFunction ;

    public:
      /**
       *  @brief  Read an attribute from the xml element
       *
       *  @param  element the XML element
       *  @param  name the attribute name to read
       */
      template <typename T>
      static T readAttribute( const TiXmlElement *const element, const std::string &name ) ;

      /**
       *  @brief  Read an attribute from the xml element
       *
       *  @param  element the XML element
       *  @param  name the attribute name to read
       *  @param  fallback the fallback value if the attribute doesn't exists
       */
      template <typename T>
      static T readAttribute( const TiXmlElement *const element, const std::string &name, const T &fallback ) ;

      /**
       *  @brief  Read a parameter from the XML element.
       *  Look in child nodes for an XML element "parameter" with
       *  an attribute called "name"
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name
       */
      template <typename T>
      static T readParameter( const TiXmlElement *const element, const std::string &name ) ;

      /**
       *  @brief  Read a parameter from the XML element.
       *  Look in child nodes for an XML element "parameter" with
       *  an attribute called "name"
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name
       *  @param  validator a predicate to validate the read parameter value
       */
      template <typename T>
      static T readParameter( const TiXmlElement *const element, const std::string &name, std::function<bool(const T&)> validator ) ;

      /**
       *  @brief  Read a parameter from the XML element.
       *  Look in child nodes for an XML element "parameter" with
       *  an attribute called "name"
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name
       *  @param  fallback the fallback value if the parameter doesn't exists
       */
      template <typename T>
      static T readParameter( const TiXmlElement *const element, const std::string &name, const T &fallback ) ;

      /**
       *  @brief  Read a vector of parameters from the XML element.
       *  Look in child nodes for an XML element "parameter" with
       *  an attribute called "name". The parameter are split with
       *  a space " " character.
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name
       */
      template <typename T>
      static std::vector<T> readParameters( const TiXmlElement *const element, const std::string &name ) ;

      /**
       *  @brief  Read a parameter from the XML element.
       *  Look in child nodes for an XML element "parameter" with
       *  an attribute called "name"
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name
       *  @param  validator a predicate to validate the read parameter value
       */
      template <typename T>
      static std::vector<T> readParameters( const TiXmlElement *const element, const std::string &name, std::function<bool(const std::vector<T>&)> validator ) ;

      /**
       *  @brief  Read a parameter from the XML element.
       *  Look in child nodes for an XML element "parameter" with
       *  an attribute called "name"
       *
       *  @param  element the XML element to parse
       *  @param  name the parameter name
       *  @param  fallback the fallback value if the parameter doesn't exists
       */
      template <typename T>
      static std::vector<T> readParameters( const TiXmlElement *const element, const std::string &name, const std::vector<T> &fallback ) ;

      /**
       *  @brief  Iterate over parameter XML element contained in the provided XML element.
       *  The second argument corresponds to the function called when a parameter is found.
       *  This callback function can return a boolean specifying if the iteration should
       *  stop (true to continue, false to stop the iteration).
       *  Example with a lambda function:
       *  \code{cpp}
       *  XMLHelper::iterateParameters( element, []( const std::string &name, const std::string &value ) {
       *    std::cout << "Parameter: " << name << " = " << value << std::endl;
       *    if( name == "StopAfterMe" ) {
       *      return false;
       *    }
       *    return true ;
       *  });
       *  \endcode
       *
       *  @param  element the parent XML element
       *  @param  callback the parameter (element, name, value) callback function
       */
      static void iterateParameters( const TiXmlElement *const element, IterateFunction callback ) ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLHelper::readAttribute( const TiXmlElement *const element, const std::string &name ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readAttribute: invalid XML element!" ) ;
      }
      if ( nullptr == element->Attribute(name) ) {
        throw Exception( "XMLHelper::readAttribute: attribute '" + name + "' not found!" ) ;
      }
      std::string attribute( element->Attribute(name.c_str()) ) ;
      return StringUtil::stringToType<T>( attribute ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLHelper::readAttribute( const TiXmlElement *const element, const std::string &name, const T &fallback ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readAttribute: invalid XML element!" ) ;
      }
      if ( nullptr == element->Attribute(name) ) {
        return fallback ;
      }
      std::string attribute( element->Attribute(name.c_str()) ) ;
      return StringUtil::stringToType<T>( attribute ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLHelper::readParameter( const TiXmlElement *const element, const std::string &name ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readParameter: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string aname ;
        try {
          aname = XMLHelper::readAttribute<std::string>( element, "name" ) ;
          if ( aname != name ) {
            continue ;
          }
        }
        catch(Exception &) {
          continue ;
        }
        std::string value ;
        try {
          return XMLHelper::readAttribute<T>( element, "value" ) ;
        }
        catch(Exception &) {
          return StringUtil::stringToType<T>( element->GetText() ? element->GetText() : "" ) ;
        }
      }
      throw Exception( "XMLHelper::readParameter: parameter '" + name + "' not found!" ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLHelper::readParameter( const TiXmlElement *const element, const std::string &name, std::function<bool(const T&)> validator ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readParameter: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string aname ;
        try {
          aname = XMLHelper::readAttribute<std::string>( element, "name" ) ;
          if ( aname != name ) {
            continue ;
          }
        }
        catch(Exception &) {
          continue ;
        }
        T value ;
        try {
          value = XMLHelper::readAttribute<T>( element, "value" ) ;
        }
        catch(Exception &) {
          value = StringUtil::stringToType<T>( element->GetText() ? element->GetText() : "" ) ;
        }
        if ( not validator( value ) ) {
          throw Exception( "XMLHelper::readParameter: invalid parameter (name=" + name + ")" ) ;
        }
      }
      throw Exception( "XMLHelper::readParameter: parameter '" + name + "' not found!" ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T XMLHelper::readParameter( const TiXmlElement *const element, const std::string &name, const T &fallback ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readParameter: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string aname ;
        try {
          aname = XMLHelper::readAttribute<std::string>( element, "name" ) ;
          if ( aname != name ) {
            continue ;
          }
        }
        catch(Exception &) {
          continue ;
        }
        try {
          return XMLHelper::readAttribute<T>( element, "value", fallback ) ;
        }
        catch(Exception &) {
          if ( nullptr != element->GetText() ) {
            return StringUtil::stringToType<T>( element->GetText() ) ;
          }
          else {
            return fallback ;
          }
        }
      }
      return fallback ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> XMLHelper::readParameters( const TiXmlElement *const element, const std::string &name ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readParameters: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string aname ;
        try {
          aname = XMLHelper::readAttribute<std::string>( element, "name" ) ;
          if ( aname != name ) {
            continue ;
          }
        }
        catch(Exception &) {
          continue ;
        }
        std::string value ;
        try {
          value = XMLHelper::readAttribute<std::string>( element, "value" ) ;
        }
        catch(Exception &) {
          value = (element->GetText() ? element->GetText() : "") ;
        }
        return StringUtil::split<T>( value, " " );
      }
      throw Exception( "XMLHelper::readParameters: parameter '" + name + "' not found!" ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> XMLHelper::readParameters( const TiXmlElement *const element, const std::string &name, std::function<bool(const std::vector<T>&)> validator ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readParameters: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string aname ;
        try {
          aname = XMLHelper::readAttribute<std::string>( element, "name" ) ;
          if ( aname != name ) {
            continue ;
          }
        }
        catch(Exception &) {
          continue ;
        }
        std::string value ;
        try {
          value = XMLHelper::readAttribute<std::string>( element, "value" ) ;
        }
        catch(Exception &) {
          value = (element->GetText() ? element->GetText() : "") ;
        }
        std::vector<T> valueVec = StringUtil::split<T>( value, " " ) ;
        if ( not validator( valueVec ) ) {
          throw Exception( "XMLHelper::readParameters: invalid parameter (name=" + name + ")" ) ;
        }
      }
      throw Exception( "XMLHelper::readParameters: parameter '" + name + "' not found!" ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::vector<T> XMLHelper::readParameters( const TiXmlElement *const element, const std::string &name, const std::vector<T> &fallback ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::readParameters: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string aname ;
        try {
          aname = XMLHelper::readAttribute<std::string>( element, "name" ) ;
          if ( aname != name ) {
            continue ;
          }
        }
        catch(Exception &) {
          continue ;
        }
        std::string value ;
        try {
          value = XMLHelper::readAttribute<std::string>( element, "value" ) ;
        }
        catch(Exception &) {
          if ( nullptr != element->GetText() ) {
            value = StringUtil::stringToType<T>( element->GetText() ) ;
          }
          else {
            return fallback ;
          }
        }
        return StringUtil::split<T>( value, " " ) ;
      }
      return fallback ;
    }

    //--------------------------------------------------------------------------

    inline void XMLHelper::iterateParameters( const TiXmlElement *const element, IterateFunction callback ) {
      if (nullptr == element) {
        throw Exception( "XMLHelper::iterateParameters: invalid XML element!" ) ;
      }
      for ( auto child = element->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter") ) {
        std::string name = XMLHelper::readAttribute<std::string>( element, "name" ) ;
        std::string value ;
        try {
          value = XMLHelper::readAttribute<std::string>( element, "value" ) ;
        }
        catch(Exception &) {
          if ( nullptr != element->GetText() ) {
            value = StringUtil::stringToType<std::string>( element->GetText() ) ;
          }
        }
        if( not callback( child, name, value ) ) {
          break;
        }
      }
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
