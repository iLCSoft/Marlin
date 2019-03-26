#include "marlin/Application.h"
#include "marlin/PluginManager.h"
#include "marlin/Utils.h"
#include "marlin/XMLParser.h"
#include "marlin/Parser.h"

// -- std headers
#include <cstring>

namespace marlin {

  void Application::init( int argc, char **argv ) {
    _programName = argv[0] ;
    for ( int i=1 ; i<argc ; i++ ) {
      _arguments.push_back( argv[i] ) ;
    }
    _logger = Logging::createLogger( program() ) ;
    _logger->setLevel( "MESSAGE" ) ; // set initial log level
    parseCommandLine() ;
    // parse the steering file
    _parser = createParser() ;
    _parser->parse() ;
    // initialize logging
    std::string verbosity = globalParameters()->getStringVal( "Verbosity" ) ;
    if ( not verbosity.empty() ) {
      _logger->setLevel( verbosity ) ;
    }
    // sub-class initialization
    init() ;
    _initialized = true ;
  }

  // //--------------------------------------------------------------------------
  //
  // void Application::printUsage() const {
  //   std::cout << " Usage: MarlinMT [OPTION] [FILE]..." << std::endl
  //       << "   runs a parallel Marlin application (MT)" << std::endl
  //       << std::endl
  //       << " Running the application with a given steering file:" << std::endl
  //       << "   MarlinMT steer.xml   " << std::endl
  //       << std::endl
  //       << " Options:" << std::endl
  //       << "   -p        dump the registered plugins and exit" << std::endl
  //       << "   -j N      set the maximum number of concurrent tasks" << std::endl
  //       << "   -h/-?     print this help information" << std::endl
  //       << std::endl
  //       << " Example: " << std::endl
  //       << " To run a Marlin application on 8 cores, run" << std::endl
  //       << "     MarlinMT -j 8 steer.xml" << std::endl
  //       << " Dynamic command line options may be specified in order to overwrite individual steering file parameters, e.g.:" << std::endl
  //       << "     MarlinMT --constant.Verbosity=DEBUG --input.FileName=inputfile.slcio steer.xml" << std::endl << std::endl
  //       << std::endl ;
  // }

  //--------------------------------------------------------------------------

  void Application::parseCommandLine() {
    _steeringFileName.clear() ;
    _cmdLineOptions.clear() ;
    auto cmdLineArgs = _arguments ;
    if ( cmdLineArgs.empty() ) {
      printUsage() ;
      std::cerr << "No command line option provided. Expected at least a steering file..." << std::endl ;
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
          std::cerr << "*** invalid command line option: " << arg << std::endl ;
          ::exit( 1 ) ;
        }
        auto argKey = StringUtil::split<std::string>( argVec[0] , "." ) ;
        if ( argKey.size() != 2 ) {
          printUsage() ;
          std::cerr << "*** invalid command line option: " << arg << std::endl ;
          ::exit( 1 ) ;
        }
        _cmdLineOptions[ argKey[0] ][ argKey[1] ] = argVec[1] ;
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
    }
    if ( _steeringFileName.empty() ) {
      printUsage() ;
      std::cerr << "No steering file provided ..." << std::endl ;
      ::exit( 1 ) ;
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<StringParameters> Application::globalParameters () const {
    return (nullptr == _parser) ? nullptr : _parser->getParameters( "Global" ) ;
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

  StringVec Application::activeProcessors () const {
    StringVec list ;
    if( nullptr == _parser ) {
      return list ;
    }
    _parser->getParameters( "Global" )->getStringVals( "ActiveProcessors", list ) ;
    return list ;
  }

  //--------------------------------------------------------------------------

  StringVec Application::processorConditions () const {
    StringVec list ;
    if( nullptr == _parser ) {
      return list ;
    }
    _parser->getParameters( "Global" )->getStringVals( "ProcessorConditions", list ) ;
    return list ;
  }

  //--------------------------------------------------------------------------

  bool Application::isInitialized() const {
    return _initialized ;
  }

  //--------------------------------------------------------------------------

  Application::Logger Application::logger() const {
    return _logger ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<IParser> Application::parser() const {
    return _parser ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<IParser> Application::createParser() const {
    if( _steeringFileName.rfind(".xml") == std::string::npos // .xml not found at all
      || !(  _steeringFileName.rfind(".xml") + strlen(".xml") == _steeringFileName.length() ) ) {
      return std::make_shared<Parser>( _steeringFileName );
    }
    else {
      auto parser = std::make_shared<XMLParser>( _steeringFileName ) ;
      // tell parser to take into account any options defined on the command line
      parser->setCmdLineParameters( _cmdLineOptions ) ;
      return parser ;
    }
  }

} // namespace marlin
