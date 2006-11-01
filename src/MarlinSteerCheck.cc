#include "marlin/MarlinSteerCheck.h"

// LCIO INCLUDES ///////////////
#include "lcio.h"
#include "IO/LCReader.h"
#include "EVENT/LCCollection.h"
///////////////////////////////

// MARLIN INCLUDES /////////////
#include "marlin/ProcessorMgr.h"
#include "marlin/XMLParser.h"
#include "marlin/tinyxml.h"
#include "marlin/Global.h"
                                                                                                                                                             
#ifdef USE_GEAR
#include "gearimpl/Util.h"
#include "gearxml/GearXML.h"
#include "gearimpl/GearMgrImpl.h"
#endif

////////////////////////////////

#include <assert.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <stdlib.h>
//#include <time.h>

using namespace std;

namespace marlin{

  MarlinSteerCheck::MarlinSteerCheck( const char* steeringFile ) : _parser(NULL), _gparam(NULL) {
    if( steeringFile != 0 ){
      _steeringFile=steeringFile;
      //parse the file
      parseXMLFile( steeringFile );
    }
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
    }
    //get a list of all available processor types from marlin Processor Manager
    _procTypes = ProcessorMgr::instance()->getAvailableProcessorTypes();
  }

  MarlinSteerCheck::~MarlinSteerCheck(){
    if(_parser){
	delete _parser;
    }
    if(_gparam){
	delete _gparam;
    }
  }

  /**************************************************************
   * Returns a list of all available Collections of a given Type
   * for a given Processor (to use in a ComboBox)
   **************************************************************/
  sSet& MarlinSteerCheck::getColsSet( const string& type, CCProcessor* proc ){
    
    _colValues.clear();
    ColVec v = getAllCols();
    
    for( unsigned int i=0; i<v.size(); i++ ){
      if( v[i]->getSrcProc() != proc && v[i]->getType() == type ){
        _colValues.insert( v[i]->getValue() );
      }
    }
    return _colValues;
  }
 
  /***************************************************
   * Add LCIO file and read all collections inside it
   ***************************************************/
  void MarlinSteerCheck::addLCIOFile( const string& file ){
   
    //FIXME: this is to prevent crashing the application if
    //the file doesn't exist (is there a better way to handle this??)
    string cmd= "ls ";
    cmd+=file;
    cmd+= " >/dev/null";
    if( system( cmd.c_str() ) ){ return; }
    
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

  /*****************************
   * Get the list of LCIO Files
   *****************************/
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
  void MarlinSteerCheck::remProcessor( unsigned int index, bool status ){

    if( status == ACTIVE ){
	if( index >=0 && index < _aProc.size() ){
	  popProc( _aProc, _aProc[index] );
	}
    }
    else{
	if( index >=0 && index < _iProc.size() ){
	  popProc( _iProc, _iProc[index] );
	}
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
	matchCols = findMatchingCols( availableCols, _aProc[i], requiredCols[j]->getType(), requiredCols[j]->getValue() );
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
      _gparam = _parser->getParameters( "Global" );
      
      //============================================================
      //READ PARAMETERS
      //============================================================
	
      //list of lcio files defined in the global section
      _gparam->getStringVals( "LCIOInputFiles" , lcioFiles );
      _gparam->erase("LCIOInputFiles");
	
      //list of all processors defined in the xml file NOT including the ones in the execute section
      _gparam->getStringVals( "AvailableProcessors" , availableProcs );
      _gparam->erase("AvailableProcessors");
	
      //this gets a name list of all processors defined in the execute section
      _gparam->getStringVals( "ActiveProcessors" , activeProcs );
      _gparam->erase("ActiveProcessors");
    
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
  ColVec& MarlinSteerCheck::findMatchingCols( ColVec& v, CCProcessor* srcProc, const string& type, const string& value ){
    static ColVec cols;

    cols.clear();
    for( unsigned int i=0; i<v.size(); i++ ){
      if( v[i]->getSrcProc() != srcProc && v[i]->getType() == type ){
	if( value != "UNDEFINED" ){
	  if( v[i]->getValue() == value ){
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

  //processor's available collections
  ColVec& MarlinSteerCheck::getProcCols( const ProcVec& v ) const {
    static ColVec cols;
    
    cols.clear();
    for( unsigned int i=0; i<v.size(); i++ ){
      cols.insert( cols.end(), v[i]->getCols( OUTPUT ).begin(), v[i]->getCols( OUTPUT ).end() );
    }
    return cols;
  }

  //all available collections
  ColVec& MarlinSteerCheck::getAllCols() const {
    static ColVec cols;
    
    ColVec lcCols = getLCIOCols();
    ColVec aPCols = getProcCols( _aProc );
    ColVec iPCols = getProcCols( _iProc );
    
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
    
    if( file.size() == 0 ){ return; }
   
    ofstream outfile( file.c_str() );
    
    outfile << "<marlin>\n\n";

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
    // execute section
    //============================================================
    
    outfile << "   <execute>\n";
    
    for( unsigned int i=0; i<_aProc.size(); i++ ){
      outfile << "      <processor name=\"" << _aProc[i]->getName() << "\"/>\n";
    }
    for( unsigned int i=0; i<_iProc.size(); i++ ){
      outfile << "      <Xprocessor name=\"" << _iProc[i]->getName() << "\"/>\n";
    }
   
    outfile << "   </execute>\n\n";
    
    for( unsigned int i=0; i<_aProc.size(); i++ ){
      _aProc[i]->writeToXML( outfile );
    }
    for( unsigned int i=0; i<_iProc.size(); i++ ){
      _iProc[i]->writeToXML( outfile );
    }
    
    outfile << "\n</marlin>\n";
  }

  ////////////////////////////////////////////////////////////////////////////////
  // DUMP METHODS
  ////////////////////////////////////////////////////////////////////////////////

  void MarlinSteerCheck::dump_information()
  {
    bool found_errors = false;
    
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
    for( unsigned int i=0; i<_aProc.size(); i++ ){
      
      //print title
      if( i == 0 ){ dunderline(); cout << "\nActive Processors:" << endl; endcolor(); }
      
      if( !_aProc[i]->hasErrors() ){ dgreen(); }
      else{ dred(); }
	
      cout << setw(40) << left << _aProc[i]->getName() <<
	setw(30) << left << _aProc[i]->getType() << 
	" [ " <<  _aProc[i]->getStatusDesc()
	;

      //print processor errors
      if( _aProc[i]->hasErrors() ){
	found_errors = true;
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
    for( unsigned int i=0; i<_iProc.size(); i++ ){
      
      //print title
      if( i == 0 ){ dunderline(); cout << "\nInactive Processors:" << endl; endcolor(); }
      
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
    
    if( !(dump_colErrors() && found_errors) ){
	cout << "\nNo Errors were found :)" << endl;
    }

    cout << endl;
  }

  bool MarlinSteerCheck::dump_colErrors(){
    
    bool found_errors = false;
    
    for( unsigned int i=0; i<_aProc.size(); i++ ){
      //skip processor if it has no col errors
      if( _aProc[i]->hasUnavailableCols() ){
	  
	found_errors = true;
	
	dred(); dunderline();
	cout << "\nProcessor [" <<
	  _aProc[i]->getName() << "] of type [" <<
	  _aProc[i]->getType() << "] has following errors:" <<
	endl;
	endcolor();

	sSet uTypes = _aProc[i]->getUColTypes();

	for( sSet::const_iterator p=uTypes.begin(); p!=uTypes.end(); p++){
	    cout << "\n* Following Collections of type [";
	    dhell(); dblue(); cout << (*p); endcolor();
	    cout <<"] are unavailable:\n";

	    ColVec uCols = _aProc[i]->getCols( UNAVAILABLE, (*p) );
	    for( unsigned int j=0; j<uCols.size(); j++ ){
	      cout << "   -> [";
	      dyellow(); cout << uCols[j]->getValue(); endcolor();
	      cout << "]\n";
	    }
	    cout << endl;

	    //find collections that match the type of the unavailable collection
	    ColVec avCols = findMatchingCols( getAllCols(), _aProc[i], (*p) );
	    
	    if( avCols.size() != 0 ){
		dgreen();
		cout << "   * Following available collections of the same type were found:" << endl;
		endcolor();
		for( unsigned int k=0; k<avCols.size(); k++ ){
		  cout << "      -> [";
		  
		  for( unsigned int j=0; j<uCols.size(); j++ ){
		    if( avCols[k]->getValue() == uCols[j]->getValue() ){ dyellow(); }
		  }
		  cout << avCols[k]->getValue();
		  endcolor();

		  if( avCols[k]->getSrcProc() == 0 ){
		    cout << "] in LCIO file: [" << avCols[k]->getName() << "]" << endl;
		  }
		  else{
		    cout << "] in [" <<
		      avCols[k]->getSrcProc()->getStatusDesc() << "] Processor [" <<
		      avCols[k]->getSrcProc()->getName() << "] of Type [" <<
		      avCols[k]->getSrcProc()->getType() << "]" <<
		    endl;
		  }
		}
	    }
	    //no collections that match the unavailable collection were found
	    else{
	      cout << "   * Sorry, no suitable collections were found." << endl;
	    }
	  }
       }
    }
    return found_errors;
  }
  
  const string& MarlinSteerCheck::getErrors( unsigned int index ){
    static string errors;
    ColVec avCols, uCols;
    
    errors.clear();
    //skip if index is not valid or processor has no col errors
    if( index>=0 && index<_aProc.size() && _aProc[index]->hasUnavailableCols() ){
	
	sSet uTypes = _aProc[index]->getUColTypes();

	for( sSet::const_iterator p=uTypes.begin(); p!=uTypes.end(); p++){

	    errors+= "\n* Following Collections of type [";
	    errors+= (*p);
	    errors+= "] are unavailable:\n";
	    errors+= "--------------------------------------------------------------------------------";
	    errors+= "--------------------------------------------------------------------------------\n";

	    ColVec uCols = _aProc[index]->getCols( UNAVAILABLE, (*p) );
	    for( unsigned int j=0; j<uCols.size(); j++ ){
	      errors+= "   -> [";
	      errors+= uCols[j]->getValue();
	      errors+= "]\n";
	    }
	    errors+= "\n";

	    //find collections that match the type of the unavailable collection
	    ColVec avCols = findMatchingCols( getAllCols(), _aProc[index], (*p) );
	    
	    if( avCols.size() != 0 ){
		errors+= "* Following available collections of the same type were found:\n";
		for( unsigned int k=0; k<avCols.size(); k++ ){
		  errors+= "      -> [";
		  errors+= avCols[k]->getValue();

		  if( avCols[k]->getSrcProc() == 0 ){
		    errors+= "] in LCIO file: [";
		    errors+= avCols[k]->getName();
		    errors+= "]\n";
		  }
		  else{
		    errors+= "] in [";
		    errors+= avCols[k]->getSrcProc()->getStatusDesc();
		    errors+= "] Processor [";
		    errors+= avCols[k]->getSrcProc()->getName();
		    errors+= "] of Type [";
		    errors+= avCols[k]->getSrcProc()->getType();
		    errors+= "]\n";
		  }
		}
	    }
	    //no collections that match the unavailable collection were found
	    else{
	      errors+= "   * Sorry, no suitable collections were found.\n";
	    }
	    errors+= "--------------------------------------------------------------------------------";
	    errors+= "--------------------------------------------------------------------------------\n";
	  }
       }
    return errors;
  }

} // namespace
