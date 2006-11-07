#ifndef MARLINSTEERCHECK_H
#define MARLINSTEERCHECK_H

/**@class
 * this class is a Marlin Steering File consistency check Tool.
 * It can parse XML steering files using the marlin parser and search
 * the lcio file(s) specified at the global section for available collections.
 * Together with the active and inactive processors specified in the steering file
 * and their respective collections it can perform a full consistency check and
 * check for inavailable collections.  The Program checks if an adequate
 * available collection exists for every input collection of a processor.
 * 
 * (where input collection means a collection the processor requires as input, 
 * and output collections mean a collection that is created in the processor and
 * marked as available for the upcoming processors)
 *
 * Information about the consistency check can be dumped.
 * 
 *
 * @author Benjamin Eberhardt, Jan Engels
 * @version $Id: MarlinSteerCheck.h,v 1.9 2006-11-07 11:07:22 engels Exp $
 */

#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"

#include "marlin/XMLParser.h"

// to make colored output
#define clrscr() printf("\033[2J")
#define dblue() printf("\x1b[34m")
#define dred() printf("\x1b[31m")
#define dyellow() printf("\x1b[33m")
#define dgreen() printf("\x1b[32m")
#define dunderline() printf("\x1b[4m")
#define ditalic() printf("\x1b[3m")
#define ddunkel() printf("\x1b[2m")
#define dhell() printf("\x1b[1m")
#define dblink() printf("\x1b[5m")
#define endcolor() printf("\x1b[m")


namespace marlin {

  class CCCollection;
  class CCProcessor;

  typedef std::vector< CCProcessor* > ProcVec;

  class MarlinSteerCheck{

  public:

    // Constructor
    MarlinSteerCheck( const char* steerFileName=NULL );

    // Destructor
    ~MarlinSteerCheck();

    /** Returns the Active Processors */
    ProcVec& getAProcs() { return _aProc; };
    
    /** Returns the Inactive Processors */
    ProcVec& getIProcs() { return _iProc; };
    
    /** Returns the collections read from LCIO files */
    ColVec& getLCIOCols() const;
    
    /** Returns the names of the LCIO files found in the global section*/
    StringVec& getLCIOFiles(){ return _lcioFiles; }

    /** Returns a list of all available Collections for a given type and processor (to use in a ComboBox) */
    sSet& getColsSet( const std::string& type, CCProcessor* proc );

    /** Returns a list of all available Processor Types (to use in a ComboBox) */
    ssMap& getAvailableProcTypes(){ return _procTypes; }

    /** Returns the Global Parameters */
    StringParameters* getGlobalParameters(){ return _gparam; }
    
    /** Returns the Errors for an Active Processor at the given index */
    const std::string& getErrors( unsigned int index );
  
    /** Add LCIO file and read all collections inside it */
    void addLCIOFile( const std::string& file );

    /** Remove LCIO file and all collections associated to it */
    void remLCIOFile( const std::string& file );
    
    /** Change the LCIO File at the given index to the new given position */
    void changeLCIOFilePos( unsigned int pos, unsigned int newPos );
 
    /** Add a new processor */
    void addProcessor( bool status, const std::string& name, const std::string& type, StringParameters* p=NULL );

    /** Remove processor with the given status at the given index */
    void remProcessor( unsigned int index, bool status );

    /** Activate processor at the given index */
    void activateProcessor( unsigned int index );
    
    /** Deactivate processor at the given index */
    void deactivateProcessor( unsigned int index );

    /** Change the active processor at the given index to the new given position */
    void changeProcessorPos( unsigned int pos, unsigned int newPos );
    
    /** Performs a check at all active processors to search for unavailable collections */
    void consistencyCheck();

    /** Saves the data to an XML file with the given name */
    void saveAsXMLFile( const std::string& file );
    
    /** Dumps all information read from the steering file to stdout */
    void dump_information();

    /** Dumps collection errors found in the steering file for all active processors */
    bool dump_colErrors();

    
  private:
    
    /////////////////////////////////////////////////////
    //METHODS
    /////////////////////////////////////////////////////

    // Return all available collections from the active or inactive processors
    ColVec& getProcCols( const ProcVec& v ) const;

    // Return all available collections found in LCIO files and active/inactive processors
    ColVec& getAllCols() const;
   
    // Parse the given steering file
    void parseXMLFile( const std::string& file );
    
    // Pop a processor out of a vector from processors
    CCProcessor* popProc(ProcVec& v, CCProcessor* p);

    // Find matching collections of a given type and value in a vector of collections for a given processor
    ColVec& findMatchingCols( ColVec& v, CCProcessor* srcProc, const std::string& type, const std::string& value="UNDEFINED" );
    
    /////////////////////////////////////////////////////
    //VARIABLES
    /////////////////////////////////////////////////////

    IParser* _parser;			//parser
    StringParameters* _gparam;		//global parameters (without LCIO Files)
    std::string _steeringFile;		//steering file
    ProcVec _aProc;			//active processors
    ProcVec _iProc;			//inactive processors
    sColVecMap _lcioCols;		//LCIO collections
    StringVec _lcioFiles;		//LCIO filenames
    
    ssMap _procTypes;			//all available processor types and their corresponding descriptions (use in ComboBox)
    sSet _colValues;			//all available collection values for a given type (use in ComboBox)
  };

} // namespace
#endif
