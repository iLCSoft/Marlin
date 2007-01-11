#ifndef CCPROCESSOR_H
#define CCPROCESSOR_H

#include "marlin/Processor.h"

#include <set>
#include <fstream>

#define MAX_ERRORS 3
#define ACTIVE true
#define INACTIVE false
#define NO_PARAMETERS 0
#define NOT_INSTALLED 1
#define COL_ERRORS 2
#define INPUT "lcioInType"
#define OUTPUT "lcioOutType"
#define UNAVAILABLE "lcioUnavailableType"
#define DUPLICATE "lcioDuplicate"

namespace marlin {

  class CCCollection;

  typedef std::set< std::string > sSet;
  typedef std::vector< CCCollection* > ColVec;
  typedef std::map< std::string, std::string > ssMap;
  typedef std::map< std::string, ssMap > sssMap;
  typedef std::map< std::string, ColVec > sColVecMap;
  typedef std::map< std::string, sColVecMap > ssColVecMap;

/**
 * handles information about marlin processors and 
 * their collections needed by MarlinSteerCheck
 *
 * @author Jan Engels, Benjamin Eberhardt, DESY
 * @version $Id: CCProcessor.h,v 1.23 2007-01-11 16:56:37 engels Exp $
 */

  class CCProcessor{

  public:

    // Constructor
    CCProcessor( bool status, const std::string& name, const std::string& type, StringParameters* p=NULL);

    // Copy Constructor
    CCProcessor( CCProcessor &p );
    
    // Destructor
    ~CCProcessor();
    
    /** Returns true if the processor has errors */
    bool hasErrors();
    
    /** Returns true if the processor has parameters */
    bool hasParameters(){ return !_error[ NO_PARAMETERS ]; }
    
    /** Returns true if the processor has collection errors */
    bool hasErrorCols(){ return _error[ COL_ERRORS ]; }

    /** Returns true if the processor is installed */
    bool isInstalled(){ return !_error[ NOT_INSTALLED ]; }

    /** Returns true if the processor is active */
    bool isActive(){ return _status; }

    /** Returns true if the processor is constrained by the given condition */
    bool hasCondition( const std::string& condition );

    /** Returns the Name of the processor */
    const std::string& getName(){ return _name; }

    /** Returns the Type of the processor */
    const std::string& getType(){ return _type; }

    /** Returns the Conditions of the processor */
    sSet& getConditions(){ return _conditions; }

    /** Returns the Description of the processor */
    const std::string getDescription(){	return (isInstalled() ? _proc->description() : 
	    "This processor is NOT installed in your Marlin binary: parameter descriptions and types lost!!");
    }
    
    /* Returns a string vector with the errors of the processor - Obsolete: use getError() instead */
    //const StringVec& getErrors(){ return _errors; }
    
    /** Returns a string with the error of the processor */
    const std::string getError(){ return ( _errors.size() != 0 ? _errors[0] : "" ); }
    
    /** Returns a string with the processor status ( "Active", "Inactive" ) */
    const std::string getStatusDesc(){ return ( isActive() ? "Active" : "Inactive" ); }
    
    /** Returns true if the given collection is in the unavailable or duplicate list of this processor */
    bool isErrorCol( const std::string& type, const std::string& value );
    
    /** Returns true if a parameter is optional (optional means the parameter will be written out as a comment) */
    bool isParamOptional( const std::string& key );
    
    /** Returns the string parameters for this processor */
    StringParameters* getParameters(){ return _param; }
    
    /** Returns a map with collection names and their respective types for INPUT/OUTPUT collections of this processor */
    const ssMap& getColHeaders( const std::string& iotype ){ return _types[iotype]; }

    /** Returns collections of a given iotype ( INPUT, OUTPUT, UNAVAILABLE, DUPLICATE ) for a given name or type
     *  If iotype == INPUT/OUTPUT then type_name is the name of the collection
     *  If iotype == UNAVAILABLE/DUPLICATE then type_name is the type of the collection */
    ColVec& getCols( const std::string& iotype, const std::string& type_name="ALL_COLLECTIONS" );

    /** Returns collection's types/names of a given iotype found in the processor
     *  If iotype == INPUT/OUTPUT then names are returned
     *  If iotype == UNAVAILABLE/DUPLICATE then types are returned */
    sSet& getColTypeNames( const std::string& iotype );
    
    /** Adds a collection of the given iotype ( INPUT / OUTPUT ) with the given name, type and value */
    void addCol( const std::string& iotype, const std::string& name, const std::string& type, const std::string& value );

    /** Removes collection of the given iotype ( INPUT / OUTPUT ) with the given name at the given index */
    void remCol( const std::string& iotype, const std::string& name, unsigned int index );

    /** Adds an unavailable collection to this processor */
    void addUCol( CCCollection* c );
   
    /** Adds a duplicate collection to this processor */
    void addDCol( CCCollection* c );
   
    /** Changes the processor status ( ACTIVE->INACTIVE or INACTIVE->ACTIVE ) */
    void changeStatus();

    /** Sets the processor's name */
    void setName( const std::string& name ){ _name = name; };
    
    /** Sets the processor's conditions */
    void setConditions( const std::string& conditions );
    
    /** Activates an error flag in this processor ( NO_PARAMETERS=0, NOT_INSTALLED=1, COL_ERRORS=2 ) */
    void setError( int error );

    /** Clears an error flag in this processor ( NO_PARAMETERS=0, NOT_INSTALLED=1, COL_ERRORS=2 ) */
    void clearError( int error );

    /** Sets a parameter as optional (if optional=true parameter is written out as a comment) */
    void setOptionalParam( const std::string& key, bool optional=true );
    
    /** Writes this processor to a stream using the XML format */
    void writeToXML( std::ofstream& stream );

  private:

    ///////////////////////////////////////////////////////////////////////////////
    // METHODS
    ///////////////////////////////////////////////////////////////////////////////
    void addColsFromParam( StringParameters* p );
    void writeColsToParam();
    void clearParameters();
    void setMarlinProc();	    //sets error flag NOT_INSTALLED if processor couldn't be set
    CCCollection* popCol( ColVec& v, CCCollection* c );   

    ///////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ///////////////////////////////////////////////////////////////////////////////
    bool _status;		    // false = INACTIVE ; true = ACTIVE
    bool _error[MAX_ERRORS];	    // 0 = proc has no parameters; 1 = proc is not build in this marlin installation; 2 = unavailable collections
    std::string _name;		    // name of the processor
    std::string _type;		    // type of the processor
    StringParameters* _param;	    // parameters from processor
    Processor* _proc;		    // associated Marlin processor
 
    StringVec _error_desc;	    // error descriptions for all processors
    StringVec _errors;		    // list of errors found in a processor

    sSet _conditions;		    // processor's conditions
  
    ssColVecMap _cols;		    // first key for Types INPUT : OUTPUT : UNAVAILABLE : DUPLICATE
				    // for INPUT/OUPUT the second key is for Collection Names
				    // for UNAVAILABLE/DUPLICATE the second key is for Collection Types

    sssMap _types;		    // first key for Types INPUT : OUTPUT
				    // second key is for Collection Names and the third string for Collection Types

    sSet _optParams;		    // list of optional parameters that shall be written out as normal parameters
  };

} // end namespace marlin
#endif
