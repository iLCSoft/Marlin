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
      // traits
      typedef std::vector<std::string> CmdLineArguments ;

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

      /**
       *  @brief  Initialize the application
       *
       *  @param  argc argc from main function
       *  @param  argv argv from main function
       */
      void init( int argc, char **argv ) ;

      /**
       *  @brief  Run the Marlin application
       */
      void run() ;

    private:
      /// Global parameters of the application
      Parameters            _global {} ;
      /// The program name. Initialized on init()
      std::string           _programName {} ;
      /// The arguments from the main function after init has been called
      CmdLineArguments      _arguments {} ;
      /// Whether the application has been initialized
      bool                  _initialized {false} ;
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
