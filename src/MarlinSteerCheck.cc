#include "marlin/MarlinSteerCheck.h"

// LCIO INCLUDES ///////////////
#include "lcio.h"
#include "IO/LCReader.h"
#include "EVENT/LCCollection.h"
///////////////////////////////

// MARLIN INCLUDES /////////////
#include "marlin/XMLParser.h"
#include "marlin/tinyxml.h"
////////////////////////////////

#include <iomanip>
#include <fstream>
#include <iostream>
//#include <time.h>

using namespace std;

MarlinSteerCheck::MarlinSteerCheck( const char* steeringFile ) : _steeringFile( steeringFile ){
    if( steeringFile != 0 ){
	//parse the file
	parseXMLFile( steeringFile );
    }
}

/***************************************************
 * Add LCIO file and read all collections inside it
 ***************************************************/
void MarlinSteerCheck::addLCIOFile( const string& file ){
    HANDLE_LCIO_EXCEPTIONS;
    ColVec newCols;
    
    LCReader* lcReader = LCFactory::getInstance()->createLCReader();
    lcReader->open( file );
    
    LCEvent* evt = lcReader->readNextEvent();
    const StringVec* strVec = evt->getCollectionNames();
    StringVec::const_iterator name;

    for( name = strVec->begin(); name != strVec->end(); name++ ){
	LCCollection* col = evt->getCollection( *name ) ;
	
	//store the LCIO Filename in the unused name variable from the processor parameters
	CCCollection* newCol = new CCCollection();

	newCol->setType( col->getTypeName() );
	newCol->setValue( *name );
	newCol->setName( file );
	
	newCols.push_back( newCol );
    }
    
    //add the new Collections vector to the vector of LCIO file's Collections
    _lcioCols[file]=newCols;
    
    lcReader->close();
    delete lcReader;

    consistencyCheck();
}

/********************************************************
 * Remove lcio file and all collections associated to it
 ********************************************************/

void MarlinSteerCheck::remLCIOFile( const string& file ){
    _lcioCols.erase( file );
    consistencyCheck();
}


/********************************************************
 * Get the list of LCIO Files
 ********************************************************/
StringVec& MarlinSteerCheck::getLCIOFiles() const{
    static StringVec filenames;
    
    filenames.clear();
    
    for( sColVecMap::const_iterator p=_lcioCols.begin(); p!=_lcioCols.end(); p++ ){
	filenames.push_back( (*p).first );
    }

    return filenames;
}

/**********************
 * Add a new Processor
 **********************/
void MarlinSteerCheck::addProcessor( bool status, const string& name, const string& type, StringParameters* p ){

    CCProcessor* newProc = new CCProcessor(status, name, type, p);
   
    if( status == ACTIVE ){
	_aProc.push_back( newProc );
    }
    else{
	_iProc.push_back( newProc );
    }
    consistencyCheck();
}

/*********************
 * Remove a Processor
 *********************/
void MarlinSteerCheck::remProcessor( int index, bool status ){

    if( status == ACTIVE ){
	popProc( _aProc, _aProc[index] );
    }
    else{
	popProc( _iProc, _iProc[index] );
    }
    consistencyCheck();
}

/***********************
 * Activate a processor
 ***********************/
void MarlinSteerCheck::activateProcessor( unsigned int index ){

    if( index >=0 && index < _iProc.size() ){
    
	//changes the processor status
	_iProc[index]->changeStatus();
    
	//adds the processor to the active processors vector
	_aProc.push_back( popProc( _iProc, _iProc[index] ));
    
	consistencyCheck();
    }
    else{
	cerr << "activateProcessor: Index out of bounds!!" << endl;
    }
}

/***********************
 * Deactivate a processor
 ***********************/
void MarlinSteerCheck::deactivateProcessor( unsigned int index ){
    
    if( index >=0 && index < _aProc.size() ){
    
	//changes the processor status
	_aProc[index]->changeStatus();
    
	//adds the processor to the inactive processors vector
	_iProc.push_back( popProc( _aProc, _aProc[index] ));
    
	consistencyCheck();
    }
    else{
	cerr << "activateProcessor: Index out of bounds!!" << endl;
    }
}

/****************************************
 * Change an active processor's position
 ****************************************/
void MarlinSteerCheck::changeProcessorPos( unsigned int pos, unsigned int newPos ){
    //check if positions are valid
    if( pos != newPos && pos >= 0 && newPos >= 0 && pos < _aProc.size() && newPos < _aProc.size() ){
	
	CCProcessor* p = popProc(_aProc, _aProc[pos] );
	
	if(newPos == _aProc.size() ){
	    _aProc.push_back( p );
	}
	else{
	    ProcVec v;
		
	    for( unsigned int i=0; i<_aProc.size(); i++ ){
	        if( i == newPos ){
		    v.push_back( p );
		}
		v.push_back( _aProc[i] );
	    }
	    _aProc = v;
	}
	consistencyCheck();
    }
    else{
	cerr << "changeProcessorPos: Index out of bounds!!" << endl;
    }
}

/******************************************************
 * Check active processors for unavailable collections
 ******************************************************/
void MarlinSteerCheck::consistencyCheck(){
    //the availableCols vector will contain all the available collections read from LCIO files
    //and all output collections from active processors found before the processor being checked
    ColVec availableCols = getLCIOCols();
    ColVec requiredCols, matchCols;

    //loop through all active processors and check for unavailable collections
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	//first clear the processor collection errors
	_aProc[i]->clearError( COL_ERRORS );
	//initialize required collections for every processor
	requiredCols.clear();
	requiredCols = _aProc[i]->getCols( INPUT );
	//loop through all required collections
	for( unsigned int j=0; j<requiredCols.size(); j++ ){
	    matchCols.clear();
	    //check if required collections are found in available collections
	    matchCols = findMatchingCols(availableCols, requiredCols[j], true );
	    //if the collection is not available
	    if( matchCols.size() == 0 ){
		//add it to the unavailable collections list of the processor
		_aProc[i]->addUCol( requiredCols[j] );
	    }
	}
	//insert all Output Collections from this processor into availableCols
	availableCols.insert( availableCols.end(), _aProc[i]->getCols( OUTPUT ).begin(), _aProc[i]->getCols( OUTPUT ).end() );
    }
}

////////////////////////////////////////////////////////////////////////////////
// UTILITY METHODS
////////////////////////////////////////////////////////////////////////////////

//parse an xml file and initialize data
void MarlinSteerCheck::parseXMLFile( const string& file ){
    TiXmlDocument doc( file );

    //if file loads with no errors
    if( doc.LoadFile() ){
	StringVec lcioFiles, availableProcs, activeProcs;

	//============================================================
	//PARSE THE XML FILE
	//============================================================
	
	_parser = new XMLParser( file ) ;
	_parser->parse();

	//============================================================
	//READ PARAMETERS
	//============================================================
	
	//list of lcio files defined in the global section
	_parser->getParameters( "Global" )->getStringVals( "LCIOInputFiles" , lcioFiles );
	
	//list of all processors defined in the xml file NOT including the ones in the execute section
	_parser->getParameters( "Global" )->getStringVals( "AvailableProcessors" , availableProcs );
	
	//this gets a name list of all processors defined in the execute section
	_parser->getParameters( "Global" )->getStringVals( "ActiveProcessors" , activeProcs );
	
	//============================================================
	//ADD LCIO FILES
	//============================================================
	
	for( unsigned int i=0; i<lcioFiles.size(); i++ ){
	    addLCIOFile( lcioFiles[i] );
	}
	
	//============================================================
	//INITIALIZE PROCESSORS
	//============================================================
	
	for( unsigned int i=0; i<availableProcs.size(); i++ ){
	    
	    //get StringParameters from xml file for the name of the CCProcessor
	    StringParameters* p = _parser->getParameters( availableProcs[i] );
	    
	    //get type of processor from the parameters
	    string type = p->getStringVal( "ProcessorType" );

	    //TODO
	    //add type to map procTypes
	    if( _procTypes.find( type ) == _procTypes.end() ){
		_procTypes[ type ] = _procTypes.size()-1;
	    }

	    //add this new processor
	    addProcessor( INACTIVE, availableProcs[i], type, p);
	}

	//activate the processors in the execute section
	for( unsigned int i=0; i<activeProcs.size(); i++ ){
	    
	    bool found = false;
	    //search all available processors to check if there are processors
	    //defined in the execute section that have no parameters
	    for( unsigned int j=0; j<_iProc.size(); j++ ){
		if( activeProcs[i] == _iProc[j]->getName() ){
		    activateProcessor(j);
		    found = true;
		}
	    }
	    //if processor has no parameters set the type to "Undefined!!"
	    if( !found ){
		//add the processor to the active processors
		addProcessor( ACTIVE, activeProcs[i], "Undefined!!");
	    }
	}

	//============================================================
	//do a consistency check
	//============================================================
	
	consistencyCheck();
    }
    else{
	cerr << "parseXMLFile: Failed to load file: " << _steeringFile << endl;
    }
}

//find matching collections on the given vector
ColVec& MarlinSteerCheck::findMatchingCols( ColVec& v, CCCollection* c, bool matchvalue ){
    static ColVec cols;

    cols.clear();
    for( unsigned int i=0; i<v.size(); i++ ){
	if( v[i]->getSrcProc() != c->getSrcProc() && v[i]->getType() == c->getType() ){
	    if( matchvalue ){
		if( v[i]->getValue() == c->getValue() ){
		    cols.push_back( v[i] );
		}
	    }
	    else{
		cols.push_back( v[i] );
	    }
	}
    }
    return cols;
}

//pop a processor out of the given vector
CCProcessor* MarlinSteerCheck::popProc( ProcVec& v, CCProcessor* p ){
    //test if the vector is empty
    if( v.size() == 0 ){
	return p;
    }
    
    ProcVec newVec;
    for( unsigned int i=0; i<v.size(); i++ ){
	if( v[i] != p ){
	    newVec.push_back( v[i] );
	}
    }

    v.assign( newVec.begin(), newVec.end() );
    
    return p;
}

////////////////////////////////////////////////////////////////////////////////
// COLLECTIONS RETRIEVAL METHODS
////////////////////////////////////////////////////////////////////////////////

//lcio collections
ColVec& MarlinSteerCheck::getLCIOCols() const {
    static ColVec cols;
    
    cols.clear();
    sColVecMap::const_iterator p;
    for( p=_lcioCols.begin(); p!=_lcioCols.end(); p++ ){
	cols.insert( cols.end(), (*p).second.begin(), (*p).second.end() );
    }
    return cols;
}

//active processor's collections
ColVec& MarlinSteerCheck::getAProcCols() const {
    static ColVec cols;
    
    cols.clear();
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	cols.insert( cols.end(), _aProc[i]->getCols( OUTPUT ).begin(), _aProc[i]->getCols( OUTPUT ).end() );
    }
    return cols;
}

//inactive processor's collections
ColVec& MarlinSteerCheck::getIProcCols() const {
    static ColVec cols;
    
    cols.clear();
    for( unsigned int i=0; i<_iProc.size(); i++ ){
	cols.insert( cols.end(), _iProc[i]->getCols( OUTPUT ).begin(), _iProc[i]->getCols( OUTPUT ).end() );
    }
    return cols;
}

//all available collections
ColVec& MarlinSteerCheck::getAllCols() const {
    static ColVec cols;
    
    ColVec lcCols = getLCIOCols();
    ColVec aPCols = getAProcCols();
    ColVec iPCols = getIProcCols();
    
    cols.assign( lcCols.begin(), lcCols.end() );
    cols.insert( cols.end(), aPCols.begin(), aPCols.end() );
    cols.insert( cols.end(), iPCols.begin(), iPCols.end() );
    
    return cols;
}

void MarlinSteerCheck::saveAsXMLFile( const string& file ){
    /*
    const time_t* pnow;
    time_t now;
    time(&now);
    pnow=&now;

    int length = 57;
    int slength = _steeringFileName.size();
    int llength = 0;
    int rlength = 0;
    if(length > slength){
    llength = (length - slength)/2;
    rlength = length - (llength + slength);
    }
    cout  << "<!--###########################################################" << endl
	     << "    #                                                         #" << endl
	     << "    #     Steering File for Marlin generated by myColCheck    #" << endl
	     << "    #                  "<< ctime(pnow)
	     << "    #       Comments made in the original steering file:      #" << endl;
    printf("    #"); for (int i=0; i<llength; i++) printf(" "); cout << _steeringFileName; for (int i=0; i<rlength; i++) printf(" "); printf("#\n");
    cout  << "    #                        are lost!                        #" << endl
	     << "    #                                                         #" << endl
	     << "    ###########################################################-->" << endl
	     <<  endl ;

    cout  <<  endl 
    */

    ofstream outfile( file.c_str() );
    outfile << "<marlin>\n";
    outfile << "   <execute>\n";
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	outfile << "      <processor name=\"" << _aProc[i]->getName() << "\"/>\n";
    }
    outfile << "   </execute>\n";
    
    outfile << "   <global>\n";
    //TODO
    outfile << "   </global>\n";
    outfile << "</marlin>\n";
}

////////////////////////////////////////////////////////////////////////////////
// DUMP METHODS
////////////////////////////////////////////////////////////////////////////////

const string& MarlinSteerCheck::getErrors( unsigned int index ){
    static string errors="";
    ColVec avCols;
    
    //skip if index is not valid or processor has no col errors
    if( index>=0 && index<_aProc.size() && _aProc[index]->hasUnavailableCols() ){
	errors.clear();
	//loop through all unavailable collections
	for( unsigned int i=0; i<_aProc[index]->getUCols().size(); i++ ){
	    errors+= "\nCollection [";
	    errors+= _aProc[index]->getUCols()[i]->getValue();
	    errors+= "] of type[";
	    errors+= _aProc[index]->getUCols()[i]->getType();
	    errors+= "] is unavailable!!\n";
	    
	    //find collections in inactive processors that match name and type
	    avCols.clear();
	    avCols = findMatchingCols( getIProcCols(), _aProc[index]->getUCols()[i] );
	    
	    bool found = false;
	    if( avCols.size() != 0 ){
		found = true;
		errors+= "   * Following inactive processors have a matching available collection:\n";
		
		for( unsigned int j=0; j<avCols.size(); j++ ){
		    errors+= "      -> Name: [";
		    errors+= avCols[j]->getSrcProc()->getName();
		    errors+= "] Type: [";
		    errors+= avCols[j]->getSrcProc()->getType();
		    errors+= "]\n";
		    errors+= "      -> TIP: Activate the processor [";
		    errors+=  avCols[j]->getSrcProc()->getName();
		    errors+= "] and set it before [";
		    errors+= _aProc[index]->getName();
		    errors+= "]\n";
		}
	    }	

	    //find collections in active processors that match name and type
	    avCols.clear();
	    avCols = findMatchingCols( getAProcCols(), _aProc[index]->getUCols()[i] );
	    
	    if( !found && avCols.size() != 0 ){
		found = true;
                errors+= "   * Following active processors have a matching available collection:\n";
		
		for( unsigned int j=0; j<avCols.size(); j++ ){
		    errors+= "      -> Name: [";
		    errors+= avCols[j]->getSrcProc()->getName();
		    errors+= "] Type: [";
		    errors+= avCols[j]->getSrcProc()->getType();
		    errors+= "]\n";
		    errors+= "      -> TIP: Set the processor [";
		    errors+=  avCols[j]->getSrcProc()->getName();
		    errors+= "] before [";
		    errors+= _aProc[index]->getName();
		    errors+= "]\n";
                }
	    }

	    //find collections that match the type of the unavailable collection
	    avCols.clear();
	    avCols = findMatchingCols( getAllCols(), _aProc[index]->getUCols()[i], false );
	    
	    if( !found && avCols.size() != 0){
		found = true;
		errors+= "   * Following available collections of the same type were found:\n";
		
		for( unsigned int j=0; j<avCols.size(); j++ ){
		    errors+= "      -> Name: [";
		    errors+= avCols[j]->getValue();
		    errors+= "] Type: [";
		    errors+= avCols[j]->getType();
		    errors+= "] in ";
	    
		    if( avCols[j]->getSrcProc() == NULL ){
			errors+= "LCIO file: [";
			errors+= avCols[j]->getName();
			errors+= "]\n";
		    }
		    else{
			errors+= "processor with Name: [";
			errors+= avCols[j]->getSrcProc()->getName();
			errors+= "] and Type: [";
			errors+= avCols[j]->getSrcProc()->getType();
			errors+= "]\n";

		    }
                }
	    }
	    //no collections that match the unavailable collection were found
	    if( !found ){
		errors+= "   * Sorry, no suitable collections were found.\n";
	    }
	}
    }
    return errors;
}

void MarlinSteerCheck::dump_information()
{
    /*
    //temp
    dunderline(); cout << "\nCollection Types found:" << endl; endcolor();
    map<string,int>::iterator itc;
    for( itc=_collTypes.begin(); itc!=_collTypes.end(); itc++ ){
	cout << (*itc).first << ": " << (*itc).second << endl;
    }
    //temp
    dunderline(); cout << "\nProcessor Types found:" << endl; endcolor();
    map<string,int>::iterator itp;
    for( itp=_procTypes.begin(); itp!=_procTypes.end(); itp++ ){
	cout << (*itp).first << ": " << (*itp).second << endl;
    }
    */

    //steering file
    dunderline(); cout << "\nSteering File:" << endl; endcolor();
    dhell(); dblue(); cout << _steeringFile << endl; endcolor();
    
    //LCIO files
    dunderline(); cout << "\nLCIO Input Files:" << endl; endcolor();
    dhell(); dblue();
    
    sColVecMap::const_iterator p;
    for( p=_lcioCols.begin(); p!=_lcioCols.end(); p++ ){
	cout << (*p).first << endl;
    }
    
    endcolor();

    //LCIO Collections
    dunderline(); cout << "\nLCIO Available Collections:" << endl; endcolor();
    dhell(); dblue();
    //ColVec lcCols = getLCIOCols();
    for( unsigned int i=0; i<getLCIOCols().size(); i++){
	cout << setw(40) << left << getLCIOCols()[i]->getValue();
	cout << setw(30) << left << getLCIOCols()[i]->getType();
	cout << getLCIOCols()[i]->getName() << endl;
    }
    endcolor();
    
    //Active Processors
    dunderline(); cout << "\nActive Processors:" << endl; endcolor();
    for (unsigned int i=0; i<_aProc.size(); i++){
	if( !_aProc[i]->hasErrors() ){ dgreen(); }
	else{ dred(); }
	
	cout << setw(40) << left << _aProc[i]->getName() <<
		setw(30) << left << _aProc[i]->getType() << 
		" [ " <<  _aProc[i]->getStatusDesc()
	;

	//print processor errors
	if( _aProc[i]->hasErrors() ){
	    cout << " : " << _aProc[i]->getError();
	   /* 
	    for( unsigned int j=0; j<_aProc[i]->getErrors().size(); j++ ){
		cout << " : " << _aProc[i]->getErrors()[j];
	    }
	    */
	}
	cout << " ]" << endl;
	endcolor();
    }

    //Inactive Processors
    dunderline(); cout << "\nInactive Processors:" << endl; endcolor();
    for (unsigned int i=0; i<_iProc.size(); i++){
	ddunkel(); dyellow();
	
	cout << setw(40) << left << _iProc[i]->getName() <<
		setw(30) << left << _iProc[i]->getType() << 
		" [ " << _iProc[i]->getStatusDesc() 
	;
	
	//print processor errors
	if( _iProc[i]->hasErrors() ){
	    cout << " : " << _iProc[i]->getError();
	    /*
	    for( unsigned int j=0; j<_iProc[i]->getErrors().size(); j++ ){
		cout << " : " << _iProc[i]->getErrors()[j];
	    }
	    */
	}

	cout << " ]" << endl;
	endcolor();
    }

    cout << endl;
    
    dump_colErrors();

    cout << endl;
}

void MarlinSteerCheck::dump_colErrors(){
    ColVec avCols;
    
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	//skip processor if it has no col errors
	if( _aProc[i]->hasUnavailableCols() ){
	    dred(); dunderline();
	    cout << "\nProcessor [" <<
		_aProc[i]->getName() << "] of type [" <<
		_aProc[i]->getType() << "] has following errors:" <<
	    endl;
	    endcolor();
	    
	    //loop through all unavailable collections
	    for( unsigned int j=0; j<_aProc[i]->getUCols().size(); j++ ){
		cout << "\nCollection [" <<
		    _aProc[i]->getUCols()[j]->getValue() << "] of type [" <<
		    _aProc[i]->getUCols()[j]->getType() << "] is unavailable!!" <<
		endl;
		
		//find collections in inactive processors that match name and type
		avCols.clear();
		avCols = findMatchingCols( getIProcCols(), _aProc[i]->getUCols()[j] );
		
		bool found = false;
		if( avCols.size() != 0){
		    found = true;
		    cout << "   * Following inactive processors have a matching available collection:\n";
		    for( unsigned int k=0; k<avCols.size(); k++ ){
			cout << "      -> Name: [" <<
			    avCols[k]->getSrcProc()->getName() << "] Type: [" <<
			    avCols[k]->getSrcProc()->getType() << "]" <<
			endl;
			dgreen();
			cout << "      -> TIP: Activate the processor [" <<
			    avCols[k]->getSrcProc()->getName() << "] and set it before [" <<
			    _aProc[i]->getName() << "]" <<
			endl;
			endcolor();
		    }
		}	
		
		//find collections in active processors that match name and type
		avCols.clear();
		avCols = findMatchingCols( getAProcCols(), _aProc[i]->getUCols()[j] );
		
		if( !found && avCols.size() != 0){
		    found = true;
		    cout << "   * Following active processors have a matching available collection:\n";
		    for( unsigned int k=0; k<avCols.size(); k++ ){
			cout << "      -> Name: [" <<
			    avCols[k]->getSrcProc()->getName() << "] Type: [" <<
			    avCols[k]->getSrcProc()->getType() << "]" <<
			endl;
			dgreen();
			cout << "      -> TIP: Set the processor [" <<
			    avCols[k]->getSrcProc()->getName() << "] before [" <<
			    _aProc[i]->getName() << "]" <<
			endl;
			endcolor();
		    }
		}

		//find collections that match the type of the unavailable collection
		avCols.clear();
		avCols = findMatchingCols( getAllCols(), _aProc[i]->getUCols()[j], false );
		
		if( !found && avCols.size() != 0){
		    found = true;
		    cout << "   * Following available collections of the same type were found:" << endl;
		    for( unsigned int k=0; k<avCols.size(); k++ ){
			cout << "      -> [Name: " <<
			    avCols[k]->getValue() << "] [Type: " <<
			    avCols[k]->getType() << "] in "
			;
			if( avCols[k]->getSrcProc() == 0 ){
			    cout << "LCIO file: " << avCols[k]->getName() << endl;
			}
			else{
			    cout << "processor [Name: " <<
				avCols[k]->getSrcProc()->getName() << "] [Type: " <<
				avCols[k]->getSrcProc()->getType() << "]" <<
			    endl;
			}
		    }
		}
		//no collections that match the unavailable collection were found
		if( !found ){
		    cout << "   * Sorry, no suitable collections were found." << endl;
		}
	    }
	}
    }
}
