// -- std headers
#include <memory>

// -- marlin headers
#include "marlin/concurrency/PluginManager.h"
#include "marlin/concurrency/Application.h"

using PluginManager = marlin::concurrency::PluginManager ;
using Application = marlin::concurrency::Application ;

int main(int argc, char** argv ) {

  // ---- catch all uncaught exceptions in the end ...
  try {
    // load plugins first
    auto &mgr = PluginManager::instance() ;
    if ( not mgr.loadLibraries() ) {
      throw Exception( "Couldn't load shared libraries from MARLIN_DLL !" );
    }
    // create, init and run the MarlinMT application
    auto app = std::make_shared<Application>() ;
    app->init( argc, argv ) ;
    app->run() ;
    return 0 ;
  }
  catch( std::exception& e) {

    std::cerr << " ***********************************************\n"
	      << " A runtime error occured - (uncaught exception):\n"
	      << "      " <<    e.what() << "\n"
	      << " MarlinMT will have to be terminated, sorry.\n"
	      << " ***********************************************\n"
	      << std:: endl ;
  }
  return 1 ;
}
