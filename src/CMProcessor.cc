#include "marlin/CMProcessor.h"

#include <iostream>

using namespace std;

namespace marlin{
    
    CMProcessor* CMProcessor::_me = NULL;
    
    CMProcessor* CMProcessor::instance() {
	if( _me == NULL ){
	    _me = new CMProcessor;
	}
	return _me ;
    }

    CMProcessor::CMProcessor(){
	
    	//get a list of all available processor types from the marlin Processor Manager
    	_mpTypes = ProcessorMgr::instance()->getAvailableProcessorTypes();
	
	for( sSet::const_iterator p = _mpTypes.begin() ; p != _mpTypes.end() ; p++ ){
	    
	    Processor* pp = ProcessorMgr::instance()->getProcessor( (*p) );
	    //checks if marlin has this processor type installed
	    if( pp ){
		
		//create a new marlin processor of this type
		_mProcs[ (*p) ] = pp->newProcessor();
		
		//set it's status
		_mpStatus[ (*p) ] = true;

		//add it's description
		_mpDescriptions[ (*p) ] = pp->description();

		//create new string parameters for each processor
		_mpSParameters[ (*p) ] = new StringParameters;

		//set the parameters keys
		_mProcs[ (*p) ]->setProcessorParameters( _mpSParameters[ (*p) ] );
		_mProcs[ (*p) ]->updateParameters();
		
		//get the parameters keys
		StringVec keys;
		_mpSParameters[ (*p) ]->getStringKeys( keys );

		//for each key
		for( unsigned i=0; i<keys.size(); i++ ){
		    StringVec values;
		    values.push_back( keys[i] );
		    
		    //set the parameter with the default value
		    //since the default value is always a string we might have to tokenize it
		    if( isParamVec( *p, keys[i] )){
			tokenize( getParam( (*p), keys[i] )->defaultValue(), values );
		    }
		    else{
			values.push_back( getParam( (*p), keys[i] )->defaultValue() );
		    }
		    //add the default value to the parameters
		    _mpSParameters[ (*p) ]->add(values);
		}
	    }
	    //this processor type is not installed
	    else{
		_mProcs[ (*p) ] = NULL;
		
		//set it's status
		_mpStatus[ (*p) ] = false;

		//add it's description
		_mpDescriptions[ (*p) ] = "This processor is NOT installed!! Please install it before using it...";

		//set string parameters
		_mpSParameters[ (*p) ] = NULL;
	    }
    	}
    } //end constructor

    //destructor
    CMProcessor::~CMProcessor(){}
    
    bool CMProcessor::isInstalled( const string& type ){
	if( _mpStatus.find( type ) != _mpStatus.end() ){
	    return _mpStatus[ type ];
	}
	return false;
    }
    
    Processor* CMProcessor::getProc( const string& type ){
	if( isInstalled( type )){
	    return _mProcs[ type ];
	}
	return NULL;
    }
    
    StringParameters* CMProcessor::getSParams( const string& type ){
	if( isInstalled( type )){
	    return _mpSParameters[ type ];
	}
	return NULL;
    }
    
    StringParameters* CMProcessor::mergeParams( const string& type, StringParameters* sp ){
	if( isInstalled( type )){
	    if( sp ){
		StringVec keys;
		//get the keys for the default string parameters of this processor
		_mpSParameters[ type ]->getStringKeys( keys );

		//for each key
		for( unsigned i=0; i<keys.size(); i++ ){
		    
		    //check if the parameter value is already set
		    if( !sp->isParameterSet( keys[i] )){
			
			//get the default values for this key
			StringVec values;
			_mpSParameters[ type ]->getStringVals( keys[i], values );

			//add the default parameter to the given (xml) parameters
			sp->add( keys[i], values);
		    }
		    //parameter is already set in the xml file
		    else{
			//if the parameter is an optional parameter that isn't
			//commented out in the given xml file we don't want to
			//write it as a comment when saving to an xml file again,
			//so we pass this information to the CCProcessor
			if( isParamOpt( type, keys[i] )){
			    StringVec optParams;
			    optParams.push_back( keys[i] );
			    sp->add( "Opt_Params_Set", optParams );
			}
		    }
		}
		return sp;
	    }
	    //if processor is installed and no parameters were passed (from the xml file)
	    //create a copy from the default parameters for this processor and return it
	    StringParameters *defaultSP= new StringParameters( *_mpSParameters[ type ] );

	    return defaultSP;
	}
	//if processor is not installed no parameters can be merged
	//just return the given parameters back
	return sp;
    }
   
    ProcessorParameter* CMProcessor::getParam( const string& type, const string& key ){
	if( isInstalled( type )){
	    ProcParamMap::const_iterator p = _mProcs[ type ]->procMap().find( key );
	    if( p != _mProcs[ type ]->procMap().end() ){
		ProcessorParameter* par = p->second;
		if( par ){
		    return par;
		}
	    }
	}
	return NULL;
    }

    const string CMProcessor::getParamD( const string& type, const string& key ){
	ProcessorParameter* par = getParam( type, key );
	if( par ){
	    return par->description();
	}
	return "Sorry, no description for this parameter";
    }

    const string CMProcessor::getParamT( const string& type, const string& key ){
	ProcessorParameter* par = getParam( type, key );
	if( par ){
	    return par->type();
	}
	return "Undefined!!";
    }

    int CMProcessor::getParamSetSize( const string& type, const string& key ){
	ProcessorParameter* par = getParam( type, key );
	if( par ){
	    return par->setSize();
	}
	return 0;
    }

    bool CMProcessor::isParamOpt( const string& type, const string& key ){
	ProcessorParameter* par = getParam( type, key );
	if( par ){
	    return par->isOptional();
	}
	//for consistency reasons return false if parameter isn't recognized
	return false;
    }
    
    bool CMProcessor::isParamVec( const std::string& type, const std::string& key ){
	ProcessorParameter* par = getParam( type, key );
	if( par ){
	    if( par->type() == "StringVec" || par->type() == "FloatVec" || par->type() == "IntVec" ){
		return true;
	    }
	    else{
		return false;
	    }
	}
	//for consistency reasons return true if parameter isn't recognized
	return true;
    }
    
    void CMProcessor::tokenize( const string str, StringVec& tokens, const string& delimiters ){
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while( string::npos != pos || string::npos != lastPos ){
	    // Found a token, add it to the vector.
	    tokens.push_back( str.substr( lastPos, pos - lastPos ));
	    // Skip delimiters.  Note the "not_of"
	    lastPos = str.find_first_not_of( delimiters, pos );
	    // Find next "non-delimiter"
	    pos = str.find_first_of( delimiters, lastPos );
	}
    }
    
} //end namespace marlin

