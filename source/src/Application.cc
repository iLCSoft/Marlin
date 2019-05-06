#include "marlin/Application.h"
#include "marlin/PluginManager.h"
#include "marlin/Utils.h"
#include "marlin/XMLParser.h"
#include "marlin/Parser.h"
#include "marlin/MarlinConfig.h"

// -- std headers
#include <cstring>

namespace marlin {

  void Application::init( int argc, char **argv ) {
    printBanner() ;
    std::string pname = argv[0] ;
    auto pos = pname.find_last_of("/") ;
    _programName = pname.substr(pos+1) ;
    for ( int i=1 ; i<argc ; i++ ) {
      _arguments.push_back( argv[i] ) ;
    }
    // set the main logger name a bit early for clearer logging
    _loggerMgr.mainLogger()->setName( program() ) ;
    parseCommandLine() ;
    // parse the steering file
    _parser = createParser() ;
    _parser->parse() ;
    // write parsed steering file if global parameter is there
    auto globals = globalParameters() ;
    auto outputSteeringFile = globals->getValue<std::string>( "OutputSteeringFile", "" ) ;
    if ( not outputSteeringFile.empty() ) {
      parser()->write( outputSteeringFile ) ;
    }
    // initialize logging
    _loggerMgr.init( this ) ;
    // initialize geometry
    _geometryMgr.init( this ) ;
    // sub-class initialization
    init() ;
    _initialized = true ;
  }

  //--------------------------------------------------------------------------

  void Application::run() {
    runApplication() ;
    _geometryMgr.clear();
    end() ;
  }

  //--------------------------------------------------------------------------

  void Application::parseCommandLine() {
    logger()->log<MESSAGE>() << "Parsing command line ..." << std::endl ;
    _steeringFileName.clear() ;
    _cmdLineOptions.clear() ;
    auto cmdLineArgs = _arguments ;
    if ( cmdLineArgs.empty() ) {
      printUsage() ;
      logger()->log<ERROR>() << "No command line option provided. Expected at least a steering file..." << std::endl ;
      ::exit( 1 ) ;
    }
    auto iter = cmdLineArgs.begin() ;
    // start with dynamic arguments
    while ( cmdLineArgs.end() != iter ) {
      auto arg = *iter ;
      if ( arg.substr( 0, 2 ) == "--" ) {
        auto argVec = StringUtil::split<std::string>( arg.substr( 2 ) , "=" ) ;
        if ( argVec.size() != 2 ) {
          printUsage() ;
          logger()->log<ERROR>() << "*** invalid command line option: " << arg << std::endl ;
          ::exit( 1 ) ;
        }
        auto argKey = StringUtil::split<std::string>( argVec[0] , "." ) ;
        if ( argKey.size() != 2 ) {
          printUsage() ;
          logger()->log<ERROR>() << "*** invalid command line option: " << arg << std::endl ;
          ::exit( 1 ) ;
        }
        _cmdLineOptions[ argKey[0] ][ argKey[1] ] = argVec[1] ;
        logger()->log<MESSAGE9>()
          << "steering file " << argKey[0] << ": "
          << "[ " << argKey[0] << "." << argKey[1] << " ]"
          << " will be OVERWRITTEN with value: [\"" << argVec[1] << "\"]"  << std::endl ;
        iter = cmdLineArgs.erase( iter ) ;
        continue;
      }
      ++iter ;
    }
    // parse remaining arguments
    iter = cmdLineArgs.begin() ;
    while ( cmdLineArgs.end() != iter  ) {
      auto arg = *iter ;
      // print plugin manager content and exit
      if ( arg == "-p" ) {
        PluginManager::instance().dump() ;
        ::exit( 0 ) ;
      }
      // print help and exit
      else if ( arg == "-h" || arg == "-?" ) {
        printUsage() ;
        ::exit( 0 ) ;
      }
      // last argument is steering file
      else if( std::next( iter ) == cmdLineArgs.end() ) {
        _steeringFileName = arg ;
      }
      else {
        _filteredArguments.push_back( arg ) ;
      }
      ++iter ;
    }
    if ( _steeringFileName.empty() ) {
      printUsage() ;
      logger()->log<ERROR>() << "No steering file provided ..." << std::endl ;
      ::exit( 1 ) ;
    }
    logger()->log<DEBUG2>() << "Parsing command line ... done" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void Application::printBanner( std::ostream &out ) const {
    out << std::endl ;
    out << "    __  __            _ _       " << std::endl ;
    out << "   |  \\/  | __ _ _ __| (_)_ __  " << std::endl ;
    out << "   | |\\/| |/ _` | '__| | | '_ \\" << std::endl ;
    out << "   | |  | | (_| | |  | | | | | |" << std::endl ;
    out << "   |_|  |_|\\__,_|_|  |_|_|_| |_|" << std::endl ;
    out << std::endl ;
    out << "         Version: "
      << MARLIN_MAJOR_VERSION << "."
      << MARLIN_MINOR_VERSION << "."
      << MARLIN_PATCH_LEVEL << std::endl ;
    out << std::endl ;
    out << "         LICENCE: GPLv3 " << std::endl ;
    out << "    Copyright (C), Marlin Authors" << std::endl ;
    out << std::endl ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<StringParameters> Application::globalParameters () const {
    return (nullptr == _parser) ? nullptr : _parser->getParameters( "Global" ) ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<StringParameters> Application::geometryParameters () const {
    return (nullptr == _parser) ? nullptr : _parser->getParameters( "Geometry" ) ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<StringParameters> Application::dataSourceParameters () const {
    return (nullptr == _parser) ? nullptr : _parser->getParameters( "DataSource" ) ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<StringParameters> Application::processorParameters ( const std::string &name ) const {
    return (nullptr == _parser) ? nullptr : _parser->getParameters( name ) ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<StringParameters> Application::constants () const {
    return (nullptr == _parser) ? nullptr : _parser->getParameters( "Constants" ) ;
  }

  //--------------------------------------------------------------------------

  std::vector<std::string> Application::activeProcessors () const {
    if( nullptr == _parser ) {
      return std::vector<std::string>() ;
    }
    return _parser->getParameters( "Global" )->getValues<std::string>( "ActiveProcessors", std::vector<std::string>() ) ;
  }

  //--------------------------------------------------------------------------

  std::vector<std::string> Application::processorConditions () const {
    if( nullptr == _parser ) {
      return std::vector<std::string>() ;
    }
    return _parser->getParameters( "Global" )->getValues<std::string>( "ProcessorConditions", std::vector<std::string>() ) ;
  }

  //--------------------------------------------------------------------------

  bool Application::isInitialized() const {
    return _initialized ;
  }

  //--------------------------------------------------------------------------

  Application::Logger Application::logger() const {
    return _loggerMgr.mainLogger() ;
  }

  //--------------------------------------------------------------------------

  Application::Logger Application::createLogger( const std::string &name ) const {
    if ( _loggerMgr.isInitialized() ) {
      return _loggerMgr.createLogger( name ) ;
    }
    else {
      return Logging::createLogger( name ) ;
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<IParser> Application::parser() const {
    return _parser ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<IParser> Application::createParser() const {
    if( _steeringFileName.rfind(".xml") == std::string::npos // .xml not found at all
      || !(  _steeringFileName.rfind(".xml") + strlen(".xml") == _steeringFileName.length() ) ) {
      logger()->log<DEBUG2>() << "createParser: create old steering file parser" << std::endl ;
      return std::make_shared<Parser>( _steeringFileName );
    }
    else {
      logger()->log<DEBUG2>() << "createParser: create XML steering file parser" << std::endl ;
      auto parser = std::make_shared<XMLParser>( _steeringFileName ) ;
      // tell parser to take into account any options defined on the command line
      parser->setCmdLineParameters( _cmdLineOptions ) ;
      return parser ;
    }
  }

} // namespace marlin
