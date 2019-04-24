#ifndef MARLIN_PROCESSOR_h
#define MARLIN_PROCESSOR_h 1

// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>

// -- marlin headers
#include <marlin/StringParameters.h>
#include <marlin/ProcessorParameter.h>
#include <marlin/Logging.h>
#include <marlin/MarlinConfig.h>  // for Marlin version macros

// -- std headers
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>

namespace marlin {

  class ProcessorMgr ;
  class ProcessorEventSeeder ;
  class PluginManager ;
  class Application ;
  class Scheduler ;
  class XMLFixCollTypes ;
  class ProcessorFactory ;

  typedef std::map<std::string, ProcessorParameter* > ProcParamMap ;
  typedef std::map<std::string, std::string >         LCIOTypeMap ;

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
  class Processor {
    friend class ProcessorMgr ; // TODO to be removed
    friend class ProcessorEventSeeder ; // TODO to be removed
    friend class ProcessorFactory ;
    friend class Scheduler ;
    friend class CMProcessor ;
    friend class XMLFixCollTypes ;

    using Logger = Logging::Logger ;

  private:
    // prevent users from making (default) copies of processors
    Processor() = delete ;
    Processor(const Processor&) = delete ;
    Processor& operator=(const Processor&) = delete ;
    
  public:
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
     *  @brief  Destructor
     */
    virtual ~Processor() ;

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
    virtual std::shared_ptr<StringParameters> parameters() ;

    /**
     *  @brief  Print information about this processor in ASCII steering file format.
     */
    virtual void printDescription() ;

    /**
     *  @brief  Print information about this processor in XML steering file format.
     */
    virtual void printDescriptionXML(std::ostream& stream=std::cout) ;

    /**
     *  @brief  Print the parameters and their values depending on the given verbosity level.
     */
    template <class T>
    void printParameters() ;

    /**
     *  @brief  Print the parameters and their values with verbosity level MESSAGE.
     */
    void printParameters() ;

    /**
     *  @brief  Description of processor.
     */
    const std::string& description() ;

    /**
     *  @brief  True if first event in processEvent(evt) - use this e.g. to initialize histograms etc.
     */
    bool isFirstEvent() ;

    /**
     *  @brief  Return the LCIO input type for the collection colName - empty string if colName is
     *  not a registered collection name
     */
    std::string getLCIOInType( const std::string& colName ) ;

    /**
     *  @brief  Return the LCIO output type for the collection colName - empty string if colName is
     *  not a registered collection name
     */
    std::string getLCIOOutType( const std::string& colName ) ;

    /**
     *  @brief  True if the given parameter defines an LCIO input collection, i.e. the type has
     *  been defined with setLCIOInType().
     */
    bool isInputCollectionName( const std::string& parameterName  ) ;

    /**
     *  @brief   True if the given parameter defines an LCIO output collection
     */
    bool isOutputCollectionName( const std::string& parameterName  ) ;
    
    /**
     *  @brief  Get the forced runtime option settings.
     *  The pair returned contains two boolean values:
     *    - first: whether the option appears in the forced options
     *    - second: if first is true, then contains the option flag
     *    
     *  @param  option the runtime option to get
     */
    std::pair<bool,bool> getForcedRuntimeOption( RuntimeOption option ) const ;

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
     *  @brief  Set the return value for this processor - typically at end of processEvent().
     *  The value can be used in a condition in the steering file referred to by the name
     *  of the processor.
     */
    void setReturnValue( bool val) ;

    /**
     *  @brief  Set a named return value for this processor - typically at end of processEvent()
     *  The value can be used in a condition in the steering file referred to by
     *  ProcessorName.name of the processor.
     */
    void setReturnValue( const std::string& name, bool val ) ;

    /**
     *  @brief  Register a steering variable for this processor - call in constructor of processor.
     *  The default value has to be of the _same_ type as the parameter, e.g.<br>
     *  float _cut ;<br>
     *  ...<br>
     *   registerProcessorParameter( "Cut", "cut...", _cut , float( 3.141592 ) ) ;<br>
     *  as implicit conversions don't work for templates.<br>
     *  The optional parameter setSize is used for formating the printout of parameters.
     *  This can be used if the parameter values are expected to come in sets of fixed size.
     */
     template<class T>
     void registerProcessorParameter(const std::string& parameterName,
 				    const std::string& parameterDescription,
 				    T& parameter,
 				    const T& defaultVal,
				    int setSize=0 );

    /**
     *  @brief  Specialization of registerProcessorParameter() for a parameter that defines an
     *  input collection - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollection(const std::string& collectionType,
				 const std::string& parameterName,
				 const std::string& parameterDescription,
				 std::string& parameter,
				 const std::string& defaultVal,
				 int setSize=0 ) ;

    /**
     *  @brief  Specialization of registerProcessorParameter() for a parameter that defines an
     *  output collection - can be used fo checking the consistency of the steering file.
     */
    void registerOutputCollection(const std::string& collectionType,
				  const std::string& parameterName,
				  const std::string& parameterDescription,
				  std::string& parameter,
				  const std::string& defaultVal,
				  int setSize=0 ) ;

    /**
     *  @brief  Specialization of registerProcessorParameter() for a parameter that defines one or several
     *  input collections - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollections(const std::string& collectionType,
				  const std::string& parameterName,
				  const std::string& parameterDescription,
				  EVENT::StringVec& parameter,
				  const EVENT::StringVec& defaultVal,
				  int setSize=0 ) ;

    /**
     *  @brief  Same as registerProcessorParameter except that the parameter is optional.
     *  The value of the parameter will still be set to the default value, which
     *  is used to print an example steering line.
     *  Use parameterSet() to check whether it actually has been set in the steering
     *  file.
     */
    template<class T>
    void registerOptionalParameter(const std::string& parameterName,
				   const std::string& parameterDescription,
				   T& parameter,
				   const T& defaultVal,
				   int setSize=0 ) ;

    /**
     *  @brief  Tests whether the parameter has been set in the steering file
     */
    bool parameterSet( const std::string& name ) ;

    /**
     *  @brief  Tests whether the parameter has been registered before
     *
     *  @param name name of the parameter to check
     *  @throw logic_error if parameter has been registered before
     */
    void checkForExistingParameter( const std::string& parameterName ) ;

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

    /** Initialize the parameters */
    virtual void setParameters( std::shared_ptr<StringParameters> parameters) ;

    /** Sets the registered steering parameters before calling init() */
    virtual void baseInit( const Application *application ) ;

    /** Called by ProcessorMgr */
    void setFirstEvent( bool firstEvent ) ;

    // called internally

    /** Set the expected LCIO type for a parameter that refers to one or more
     *  input collections, e.g.:<br>
     *  &nbsp;&nbsp;  setReturnValue( "InputCollecitonName" , LCIO::MCPARTICLE ) ; <br>
     *  Set to LCIO::LCObject if more than one type is allowed, e.g. for a viewer processor.
     */
    void setLCIOInType(const std::string& colName,  const std::string& lcioInType) ;

    /** Set the  LCIO type for a parameter that refers to an output collections, i.e. the type has
     *  been defined with setLCIOOutType().
     */
    void setLCIOOutType(const std::string& collectionName,  const std::string& lcioOutType) ;

    /** Helper function for fixing old steering files */
    const ProcParamMap& procMap() ;

    /** Set the scheduler instance in which this processor is scheduled at runtime */
    void setScheduler( Scheduler *scheduler ) ;

  protected:
    /// The processor description
    std::string                        _description {""} ;
    /// The processor type
    std::string                        _typeName {""} ;
    /// The processor name
    std::string                        _processorName {""} ;
    /// The processor parameters
    std::shared_ptr<StringParameters>  _parameters {nullptr} ;
    /// A map fixing the old steering file implementation (deprecated)
    ProcParamMap                       _map {} ;
    /// Whether the current event is the first being processed
    bool                               _isFirstEvent {false} ;
    /// The input collection information
    LCIOTypeMap                        _inTypeMap {} ;
    /// The output collection information
    LCIOTypeMap                        _outTypeMap {} ;
    /// The processor logger level
    std::string                        _logLevelName {} ;

  private:
    /// The scheduler managing the processor
    Scheduler                         *_scheduler {nullptr} ;
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
  inline void Processor::printParameters() {
    if( _logger->wouldWrite<T>() ) {
      _logger->log<T>() << std::endl << "---- " << name()  <<" -  parameters: " << std::endl ;
      for( auto i = _map.begin() ; i != _map.end() ; i ++ ) {
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

  inline std::shared_ptr<StringParameters> Processor::parameters() {
    return _parameters ;
  }

  //--------------------------------------------------------------------------

  inline const std::string& Processor::description() {
    return _description ;
  }

  //--------------------------------------------------------------------------

  inline bool Processor::isFirstEvent() {
    return _isFirstEvent ;
  }

  //--------------------------------------------------------------------------

  template<class T>
  inline void Processor::registerProcessorParameter(const std::string& parameterName,
         const std::string& parameterDescription,
         T& parameter,
         const T& defaultVal,
         int setSize ) {
    checkForExistingParameter( parameterName );
    _map[ parameterName ] = new ProcessorParameter_t<T>( parameterName , parameterDescription,
            parameter, defaultVal,
            false , setSize) ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::registerInputCollection(const std::string& collectionType,
       const std::string& parameterName,
       const std::string& parameterDescription,
       std::string& parameter,
       const std::string& defaultVal,
       int setSize ) {
    setLCIOInType( parameterName , collectionType ) ;
    registerProcessorParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::registerOutputCollection(const std::string& collectionType,
        const std::string& parameterName,
        const std::string& parameterDescription,
        std::string& parameter,
        const std::string& defaultVal,
        int setSize ) {
    setLCIOOutType( parameterName , collectionType ) ;
    registerProcessorParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::registerInputCollections(const std::string& collectionType,
        const std::string& parameterName,
        const std::string& parameterDescription,
        EVENT::StringVec& parameter,
        const EVENT::StringVec& defaultVal,
        int setSize ) {
    setLCIOInType( parameterName , collectionType ) ;
    registerProcessorParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;
  }

  //--------------------------------------------------------------------------

  template<class T>
  inline void Processor::registerOptionalParameter(const std::string& parameterName,
         const std::string& parameterDescription,
         T& parameter,
         const T& defaultVal,
         int setSize ) {
    checkForExistingParameter( parameterName );
    _map[ parameterName ] = new ProcessorParameter_t<T>( parameterName , parameterDescription,
            parameter, defaultVal,
            true , setSize) ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::checkForExistingParameter( const std::string& parameterName ) {
     auto paraIt = _map.find( parameterName );
     if (paraIt != _map.end() ) {
       std::stringstream errorMessage;
       errorMessage << "Parameter " << parameterName
                    << " already defined for processor "
                    << this->type()
                    << std::endl;
       throw std::logic_error(  errorMessage.str() );
     }
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

  //--------------------------------------------------------------------------

  inline void Processor::setFirstEvent( bool firstEvent ) {
    _isFirstEvent =  firstEvent ;
  }

  //--------------------------------------------------------------------------

  inline const ProcParamMap& Processor::procMap() {
    return _map ;
  }

  //--------------------------------------------------------------------------

  inline void Processor::setScheduler( Scheduler *scheduler ) {
    _scheduler = scheduler ;
  }

} // end namespace marlin

#endif
