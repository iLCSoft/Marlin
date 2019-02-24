#include "lcio.h"

#include "marlin/concurrency/PluginManager.h"

int printUsage() ;

using PluginManager = marlin::concurrency::PluginManager ;

int main(int argc, char** argv ) {

  // ---- catch all uncaught exceptions in the end ...
  try {

    auto &mgr = PluginManager::instance() ;
    if ( not mgr.loadLibraries() ) {
      throw Exception( "Couldn't load shared libraries from MARLIN_DLL !" );
    }

    if( argc > 1 ) {
      if( std::string(argv[1]) == "-p" ){
        mgr.dump() ;
        return(0) ;
      }
      else if( std::string(argv[1]) == "-h"  || std::string(argv[1]) == "-?" ){
        return printUsage() ;
      }
    }
    else {
      return printUsage() ;
    }
    return 0 ;

  } catch( std::exception& e) {

    std::cerr << " ***********************************************\n"
	      << " A runtime error occured - (uncaught exception):\n"
	      << "      " <<    e.what() << "\n"
	      << " MarlinMT will have to be terminated, sorry.\n"
	      << " ***********************************************\n"
	      << std:: endl ;
  }
  return 1 ;
}

int printUsage() {

  std::cout << " Usage: MarlinMT [OPTION]" << std::endl
	    << "   runs a parallel Marlin application " << std::endl
	    << std::endl
      << "   Marlin [-h/-?]             \t print this help information" << std::endl
	    << "   Marlin -p                  \t dump the registered plugins and exit" << std::endl
      << "   Marlin -j N                \t set the number of cores to use for parallel processing" << std::endl
	    << std::endl ;

  return(0) ;

}
