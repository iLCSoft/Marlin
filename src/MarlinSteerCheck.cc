#include "marlin/MarlinSteerCheck.h"
#include "marlin/CMProcessor.h"

// LCIO INCLUDES ///////////////
#include "lcio.h"
#include "IO/LCReader.h"
#include "EVENT/LCCollection.h"
///////////////////////////////

// MARLIN INCLUDES /////////////
//#include "marlin/ProcessorLoader.h"
#include "marlin/ProcessorMgr.h"
#include "marlin/XMLParser.h"
#include "marlin/Parser.h"
#include "marlin/tinyxml.h"
#include "marlin/Global.h"
////////////////////////////////

#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>

using namespace std;

namespace marlin{

  // Constructor
  MarlinSteerCheck::MarlinSteerCheck( const char* steeringFile ) : _parser(NULL), _gparam(NULL), _steeringFile("Untitled.xml") {

    if( steeringFile != 0 ){
      _steeringFile=steeringFile;

      if(!parseXMLFile( steeringFile )){
	_errors.insert("XML File parsing error");
      }
    }
    //create Global Parameters
    else{
	_gparam = new StringParameters;
	StringVec value;
	value.push_back("5001");
	_gparam->add("MaxRecordNumber", value);
	value.clear();
	value.push_back("0");
	_gparam->add("SkipNEvents", value);
	value.clear();
	value.push_back("false");
	_gparam->add("SupressCheck", value);
	value.clear();
	value.push_back("gear_ldc.xml");
	_gparam->add("GearXMLFile", value);
	value.clear();
	value.push_back("MESSAGE");
	_gparam->add("Verbosity", value);
    }

//     _XMLFileAbsPath="/";
//     _XMLFileRelPath="";

//     //save the fileName and erase it from the Path
//     StringVec path;
//     CMProcessor::instance()->tokenize(_steeringFile, path, "/");
//     _XMLFileName=path[path.size()-1];
//     path.erase(path.end());

//     //if the path is relative
//     if(_steeringFile[0]!='/'){
//       string absPath=getenv("PWD");
//       StringVec absPathT;
//       CMProcessor::instance()->tokenize(absPath, absPathT, "/");
//       for( unsigned int i=0; i<path.size(); i++ ){
// 	  if(path[i]!="."){
// 	      _XMLFileRelPath+=path[i];
// 	      _XMLFileRelPath+="/";
	      
// 	      if(path[i]==".."){
// 		  absPathT.erase(absPathT.end());
// 	      }
// 	      else{
// 		  absPathT.push_back(path[i]);
// 	      }
// 	  }
//       }
//       for( unsigned int i=0; i<absPathT.size(); i++ ){
// 	  _XMLFileAbsPath+=absPathT[i];
// 	  _XMLFileAbsPath+="/";
//       }
//     }
//     else{
//       for( unsigned int i=0; i<path.size(); i++ ){
// 	  _XMLFileAbsPath+=path[i];
// 	  _XMLFileAbsPath+="/";
//       }
//     }

//     //parse the file
//     if( steeringFile != 0 ){
// 	if(!parseXMLFile( steeringFile )){
// 	    _errors.insert("XML File parsing error");
// 	}
//     }

    _marlinProcs = CMProcessor::instance();

  }

  // Destructor
  MarlinSteerCheck::~MarlinSteerCheck(){
    if(_parser){
	delete _parser;
	_parser=NULL;
    }
    if(_gparam){
	delete _gparam;
	_gparam=NULL;
    }
  }

  // Returns a list of all available Collections of a given Type
  // for a given Processor (to use in a ComboBox)
  sSet& MarlinSteerCheck::getColsSet( const string& type, const string& name, CCProcessor* proc ){
    
    _colValues.clear();

    ColVec v;
    if(!proc->isActive()){
	
	v = findMatchingCols( getAllCols(), proc, type );
	for( unsigned int i=0; i<v.size(); i++ ){
	    //check if collection already exists
	    CCProcessor tmp(ACTIVE, "Temporary", "Temporary");
	    if( findMatchingCols( proc->getCols( INPUT ), &tmp, type, v[i]->getValue(), name ).size() == 0 ){
		_colValues.insert( v[i]->getValue() );
	    }
	}
    }
    else{
	//get LCIO collections
	v = getLCIOCols();
	
	//insert collections from previous active processors
	for( unsigned int i=0; i<_aProc.size(); i++ ){
	    
	    //abort if we reach the processor
	    if( _aProc[i] == proc ){ break; }
	    
	    ColVec av = _aProc[i]->getCols( OUTPUT );
	    v.insert( v.end(), av.begin(), av.end() );
	}
	
	//create temporary processor to find matching collections
	CCProcessor tmp(ACTIVE, "Temporary", "Temporary");
	
	for( unsigned int i=0; i<v.size(); i++ ){
	    if( v[i]->getType() == type ){
		//check if collection already exists
		if( findMatchingCols( proc->getCols( INPUT ), &tmp, type, v[i]->getValue(), name ).size() == 0 ){
		    _colValues.insert( v[i]->getValue() );
		}
	    }
	}
    }

    return _colValues;
  }
 
  // Add LCIO file and read all collections inside it
  int MarlinSteerCheck::addLCIOFile( const string& file ){

//     string fileName="/";
    
//     //if path is relative concatenate XMLFileAbsPath with relative path of file
//     if(file[0]!='/'){
// 	StringVec LCIOFilePath;
// 	//initialize LCIOFilePath with the absolute path of the xml file
// 	CMProcessor::instance()->tokenize(_XMLFileAbsPath, LCIOFilePath, "/");
	
// 	StringVec path;
// 	CMProcessor::instance()->tokenize(file, path, "/");
// 	for( unsigned int i=0; i<path.size(); i++ ){
// 	    if(path[i]!="."){
// 		LCIOFilePath.push_back(path[i]);
// 	    }
// 	}
// 	for( unsigned int i=0; i<LCIOFilePath.size()-1; i++ ){
// 	    fileName+=LCIOFilePath[i];
// 	    fileName+="/";
// 	}
// 	//add the filename to the path
// 	fileName+=LCIOFilePath[LCIOFilePath.size()-1];
//     }
//     else{
// 	fileName=file;
//     }

    HANDLE_LCIO_EXCEPTIONS;
    ColVec newCols;
    
    LCReader* lcReader = LCFactory::getInstance()->createLCReader();
    try{
        lcReader->open( file );
    }
    catch( Exception& e){
        stringstream error;
        error << "Error opening LCIO file [" << file << "]. File doesn't exist, or link is not valid!!";
        _errors.insert(error.str());
        cerr << "addLCIOFile Exception: " << e.what() << endl;
        lcReader->close();
        delete lcReader;
        return 0;
    }
    
    LCEvent* evt;
    int nEvents=0;
    
    cout << "Loading LCIO file [" << file << "]\n";
    cout << "Reading Events...";
    
    while( ((evt = lcReader->readNextEvent()) != 0) && nEvents < MAXEVENTS ){
	cout << ".";
	cout.flush();
	
	const StringVec* strVec = evt->getCollectionNames();
	StringVec::const_iterator name;

	for( name = strVec->begin(); name != strVec->end(); name++ ){
	  LCCollection* col = evt->getCollection( *name ) ;
	 
	  //check if collection already exists
	  CCProcessor tmp(ACTIVE, "Temporary", "Temporary");
	  if( findMatchingCols(newCols, &tmp, col->getTypeName(), *name).size() == 0 ){
	      //store the LCIO Filename in the unused name variable from the processor parameters
	      CCCollection* newCol = new CCCollection( *name, col->getTypeName(), file);

	      newCols.push_back( newCol );
	  }
	}
	
	nEvents++;
    }
    
    //add the new Collections vector to the vector of LCIO file's Collections
    _lcioCols[file]=newCols;

    //add the file to the list of LCIO files
    _lcioFiles.push_back(file);

    lcReader->close();
    delete lcReader;

    cout << "\nLCIO file [" << file << "] was loaded successfully\n";

    consistencyCheck();

    return 1;
  } 

  // Remove lcio file and all collections associated to it
  void MarlinSteerCheck::remLCIOFile( const string& file ){
  
    //erase the allocated memory for the collections associated to the file
    sColVecMap::const_iterator q=_lcioCols.find( file );
    for( unsigned int i=0; i<q->second.size(); i++ ){
	delete q->second[i];
    }

    //delete the file from the map
    _lcioCols.erase( file );
    
    //delete the file from the list of files
    for( StringVec::iterator p=_lcioFiles.begin(); p != _lcioFiles.end(); p++ ){
	if( (*p) == file ){
	    _lcioFiles.erase(p);
	    break;
	}
    }
    
    consistencyCheck();
  }

  // Change LCIO File position
  void MarlinSteerCheck::changeLCIOFilePos( unsigned int pos, unsigned int newPos ){
  
    //check if positions are valid
    if( pos != newPos && pos >= 0 && newPos >= 0 && pos < _lcioFiles.size() && newPos < _lcioFiles.size() ){
	
	string file = _lcioFiles[pos];
	     
	for( StringVec::iterator p=_lcioFiles.begin(); p != _lcioFiles.end(); p++ ){
	    if( (*p) == file ){
		_lcioFiles.erase(p);
		break;
	    }
	}

      if(newPos == _lcioFiles.size() ){
	_lcioFiles.push_back( file );
      }
      else{
	StringVec v;
		
	for( unsigned int i=0; i<_lcioFiles.size(); i++ ){
	  if( i == newPos ){
	    v.push_back( file );
	  }
	  v.push_back( _lcioFiles[i] );
	}
	_lcioFiles = v;
      }
    }
    else{
      cerr << "changeLCIOFilePos: Index out of bounds!!" << endl;
    }
  }

  // Add a new Processor
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

  // Remove a Processor
  void MarlinSteerCheck::remProcessor( unsigned int index, bool status ){

    if( status == ACTIVE ){
	if( index >=0 && index < _aProc.size() ){
	  CCProcessor *p=popProc( _aProc, _aProc[index] );
	  delete p;
	}
    }
    else{
	if( index >=0 && index < _iProc.size() ){
	  CCProcessor *p=popProc( _iProc, _iProc[index] );
	  delete p;
	}
    }
    consistencyCheck();
  }
  
  //0 = does not exist ; 1 = exists and is active ; 2 = exists and is inactive
  int MarlinSteerCheck::existsProcessor( const string& type, const string& name ){
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	if(( name == "" && _aProc[i]->getType() == type ) || ( _aProc[i]->getType() == type && _aProc[i]->getName() == name )){
	    return 1;
	}
    }
    for( unsigned int i=0; i<_iProc.size(); i++ ){
	if(( name == "" && _iProc[i]->getType() == type ) || ( _iProc[i]->getType() == type && _iProc[i]->getName() == name )){
	    return 2;
	}
    }
    return 0;
  }

  // Activate a processor
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

  // Deactivate a processor
  void MarlinSteerCheck::deactivateProcessor( unsigned int index ){
    
    if( index >=0 && index < _aProc.size() ){
    
      //changes the processor status
      _aProc[index]->changeStatus();
      
      //clear processor conditions
      if( _aProc[index]->getConditions().size() != 0 ){
	  _aProc[index]->getConditions().clear();
      }
    
      //adds the processor to the inactive processors vector
      _iProc.push_back( popProc( _aProc, _aProc[index] ));
    
      consistencyCheck();
    }
    else{
      cerr << "activateProcessor: Index out of bounds!!" << endl;
    }
  }

  // Change an active processor's position
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

  // Check active processors for unavailable collections
  void MarlinSteerCheck::consistencyCheck(){
    //the availableCols vector will contain all the available collections read from LCIO files
    //and all output collections from active processors found before the processor being checked
    ColVec availableCols = getLCIOCols();
    ColVec inputCols, outputCols, matchCols;

    //loop through all active processors and check for unavailable collections
    for( unsigned int i=0; i<_aProc.size(); i++ ){
      //first clear the processor collection errors
      _aProc[i]->clearError( COL_ERRORS );
      
      //initialize input collections for every processor
      inputCols.clear();
      inputCols = _aProc[i]->getCols( INPUT );
      
      //initialize output collections for every processor
      outputCols.clear();
      outputCols = _aProc[i]->getCols( OUTPUT );
      
      //loop through all required collections
      for( unsigned int j=0; j<inputCols.size(); j++ ){
	matchCols.clear();
	//check if required collections are found in available collections
	matchCols = findMatchingCols( availableCols, _aProc[i], inputCols[j]->getType(), inputCols[j]->getValue() );
	//if the collection is not available
	if( matchCols.size() == 0 ){
	  //add it to the unavailable collections list of the processor
	  _aProc[i]->addUCol( inputCols[j] );
	}
      }
      
      //loop through all output collections
      for( unsigned int j=0; j<outputCols.size(); j++ ){
	matchCols.clear();
	//check if output collections are found in available collections
	matchCols = findMatchingCols( availableCols, _aProc[i], outputCols[j]->getType(), outputCols[j]->getValue() );
	//if the collection is already available
	if( matchCols.size() != 0 ){
	  //add it to the duplicate collections list of the processor
	  _aProc[i]->addDCol( outputCols[j] );
	}
      }

      //insert all Output Collections from this processor into availableCols
      availableCols.insert( availableCols.end(), outputCols.begin(), outputCols.end() );
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  // UTILITY METHODS
  ////////////////////////////////////////////////////////////////////////////////

  //parse an xml file and initialize data
  bool MarlinSteerCheck::parseXMLFile( const string& file ){

      stringstream cmd;
      cmd << "ls " << file << " >/dev/null 2>/dev/null";
      if( system( cmd.str().c_str() ) ){
	 cerr << "parseXMLFile: Failed to load file: " << _steeringFile << endl;
	 return false;
      }
      
      StringVec lcioFiles, availableProcs, activeProcs, conditions;

      //============================================================
      //PARSE THE XML FILE
      //============================================================
	
      //fg: allow xml and old steering files 
      std::string filen(  file ) ;

      if( filen.rfind(".xml") == std::string::npos ||  // .xml not found at all
	  !(  filen.rfind(".xml")
	      + strlen(".xml") == filen.length() ) ) {  
	_parser = new Parser( file ) ;
	
      } else {
	//test if file is valid
	TiXmlDocument doc( file );
	if( !doc.LoadFile() ){
	    cerr << "parseXMLFile: Failed to load file: " << _steeringFile << endl;
	    return false;
	}
	_parser = new XMLParser( file, true ) ;
      }
    try{
      _parser->parse();
    }
    catch( Exception& e ){
      cerr << "parseXMLFile: Failed to load file: " << e.what() << endl;
      return false;
    }
      _gparam = _parser->getParameters( "Global" );
      
      //============================================================
      //READ PARAMETERS
      //============================================================
	
      //add default values if there are none
	StringVec value;
	_gparam->getStringVals( "MaxRecordNumber" , value );
	if( value.size() == 0 ){
	    value.push_back("5001");
	    _gparam->add("MaxRecordNumber", value);
	}
	value.clear();
	_gparam->getStringVals( "SkipNEvents" , value );
	if( value.size() == 0 ){
	    value.push_back("0");
	    _gparam->add("SkipNEvents", value);
	}
	value.clear();
	_gparam->getStringVals( "SupressCheck" , value );
	if( value.size() == 0 ){
	    value.push_back("false");
	    _gparam->add("SupressCheck", value);
	}
	value.clear();

        //check GEAR File
	_gparam->getStringVals( "GearXMLFile" , value );
	if( value.size() == 0 ){
	    value.push_back("__unknown_gear_file__");
	    _gparam->add("GearXMLFile", value);
	}
	//remove other files if there are any
	if( value.size() > 1 ){
	    string gfile=value[0];
	    value.clear();
	    value.push_back(gfile);
	    _gparam->erase( "GearXMLFile" );
	    _gparam->add( "GearXMLFile", value );
	}

      cmd.str("");
      cmd << "ls " << value[0] << " >/dev/null 2>/dev/null";

      if( system( cmd.str().c_str() )){
	stringstream error;
	error << "Error opening GEAR file [" << value[0] << "]. File doesn't exist, or link is not valid!!";
	_errors.insert(error.str());
      }
      
      //list of lcio files defined in the global section
      _gparam->getStringVals( "LCIOInputFiles" , lcioFiles );
      _gparam->erase("LCIOInputFiles");
	
      //list of all processors defined in the body of the xml file
      //(NOT including the ones in the execute section)
      _gparam->getStringVals( "AvailableProcessors" , availableProcs );
      _gparam->erase("AvailableProcessors");
	
      //list of all processors defined in the execute section
      _gparam->getStringVals( "ActiveProcessors" , activeProcs );
      _gparam->erase("ActiveProcessors");

      //get the processor's conditions
      _gparam->getStringVals( "ProcessorConditions" , conditions );
      _gparam->erase("ProcessorConditions");

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
	    
	//get StringParameters from xml file for the name of the Processor
	StringParameters* p = _parser->getParameters( availableProcs[i] );
	    
	//get type of processor from the parameters
	string type = p->getStringVal( "ProcessorType" );
	
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
	  _errors.insert("Some Processors have no parameters");
	}
      }

      //============================================================
      //do a consistency check
      //============================================================
	
      consistencyCheck();

      for( unsigned int i=0; i<_aProc.size(); i++ ){
	  
	  //add processor conditions
	if( conditions.size() == _aProc.size() )
	  _aProc[i]->setConditions( conditions[i] );
	else
	  _aProc[i]->setConditions( "true" ); // fix for old steering files
	  
	  for( sSet::const_iterator p=_aProc[i]->getConditions().begin();
		  p!=_aProc[i]->getConditions().end(); p++ ){
	    _pConditions.insert( *p );
	  }
	  
	  if( !_aProc[i]->isInstalled() ){
	      _errors.insert("Some Active Processors are not installed");
	  }
	  if( _aProc[i]->hasErrorCols() ){
	      _errors.insert("Some Active Processors have collection errors");
	  }
      }
      for( unsigned int i=0; i<_iProc.size(); i++ ){
	  if( !_iProc[i]->isInstalled() ){
	      _errors.insert("Warning: Some Inactive Processors are not installed");
	  }
      }
      return true;
  }

  //find matching collections on the given vector
  ColVec& MarlinSteerCheck::findMatchingCols( ColVec& v, CCProcessor* srcProc, const string& type, const string& value, const string& name ){
    static ColVec cols;

    cols.clear();
    for( unsigned int i=0; i<v.size(); i++ ){
      if( v[i]->getSrcProc() != srcProc && v[i]->getType() == type ){
	if( value != "UNDEFINED" ){
	  if( v[i]->getValue() == value ){
	    if( name != "UNDEFINED" ){
		if( v[i]->getName() == name ){
		    cols.push_back( v[i] );
		}
	    }
	    else{
		cols.push_back( v[i] );
	    }
	  }
	}
	//if value == "UNDEFINED" it means we want all collections of a given type
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
  
  //all processors
  ProcVec& MarlinSteerCheck::getAllProcs() const {
    static ProcVec procs;

    procs.clear();
    
    procs.assign( _aProc.begin(), _aProc.end() );
    procs.insert( procs.end(), _iProc.begin(), _iProc.end() );
    
    return procs;
  }

  const string MarlinSteerCheck::getCondition( unsigned int index ){
    if( index >= 0 && index < _pConditions.size()){
	sSet::const_iterator p;
	advance( p=_pConditions.begin(), index );
	return *p;
    }
    return "";
  }

  void MarlinSteerCheck::addCondition( const std::string& condition ){
      _pConditions.insert( condition );
  }
  
  void MarlinSteerCheck::repCondition( const std::string& oldCond, const std::string& newCond ){
      _pConditions.erase( oldCond );
      _pConditions.insert( newCond );
      
      for( unsigned int i=0; i<_aProc.size(); i++ ){
	if( _aProc[i]->hasCondition( oldCond )){
	    _aProc[i]->getConditions().erase( oldCond );
	    _aProc[i]->getConditions().insert( newCond );
	}
      }
  }

  void MarlinSteerCheck::remCondition( const std::string& condition ){
      _pConditions.erase( condition );
      
      for( unsigned int i=0; i<_aProc.size(); i++ ){
	if( _aProc[i]->hasCondition( condition )){
	    _aProc[i]->getConditions().erase( condition );
	}
      }
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  // COLLECTIONS RETRIEVAL METHODS
  ////////////////////////////////////////////////////////////////////////////////

  //lcio collections
  ColVec& MarlinSteerCheck::getLCIOCols() const {
    static ColVec cols;
    
    cols.clear();

    for( sColVecMap::const_iterator p=_lcioCols.begin(); p!=_lcioCols.end(); p++ ){
      cols.insert( cols.end(), p->second.begin(), p->second.end() );
    }
    return cols;
  }

  //processor's available collections
  ColVec& MarlinSteerCheck::getProcCols( const ProcVec& v, const string& iotype ) const {
    static ColVec cols;
    
    cols.clear();
    for( unsigned int i=0; i<v.size(); i++ ){
      cols.insert( cols.end(), v[i]->getCols( iotype ).begin(), v[i]->getCols( iotype ).end() );
    }
    return cols;
  }

  //all available collections
  ColVec& MarlinSteerCheck::getAllCols() const {
    static ColVec cols;
    
    ColVec lcCols = getLCIOCols();
    ColVec aPCols = getProcCols( _aProc, OUTPUT );
    ColVec iPCols = getProcCols( _iProc, OUTPUT );
    
    cols.assign( lcCols.begin(), lcCols.end() );
    cols.insert( cols.end(), aPCols.begin(), aPCols.end() );
    cols.insert( cols.end(), iPCols.begin(), iPCols.end() );
    
    return cols;
  }

  // Saves the data to an XML file with the given name
  bool MarlinSteerCheck::saveAsXMLFile( const string& file ){

    if( file.size() == 0 ){ return false; }
    
    ofstream outfile;
    outfile.open( file.c_str() );

    //abort if file cannot be created or modified
    if( !outfile ){
	cerr << "MarlinSteerCheck::saveAsXMLFile: Error creating or modifying XML File [" << file << "]\n";
	return false;
    }
    
    const time_t* pnow;
    time_t now;
    time(&now);
    pnow=&now;

    outfile << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << endl;
    outfile << "<!-- ?xml-stylesheet type=\"text/xsl\" href=\"http://ilcsoft.desy.de/marlin/marlin.xsl\"? -->" << endl;
    outfile << "<!-- ?xml-stylesheet type=\"text/xsl\" href=\"marlin.xsl\"? -->" << endl << endl;

    outfile << "<!--\n";
    outfile << "============================================================================================================================\n";
    outfile << "   Steering File generated by Marlin GUI on " << ctime(pnow) << endl;
    outfile << "   WARNING: - Please be aware that comments made in the original steering file were lost.\n";
    outfile << "            - Processors that are not installed in your Marlin binary lost their parameter's descriptions and types as well.\n";
    outfile << "            - Extra parameters that aren't categorized as default in a processor lost their description and type.\n";
    outfile << "============================================================================================================================\n";
    outfile << "-->\n";
    
    outfile << "\n\n<marlin xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    outfile << "xsi:noNamespaceSchemaLocation=\"http://ilcsoft.desy.de/marlin/marlin.xsd\">" << endl << endl;

    //============================================================
    // execute section
    //============================================================
    
    outfile << "   <execute>\n";

    //Conditions variables
    vector< pair<sSet, int> > openConditions;

    for( unsigned int i=0; i<_aProc.size(); i++ ){
	//if processor has conditions
	if( _aProc[i]->getConditions().size() != 0 ){
	    map<int, sSet> condLength;
	    //get length of conditions
	    for( sSet::const_iterator p=_aProc[i]->getConditions().begin(); p!=_aProc[i]->getConditions().end(); p++ ){
		int length=1;
		//check if processors below have the same condition
		for( unsigned int j=(i+1); j<_aProc.size(); j++ ){
		    if( _aProc[j]->hasCondition(*p) ){
			length++;
		    }
		    else{
			//abort searching if chain is broken
			break;
		    }
		}
		bool found=false;
		//check if the condition is already open
		for( unsigned int j=0; j<openConditions.size(); j++ ){
		    if( openConditions[j].first.find(*p) != openConditions[j].first.end() ){
			found=true;
			break;
		    }
		}
		if(!found){
		    //add condition length to a map
		    condLength[length].insert(*p);
		}
	    }
	    //update open conditions
	    for( unsigned int j=0; j<openConditions.size(); j++ ){
		openConditions[j].second--;
	    }
	    //push new conditions into open conditions and write them to the xml file
	    for( map<int, sSet>::reverse_iterator p=condLength.rbegin(); p!=condLength.rend(); p++ ){
		openConditions.push_back( make_pair( p->second, p->first ));
		
		outfile << string( (openConditions.size()+1)*3,' ') << "<if condition=\"";
		unsigned int size=0;
		if( p->second.size() != 1 ){
		    for( sSet::const_iterator q=p->second.begin(); q!=p->second.end(); q++ ){
			if( ((*q).find('&',0)!=string::npos) || ((*q).find('|',0)!=string::npos) || ((*q).find('!',0)!=string::npos) ){
			    outfile << "(" << *q << ")";
			}
			else{
			    outfile << *q;
			}
			if( ++size < p->second.size() ){
			    outfile << " && ";
			} 
		    }
		}
		else{
		    outfile << *p->second.begin();
		}
		outfile << "\">\n";
	    }
	    outfile << string( (openConditions.size()+2)*3,' ') << "<processor name=\"" << _aProc[i]->getName() << "\"/>\n";
	    
	    //check if there are final conditions
	    unsigned int pos;
	    for( pos=0; pos<openConditions.size(); pos++ ){
		if( openConditions[pos].second==1 ){
		    break;
		}
	    }
	    if( pos != openConditions.size() ){
		for( unsigned int j=openConditions.size(); j>pos; j-- ){
		    outfile << string( (openConditions.size()+1)*3,' ') << "</if>\n";
		    openConditions.pop_back();
		}
	    }
	}
	else{
	    outfile << "      <processor name=\"" << _aProc[i]->getName() << "\"/>\n";
	}
    }
    for( unsigned int i=0; i<_iProc.size(); i++ ){
      outfile << "      <!--processor name=\"" << _iProc[i]->getName() << "\"/-->\n";
    }
   
    outfile << "   </execute>\n\n";
    
    //============================================================
    // global section
    //============================================================
    
    outfile << "   <global>\n";
      
      //LCIO Files
      outfile << "      <parameter name=\"LCIOInputFiles\">";
      
      for( unsigned int i=0; i<getLCIOFiles().size(); i++ ){
	outfile << " " << getLCIOFiles()[i];
      }
      outfile << " </parameter>\n";
 
      StringVec keys;
      _gparam->getStringKeys( keys );
      
      //Other parameters
      for( unsigned int i=0; i<keys.size(); i++ ){
	outfile << "      <parameter name=\"" << keys[i] << "\"";

	StringVec values;
	//get the values for the given key
	_gparam->getStringVals( keys[i], values );

	outfile << ( values.size() == 1 ? " value=\"" : ">" );
      
        for( unsigned int j=0; j<values.size(); j++ ){
	    outfile << ( values.size() == 1 ? "" : " ") << values[j];
	}
	outfile << ( values.size() == 1 ? "\"/>\n" : " </parameter>\n" );
      }
    
    outfile << "   </global>\n\n";

    //============================================================
    // processors (active + inactive)
    //============================================================

    for( unsigned int i=0; i<_aProc.size(); i++ ){
      _aProc[i]->writeToXML( outfile );
    }
    for( unsigned int i=0; i<_iProc.size(); i++ ){
      _iProc[i]->writeToXML( outfile );
    }
    
    outfile << "</marlin>\n";

    return true;
  }

  // Saves steering file in dot format
  bool MarlinSteerCheck::saveAsDOTFile(const std::string& file){
    
    if( _errors.find("XML File parsing error") != _errors.end() ){
	return false;
    }
    
    if( file.size() == 0 ){ return false; }
    
    ofstream dotfile;
    dotfile.open( file.c_str() );

    //abort if file cannot be created or modified
    if( !dotfile ){
	cerr << "MarlinSteerCheck::saveAsDOTFile: Error creating or modifying DOT File [" << file << "]\n";
	return false;
    }
    
    //Header of dot file
    dotfile << "digraph MarlinSteering {\n"
            << "node [ fontname = \"Helvetica\","
	    << "style = filled ];\n";
    
    //LCIO File (just the first one)
    if (!getLCIOFiles().empty())
	dotfile << "LCIOFile [color=darkgreen, fontcolor=white, label=\"" << getLCIOFiles()[0] << "\"]\n";
    else
	dotfile << "LCIOFile [color=darkgreen, fontcolor=white, label=\"LCIO File\"]\n";
    
    //This map holds the last processor to output a collection with this name
    map<string,string> producer_of;
    map<string,string> type_of;
    
    //Keep track of used outputs (we don't display the unused ones)
    vector<string> used_outputs;
    
    //We start with the collections produced by the LCIO file
    ColVec lcioCols = getLCIOCols();
    for( unsigned int i=0; i<lcioCols.size(); i++ ){
	producer_of[lcioCols[i]->getValue()] = "LCIOFile";
	type_of[lcioCols[i]->getValue()] = lcioCols[i]->getType();
    }
    
    //Loop over the processors finding where their inputs come from and
    //adding their outputs to the list of producers
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	ColVec inputCols,outputCols;
	inputCols = _aProc[i]->getCols( INPUT );
	outputCols = _aProc[i]->getCols( OUTPUT );
	for( unsigned int j=0; j<inputCols.size(); j++ ){
		string producer = producer_of[inputCols[j]->getValue()];
		if (producer.size() > 0){
			//Producer found, create link
			dotfile << inputCols[j]->getValue() << " -> " << _aProc[i]->getName() << ";\n";
			//Note that this collection was used
			used_outputs.push_back(inputCols[j]->getValue());
		}
		else
			cout << inputCols[j]->getValue() << " not found for " << _aProc[i]->getName() << "\n";
	}
	for( unsigned int j=0; j<outputCols.size(); j++ ){
		producer_of[outputCols[j]->getValue()] = _aProc[i]->getName();
		type_of[outputCols[j]->getValue()] = outputCols[j]->getType();
	}
    }
    
    //Nodes for used collections and links from producers to output collections
    sort(used_outputs.begin(), used_outputs.end());
    vector<string>::iterator new_end  = unique(used_outputs.begin(), used_outputs.end());
    for(int i=0; i<new_end-used_outputs.begin(); i++ ){
	dotfile << used_outputs[i] << " [color=lightsteelblue2, label=<<TABLE BORDER=\"0\"><TR><TD>"
	<< used_outputs[i] << "</TD></TR><TR><TD><FONT POINT-SIZE=\"11.0\">"
	<< type_of[used_outputs[i]] << "</FONT></TD></TR></TABLE>>];\n";
	dotfile << producer_of[used_outputs[i]] << " -> " << used_outputs[i] << ";\n";	
    }
    
    //Processor Nodes
    for( unsigned int i=0; i<_aProc.size(); i++ ){
	if (_aProc[i]->getType() != "LCIOOutputProcessor")
		dotfile << _aProc[i]->getName() << " [color=navyblue, fontcolor=white, label=\"" 
		<< _aProc[i]->getName() << "\"shape=rectangle];\n";
    }
    
    //Footer
    dotfile << "};\n";
    
    dotfile.close();
    cout << "DOT File written...\n";
    cout << "Now run dot eg: dot -Tps " << file << " > "<< file << ".ps\n";
    return true;
  }
  

  ////////////////////////////////////////////////////////////////////////////////
  // DUMP METHODS
  ////////////////////////////////////////////////////////////////////////////////

  // Dumps all information read from the steering file to stdout
  void MarlinSteerCheck::dump_information()
  {
    if( _errors.find("XML File parsing error") != _errors.end() ){
	return;
    }
    //steering file
    dunderline(); cout << "\nSteering File:" << endl; endcolor();
    dhell(); dblue(); cout << _steeringFile << endl; endcolor();
    
    //LCIO files
    dunderline(); cout << "\nLCIO Input Files:" << endl; endcolor();
    dhell(); dblue();
    
    for( unsigned int i=0; i<getLCIOFiles().size(); i++ ){
      cout << getLCIOFiles()[i] << endl;
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
    for( unsigned int i=0; i<_aProc.size(); i++ ){
      
      //print title
      if( i == 0 ){ dunderline(); cout << "\nActive Processors:" << endl; endcolor(); }
      
      if( !_aProc[i]->hasErrors() ){ dgreen(); }
      else{ dred(); }
	
      cout << setw(40) << left << _aProc[i]->getName() <<
	setw(30) << left << _aProc[i]->getType() << 
	" [ " <<  _aProc[i]->getStatusDesc() << " ] ";

      //print processor errors
      if( _aProc[i]->hasErrors() ){
	cout << " ( " << _aProc[i]->getError() << " )";
	/* 
	   for( unsigned int j=0; j<_aProc[i]->getErrors().size(); j++ ){
	   cout << " : " << _aProc[i]->getErrors()[j];
	   }
	*/
      }
      cout << endl;
      endcolor();
    }

    //Inactive Processors
    for( unsigned int i=0; i<_iProc.size(); i++ ){
      
      //print title
      if( i == 0 ){ dunderline(); cout << "\nInactive Processors:" << endl; endcolor(); }
      
      ddunkel(); dyellow();
	
      cout << setw(40) << left << _iProc[i]->getName() <<
	setw(30) << left << _iProc[i]->getType() << 
	" [ " << _iProc[i]->getStatusDesc() << " ] ";
	
      //print processor errors
      if( _iProc[i]->hasErrors() ){
	cout << " ( " << _iProc[i]->getError() << " )";
	/*
	  for( unsigned int j=0; j<_iProc[i]->getErrors().size(); j++ ){
	  cout << " : " << _iProc[i]->getErrors()[j];
	  }
	*/
      }

      cout << endl;
      endcolor();
    }

    cout << endl;

    for( unsigned int i=0; i<_aProc.size(); i++ ){
	dumpColErrors( i, cout );
    }
   
    cout << endl << endl;
    
    for( sSet::const_iterator p=_errors.begin(); p!=_errors.end(); p++){
	dred();
	cout << (*p) << endl;
	endcolor();
    }
   
    if( _errors.size()==0 || (_errors.size()==1 && (_errors.find("Warning: Some Inactive Processors are not installed")!=_errors.end()))){
	cout << "\nNo Errors were found :)" << endl;
    }
    else{
	cout << "\nErrors were found (see above)..." << endl;
    }

    cout << endl;
  }

  // Returns the Errors for an Active Processor at the given index
  void MarlinSteerCheck::dumpColErrors( unsigned int i, ostream& stream, bool separators ){
    
    //skip if index is not valid or processor has no col errors
    if( i>=0 && i<_aProc.size() && _aProc[i]->hasErrorCols() ){
	
	dred(); dunderline();
	stream << "\nProcessor [" <<
	  _aProc[i]->getName() << "] of type [" <<
	  _aProc[i]->getType() << "] has following errors:" <<
	endl;
	endcolor();

	sSet dTypes = _aProc[i]->getColTypeNames( DUPLICATE );

	for( sSet::const_iterator q=dTypes.begin(); q!=dTypes.end(); q++){
	    stream << "\n* Following Collections of type [";
	    dhell(); dblue(); stream << (*q); endcolor();
	    stream << "] were already found in the event:\n";
	    if( separators ){
		stream << string( 170, '-') << endl;
	    }

	    ColVec dCols = _aProc[i]->getCols( DUPLICATE, (*q) );
	    for( unsigned int j=0; j<dCols.size(); j++ ){
		stream << "   -> [";
		dyellow(); stream << dCols[j]->getValue(); endcolor();
		stream << "]\n";
	    }
	    stream << "\n   * Following collections are in conflict with this collection:\n";
	    for( unsigned int j=0; j<dCols.size(); j++ ){

		ColVec lcioCols = findMatchingCols( getLCIOCols(), _aProc[i], dCols[j]->getType(), dCols[j]->getValue() );
		if( lcioCols.size() != 0 ){
		    for( unsigned int k=0; k<lcioCols.size(); k++ ){
			stream << "      -> [";
			dyellow(); stream << lcioCols[k]->getValue(); endcolor();
			stream << "] in LCIO file: [" << lcioCols[k]->getName() << "]\n";
		    }
		}
		ColVec oCols = findMatchingCols( getProcCols(_aProc, OUTPUT), _aProc[i], dCols[j]->getType(), dCols[j]->getValue() );
		if( oCols.size() != 0 ){
		    for( unsigned int k=0; k<oCols.size(); k++ ){
			stream << "      -> [";
			dyellow(); stream << oCols[k]->getValue(); endcolor();
			stream << "] in [Active] Processor [" << oCols[k]->getSrcProc()->getName() 
			     << "] of Type [" << oCols[k]->getSrcProc()->getType() << "]\n";
		    }
		}
	    }
	    if( separators ){
		stream << string( 170, '-') << endl;
	    }
	}

	sSet uTypes = _aProc[i]->getColTypeNames( UNAVAILABLE );

	for( sSet::const_iterator p=uTypes.begin(); p!=uTypes.end(); p++){
	    stream << "\n* Following Collections of type [";
	    dhell(); dblue(); stream << (*p); endcolor();
	    stream << "] are unavailable:\n";
	    if( separators ){
		stream << string( 170, '-') << endl;
	    }

	    ColVec uCols = _aProc[i]->getCols( UNAVAILABLE, (*p) );
	    for( unsigned int j=0; j<uCols.size(); j++ ){
	      stream << "   -> [";
	      dyellow(); stream << uCols[j]->getValue(); endcolor();
	      stream << "]\n";
	    }
	    stream << endl;

	    //find collections that match the type of the unavailable collection
	    ColVec avCols = findMatchingCols( getAllCols(), _aProc[i], (*p) );
	    
	    if( avCols.size() != 0 ){
		dgreen();
		stream << "   * Following available collections of the same type were found:" << endl;
		endcolor();
		for( unsigned int k=0; k<avCols.size(); k++ ){
		  stream << "      -> [";
		  
		  for( unsigned int j=0; j<uCols.size(); j++ ){
		    if( avCols[k]->getValue() == uCols[j]->getValue() ){ dyellow(); }
		  }
		  stream << avCols[k]->getValue();
		  endcolor();

		  if( avCols[k]->getSrcProc() == 0 ){
		    stream << "] in LCIO file: [" << avCols[k]->getName() << "]" << endl;
		  }
		  else{
		    stream << "] in [" <<
		      avCols[k]->getSrcProc()->getStatusDesc() << "] Processor [" <<
		      avCols[k]->getSrcProc()->getName() << "] of Type [" <<
		      avCols[k]->getSrcProc()->getType() << "]" <<
		    endl;
		  }
		}
	    }
	    //no collections that match the unavailable collection were found
	    else{
	      stream << "   * Sorry, no suitable collections were found." << endl;
	    }
	    if( separators ){
		stream << string( 170, '-') << endl;
	    }
	  }
       }
  }

} // namespace
