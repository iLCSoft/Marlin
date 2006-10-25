#ifndef CCPROCESSOR_H
#define CCPROCESSOR_H

/*
 * This class handles information about
 * marlin processors and their collections
 * needed by MarlinSteerCheck
 *
 * @author Benjamin Eberhardt, Jan Engels
 *
 */

#include "marlin/Processor.h"

#include <set>

#define MAX_ERRORS 3
#define ACTIVE true
#define INACTIVE false
#define NO_PARAMETERS 0
#define NOT_INSTALLED 1
#define COL_ERRORS 2
#define INPUT "_marlin.lcioInType"
#define OUTPUT "_marlin.lcioOutType"
#define UNAVAILABLE "_marlin.lcioUnavailableType"

namespace marlin {

  class CCCollection;

  typedef std::set< std::string > sSet;
  typedef std::vector< CCCollection* > ColVec;
  typedef std::map< std::string, std::string > ssMap;
  typedef std::map< std::string, ssMap > sssMap;
  typedef std::map< std::string, ColVec > sColVecMap;
  typedef std::map< std::string, sColVecMap > ssColVecMap;

  class CCProcessor{

  public:

    CCProcessor( bool status, const std::string& name, const std::string& type, StringParameters* p=NULL);
   
    bool hasErrors();
    bool hasParameters();
    bool hasUnavailableCols();
    bool isInstalled();
    bool isActive();

    bool getStatus(){ return _status; }
    const std::string& getName(){ return _name; }
    const std::string& getType(){ return _type; }
    const StringVec& getErrors(){ return _errors; }
    const std::string getError(){ return ( _errors.size() != 0 ? _errors[0] : "" ); }
    const std::string getStatusDesc(){ return ( isActive() ? "Active" : "Inactive" ); }
    
    ColVec& getCols( const std::string& iotype, const std::string& name="ALL_COLLECTIONS" );
    StringParameters* getParameters(){ return _param; }
    Processor* getMarlinProcessor(){ return _proc; }
    void updateMarlinProcessor();
    void clearParameters();
    
    sSet& getUColTypes();
    const ssMap& getColHeaders( const std::string& iotype ){ return _types[iotype]; }

    void addCol( const std::string& iotype, const std::string& name, const std::string& type, const std::string& value );
    void remCol( const std::string& iotype, const std::string& name, unsigned int index );
    void addUCol( CCCollection* c );
   
    void changeStatus();
    void setName( const std::string& name );
    void setParameters( StringParameters* p );
    
    void setError( int error );
    void clearError( int error );

  private:

    //utility methods
    void createMarlinProc();	    //sets error flag NOT_INSTALLED if processor couldn't be created
    CCCollection* popCol( ColVec& v, CCCollection* c );   
    void tokenize( const std::string str, StringVec& tokens, const std::string& delimiters = " " );

    //VARIABLES
    bool _status;		    // false = INACTIVE ; true = ACTIVE
    bool _error[MAX_ERRORS];	    // 0 = proc has no parameters; 1 = proc is not build in this marlin installation; 2 = unavailable collections
    std::string _name;		    // name of the processor
    std::string _type;		    // type of the processor
    StringParameters* _param;	    // parameters from processor
    Processor* _proc;		    // associated Marlin processor
 
    StringVec _error_desc;	    // error descriptions for all processors
    StringVec _errors;		    // list of errors found in a processor
  
    ssColVecMap _cols;		    // first key for Types INPUT : OUTPUT : UNAVAILABLE
				    // for INPUT/OUPUT the second key is for Collection Names
				    // for UNAVAILABLE the second key is for Collection Types

    sssMap _types;		    // first key for Types INPUT : OUTPUT
				    // second key is for Collection Names and the third string for Collection Types
  };

} // end namespace marlin
#endif
