#ifndef MARLIN_MARLINAPPLICATION_h
#define MARLIN_MARLINAPPLICATION_h 1

// -- marlin headers
#include "marlin/Application.h"

namespace marlin {

  /**
   *  @brief  MarlinApplication class
   *  Implementation of standard Marlin processing (single threaded program).
   *  Parse a steering file from which a processor chain is described, configure
   *  processor and run the processor chain using LCIO input files, read one by one.
   */
  class MarlinApplication : public Application {
  public:
    MarlinApplication() = default ;
    
  private:
    void run() ;
    void init() ;
    void printUsage() const ;

  // protected:
  //   /// The arguments from main function after command line arguments have been removed
  //   CmdLineArguments           _filteredArguments {} ;
  // 
  // private:
  //   /// The program name. Initialized on init()
  //   std::string                _programName {} ;
  //   /// The arguments from main function
  //   CmdLineArguments           _arguments {} ;
  //   /// The command line parameter arguments (parameters and constants)
  //   CommandLineParametersMap   _cmdLineOptions {} ;
  //   /// Whether the application has been initialized
  //   bool                       _initialized {false} ;
  //   /// The steering file name
  //   std::string                _steeringFileName {} ;
  //   /// The XML steering file parser
  //   std::shared_ptr<IParser>   _parser {nullptr} ;
  //   /// The application logger
  //   Logger                     _logger {nullptr} ;
  };

} // end namespace marlin

#endif
