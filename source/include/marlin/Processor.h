#ifndef Processor_h
#define Processor_h 1

#include "lcio.h"

#include "IO/LCRunListener.h"
#include "IO/LCEventListener.h"
#include "IO/LCReader.h"

#include "EVENT/LCEvent.h"
#include "EVENT/LCRunHeader.h"

#include "StringParameters.h"
#include "ProcessorParameter.h"
//#include "LogStream.h"
#include "marlin/VerbosityLevels.h"

//----- Marlin version macros are now defined in MarlinConfig.h generated by cmake -----
#include "marlin/MarlinConfig.h"

#include "streamlog/streamlog.h"

#include <map>
#include <memory>

// ----- define some useful macros-----------
// for backward compatibility - use streamlog_out( MESSAGE )  instead 
// - this conflicts with a boost header und should not be used by now anywhere ...
//#define m_out( VERBOSITY ) streamlog_out( VERBOSITY )  
//#define m_endl std::endl 
//----------------------------------------

class MarlinProcessorWrapper;

using namespace lcio ;


namespace marlin{
/** Helper class that gives an external processor manager access to private
 * methods where necessary. Essentially only provides a constructor from a
 * MarlinProcessorWrapper, to at least avoid the most obvious misuses of this.
 */
struct ExternalProcessorMgrAccessor {
  explicit ExternalProcessorMgrAccessor(MarlinProcessorWrapper*){};
  ExternalProcessorMgrAccessor() = delete;
  ExternalProcessorMgrAccessor(const ExternalProcessorMgrAccessor&) = delete;
  ExternalProcessorMgrAccessor& operator=(const ExternalProcessorMgrAccessor&) = delete;
  ExternalProcessorMgrAccessor(ExternalProcessorMgrAccessor&&) = delete;
  ExternalProcessorMgrAccessor& operator=(ExternalProcessorMgrAccessor&&) = delete;
};

  class ProcessorMgr ;
  //  class ProcessorParameter ;
  class XMLFixCollTypes ;

  typedef std::map<std::string, ProcessorParameter* > ProcParamMap ;
  typedef std::map<std::string, std::string >         LCIOTypeMap ;

  /** Base class for Marlin processors.
   * Users can optionaly overwrite the following methods: <br>
   *   init, processRun, processEvent and end.<br>
   * Use registerProcessorParameter to define all parameters that the module uses.
   * Registered parameters are filled automatically before init() is called.
   * With MyAplication -l you can print a list of available processors including
   * the steering parameters they use/need.<br>
   * With MyAplication -x you can print an example XML steering file for all known
   * processors.
   *
   * @see init 
   * @see processRun
   * @see processEvent
   * @see end
   *
   *  @author F. Gaede, DESY
   *  @version $Id: Processor.h,v 1.38 2008-06-26 10:25:36 gaede Exp $ 
   */
  
  class Processor {
    
    friend class ProcessorMgr ;
    friend class CMProcessor ;
    friend class XMLFixCollTypes ;

  private:
    //prevent users from making (default) copies of processors
    Processor(const Processor& /*typeName*/) = delete;
    Processor& operator=(const Processor& /*typeName*/) = delete;

  public:

// 	/** Possible verbosity levels */
//     enum{ VERBOSE = 0, DEBUG = 0, MESSAGE = 1, WARNING = 2, ERROR = 3, SILENT = 4 };
 
// 	/** Global variable used to set the verbosity level */
//     static int Verbosity;
  
    /** Default constructor - subclasses need to call this in their
     * default constructor.
     */
    Processor(const std::string& typeName) ; 

    /** Destructor */
    virtual ~Processor() ; 
  
  
    /**Return a new instance of the processor.
     * Has to be implemented by subclasses.
     */
    virtual Processor*  newProcessor() = 0 ;
  

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() { }

    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    virtual void processRunHeader( LCRunHeader* ) { } 

    /** Called for every event - the working horse. 
     */
    virtual void processEvent( LCEvent * ) { }

    /** Called for every event - right after processEvent()
     *  has been called for  this processor.
     *  Use to check processing and/or produce check plots.
     */
    virtual void check( LCEvent* ) { }

    /**
     * Public overload of setFirstEvent for use in external processor managers
     */
    virtual void setFirstEvent(bool firstEvent, ExternalProcessorMgrAccessor) {
      setFirstEvent(firstEvent);
    }

    /** Called after data processing for clean up in the inverse order of the init()
     *  method so that resources allocated in the first processor also will be available
     *  for all following processors.
     */
    virtual void end(){ }
  

    /** Return type name for the processor (as set in constructor).
     */
    virtual const std::string & type() const { return _typeName ; } 

    /** Return  name of this  processor.
     */
    virtual const std::string & name() const { return _processorName ; } 

    /** Return  name of the local verbosity level of this  processor - "" if not set.
     */
    virtual const std::string & logLevelName() const { return _logLevelName ; } 


    /** Return parameters defined for this Processor.
     */
    virtual std::shared_ptr<StringParameters> parameters() { return _parameters ; }


    /** Print information about this processor in ASCII steering file format.
     */
    virtual void printDescription() ;

    /** Print information about this processor in XML steering file format.
     */
    virtual void printDescriptionXML(std::ostream& stream=std::cout) ;


    /** Print the parameters and their values depending on the given verbosity level.
     */
    template <class T>
    void printParameters() {
    
      
      if( streamlog::out.template write<T>() ) {

	
	typedef ProcParamMap::iterator PMI ;

	streamlog::out()  << std::endl  
			<< "---- " << name()  <<" -  parameters: " << std::endl ;
	
	
	for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
	  
	  if( ! i->second->isOptional() || i->second->valueSet() ){
	    streamlog::out.template write<T>() ;
	    streamlog::out() << "\t"   << i->second->name()   
			     << ":  "  << i->second->value() 
			     << std::endl ;
	  }
	}
	
	streamlog::out.template write<T>() ;
	streamlog::out() << "-------------------------------------------------" 
			 << std::endl ;
	
      }
    }

    /** Print the parameters and their values with verbosity level MESSAGE.
     */
    void printParameters() ; 
  


    /** Description of processor.
     */
    const std::string& description() { return _description ; }


    /** True if first event in processEvent(evt) - use this e.g. to initialize histograms etc.
     */
    bool isFirstEvent() { return _isFirstEvent ; } ;
    
    /** Return the LCIO input type for the collection colName - empty string if colName is
     *  not  a registered collection name */
    std::string getLCIOInType( const std::string& colName ) ;

    /** Return the LCIO output type for the collection colName - empty string if colName is
     *  not  a registered collection name */
    std::string getLCIOOutType( const std::string& colName ) ;

    /** True if the given parameter defines an LCIO input collection, i.e. the type has 
     *  been defined with setLCIOInType().
     */

    bool isInputCollectionName( const std::string& parameterName  ) ;  


    /** True if the given parameter defines an LCIO output collection */
    bool isOutputCollectionName( const std::string& parameterName  ) ;  


//     /** Helper function returns the ProcessorParameter for the given name
//      */
//     ProcessorParameter* getProcessorParameter( const std::string name) ;
    
    /** Initialize the parameters */
    virtual void setParameters( std::shared_ptr<StringParameters> parameters) ;

    /** Set processor name */
    virtual void setName( const std::string & processorName) { _processorName = processorName ; }

  protected:

    /** Set the return value for this processor - typically at end of processEvent(). 
     *  The value can be used in a condition in the steering file referred to by the name
     *  of the processor. 
     */
    void setReturnValue( bool val) ;

    /** Set a named return value for this processor - typically at end of processEvent() 
     *  The value can be used in a condition in the steering file referred to by 
     *  ProcessorName.name of the processor. 
     */
    void setReturnValue( const std::string& name, bool val ) ;


    /** Register a steering variable for this processor - call in constructor of processor.
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
				    int setSize=0 ) {
       checkForExistingParameter( parameterName );
       _map[ parameterName ] = new ProcessorParameter_t<T>( parameterName , parameterDescription, 
						   parameter, defaultVal, 
						   false , setSize) ;
     }
    
    /** Specialization of registerProcessorParameter() for a parameter that defines an 
     *  input collection - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollection(const std::string& collectionType,
				 const std::string& parameterName, 
				 const std::string& parameterDescription,
				 std::string& parameter,
				 const std::string& defaultVal,
				 int setSize=0 ) {
      
      setLCIOInType( parameterName , collectionType ) ;
      registerProcessorParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ; 
    }
    
    /** Specialization of registerProcessorParameter() for a parameter that defines an 
     *  output collection - can be used fo checking the consistency of the steering file.
     */
    void registerOutputCollection(const std::string& collectionType,
				  const std::string& parameterName, 
				  const std::string& parameterDescription,
				  std::string& parameter,
				  const std::string& defaultVal,
				  int setSize=0 ) {
      
      setLCIOOutType( parameterName , collectionType ) ;
      registerProcessorParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ; 
    }

    /** Specialization of registerProcessorParameter() for a parameter that defines one or several 
     *  input collections - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollections(const std::string& collectionType,
				  const std::string& parameterName, 
				  const std::string& parameterDescription,
				  StringVec& parameter,
				  const StringVec& defaultVal,
				  int setSize=0 ) {
      
      setLCIOInType( parameterName , collectionType ) ;
      registerProcessorParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ; 
    }
    
    /** Same as  registerProcessorParameter except that the parameter is optional.
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
				   int setSize=0 ) {

      checkForExistingParameter( parameterName );
      _map[ parameterName ] = new ProcessorParameter_t<T>( parameterName , parameterDescription, 
						  parameter, defaultVal, 
						  true , setSize) ;
    }
    
    /** Tests whether the parameter has been set in the steering file
     */
    bool parameterSet( const std::string& name ) ;
    
    /** Tests whether the parameter has been registered before
     *
     * @param name name of the parameter to check
     * @throw logic_error if parameter has been registered before
     */
    void checkForExistingParameter( const std::string& parameterName ) {
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

    /** Print message according to  verbosity level of the templated parameter (one of
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
    void message(  const std::string& m ) const {
      

      if( streamlog::out.template write<T>() ) 
	streamlog::out() << m << std::endl ;
      
    }
    

    /** Same as  message(const std::string& message) except that it allows the output of 
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
    inline void message( const std::basic_ostream<char, std::char_traits<char> >& m) const {

     if( T::active ){  // allow the compiler to optimize this away ...

       try{
	 const std::stringstream& mess = dynamic_cast<const std::stringstream&>( m ) ; 

	 this->template message<T>( mess.str() ) ;

       }
       catch( std::bad_cast ) {}
     }
    }

    /** Returns an empty stringstream that is used by the message method.
     * @deprecated
     */
    std::stringstream& log() const ;


  private: // called by ProcessorMgr
    
    /** Allow friend class CCProcessor to change/reset processor parameters */
    virtual void setProcessorParameters( std::shared_ptr<StringParameters> processorParameters) {
	setParameters( processorParameters ) ;
    }
    
    /** Allow friend class CCProcessor to update processor parameters */
    virtual void updateParameters();
    
    /** Sets the registered steering parameters before calling init() 
     */
    virtual void baseInit() ;
    
    /** Called by ProcessorMgr */
    void setFirstEvent( bool firstEvent ) { _isFirstEvent =  firstEvent ; }

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
    const ProcParamMap& procMap() { return _map ; }  


    
  protected:

    /**Describes what the processor does. Set in constructor.
     */
    std::string _description="";
    std::string _typeName="";
    std::string _processorName="";
    std::shared_ptr<StringParameters> _parameters{};

    ProcParamMap _map{};
    bool _isFirstEvent = false;
    LCIOTypeMap   _inTypeMap{};
    LCIOTypeMap   _outTypeMap{};

    std::string _logLevelName{};
    
  private:
    mutable std::stringstream* _str=NULL;

    Processor() ; 
  
  };
 
} // end namespace marlin 

#endif
