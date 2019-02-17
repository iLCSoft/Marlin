#ifndef MARLIN_CONCURRENCY_PROCESSOR_h
#define MARLIN_CONCURRENCY_PROCESSOR_h 1

#include <map>
#include <string>

#include "marlin/Exceptions.h"
#include "marlin/concurrency/Internal.h"
#include "marlin/concurrency/Event.h"
#include "marlin/concurrency/Parameters.h"

namespace marlin {

  namespace concurrency {

    template<typename T>
    class ProcessorFactoryT ;
    class Application ;

    /**
     *  @brief  Processor class
     *  Base class for user processor
     */
    class Processor {
      // friendship for creation through PluginManager
      template<typename T>
      friend class ProcessorFactoryT;
      // no copy, no assignment
      Processor(const Processor &) = delete ;
      Processor& operator=(const Processor &) = delete ;

    public:
      /**
       *  @brief  Constructor
       *
       *  @param  t the processor type
       */
      Processor( const std::string &t ) ;

      /**
       *  @brief  Destructor
       */
      virtual ~Processor() ;

      /**
       *  @brief  Get the processor type
       */
      const std::string &type() const ;

      /**
       *  @brief  Get the processor name
       */
      const std::string &name() const ;

      /**
       *  @brief  Get the processor description
       */
      const std::string &description() const ;

      /**
       *  @brief  Initialize the processor.
       */
      virtual void init() { /* nop */ }

      /**
       *  @brief  Called at start of new run
       *
       *  @brief  header the run header to process
       */
      virtual void processorRunHeader( std::shared_ptr<EVENT::LCRunHeader> /*header*/ ) { /* nop */ }

      /**
       *  @brief  Called for every event
       *
       *  @brief  event the event to process
       */
      virtual void processorEvent( std::shared_ptr<Event<EVENT::LCEvent>> /*event*/ ) const { /* nop */ }

      /**
       *  @brief  Called at application termination.
       *  Best place to clear allocated resources!
       */
      virtual void end() { /* nop */ }

      /**
       *  @brief  The Marlin application in which the processor runs
       */
      const Application &app() const ;

      /**
       *  @brief  The Marlin application in which the processor runs
       */
      Application &app() ;

      /**
       *  @brief  Get the processor parameters
       */
      const Parameters &parameters() const ;

      /**
       *  @brief  Print processor parameters in console
       */
      void printParameters() const ;

    protected:
      /**
       *  @brief  Register a processor parameter.
       *  The parameter is not initialized. If the steering file
       *  does not set the parameter, an exception is thrown on
       *  parameter access. Parameters are accessed in processor
       *  methods as:
       *  \code{cpp}
       *  // this throws an exception if the parameter is
       *  // not set in the steering file
       *  auto intParam = parameters().get<int>("MyIntParameters") ;
       *  // use a fallback value to avoid this
       *  auto safeParam = parameters().get<int>("MyIntParameters", 42) ;
       *  \endcode
       *
       *  @param parameterName the parameter name
       *  @param parameterDescription the parameter description
       */
      void registerProcessorParameter( const std::string &parameterName,
        const std::string &parameterDescription );

      /**
       *  @brief  Register a processor parameter.
       *  The parameter is initialized with a default value.
       *
       *  @param  parameterName the parameter name
       *  @param  parameterDescription the parameter description
       *  @param  value the default parameter value
       */
      template <typename T>
      void registerProcessorParameter( const std::string &parameterName,
        const std::string &parameterDescription,
        const T &value);

    private:
      /**
       *  @brief  Set processor name
       */
      void setName( const std::string &n ) ;

      /**
       *  @brief  Set the Marlin application
       *
       *  @param  application the Marlin application
       */
      void setApp( Application *application ) ;

    protected:
      /// The processor description
      std::string            _description {} ;

    private:
      /// The processor type
      std::string            _type {} ;
      /// The processor name
      std::string            _name {} ;
      /// The Marlin application in which the processor runs
      Application           *_application {nullptr} ;
      /// The processor parameters
      Parameters             _parameters {} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline void Processor::registerProcessorParameter( const std::string &parameterName,
      const std::string &parameterDescription,
      const T &value ) {
      if ( _parameters.exists( parameterName ) ) {
        throw Exception( "Parameter '" + parameterName + "' registered twice!" );
      }
      auto &parameter = _parameters.set( parameterName, value ) ;
      parameter.setDescription( parameterDescription );
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
