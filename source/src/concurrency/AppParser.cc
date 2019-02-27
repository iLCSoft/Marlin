#include "marlin/concurrency/AppParser.h"

// -- std headers
#include <thread>

namespace marlin {

  namespace concurrency {

    void AppParser::setParserOption( XMLParser::Option opt, bool set ) {
      _parser.setOption( opt, set );
    }

    //--------------------------------------------------------------------------

    void AppParser::parse( const std::string &fname ) {
      clear() ;
      _parser.parse( fname );
      // TODO:
      // do specific Marlin application specific parsing here
    }

    //--------------------------------------------------------------------------

    std::vector<std::string> AppParser::constantNames() const {
      std::vector<std::string> names ;
      for ( auto iter : _parser.constants() ) {
        names.push_back( iter.first );
      }
      return names ;
    }

    //--------------------------------------------------------------------------

    bool AppParser::processorExists( const std::string &name ) const {
      return (nullptr != processorElement( name ) );
    }

    //--------------------------------------------------------------------------

    void AppParser::processorParameters( const std::string &name, Parameters &parameters ) const {
      auto processor = processorElement( name ) ;
      if ( nullptr == processor ) {
        return ;
      }
      for (const TiXmlElement *parameter = processor->FirstChildElement("parameter"); parameter != nullptr;
        parameter = parameter->NextSiblingElement("parameter")) {
        if (parameter->Attribute("name") == nullptr) {
          throw Exception( "AppParser::processorParameters: parameter element without name !" ) ;
        }
        std::string pname( parameter->Attribute("name") ) ;
        if ( pname.empty() ) {
          throw Exception( "AppParser::processorParameters: parsed empty parameter name !" ) ;
        }
        if ( parameters.exists( pname ) ) {
          throw Exception( "AppParser::processorParameters: duplicate parameter name '" + pname + "' !" );
        }
        std::string value ;
        if (parameter->Attribute("value") != nullptr) {
          value = parameter->Attribute("value") ;
        }
        else {
          if (parameter->FirstChild() != nullptr) {
            value = parameter->FirstChild()->Value() ;
          }
        }
        // Check whether the parameter value is
        std::string isListStr ;
        if (parameter->Attribute("list") != nullptr) {
          isListStr = parameter->Attribute("list") ;
        }
        bool isList = StringUtil::stringToType<bool>( isListStr ) ;
        if ( isList ) {
          auto tokens = StringUtil::split<std::string>( value ) ;
          parameters.set( name, tokens );
        }
        else {
          parameters.set( name, value );
        }
      }
    }

    //--------------------------------------------------------------------------

    unsigned int AppParser::concurrency() const {
      unsigned int ccy = std::thread::hardware_concurrency() ;
      const TiXmlElement *root = _parser.document().RootElement() ;
      if ( nullptr == root ) {
        return ccy ;
      }
      const TiXmlElement *exec = root->FirstChildElement( "execute" ) ;
      if ( nullptr == exec ) {
        return ccy ;
      }
      if (exec->Attribute("concurrency") != nullptr) {
        exec->QueryValueAttribute<unsigned int>( "concurrency", &ccy ) ;
      }
      return ccy ;
    }

    //--------------------------------------------------------------------------

    void AppParser::clear() {
      _parser.reset() ;
    }

    //--------------------------------------------------------------------------

    std::string AppParser::globalParameter( const std::string &name ) const {
      const TiXmlElement *root = _parser.document().RootElement() ;
      if ( nullptr == root ) {
        throw Exception( "AppParser::globalParameter: invalid root element!" ) ;
      }
      const TiXmlElement *glob = root->FirstChildElement( "global" ) ;
      if ( nullptr == glob ) {
        throw Exception( "AppParser::globalParameter: no global section found in document!" ) ;
      }
      for (const TiXmlElement *parameter = glob->FirstChildElement("parameter"); parameter != nullptr;
        parameter = parameter->NextSiblingElement("parameter")) {
        if (parameter->Attribute("name") == nullptr) {
          throw Exception( "AppParser::globalParameter: parameter element without name !" ) ;
        }
        std::string pname( parameter->Attribute("name") ) ;
        if ( pname.empty() ) {
          throw Exception( "AppParser::globalParameter: parsed empty parameter name !" ) ;
        }
        if ( pname != name ) {
          continue ;
        }
        std::string value ;
        if (parameter->Attribute("value") != nullptr) {
          value = parameter->Attribute("value") ;
        }
        else {
          if (parameter->FirstChild() != nullptr) {
            value = parameter->FirstChild()->Value() ;
          }
        }
        return value ;
      }
      throw Exception( "AppParser::globalParameter: global parameter '" + name + "' not found!" ) ;
    }

    //--------------------------------------------------------------------------

    const TiXmlElement *AppParser::processorElement( const std::string &name ) const  {
      const TiXmlElement *root = _parser.document().RootElement() ;
      if ( nullptr == root ) {
        throw Exception( "AppParser::processorElement: invalid root element!" ) ;
      }
      for (const TiXmlElement *processor = root->FirstChildElement("processor"); processor != nullptr;
        processor = processor->NextSiblingElement("processor")) {
        if (processor->Attribute("name") == nullptr) {
          throw Exception( "AppParser::processorElement: processor element without name !" ) ;
        }
        std::string pname( processor->Attribute("name") ) ;
        if ( pname.empty() ) {
          throw Exception( "AppParser::globalParameter: parsed empty processor name !" ) ;
        }
        if ( pname != name ) {
          continue ;
        }
        return processor ;
      }
      return nullptr ;
    }

  } // namespace concurrency

} // namespace marlin
