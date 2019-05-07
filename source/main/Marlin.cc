// -- marlin headers
#include <marlin/Application.h>
#include <marlin/SimpleScheduler.h>
#include <marlin/PluginManager.h>

using namespace marlin ;

#define MARLIN_MAIN_LOG( MLEVEL , message ) \
  if ( nullptr != application ) { \
    application->logger()->log<MLEVEL>() << message ; \
  } \
  else { \
    streamlog_out(MLEVEL) << message ; \
  }

int main( int argc, char **argv ) {
  
  std::shared_ptr<Application> application {nullptr} ;
  
  // load plugins first
  auto &mgr = PluginManager::instance() ;
  mgr.logger()->setLevel<MESSAGE>();
  if ( not mgr.loadLibraries() ) {
    throw Exception( "Couldn't load shared libraries from MARLIN_DLL !" ) ;
  }
  // configure and run application
  try {
    application = std::make_shared<Application>() ;    
    application->setScheduler( std::make_shared<SimpleScheduler>() ) ;
    application->init( argc, argv ) ;
    application->run() ;
  }
  catch ( marlin::Exception &e ) {
    MARLIN_MAIN_LOG( ERROR, "Marlin main, caught Marlin exception " << e.what() << std::endl )
    MARLIN_MAIN_LOG( ERROR, "Exiting with status 1" << std::endl )
    return 1 ;
  }
  catch ( std::exception &e ) {
    MARLIN_MAIN_LOG( ERROR, "Marlin main, caught std::exception " << e.what() << std::endl )
    MARLIN_MAIN_LOG( ERROR, "Exiting with status 1" << std::endl )
    return 1 ;
  }
  catch ( ... ) {
    MARLIN_MAIN_LOG( ERROR, "Marlin main, caught unknown exception" << std::endl )
    MARLIN_MAIN_LOG( ERROR, "Exiting with status 2" << std::endl )
    return 2 ;
  }

  return 0 ;
}
