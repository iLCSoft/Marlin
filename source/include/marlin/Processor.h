#ifndef MARLIN_PROCESSOR_h
#define MARLIN_PROCESSOR_h 1

// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>

// -- marlin headers
#include <marlin/StringParameters.h>
#include <marlin/Parameter.h>
#include <marlin/Logging.h>
#include <marlin/MarlinConfig.h>  // for Marlin version macros

// -- std headers
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>

namespace marlin {

  class Application ;
  class XMLFixCollTypes ;

  /**
   *  @brief  Processor class
   *
   *  Base class for Marlin processors.
   *  Users can optionaly overwrite the following methods: <br>
   *    init, processRun, processEvent and end.<br>
   *  Use registerProcessorParameter to define all parameters that the module uses.
   *  Registered parameters are filled automatically before init() is called.
   *  With MyAplication -l you can print a list of available processors including
   *  the steering parameters they use/need.<br>
   *  With MyAplication -x you can print an example XML steering file for all known
   *  processors.
   *
   *  @see init
   *  @see processRun
   *  @see processEvent
   *  @see end
   *
   *  @author F. Gaede, DESY
   *  @version $Id: Processor.h,v 1.38 2008-06-26 10:25:36 gaede Exp $
   */
  class Processor : public Parametrized {
    friend class XMLFixCollTypes ;
    using Logger = Logging::Logger ;

  private:
    // prevent users from making (default) copies of processors
    Processor() = delete ;
    Processor(const Processor&) = delete ;
    Processor& operator=(const Processor&) = delete ;
    
  public:
    virtual ~Processor() = default ;
    
    /**
     *  @brief  RuntimeOption enumerator
     */
    enum class RuntimeOption {
      Critical,     /// Whether the processor has to be executed in a critical section
      Clone         /// Whether the processor must be cloned in each thread worker
    };

    using RuntimeOptions = std::map<RuntimeOption, bool> ;

  public:
    /**
     *  @brief  Constructor. Subclasses need to call this in their default constructor.
     *
     *  @param  typeName the processor type
     */
    Processor(const std::string& typeName) ;

    /**
     *  @brief  Return a new instance of the processor (factory method)
     */
    virtual Processor* newProcessor() = 0 ;

    /**
     *  @brief  Initialize the processor.
     *  Called at the begin of the job before anything is read.
     *  Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() { /* nop */ }

    /**
     *  @brief  Process a run header (start of run)
     *  Called for every run, e.g. overwrite to initialize run dependent histograms.
     */
    virtual void processRunHeader( EVENT::LCRunHeader* ) { /* nop */ }

    /**
     *  @brief  Process an input event.
     *  Called for every event - the working horse.
     */
    virtual void processEvent( EVENT::LCEvent * ) { /* nop */ }

    /**
     *  @brief Called for every event - right after processEvent()
     *  has been called for this processor.
     *  Use to check processing and/or produce check plots.
     */
    virtual void check( EVENT::LCEvent* ) { /* nop */ }

    /**
     *  @brief  Terminate the processor.
     *  Called after data processing for clean up in the inverse order of the init()
     *  method so that resources allocated in the first processor also will be available
     *  for all following processors.
     */
    virtual void end() { /* nop */ }

    /**
     *  @brief  Return type name for the processor (as set in constructor).
     */
    virtual const std::string & type() const ;

    /**
     *  @brief  Return the name of this  processor.
     */
    virtual const std::string & name() const ;

    /**
     *  @brief  Return the name of the local verbosity level of this  processor - "" if not set.
     */
    virtual const std::string & logLevelName() const ;

    /**
     *  @brief  Return the parameters defined for this Processor.
     */
    virtual std::shared_ptr<StringParameters> parameters() const ;

    /**
     *  @brief  Print information about this processor in ASCII steering file format.
     */
    virtual void printDescription() const ;

    /**
     *  @brief  Print information about this processor in XML steering file format.
     */
    virtual void printDescriptionXML(std::ostream& stream=std::cout) const ;

    /**
     *  @brief  Print the parameters and their values depending on the given verbosity level.
     */
    template <class T>
    void printParameters() const ;

    /**
     *  @brief  Print the parameters and their values with verbosity level MESSAGE.
     */
    void printParameters() const ;

    /**
     *  @brief  Description of processor.
     */
    const std::string& description() const ;

    /**
     *  @brief  Get the forced runtime option settings.
     *  The pair returned contains two boolean values:
     *    - first: whether the option appears in the forced options
     *    - second: if first is true, then contains the option flag
     *
     *  @param  option the runtime option to get
     */
    std::pair<bool,bool> getForcedRuntimeOption( RuntimeOption option ) const ;

    /** Sets the registered steering parameters before calling init() */
    void baseInit( const Application *application ) ;

    /** Initialize the parameters */
    void setParameters( std::shared_ptr<StringParameters> parameters) ;

  protected:
    /**
     *  @brief  Force the runtime option to a given boolean value.
     *
     *  Depending on the implementation of your processor, setting
     *  one of the runtime option might be a necessity. For example, you
     *  handle a lot of data in your processor members that you don't want
     *  to duplicate. In this case you should call in the constructor
     *  @code{cpp}
     *  forceRuntimeOption( Processor::RuntimeOption::Clone, false ) ;
     *  @endcode
     *  The code contained in processEvent() might also not be thread safe.
     *  In this case you can ask the Marlin framework to call the processEvent()
     *  method in a critical section (using std::mutex). Do to so, call:
     *  @code{cpp}
     *  forceRuntimeOption( RuntimeOption::Critical, true ) ;
     *  @endcode
     *  If a runtime option is forced in the code and the steering file tries
     *  to overwrite it, an exception will be raised.
     *
     *  Note that this method must be called in user processor constructor
     *  to ensure it is correctly handled by the framework at configuration time.
     *
     *
     *  @param  option the runtime option to force
     *  @param  value the boolean value to set
     */
    void forceRuntimeOption( RuntimeOption option, bool value ) ;

    /**
     *  @brief  Alias function to Parametrized::registerParameter
     */
     template<class T>
     void registerProcessorParameter(const std::string& parameterName,
 				    const std::string& parameterDescription,
 				    T& parameter,
 				    const T& defaultVal,
				    int setSize = 0 ) ;

    /**
     *  @brief  Print message according to  verbosity level of the templated parameter (one of
     *  DEBUG, MESSAGE, WARNING, ERROR ) and the global parameter "Verbosity".
     *  If Marlin is compiled w/o debug mode ($MARLINDEBUG not set) then DEBUG messages
     *  will be ignored completely at compile time, i.e. no output (and code!) will be
     *  generated, regardless of the value of the "Verbosity" parameter.
     *  This is useful in order to save CPU time in production mode.<br>
     *  Every line of the output string will be prepended by the verbosity level of the
     *  message and the processor name, e.g:
     *  <pre>
     *    [ MESSAGE "MyTestProcessor" ]  processing event 42 in run 4711
     *  </pre>
     *  Use this method for simple strings. In order to use more complex messages, including
     *  numbers, use:
     *  @see  void message( const std::basic_ostream<char, std::char_traits<char> >& m)
     *  @deprecated
     */
    template <class T>
    void message(  const std::string& m ) const ;

    /**
     *  @brief  Same as  message(const std::string& message) except that it allows the output of
     *  more complex messages in the argument using the log() method, e.g.:
     * <pre>
     * message<MESSAGE>( log()
     *                   << " processing event " << evt->getEventNumber()
     *                   << "  in run "          << evt->getRunNumber()
     *                   ) ;
     * </pre>
     *
     *
     * @deprecated
     * @see void message(  const std::string& message )
     * @see std::stringstream& log()
     */
    template <class T>
    inline void message( const std::basic_ostream<char, std::char_traits<char> >& m) const ;

    /**
     *  @brief  Logging function
     *  Example usage:
     *  @code{cpp}
     *  log<DEBUG>() << "This is a DEBUG message" << std::endl ;
     *  @endcode
     */
    template <class T>
    Logging::StreamType log() const ;

    /**
     *  @brief  Get the application in which the processor is running
     *  Throws if the application is not set
     */
    const Application &app() const ;

  private:
    /** Allow friend class CCProcessor to change/reset processor parameters */
    virtual void setProcessorParameters( std::shared_ptr<StringParameters> processorParameters) ;

    /** Allow friend class CCProcessor to update processor parameters */
    virtual void updateParameters();

    /** Set processor name */
    virtual void setName( const std::string & processorName) ;

  protected:
    /// The processor description
    std::string                        _description {""} ;
    /// The processor type
    std::string                        _typeName {""} ;
    /// The processor name
    std::string                        _processorName {""} ;
    /// The processor parameters
    std::shared_ptr<StringParameters>  _parameters {nullptr} ;
    /// The processor logger level
    std::string                        _logLevelName {} ;

  private:
    /// The processor logger. See log<T>() for details
    Logger                             _logger {nullptr} ;
    /// The application in which the processor is running
    const Application                 *_application {nullptr} ;
    /// The user forced runtime options for parallel processing
    RuntimeOptions                     _forcedRuntimeOptions {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <class T>
  inline void Processor::printParameters() const {
    if( _logger->wouldWrite<T>() ) {
      _logger->log<T>() << std::endl << "---- " << name()  <<" -  parameters: " << std::endl ;
      for( auto i = this->pbegin() ; i != this->pend() ; i ++ ) {
        if( ! i->second->isOptional() || i->second->valueSet() ){
          _logger->log<T>() << "\t"   << i->second->name()
               << ":  "  << i->second->value()
               << std::endl ;
        }
      }
      _logger->log<T>() << "-------------------------------------------------" << std::endl ;
    }
  }

  //--------------------------------------------------------------------------

  inline const std::string & Processor::type() const {
    return _typeName ;
  }

  //--------------------------------------------------------------------------

  inline const std::string & Processor::name() const {
    return _processorName ;
  }

  //--------------------------------------------------------------------------

  inline const std::string & Processor::logLevelName() const {
    return _logLevelName ;
  }

  //--------------------------------------------------------------------------

  inline std::shared_ptr<StringParameters> Processor::parameters() const {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  inline const std::string& Processor::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  template<class T>
  inline void Processor::registerProcessorParameter(const std::string& parameterName,
         const std::string& parameterDescription,
         T& parameter,
         const T& defaultVal,
         int setSize ) {
    registerParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;
  }

  //--------------------------------------------------------------------------

  template <class T>
  inline void Processor::message(  const std::string& m ) const {
    _logger->log<T>() << m << std::endl ;
  }

  //--------------------------------------------------------------------------

  template <class T>
  inline void Processor::message( const std::ostream& m) const {
    if( T::active ){  // allow the compiler to optimize this away ...
      try {
        const std::stringstream& mess = dynamic_cast<const std::stringstream&>( m ) ;
        this->template message<T>( mess.str() ) ;
      }
      catch( std::bad_cast ) {}
    }
  }

  //--------------------------------------------------------------------------

  template <class T>
  inline Logging::StreamType Processor::log() const {
    return _logger->log<T>() ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::setProcessorParameters( std::shared_ptr<StringParameters> processorParameters) {
     setParameters( processorParameters ) ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::setName( const std::string & processorName) {
    _processorName = processorName ;
    _logger->setName( processorName );
  }

} // end namespace marlin

#endif
