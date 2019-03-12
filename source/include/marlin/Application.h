#ifndef MARLIN_CONCURRENCY_APPLICATION_h
#define MARLIN_CONCURRENCY_APPLICATION_h 1

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/concurrency/Parameters.h"
#include "marlin/concurrency/AppParser.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Application class
     */
    class Application {
    public:
      // traits
      typedef std::vector<std::string> CmdLineArguments ;
      typedef std::map<std::string, std::map<std::string, std::string>> CommandLineParametersMap ;

    public:
      /**
       *  @brief  Constructor
       */
      Application() ;

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

      /**
       *  @brief  Get the program name
       */
      const std::string &program() const { return _programName ; }

      /**
       *  @brief  Get the command line arguments
       */
      const CmdLineArguments &arguments() const { return _arguments ; }

      /**
       *  @brief  Get a specific constant
       *
       *  @param  name the constant name
       */
      template <typename T>
      T constant( const std::string &name ) const ;

      /**
       *  @brief  Get a specific constant
       *
       *  @param  name the constant name
       *  @param  fallback the fallback value if the constant doesn't exists
       */
      template <typename T>
      T constant( const std::string &name , const T &fallback ) const ;

      /**
       *  @brief  Print command line usage
       */
      void printUsage() const ;

    private:
      /**
       *  @brief  Parse the command line arguments
       */
      void parseCommandLine();

    private:
      /// Global parameters of the application
      Parameters                 _global {} ;
      /// The program name. Initialized on init()
      std::string                _programName {} ;
      /// The arguments from the main function after init has been called
      CmdLineArguments           _arguments {} ;
      /// Whether the application has been initialized
      bool                       _initialized {false} ;
      /// The application specific steering file parser
      AppParser                  _parser {} ;
      /// The command line options to replace in the steering file
      CommandLineParametersMap   _cmdLineOptions {} ;
      /// The steering file name
      std::string                _steeringFileName {} ;
      /// The concurrency as read from the command line or steering file
      unsigned int               _concurrency {0};
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline T Application::constant( const std::string &name ) const {
      return _parser.constant<T>( name );
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline T Application::constant( const std::string &name , const T &fallback ) const {
      return _parser.constant<T>( name, fallback );
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
