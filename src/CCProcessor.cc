#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"
#include "marlin/ProcessorMgr.h"

using namespace std;

namespace marlin{

  //constructor
  CCProcessor::CCProcessor( bool status, const string& name, const string& type, StringParameters* p ):
    _status(status),
    _name(name),
    _type(type),
    _param(0),
    _proc(0)
  {
    _error[0] = false;
    _error[1] = false;
    _error[2] = false;
 
    //FIXME: this has to be global
    //if( _error_desc.size() == 0 ){
    _error_desc.push_back( "Processor has no Parameters" );
    _error_desc.push_back( "Processor is not build in this Marlin binary" );
    _error_desc.push_back( "Some Collections are not available" );
    //}

    setError( NO_PARAMETERS );
    setError( NOT_INSTALLED );
    
    //create a new Marlin Processor
    createMarlinProc();
 
    //set the Processor Parameters
    setParameters( p );
  }

  //copy constructor
  CCProcessor::CCProcessor(CCProcessor const &p){
      _status=p._status;
      _name=p._name;
      _type=p._type;
      
      _error[0] = false;
      _error[1] = false;
      _error[2] = false;
      
      _error_desc.push_back( "Processor has no Parameters" );
      _error_desc.push_back( "Processor is not build in this Marlin binary" );
      _error_desc.push_back( "Some Collections are not available" );

      for( int i=0; i<MAX_ERRORS; i++ ){
	if(p._error[i]){
	    setError(i);
	}
      }
      
      //create a new Marlin Processor
      createMarlinProc();

      _param=new StringParameters();
      
      StringVec keys;
      p._param->getStringKeys(keys);
      
      for(unsigned int i=0; i<keys.size(); i++){
	StringVec values;
	p._param->getStringVals(keys[i],values);
	_param->add(keys[i], values);
      }

      _types=p._types;

      for( ssColVecMap::const_iterator q=p._cols.begin(); q!=p._cols.end(); q++ ){
        for( sColVecMap::const_iterator r=(*q).second.begin(); r!=(*q).second.end(); r++ ){
	  for( unsigned int i=0; i<(*r).second.size(); i++ ){
	    _cols[(*q).first][(*r).first].push_back( new CCCollection( (*(*r).second[i]) ));
	  }
	}
      }
  }
  
  CCProcessor::~CCProcessor(){
      if( _param != NULL ){
	  delete _param;
      }
      if( _proc != NULL ){
	  delete _proc;
      }
  }
  
  void CCProcessor::changeStatus(){
    _status = !_status;
    clearError( COL_ERRORS );
  }

  void CCProcessor::setName( const string& name ){
    _name = name;
    if(isInstalled()){
	_proc->setName(name);
    }
  }
  
  ProcessorParameter* CCProcessor::getProcParam( const string& key ){
      if( isInstalled() ){
        ProcParamMap::const_iterator p= _proc->procMap().find(key);
        if( p != _proc->procMap().end() ){
            ProcessorParameter* par = p->second;
            if( par ){
                return par;
            }
            else{
                return NULL;
            }
        }
      }
      return NULL;
  }
  
  const string CCProcessor::getParamDesc( const string& key ){
      ProcessorParameter* par = getProcParam(key);
      if( par ){
	return par->description();
      }
      else{
	return "Sorry, no description for this parameter";
      }
  }
  
  const string CCProcessor::getParamType( const string& key ){
      ProcessorParameter* par = getProcParam(key);
      if( par ){
	return par->type();
      }
      else{
	return "Undefined!!";
      }
  }
 
  void CCProcessor::setParameters( StringParameters* p ){
   
    //check if the Marlin Processor is installed
    if(isInstalled()){

	// set the parameters read from the xml file
	if( p != NULL ){
    	    _param=p;
	    _proc->setProcessorParameters( _param );
	    _proc->updateParameters();
	    
	    StringVec InCols, OutCols, values;
	    string name, type;

	    _param->getStringVals( INPUT, InCols );
	    _param->getStringVals( OUTPUT, OutCols );
																			     
	    unsigned int index = 0;
	    while( index < InCols.size() ){
	      name = InCols[ index++ ];
	      type = InCols[ index++ ];

	      values.clear();
	      _param->getStringVals(name, values); // get corresponding collections

	      //erase the corresponding collection from the processor's parameters
	      _param->erase( name );
	      
	      for (unsigned int i=0; i<values.size(); i++){
		addCol( INPUT, name, type, values[i] );
	      }
	    }
	    index = 0;
	    while( index < OutCols.size() ){
	      name = OutCols[index++];
	      type = OutCols[index++];

	      values.clear();
	      _param->getStringVals(name, values); // get corresponding collections

	      //erase the corresponding collection from the processor's parameters
	      _param->erase( name );
	      
	      for( unsigned int i=0; i<values.size(); i++ ){
		addCol( OUTPUT, name, type, values[i] );
	      }
	    }
	    //erase the processor type from the string parameters
	    _param->erase( "ProcessorType" );

	    //erase the "name, type" list from processor's parameters
	    _param->erase( INPUT );
	    _param->erase( OUTPUT );
	}
	//set the default parameters from the processor
	else{
	    _param = new StringParameters();

	    //initialize marlin processor parameters
	    _proc->setProcessorParameters( _param );
	    _proc->updateParameters();
	    
	    StringVec keys;
	    _param->getStringKeys(keys);
	    
	    for(unsigned int i=0; i<keys.size(); i++){
		ProcParamMap::const_iterator p= _proc->procMap().find(keys[i]);
		ProcessorParameter* par = p->second;

		//erase the collection from the parameters
		_param->erase(keys[i]);

		if(_proc->isInputCollectionName(keys[i])){
		    if( par->type() == "StringVec" ){
			StringVec v;
			tokenize(par->defaultValue(), v);
			string type =  _proc->getLCIOInType(keys[i]);
			for( unsigned int j=0; j<v.size(); j++ ){
			    addCol( INPUT, keys[i], type, v[j] );
			}
		    }
		    else{
			addCol( INPUT, keys[i], _proc->getLCIOInType(keys[i]), par->defaultValue() );
		    }
		}
		else if(_proc->isOutputCollectionName(keys[i])){
		    addCol( OUTPUT, keys[i], _proc->getLCIOOutType(keys[i]), par->defaultValue() );
		}
		else{
		    StringVec defValue;
		    defValue.push_back(par->defaultValue());
		    _param->add(keys[i], defValue );
		}
	    }
	}
	clearError( NO_PARAMETERS );
    }
    //processor is not installed
    else{
	if(p!= NULL ){
	    _param=p;
	    _param->erase("ProcessorType");
	    clearError( NO_PARAMETERS );
	}
    }
  }

  void CCProcessor::createMarlinProc(){
    //check if marlin has an installed processor of this type
    Processor* pp = ProcessorMgr::instance()->getProcessor( _type );
    //checks if marlin has this processor type installed
    if( pp ){
      //create a new marlin processor of this type
      _proc = pp->newProcessor();
      //set the processor name
      _proc->setName( _name );
      //clears error
      clearError( NOT_INSTALLED );
    }
    //this processor type is not installed
    else{
      setError( NOT_INSTALLED );
    }
  }
  
  void CCProcessor::clearParameters(){
      // skip if it does not contain parameters or is not installed
      if( hasParameters() && isInstalled() ){
	  for( sColVecMap::const_iterator p=_cols[INPUT].begin(); p!=_cols[INPUT].end(); p++ ){
	      _param->erase((*p).first );
	  }

	  for( sColVecMap::const_iterator p=_cols[OUTPUT].begin(); p!=_cols[OUTPUT].end(); p++ ){
	      _param->erase((*p).first );
	  }
      }     
  }
  
  // write IO collections back to marlin processor's parameters
  void CCProcessor::updateMarlinProcessor(){
      StringVec value;

      // skip if it does not contain parameters or is not installed
      if( hasParameters() && isInstalled() ){
	
	  // loop over all collections of this processor
	  // write every collection back to string parameters

	  for( sColVecMap::const_iterator p=_cols[INPUT].begin(); p!=_cols[INPUT].end(); p++ ){
	      
	      value.clear();
	      for(unsigned int i=0; i<(*p).second.size(); i++){
		  value.push_back( (*p).second[i]->getValue() );
	      }
	      _param->add((*p).first, value );
	  }

	  for( sColVecMap::const_iterator p=_cols[OUTPUT].begin(); p!=_cols[OUTPUT].end(); p++ ){

	      value.clear();
	      for(unsigned int i=0; i<(*p).second.size(); i++){
		  value.push_back( (*p).second[i]->getValue() );
	      }
	      _param->add((*p).first, value );
	  }
																		     
	  //update Marlin Processor's parameters
	  _proc->updateParameters();
      }
  }

/////////////////////////////////////////////////////////////
// COLLECTION METHODS
/////////////////////////////////////////////////////////////
  
  void CCProcessor::addCol( const string& iotype, const string& name, const string& type, const string& value ){
    
    CCCollection* newCol = new CCCollection();

    newCol->setName( name );
    newCol->setType( type );
    newCol->setValue( value );
    newCol->setSrcProc( this );

    //add collection to the map
    _cols[iotype][name].push_back( newCol );
    
    //add collection type to the map
    _types[iotype][name]=type;
  }

  void CCProcessor::addUCol( CCCollection* c ){
    //add collection to the map
    _cols[ UNAVAILABLE ][ c->getType() ].push_back( c );

    //set the error
    setError( COL_ERRORS );
  }
  
  void CCProcessor::remCol( const string& iotype, const string& name, unsigned int index ){
    if( index >=0 && index < _cols[iotype][name].size() ){
      popCol( _cols[iotype][name], _cols[iotype][name][index] );
    }
  }

  //pop a collection out of the given vector
  CCCollection* CCProcessor::popCol( ColVec& v, CCCollection* c ){
    //test if the vector is empty
    if( v.size() == 0 ){
      return c;
    }
                                                                                                                                                             
    ColVec newVec;
    for( unsigned int i=0; i<v.size(); i++ ){
      if( v[i] != c ){
	newVec.push_back( v[i] );
      }
    }
                                                                                                                                                             
    v.assign( newVec.begin(), newVec.end() );
                                                                                                                                                             
    return c;
  }

  ColVec& CCProcessor::getCols( const string& iotype, const string& type_name ){
    static ColVec newVec;
   
    newVec.clear();
    
    if( _cols.find( iotype ) != _cols.end() ){
      for( sColVecMap::const_iterator p=_cols[iotype].begin(); p!=_cols[iotype].end(); p++ ){
	if( type_name=="ALL_COLLECTIONS" || type_name==(*p).first ){
	  for( unsigned int i=0; i<(*p).second.size(); i++ ){
	    newVec.push_back( (*p).second[i] );
	  }
	}
      }
    }

    return newVec;
  }

  sSet& CCProcessor::getUColTypes(){
      static sSet types;
      
      types.clear();
      
      for( sColVecMap::const_iterator p=_cols[ UNAVAILABLE ].begin(); p!=_cols[ UNAVAILABLE ].end(); p++ ){
	  types.insert( (*p).first );
      }
      return types;
  }

  bool CCProcessor::isErrorCol( const string& value ){
      ColVec v = getCols( UNAVAILABLE );
      
      for( unsigned int i=0; i<v.size(); i++ ){
	  if( v[i]->getValue() == value ){
	      return true;
	  }
      }
      
      return false;
  }

  
/////////////////////////////////////////////////////////////
// ERROR METHODS
/////////////////////////////////////////////////////////////

  void CCProcessor::setError( int error ){
    if( _error[ error ] == false ){
      _error[ error ] = true;
      _errors.push_back( _error_desc[ error ] );
    }
  }

  void CCProcessor::clearError( int error ){
    if( _error[ error ] == true ){
      _error[ error ] = false;
	
      StringVec tmp;
      for( unsigned int i=0; i<_errors.size(); i++ ){
	if( _errors[i] != _error_desc[ error ] ){
	  tmp.push_back( _errors[i] );
	}
      }
      _errors.clear();
      _errors=tmp;

      if( error == COL_ERRORS ){
	_cols[ UNAVAILABLE ].clear();
      }
    }
  }

  bool CCProcessor::hasErrors(){
    for( int i=0; i<MAX_ERRORS; i++ ){
      if( _error[i] == true ){
	return true;
      }
    }
    return false;
  }

  bool CCProcessor::hasParameters(){
    return !_error[0];
  }

  bool CCProcessor::isInstalled(){
    return !_error[1];
  }

  bool CCProcessor::hasUnavailableCols(){
    return _error[2];
  }

  bool CCProcessor::isActive(){
    if( _status == true ){ return true; }
    return false;
  }

  void CCProcessor::tokenize( const string str, StringVec& tokens, const string& delimiters ){
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
                                                                                                                                                             
    while (string::npos != pos || string::npos != lastPos){
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
  }

/*
  void CCProcessor::writeToXML( ofstream& stream ){
    if( isInstalled() ){
        updateMarlinProcessor();
        _proc->printDescriptionXML( stream );
        clearParameters();
    }
  }
*/

  void CCProcessor::writeToXML( ofstream& stream ){
    stream << " <processor name=\"" <<  _name  << "\" type=\"" <<  _type << "\">\n";
    if( isInstalled() ){
        updateMarlinProcessor();
	stream << " <!--" << _proc->description() << "-->\n";
    }
    else{
	stream << " <!-- This processor is NOT Installed in your Marlin binary so ALL parameter descriptions are lost!!!-->\n";
    }
    
    
    StringVec keys;
    if( hasParameters() ){
	_param->getStringKeys( keys );
    }
    
    //parameters
    for( unsigned int i=0; i<keys.size(); i++ ){

	ProcessorParameter* p = getProcParam( keys[i] );
   
	//if the parameter is recognized by the marlin processor
        if( p != NULL ){
	    
	    stream << "  <!--" << getParamDesc( keys[i] ) << "-->" << std::endl ;
	    stream << "  <" << (p->isOptional() ? "!--" : "") << "parameter name=\"" << p->name() << "\" type=\"" << p->type() ;
	    
	    if( _proc->isInputCollectionName( keys[i] )){
		//stream << "\" lcioInType=\"" << _types[ INPUT ][ keys[i] ];
		stream << "\" lcioInType=\"" << _proc->getLCIOInType(keys[i]);
	    }

	    if( _proc->isOutputCollectionName( keys[i] )){
		//stream << "\" lcioOutType=\"" << _types[ OUTPUT ][ keys[i] ];
		stream << "\" lcioOutType=\"" << _proc->getLCIOOutType(keys[i]);
	    }
																		 
	    stream << "\">" << p->defaultValue() << " </parameter"<< (p->isOptional() ? "--" : "")  << ">\n";
	}
	//else it's a parameter from an uninstalled processor or an extra parameter from an installed processor
	else{
	    if(isInstalled()){
		stream << "  <!-- Sorry, this parameter isn't a default parameter for this processor so it's description was lost!  -->" << std::endl ;
	    }
	    stream << "  <parameter name=\"" << keys[i] << "\"";

	    StringVec values;
	    //get the values for the given key
	    _param->getStringVals( keys[i], values );

	    stream << ( values.size() == 1 ? " value=\"" : ">" );
																			 
	    for( unsigned int j=0; j<values.size(); j++ ){
		stream << ( values.size() == 1 ? "" : " ") << values[j];
	    }
	    stream << ( values.size() == 1 ? "\"/>\n" : " </parameter>\n" );
	}
    }
    stream << "</processor>\n\n";
    if( isInstalled() ){
        clearParameters();
    }
  }

} // end namespace marlin
