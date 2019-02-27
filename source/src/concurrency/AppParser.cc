#include "marlin/concurrency/AppParser.h"
#include "marlin/concurrency/XMLHelper.h"

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

    void AppParser::processorParameters( const std::string &pname, Parameters &parameters ) const {
      auto processor = processorElement( pname ) ;
      if ( nullptr == processor ) {
        return ;
      }
      XMLHelper::iterateParameters( processor , [&parameters](const TiXmlElement *const child, const std::string &name, const std::string &value){
        // Check whether the parameter value is a list
        bool isList = XMLHelper::readAttribute<bool>( child, "list", false ) ;
        if ( isList ) {
          auto tokens = StringUtil::split<std::string>( value ) ;
          parameters.set( name, tokens ) ;
        }
        else {
          parameters.set( name, value ) ;
        }
        return true ;
      }) ;
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
      return XMLHelper::readAttribute( exec, "concurrency", ccy ) ;
    }

    //--------------------------------------------------------------------------

    void AppParser::clear() {
      _parser.reset() ;
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
