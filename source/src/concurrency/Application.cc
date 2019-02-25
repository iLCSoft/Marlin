#include "marlin/concurrency/Application.h"

namespace marlin {

  namespace concurrency {

    Application::Application() {
      /* nop */
    }

    //--------------------------------------------------------------------------

    const Parameters &Application::global() const {
      return _global ;
    }

    //--------------------------------------------------------------------------

    Parameters &Application::global() {
      return _global ;
    }

    //--------------------------------------------------------------------------

    void Application::init( int argc, char **argv ) {
      _programName = argv[0] ;
      for ( int i=1 ; i<argc ; i++ ) {
        _arguments.push_back( argv[i] ) ;
      }
    }

    //--------------------------------------------------------------------------

    void Application::run() {
      /* nop */
    }

  } // namespace concurrency

} // namespace marlin
