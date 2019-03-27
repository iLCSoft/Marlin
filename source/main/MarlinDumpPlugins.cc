// -- std headers
#include <memory>

// -- marlin headers
#include <marlin/Logging.h>
#include <marlin/PluginManager.h>
#include <marlin/Exceptions.h>

using namespace marlin ;

int main() {
  // load plugins first
  auto &mgr = PluginManager::instance() ;
  mgr.logger()->setLevel<MESSAGE>();
  if ( not mgr.loadLibraries() ) {
    throw Exception( "Couldn't load shared libraries from MARLIN_DLL !" ) ;
  }
  mgr.dump() ;
  return 0 ;
}
