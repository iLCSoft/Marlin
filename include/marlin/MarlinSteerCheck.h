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
 * @version $Id: MarlinSteerCheck.h,v 1.4 2006-10-23 08:28:20 engels Exp $
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

    MarlinSteerCheck( const char* steerFileName );

    ProcVec& getAProcs() { return _aProc; };
    ProcVec& getIProcs() { return _iProc; };
    ColVec& getLCIOCols() const;
    StringVec& getLCIOFiles() const;

    sSet& getColsSet( const std::string& type, CCProcessor* proc );
    sSet& getProcTypes(){ return _procTypes; }

    StringParameters* getParameters( const std::string& key );
    
    const std::string& getErrors( unsigned int index );
   
    //operations to perform
    void addLCIOFile( const std::string& file );
    void remLCIOFile( const std::string& file );
    void addGEARFile( const std::string& file );
    void remGEARFile( const std::string& file );
    void addProcessor( bool status, const std::string& name, const std::string& type, StringParameters* p=NULL );
    void addProcessor( const std::string& name, CCProcessor* p );
    void remProcessor( unsigned int index, bool status );
    void activateProcessor( unsigned int index );
    void deactivateProcessor( unsigned int index );
    void changeProcessorPos( unsigned int pos, unsigned int newPos );
    CCProcessor* findProc(const std::string& type);
    
    void consistencyCheck();
    void saveAsXMLFile( const std::string& file );
    
    //dump methods
    void dump_information();
    bool dump_colErrors();

  private:

    //collection retrieval methods
    ColVec& getAllCols() const;
    ColVec& getProcCols( const ProcVec& v ) const;
    
    //utility methods
    void parseXMLFile( const std::string& file );
    CCProcessor* popProc(ProcVec& v, CCProcessor* p);
    ColVec& findMatchingCols( ColVec& v, CCProcessor* srcProc, const std::string& type, const std::string& value="UNDEFINED" );
    
    //VARIABLES
    IParser* _parser;			//parser
    std::string _steeringFile;		//steering file
    ProcVec _aProc;			//active processors
    ProcVec _iProc;			//inactive processors
    sColVecMap _lcioCols;		//LCIO collections
    
    sSet _procTypes;			//all available processor types (use in ComboBox)
    sSet _colValues;			//all available collection values for a given type (use in ComboBox)
  };

} // namespace
#endif
