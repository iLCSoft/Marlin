#ifndef MARLIN_CONCURRENCY_PROCESSOR_h
#define MARLIN_CONCURRENCY_PROCESSOR_h 1

#include <map>
#include <string>

#include "marlin/Exceptions.h"
#include "marlin/concurrency/Internal.h"
#include "marlin/concurrency/Event.h"
#include "marlin/concurrency/Parameters.h"

class TiXmlElement ;

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

    private:
      // no copy, no assignment
      Processor(const Processor &) = delete ;
      Processor& operator=(const Processor &) = delete ;

    public:
      /**
       *  @brief  Destructor
       */
      virtual ~Processor() = default ;

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
      virtual void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> /*header*/ ) { /* nop */ }

      /**
       *  @brief  Called for every event
       *
       *  @brief  event the event to process
       */
      virtual void processEvent( std::shared_ptr<Event> /*event*/ ) const { /* nop */ }

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
       *  @brief  Constructor
       *
       *  @param  t the processor type
       */
      Processor( const std::string &t ) ;

      /**
       *  @brief  Register a processor parameter.
       *  The parameter is not initialized. If the steering file
       *  does not set the parameter, an exception is thrown on parsing
       *  Parameters are accessed in processor methods as:
       *  \code{cpp}
       *  auto intParam = parameters().get<int>("MyIntParameter") ;
       *  auto vecParam = parameters().getVector<float>("MyFloatParameters") ;
       *  \endcode
       *
       *  @param parameterName the parameter name
       *  @param parameterDescription the parameter description
       */
      void registerParameter( const std::string &parameterName,
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
      void registerOptionalParameter( const std::string &parameterName,
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

      /**
       *  @brief  Parse the XML element containing processor parameters
       *
       *  @param  element the XML element to parse
       */
      void parseParameters( const TiXmlElement *const element ) ;

      /**
       *  @brief  Populate the XML element with all registered parameters
       *
       *  @param  element the XML element to populate
       */
      void populateXMLParameters( TiXmlElement *element ) const ;

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
    inline void Processor::registerOptionalParameter( const std::string &parameterName,
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
