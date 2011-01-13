#include "marlin/CCProcessor.h"
#include "marlin/CMProcessor.h"
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
    _error[0] = _error[1] = _error[2] = false;
 
    //FIXME: this has to be global
    //if( _error_desc.size() == 0 ){
    _error_desc.push_back( "Processor has no Parameters" );
    _error_desc.push_back( "Processor not available!" );
    _error_desc.push_back( "Some Collections have Errors" );
    //}

    setError( NO_PARAMETERS );
    
    //setup Marlin Processor
    setMarlinProc();
 
    //merge the Processor Parameters with the default ones for this type of processor
    _param=CMProcessor::instance()->mergeParams( type, p );

    //adds the input/output collections from the string parameters
    addColsFromParam( _param );

    //clears all collections related parameters and the processor type
    clearParameters();
  }

  //copy constructor
  CCProcessor::CCProcessor( CCProcessor &p) : _param(0), _proc(p._proc) {
      _status=p._status;
      _name=p._name;
      _type=p._type;
      _types=p._types;
      _optParams=p._optParams;
      _conditions=p._conditions;
      
      for( int i=0; i<MAX_ERRORS; i++ ){
	_error[i] = false;
        _error_desc.push_back( p._error_desc[i] );
      }
      
      for( int i=0; i<MAX_ERRORS; i++ ){
	if(p._error[i]){
	    setError(i);
	}
      }

      if( p._param != NULL ){
	p.writeColsToParam();
	_param=new StringParameters( *p._param );
	addColsFromParam( _param );
	p.clearParameters();
        clearParameters();
      }
  }
 
  CCProcessor::~CCProcessor(){
      if( _param != NULL){
	  delete _param;
	  _param=NULL;
      }
      for( ssColVecMap::const_iterator q=_cols.begin(); q!=_cols.end(); q++ ){
	 if( q->first != UNAVAILABLE && q->first != DUPLICATE ){
	    for( sColVecMap::const_iterator r=q->second.begin(); r!=q->second.end(); r++ ){
	      for( unsigned int i=0; i<r->second.size(); i++ ){
		delete r->second[i];
	      }
	    }
	 }
      }
  }

  void CCProcessor::addColsFromParam( StringParameters* p ){
    if( p == NULL ){
	setError( NO_PARAMETERS );
	return;
    }
    
    clearError( NO_PARAMETERS );
    
    //check if the Marlin Processor is installed
    if( isInstalled() ){
	
	StringVec keys;
	_param->getStringKeys( keys );
	
	for( unsigned int i=0; i<keys.size(); i++ ){
	    StringVec values;
	    _param->getStringVals( keys[i], values );
	    
	    if( _proc->isInputCollectionName( keys[i] )){
		for( unsigned int j=0; j<values.size(); j++ ){
		    addCol( INPUT, keys[i], _proc->getLCIOInType( keys[i] ), values[j] );
		}
	    }
	    else if( _proc->isOutputCollectionName( keys[i] )){
		for( unsigned int j=0; j<values.size(); j++ ){
		    addCol( OUTPUT, keys[i], _proc->getLCIOOutType( keys[i] ), values[j] );
		}
	    }
	}
    }
    else{
	StringVec InCols, OutCols;
	string name, type;

	_param->getStringVals( "_marlin.lcioInType", InCols );
	_param->getStringVals( "_marlin.lcioOutType", OutCols );

	unsigned int index = 0;
	while( index < InCols.size() ){
	  name = InCols[ index++ ];
	  type = InCols[ index++ ];

	  StringVec values;
	  _param->getStringVals(name, values); // get corresponding collections

	  for (unsigned int i=0; i<values.size(); i++){
	    addCol( INPUT, name, type, values[i] );
	  }
	}

	index = 0;
        while( index < OutCols.size() ){
	  name = OutCols[index++];
	  type = OutCols[index++];

	  StringVec values;
	  _param->getStringVals(name, values); // get corresponding collections

	  for( unsigned int i=0; i<values.size(); i++ ){
	    addCol( OUTPUT, name, type, values[i] );
	  }
      }
    }
  }

  void CCProcessor::setConditions( const string& conditions ){

      if( conditions == "true" ){
	  return;
      }

      int brackets=0;
      unsigned p = 0;
      unsigned q = 1;
    
      while( q < conditions.size() ){
	  if( conditions[ p ] == '(' ){
	      brackets++;
	      while( brackets ){
		  if( conditions[ q ] == '(' ){
		      brackets++;
		  }
		  if( conditions[ q ] == ')' ){
		      brackets--;
		  }
		  q++;
	      }
	      //skip the white space after the brackets
	      q++;
	  }
	  if( conditions[ q ] == '&' ){
	     if( conditions[ p ] == '(' ){
		//skip brackets
		_conditions.insert( conditions.substr( p+1, (q-p)-3 )); 
	     }
	     else{
		_conditions.insert( conditions.substr( p, q-1 ));
	     }
	     p = q+3;
	     q = p;
	  }
	  q++;
      }
      //make the last split
      if( conditions[ p ]== '(' ){
	//skip brackets
	_conditions.insert( conditions.substr( p+1, (q-p)-4 ));
      }
      else{
	_conditions.insert( conditions.substr( p, q ));
      }
  }
  
  bool CCProcessor::hasCondition( const string& condition ){
      if( _conditions.find( condition) != _conditions.end() ){
	  return true;
      }
      return false;
  }
      
  void CCProcessor::setMarlinProc(){
      if( !CMProcessor::instance()->isInstalled( _type )){
	setError( NOT_INSTALLED );
	return;
      }
      _proc = CMProcessor::instance()->getProc( _type );
      clearError( NOT_INSTALLED );
  }
  
  void CCProcessor::clearParameters(){
      // skip if it does not contain parameters or is not installed
      if( hasParameters() ){
	  for( sColVecMap::const_iterator p=_cols[INPUT].begin(); p!=_cols[INPUT].end(); p++ ){
	      _param->erase( p->first );
	  }

	  for( sColVecMap::const_iterator p=_cols[OUTPUT].begin(); p!=_cols[OUTPUT].end(); p++ ){
	      _param->erase( p->first );
	  }

	  //erase the processor type from the string parameters
	  _param->erase( "ProcessorType" );

	  //erase the "name, type" list from processor's parameters
	  _param->erase( "_marlin.lcioInType" );
	  _param->erase( "_marlin.lcioOutType" );
	  _param->erase( INPUT );
	  _param->erase( OUTPUT );

	  //if there are any optional parameters active we save the keys into _optParams
	  if( _param->isParameterSet( "Opt_Params_Set" )){
	      StringVec values;
	      _param->getStringVals( "Opt_Params_Set", values );
	      for( unsigned int i=0; i<values.size(); i++ ){
		  _optParams.insert( values[i] );
	      }
	  }

	  //erase the list of optional parameters
	  _param->erase( "Opt_Params_Set" );
      }
  }
  
  // write IO collections back to marlin processor's parameters
  void CCProcessor::writeColsToParam(){
      StringVec value;

      // skip if it does not contain parameters or is not installed
      if( hasParameters() && isInstalled() ){
	
	  // loop over all collections of this processor
	  // write every collection back to string parameters

	  for( sColVecMap::const_iterator p=_cols[INPUT].begin(); p!=_cols[INPUT].end(); p++ ){
	      
	      value.clear();
	      for(unsigned int i=0; i< p->second.size(); i++){
		  value.push_back( p->second[i]->getValue() );
	      }
	      _param->add( p->first, value );
	  }

	  for( sColVecMap::const_iterator p=_cols[OUTPUT].begin(); p!=_cols[OUTPUT].end(); p++ ){

	      value.clear();
	      for(unsigned int i=0; i< p->second.size(); i++){
		  value.push_back( p->second[i]->getValue() );
	      }
	      _param->add( p->first, value );
	  }
      }
  }
  
  void CCProcessor::changeStatus(){
    _status = !_status;
    clearError( COL_ERRORS );
  }

/////////////////////////////////////////////////////////////
// COLLECTION METHODS
/////////////////////////////////////////////////////////////
  
  void CCProcessor::addCol( const string& iotype, const string& name, const string& type, const string& value ){
    /*  
    ColVec v = getCols(iotype, name);

    //test if collection already exists to avoid duplicate collections
    for( unsigned int i=0; i<v.size(); i++){
	if( v[i]->getName() == name && v[i]->getType() == type && v[i]->getValue() == value ){
	    //abort if collection already exists
	    return;
	}
    }
    */
    CCCollection* newCol = new CCCollection(value, type, name, this);

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
  
  void CCProcessor::addDCol( CCCollection* c ){
    //add collection to the map
    _cols[ DUPLICATE ][ c->getType() ].push_back( c );

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
	if( type_name=="ALL_COLLECTIONS" || type_name == p->first ){
	  for( unsigned int i=0; i<p->second.size(); i++ ){
	    newVec.push_back( p->second[i] );
	  }
	}
      }
    }

    return newVec;
  }

  sSet& CCProcessor::getColTypeNames( const string& iotype ){
      static sSet type_names;
      
      type_names.clear();
      
      for( sColVecMap::const_iterator p=_cols[ iotype ].begin(); p!=_cols[ iotype ].end(); p++ ){
	  type_names.insert( p->first );
      }
      return type_names;
  }

  bool CCProcessor::isErrorCol( const string& type, const string& value ){
      ColVec v = getCols( UNAVAILABLE );
      v.insert( v.end(), getCols( DUPLICATE ).begin(), getCols( DUPLICATE ).end() );
      
      for( unsigned int i=0; i<v.size(); i++ ){
	  if(( v[i]->getValue() == value ) && ( v[i]->getType() == type )){
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

      if( error == COL_ERRORS && (_cols.find(UNAVAILABLE) != _cols.end())){
	_cols[ UNAVAILABLE ].clear();
      }
      if( error == COL_ERRORS && (_cols.find(DUPLICATE) != _cols.end())){
	_cols[ DUPLICATE ].clear();
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
 
  bool CCProcessor::isParamOptional( const string& key ){
      if( CMProcessor::instance()->isParamOpt( _type, key )){
	  if( _optParams.find( key ) == _optParams.end() ){
	      return true;
	  }
      }
      return false;
  }

  void CCProcessor::setOptionalParam( const string& key, bool optional ){
      if( optional ){
	  _optParams.erase( key );
      }
      else{
	  _optParams.insert( key );
      }
  }

  void CCProcessor::writeToXML( ofstream& stream ){

    stream << " <processor name=\"" <<  _name  << "\" type=\"" <<  _type << "\">\n";
    stream << " <!--" << getDescription() << "-->\n";

    StringVec value;
    //write I/O collections
    for( ssColVecMap::const_iterator p=_cols.begin(); p!=_cols.end(); p++ ){
     if( p->first != UNAVAILABLE && p->first != DUPLICATE ){
	for( sColVecMap::const_iterator q=p->second.begin(); q!=p->second.end(); q++ ){
	  if( q->second.size() != 0){
	    if( isInstalled() ){
		stream << "  <!--" << CMProcessor::instance()->getParamD( _type, q->first ) << "-->" << std::endl ;
	    }
	    stream << "  <parameter name=\"" << q->first << "\" type=\"" << (q->second.size() == 1 ? "string" : "StringVec") << "\" ";
	    stream << p->first << "=\"" << q->second[0]->getType() << "\"> ";
	    value.clear();
	    for( unsigned int i=0; i<q->second.size(); i++ ){
		stream << q->second[i]->getValue() << " ";
	    }
	    stream << "</parameter>\n";
	  }
	}
      }
    }
    
    StringVec keys;
    if( hasParameters() ){
	_param->getStringKeys( keys );
    }
    
    //parameters
    for( unsigned int i=0; i<keys.size(); i++ ){

	ProcessorParameter* p = CMProcessor::instance()->getParam( _type, keys[i] );
   
	//if the parameter is recognized by the marlin processor
        if( p != NULL ){
	    
	    stream << "  <!--" << CMProcessor::instance()->getParamD( _type, keys[i] ) << "-->" << std::endl ;

	    StringVec values;
	    _param->getStringVals( keys[i], values );
	    
	    int ssize=CMProcessor::instance()->getParamSetSize( _type, keys[i] );
	    if( ssize > 1 ){
		for( int j=0; j<((int)values.size())/ssize; j++ ){
		    
		    stream << "  <" << (isParamOptional( keys[i] ) ? "!--" : "") << "parameter name=\"" << keys[i] << "\" ";
		    stream << "type=\"" << p->type() << "\"> ";

		    for( int k=0; k<ssize; k++ ){
			stream << values[ (j*ssize)+k ] << " ";
		    }
		    
		    stream << "</parameter" << (isParamOptional( keys[i] ) ? "--" : "")  << ">\n";
		}
	    }
	    else{
		stream << "  <" << (isParamOptional( keys[i] ) ? "!--" : "") << "parameter name=\"" << keys[i] << "\" ";
		stream << "type=\"" << p->type() << "\"";
		
		StringVec values;
		_param->getStringVals( keys[i], values );
		stream << (values.size() == 1 ? " value=\"" : "> ");
		for( unsigned int j=0; j<values.size(); j++ ){
		    stream << values[j] << (values.size() > 1 ? " " : "");
		}
		stream << (values.size() == 1 ? "\"/" : "</parameter") << (isParamOptional( keys[i] ) ? "--" : "")  << ">\n";
	    }
	}
	//else it's a parameter from an uninstalled processor or an extra parameter from an installed processor
	else{
	    if( isInstalled() ){
		stream << "  <!-- Sorry, this parameter isn't a default parameter for this processor: description and type lost!!  -->" << std::endl ;
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
  }

} // end namespace marlin
