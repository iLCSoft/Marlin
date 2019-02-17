#ifndef MARLIN_CONCURRENCY_APPLICATION_h
#define MARLIN_CONCURRENCY_APPLICATION_h 1

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/concurrency/Parameters.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Application class
     */
    class Application {
    public:
      /**
       *  @brief  Constructor
       */
      Application() ;

      /**
       *  @brief  Get the global parameters (const version)
       */
      const Parameters &global() const ;

      /**
       *  @brief  Get the global parameters (non-const version)
       */
      Parameters &global() ;

    private:
      /// Global parameters of the application
      Parameters            _global {} ;
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
