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
#include <map>

using namespace lcio ;


namespace marlin{

  class ProcessorMgr ;
  //  class ProcessorParameter ;

  typedef std::map<std::string, ProcessorParameter* > ProcParamMap ;

  /** Base class for Marlin processors.
   * Users can optionaly overwrite the following methods: <br>
   *   init, processRun, processEvent and end.<br>
   * Use registerProcessorParameter to define all parameters that the module uses.
   * Registered parameters are filled automatically before init() is called.
   * With MyAPplication -l you can print a list of available processors including
   * the steering parameters they use/need.
   *
   * @see init 
   * @see processRun
   * @see processEvent
   * @see end
   */
  
  class Processor {
  
    friend class ProcessorMgr ;

  public:
  

    Processor(const std::string& typeName) ; 

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
    virtual void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse. 
     */
    virtual void processEvent( LCEvent * evt ) { }

    /** Called for every event - right after processEvent()
     *  has been called for all processors.
     *  Use to check processing and/or produce check plots.
     */
    virtual void check( LCEvent * evt ) { }


    /** Called after data processing for clean up.
     */
    virtual void end(){ }
  

    /** Return type name for the processor (as set in constructor).
     */
    virtual const std::string & type() const { return _typeName ; } 

    /** Return  name of this  processor.
     */
    virtual const std::string & name() const { return _processorName ; } 

    /** Return parameters defined for this Processor.
     */
    virtual StringParameters* parameters() { return _parameters ; } 


    /** Print information about this processor.
     */
    virtual void printDescription() ;

    /** Print the parameters and its values.
     */
    virtual void printParameters() ;


    
    /** Description of processor.
     */
    const std::string& description() { return _description ; }


  protected:

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
     void registerProcessorParameter(const std::string& name, 
 				    const std::string&description,
 				    T& parameter,
 				    const T& defaultVal,
				    int setSize=0 ) {
    
       _map[ name ] = new ProcessorParameter_t<T>( name , description, 
						   parameter, defaultVal, 
						   false , setSize) ;
     }
    
    /** Same as  registerProcessorParameter except that the parameter is optional.
     *  The value of the parameter will still be set to the default value, which
     *  is used to print an example steering line.
     *  Use parameterSet() to check whether it actually has been set in the steering 
     *  file.
     */
     template<class T>
     void registerOptionalParameter(const std::string& name, 
 				    const std::string&description,
 				    T& parameter,
 				    const T& defaultVal,
				    int setSize=0 ) {
    
       _map[ name ] = new ProcessorParameter_t<T>( name , description, 
						   parameter, defaultVal, 
						   true , setSize) ;
     }
    

    /** Tests whether the parameter has been set in the steering file
     */
    bool parameterSet( const std::string& name ) ;

// the implicit type conversion causes problems, e.g. with std::string("bla") 
//     template<class T, class U>
//     void registerProcessorParameter(const std::string& name, 
// 				    const std::string&description,
// 				    T& parameter,
// 				    const U& defaultVal) {
      
//       _map[ name ] = new ProcessorParameter_t<T>( name , description, parameter , T(defaultVal) ) ;
//     }
    

    virtual void setName( const std::string & name) { _processorName = name ; }
  
    virtual void setParameters( StringParameters* parameters) ; 
  
    /** Sets the registered steering parameters before calling init() 
     */
    virtual void baseInit() ;




    /**Describes what the processor does. Set in constructor.
     */
    std::string _description ;
    std::string _typeName  ;
    std::string _processorName ;
    StringParameters* _parameters ;

    ProcParamMap _map ;  

  private:
    Processor() ; 
  
  };
 
} // end namespace marlin 

#endif
