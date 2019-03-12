#include "marlin/Application.h"
#include "marlin/PluginManager.h"
#include "marlin/Utils.h"

// -- std headers
#include <thread>

namespace marlin {

  Application::Application() {
    /* nop */
  }

  //--------------------------------------------------------------------------

  void Application::init( int argc, char **argv ) {
    _programName = argv[0] ;
    for ( int i=1 ; i<argc ; i++ ) {
      _arguments.push_back( argv[i] ) ;
    }
    CommandLineParametersMap cmdLineOptions {} ;
    parseCommandLine( cmdLineOptions ) ;
    // parse the steering file
    _parser = std::make_shared<XMLParser>( _steeringFileName ) ;
    _parser->setCmdLineParameters( cmdLineOptions ) ;
    _parser->parse() ;
    auto globals = globalParameters() ;
    auto ccy = globals->getIntVal( "Concurrency" ) ;
    // Max concurrency preference order:
    // 1) from command line
    // 2) steering file
    // Determine also the max concurrency from the input file list size
    auto requestedCcy = (_concurrency > 0) ? _concurrency : (ccy > 0) ? ccy : 0 ;
    StringVec lcioInputFiles {} ;
    globals->getStringVals( "lcioInputFiles", lcioInputFiles ) ;
    _concurrency = std::min<unsigned int>( lcioInputFiles.size(), requestedCcy ) ;    
    if ( _concurrency > 1 ) {
      std::cout << "Running in single threaded mode !" << std::endl ;
    }
    else {
      std::cout << "Running in multi-threaded mode (nthreads=" << _concurrency << ") !" << std::endl ;
    }
    _initialized = true ;
  }

  //--------------------------------------------------------------------------

  void Application::run() {
    if ( not _initialized ) {
      throw Exception( "The MarlinMT application has not been initialized !" );
    }

  }

  //--------------------------------------------------------------------------

  void Application::printUsage() const {
    std::cout << " Usage: MarlinMT [OPTION] [FILE]..." << std::endl
        << "   runs a parallel Marlin application (MT)" << std::endl
        << std::endl
        << " Running the application with a given steering file:" << std::endl
        << "   MarlinMT steer.xml   " << std::endl
        << std::endl
        << " Options:" << std::endl
        << "   -p        dump the registered plugins and exit" << std::endl
        << "   -j N      set the maximum number of concurrent tasks" << std::endl
        << "   -h/-?     print this help information" << std::endl
        << std::endl
        << " Example: " << std::endl
        << " To run a Marlin application on 8 cores, run" << std::endl
        << "     MarlinMT -j 8 steer.xml" << std::endl
        << " Dynamic command line options may be specified in order to overwrite individual steering file parameters, e.g.:" << std::endl
        << "     MarlinMT --constant.Verbosity=DEBUG --input.FileName=inputfile.slcio steer.xml" << std::endl << std::endl
        << std::endl ;
  }

  //--------------------------------------------------------------------------

  void Application::parseCommandLine( CommandLineParametersMap &cmdLineOptions ) {
    _steeringFileName.clear() ;
    _concurrency = 0 ;
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
        cmdLineOptions[ argKey[0] ][ argKey[1] ] = argVec[1] ;
        iter = cmdLineArgs.erase( iter ) ;
        continue;
      }
      ++iter ;
    }
    // parse remaining arguments
    iter = cmdLineArgs.begin() ;
    while ( cmdLineArgs.end() != iter  ) {
      auto arg = *iter ;
      // get number of cores option
      if ( arg == "-j" ) {
        auto iterNext = std::next( iter ) ;
        if ( cmdLineArgs.end() == iterNext ) {
          printUsage() ;
          std::cerr << "Argument -j: expecting number after option!" << std::endl ;
          ::exit( 1 ) ;
        }
        unsigned int ccy = StringUtil::stringToType<unsigned int>( *iterNext ) ;
        if ( ccy > std::thread::hardware_concurrency() ) {
          std::cout << "*** WARNING: Using more threads than optimal (hardware_concurrency: " <<
          std::thread::hardware_concurrency() << ", set: " << ccy << ") !" << std::endl ;
        }
        _concurrency = ccy ;
        // erase -j and following number
        iter = cmdLineArgs.erase( cmdLineArgs.erase( iter ) ) ;
        continue ;
      }
      // print plugin manager content and exit
      else if ( arg == "-p" ) {
        PluginManager::instance().dump() ;
        ::exit( 0 ) ;
      }
      else if ( arg == "-h" || arg == "-?" ) {
        printUsage() ;
        ::exit( 0 ) ;
      }
      else {
        _steeringFileName = arg ;
        iter = cmdLineArgs.erase( iter ) ;
      }
    }
    if ( not cmdLineArgs.empty() ) {
      std::cerr << "*** ERROR: The following command line arguments have not been parsed: " ;
      for ( auto argiter : cmdLineArgs ) {
        std::cerr << argiter << " ";
      }
      std::cerr << std::endl ;
      ::exit( 1 ) ;
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
  
  unsigned int Application::concurrencyLevel() const {
    return _concurrency ;
  }

} // namespace marlin
