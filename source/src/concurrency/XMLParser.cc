// -- marlin headers
#include "marlin/concurrency/XMLParser.h"

namespace marlin {

  namespace concurrency {

    void XMLParser::parse(const std::string &fname) {
      reset() ;
      _fileName = fname ;
      bool loaded = _document.LoadFile(_fileName) ;
      if ( not loaded ) {
        std::stringstream ss; ss << "XMLParser::parse(): couldn't load xml file ['" <<
        _fileName << "', row: " << _document.ErrorRow() << ", col: " <<
        _document.ErrorCol() << "] : " << _document.ErrorDesc() ;
        throw Exception( ss.str() ) ;
      }
      if ( processIncludes() ) {
        resolveIncludes() ;
      }
      if ( processConstants() ) {
        resolveConstants() ;
      }
      if( processForLoops() ) {
        resolveForLoops() ;
      }
    }

    //----------------------------------------------------------------------------------------------------

    const std::string &XMLParser::fileName() const {
      return _fileName ;
    }

    //----------------------------------------------------------------------------------------------------

    const TiXmlDocument &XMLParser::document() const {
      return _document ;
    }

    //----------------------------------------------------------------------------------------------------

    TiXmlDocument &XMLParser::document() {
      return _document ;
    }

    //----------------------------------------------------------------------------------------------------

    const XMLParser::StringMap &XMLParser::constants() const {
      return _constants ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::reset() {
      _constants.clear() ;
      _fileName.clear() ;
      _document.Clear() ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setProcessIncludes(bool process) {
      _processIncludes = process ;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::processIncludes() const {
      return _processIncludes ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setAllowNestedIncludes(bool allow) {
      _allowNestedIncludes = allow ;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::allowNestedIncludes() const {
      return _allowNestedIncludes ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setProcessConstants(bool process) {
      _processConstants = process ;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::processConstants() const {
      return _processConstants ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setAllowEnvVariables(bool allow) {
      _allowEnvVariables = allow ;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::allowEnvVariables() const {
      return _allowEnvVariables ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setProcessForLoops(bool process) {
      _processForLoops = process ;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::processForLoops() const {
      return _processForLoops;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveIncludes() {
      TiXmlElement *root = _document.RootElement() ;
      std::string filePath ;
      getFilePath(_fileName, filePath) ;
      resolveIncludes(filePath, root) ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveConstants() {
      readConstants() ;
      replaceAllConstants() ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveIncludes(const std::string &referencePath, TiXmlNode *node) {
      TiXmlElement *child = node->FirstChildElement() ;
      while (true) {
        if (nullptr == child) {
          break ;
        }
        if ( child->Value() != std::string("include") ) {
          resolveIncludes( referencePath, child ) ;
          child = child->NextSiblingElement() ;
          continue ;
        }
        // process the include and returns the parsed document
        TiXmlDocument doc;
        resolveInclude( referencePath, child, doc ) ;

        TiXmlElement *rootInclude = doc.RootElement() ;

        if ( std::string( rootInclude->Value() ) != "marlin" ) {
          std::stringstream ss ;
          ss << "XMLParser::resolveIncludes(): no <marlin> root element in included file '" << doc.Value() << "'";;
          throw Exception( ss.str() ) ;
        }
        // add them to the xml tree
        TiXmlNode *includeAfter( child );
        for (TiXmlElement *includeElement = rootInclude->FirstChildElement(); includeElement != nullptr;
             includeElement = includeElement->NextSiblingElement()) {
          includeAfter = node->InsertAfterChild( includeAfter, *includeElement ) ;
        }
        // case where at least one element has been inserted from the include file
        if( includeAfter != child ) {
          node->RemoveChild( child ) ;
          child = includeAfter->NextSiblingElement() ;
        }
        // case where nothing was inserted
        else {
          // Find the previous element to point to if any
          TiXmlElement *previousElement( nullptr );
          TiXmlNode *previousNode( child );
          while( nullptr != previousNode->PreviousSibling() ) {
            if ( nullptr != previousNode->PreviousSibling()->ToElement() ) {
              previousElement = previousNode->PreviousSibling()->ToElement() ;
              break ;
            }
            else {
              previousNode = previousNode->PreviousSibling() ;
            }
          }
          // if a previous element is available,
          // continue the while from it
          if( nullptr != previousElement ) {
            node->RemoveChild( child ) ;
            child = previousElement ;
          }
          // if no previous element is available,
          // restart from the first child of input
          else {
            node->RemoveChild( child ) ;
            child = node->FirstChildElement() ;
          }
        }
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveInclude(const std::string &referencePath, TiXmlElement *element, TiXmlDocument &doc) {
      if ( nullptr == element->Attribute("ref") ) {
        std::stringstream ss ;
        ss << "XMLParser::resolveInclude(): missing attribute \"ref\" in element <" << element->Value() << "/>" ;
        throw Exception( ss.str() );
      }
      std::string ref( element->Attribute("ref") ) ;
      if (ref.size() < 5 || ref.substr(ref.size() - 4) != ".xml") {
        std::stringstream ss ;
        ss << "XMLParser::resolveInclude(): invalid ref file name \"" << ref << "\" in element <" << element->Value() << "/>" ;
        throw Exception( ss.str() ) ;
      }
      std::string refFileName;
      getRelativeFileName( ref, referencePath, refFileName );
      bool loadOkay = doc.LoadFile(refFileName) ;
      if ( not loadOkay ) {
        std::stringstream ss ;
        ss << "XMLParser::resolveInclude(): error in file [" << refFileName <<
        ", row: " << doc.ErrorRow() << ", col: " << doc.ErrorCol() << "] : " << doc.ErrorDesc() ;
        throw Exception( ss.str() ) ;
      }
      // process nested includes if enabled
      if ( allowNestedIncludes() ) {
        std::string includeReferencePath ;
        getFilePath( refFileName, includeReferencePath ) ;
        resolveIncludes( includeReferencePath, &doc ) ;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::getFilePath(const std::string &fName, std::string &filePath) {
      filePath = "" ;
      size_t idx = fName.find_last_of("/") ;
      if (idx != std::string::npos) {
        filePath = fName.substr(0, idx) ;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::getRelativeFileName(const std::string &fName, const std::string &relativeTo,
                                        std::string &relativeFileName) {
      if (fName.at(0) != '/') {
        relativeFileName = relativeTo + "/" + fName ;
      }
      else {
        relativeFileName = fName ;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::readConstants() {
      TiXmlElement *root = document().RootElement();
      for (TiXmlElement *element = root->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (element->Value() == std::string("constants")) {
          readConstantsSection( element ) ;
        }
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::replaceAllConstants() {
      TiXmlElement *root = _document.RootElement() ;
      for (TiXmlElement *element = root->FirstChildElement(); element != nullptr;
           element = element->NextSiblingElement()) {
        resolveConstantsInElement( element ) ;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::readConstantsSection(TiXmlElement *constantsElement) {
      for (TiXmlElement *element = constantsElement->FirstChildElement("constant"); element != nullptr;
           element = element->NextSiblingElement("constant")) {
        resolveConstantsInElement(element);
        if (element->Attribute("name") == nullptr) {
          throw Exception( "XMLParser::readConstantsSection(): constant element without name !" );
        }
        std::string name( element->Attribute("name") ) ;
        if ( name.empty() ) {
          throw Exception( "XMLParser::readConstantsSection(): parsed empty constant name !" );
        }
        if ( _constants.end() != _constants.find(name) ) {
          std::stringstream ss ;
          ss << "XMLParser::readConstantsSection(): constant \"" << name << "\" defined twice !" ;
          throw Exception( ss.str() ) ;
        }
        std::string value ;
        if (element->Attribute("value") != nullptr) {
          value = element->Attribute("value") ;
        }
        else {
          if (element->FirstChild() != nullptr) {
            value = element->FirstChild()->Value() ;
          }
        }
        if ( not _constants.insert(StringMap::value_type(name, value)).second ) {
          std::stringstream ss ;
          ss << "XMLParser::readConstantsSection(): couldn't add constant \"" << name << "\" to constant map !" ;
          throw Exception( ss.str() );
        }
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::performConstantReplacement(std::string &value) {
      size_t pos = value.find("${") ;
      std::string original(value) ;
      while (pos != std::string::npos) {
        size_t pos2 = value.find_first_of("}", pos + 2) ;
        if ( pos2 == std::string::npos) {
          throw Exception( "XMLParser::performConstantReplacement(): couldn't parse constant value !" ) ;
        }
        const std::string key( value.substr(pos + 2, pos2 - pos - 2) ) ;
        auto findConstant = _constants.find(key) ;
        const std::string replacementValue( findConstant != _constants.end() ? findConstant->second : "" ) ;
        if (replacementValue.empty()) {
          std::stringstream ss ;
          ss << "XMLParser::performConstantReplacement(): constant \"" << key << "\" not found in available constants !" ;
          throw Exception( ss.str() );
        }
        value.replace( pos, (pos2 + 1 - pos), replacementValue ) ;
        pos2 = pos + replacementValue.size() ; // new end position after replace
        pos = value.find( "${", pos2 ) ;         // find next possible key to replace
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::performEnvVariableReplacement(std::string &value) {
      if ( not allowEnvVariables() ) {
        return;
      }
      size_t pos = value.find("$ENV{") ;
      std::string original(value) ;
      while (pos != std::string::npos) {
        size_t pos2 = value.find_first_of("}", pos + 5) ;
        if (pos2 == std::string::npos) {
          throw Exception( "XMLParser::performEnvVariableReplacement(): couldn't parse env variable name !" );
        }
        const std::string key( value.substr(pos + 5, pos2 - pos - 5) ) ;
        char *env = getenv( key.c_str() );

        if ( env == nullptr ) {
          std::stringstream ss ;
          ss << "XMLParser::performEnvVariableReplacement(): env variable \"" << key << "\" not found !" ;
          throw Exception( ss.str() );
        }
        const std::string replacementValue( env ) ;
        value.replace( pos, (pos2 + 1 - pos), replacementValue ) ;
        pos2 = pos + replacementValue.size() ; // new end position after replace
        pos = value.find( "$ENV{", pos2 ) ;      // find next possible key to replace
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveConstantsInElement(TiXmlElement *element) {
      // replace constants in all attributes
      for (TiXmlAttribute *attribute = element->FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        std::string value( attribute->ValueStr() ) ;
        performConstantReplacement( value ) ;
        if ( allowEnvVariables() ) {
          performEnvVariableReplacement( value ) ;
        }
        attribute->SetValue( value ) ;
      }
      for (TiXmlNode *node = element->FirstChild(); node != nullptr; node = node->NextSibling()) {
        if ( node->Type() == TiXmlNode::ELEMENT ) {
          TiXmlElement *child = node->ToElement() ;
          resolveConstantsInElement(child) ;
        }
        else if ( node->Type() == TiXmlNode::TEXT ) {
          TiXmlText *child = node->ToText() ;
          std::string value( child->ValueStr() ) ;
          performConstantReplacement( value ) ;
          if ( allowEnvVariables() ) {
            performEnvVariableReplacement(value) ;
          }
          child->SetValue( value ) ;
        }
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveForLoops() {
      TiXmlElement *root = _document.RootElement() ;
      resolveForLoops(root) ;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveForLoops(TiXmlElement *element) {
      // first, resolve for loops in child elements
      for(auto child = element->FirstChildElement(); nullptr != child ; child = child->NextSiblingElement()) {
        resolveForLoops(child) ;
      }
      // if loop element, resolve it
      if( element->Value() == std::string("for") ) {
        auto parent = element->Parent() ;
        if(nullptr == parent) {
          return;
        }
        int begin(0), end(0), increment(0) ;
        std::string loopId ;
        if( TIXML_SUCCESS != element->QueryIntAttribute( "begin", &begin ) ) {
          throw Exception( "XMLParser::resolveForLoops: <for> element with invalid 'begin' attribute !" );
        }
        if( TIXML_SUCCESS != element->QueryIntAttribute( "end", &end ) ) {
          throw Exception( "XMLParser::resolveForLoops: <for> element with invalid 'end' attribute !" ) ;
        }
        if(TIXML_SUCCESS != element->QueryValueAttribute<std::string>("id", &loopId) or loopId.empty()) {
          throw Exception( "XMLParser::resolveForLoops: <for> element with invalid 'id' attribute !" );
        }
        // optional attribute
        element->QueryIntAttribute( "increment", &increment ) ;
        const bool forwardLoop( begin < end ) ;
        if( ( forwardLoop and increment < 0) or (not forwardLoop and increment > 0) ) {
          throw Exception( "XMLParser::resolveForLoops: <for> element with infinite loop detected !" );
        }
        for ( int i=begin ; i<=end ; i+=increment ) {
          for ( auto child = element->FirstChild() ; nullptr != child ; child = child->NextSibling() ) {
            auto cloneChild = child->Clone() ;
            if ( nullptr != cloneChild ) {
              resolveForLoop( cloneChild, loopId, i ) ;
              parent->InsertBeforeChild( element, *cloneChild ) ;
              delete cloneChild ;
            }
          }
        }
        // remove the element itself from the tree
        parent->RemoveChild(element) ;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveForLoop(TiXmlNode *node, const std::string &id, int value) {
      std::string loopIDStr = "$FOR{" + id + "}" ;
      std::string valueStr = StringUtil::typeToString( value ) ;
      if ( node->Type() == TiXmlNode::ELEMENT ) {
        TiXmlElement *nodeElement = node->ToElement() ;
        // replace occurences in all attributes
        for ( auto attr = nodeElement->FirstAttribute() ; nullptr != attr ; attr = attr->Next() ) {
          std::string attrValue( attr->ValueStr() ) ;
          size_t pos = attrValue.find( loopIDStr, 0 ) ;
          while( pos != std::string::npos ) {
            attrValue.replace( pos, loopIDStr.size(), valueStr ) ;
            pos = attrValue.find( loopIDStr, pos+valueStr.size() ) ;
          }
          attr->SetValue( attrValue ) ;
        }
        // make it recursive
        for ( auto child = nodeElement->FirstChild() ; nullptr != child ; child = child->NextSibling() ) {
          resolveForLoop( child, id, value ) ;
        }
      }
      else if ( (node->Type() == TiXmlNode::COMMENT) or (node->Type() == TiXmlNode::TEXT) ) {
        std::string nodeValue = node->ValueStr() ;
        size_t pos = nodeValue.find( loopIDStr, 0 ) ;
        while ( pos != std::string::npos ) {
          nodeValue.replace( pos, loopIDStr.size(), valueStr ) ;
          pos = nodeValue.find( loopIDStr, pos+valueStr.size() ) ;
        }
        node->SetValue( nodeValue ) ;
      }
    }

  }

}
