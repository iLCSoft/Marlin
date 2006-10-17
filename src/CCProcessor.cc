#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"
#include "marlin/ProcessorMgr.h"

using namespace std;

namespace marlin{

  CCProcessor::CCProcessor():
    _status(INACTIVE),
    _name("Unnamed!!"),
    _type("Undefined!!"),
    _param(0),
    _proc(0)
  {
    init();
  }

  CCProcessor::CCProcessor( bool status, const string& name, const string& type, StringParameters* p ):
    _status(status),
    _name(name),
    _type(type),
    _param(0),
    _proc(0)
  {
    init();
    setParameters( p );
    createMarlinProc();
  }

  void CCProcessor::init(){

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
  }

  void CCProcessor::changeStatus(){
    _status = !_status;
    clearError( COL_ERRORS );
  }

  void CCProcessor::setName( const string& name ){
    _name = name;
  }

  void CCProcessor::setType( const string& type ){
    _type = type;
  }

  void CCProcessor::setParameters( StringParameters* p ){
    if( p != NULL ){
      _param = p;
      clearError( NO_PARAMETERS );
      updateCollections();
    }
  }

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
	_errorCols.clear();
      }
    }
  }

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
    _errorCols.push_back( c );
    setError( COL_ERRORS );
  }

  ColVec& CCProcessor::getCols( const string& iotype, const string& name ){
    static ColVec newVec;
   
    newVec.clear();
    sColVecMap::const_iterator p;
    
    if( _cols.find( iotype ) != _cols.end() ){
      for( p=_cols[iotype].begin(); p!=_cols[iotype].end(); p++ ){
	if( name=="ALL_COLLECTIONS" || name==(*p).first ){
	  for(unsigned int i=0; i<(*p).second.size(); i++){
	    newVec.push_back( (*p).second[i] );
	  }
	}
      }
    }

    return newVec;
  }


  void CCProcessor::updateCollections(){
    StringVec InCols, OutCols, values;
    string name, type;

    //erase the processor type from the string parameters
    //_param->erase( "ProcessorType" );
    
    _param->getStringVals( INPUT, InCols );
    _param->getStringVals( OUTPUT, OutCols );
    
    //erase the "name, type" list from processor's parameters
    _param->erase( INPUT );
    _param->erase( OUTPUT );
	
    unsigned int index = 0;
    while( index < InCols.size() ){
      name = InCols[ index++ ];
      type = InCols[ index++ ];

      values.clear();
      _param->getStringVals(name, values); // get corresponding collection names
	
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
      _param->getStringVals(name, values); // get corresponding collection names
	
      //erase the corresponding collection from the processor's parameters
      _param->erase( name );
        
      for( unsigned int i=0; i<values.size(); i++ ){
	addCol( OUTPUT, name, type, values[i] );
      }
    }
  }
  /*
  // write IO collections back to marlin processor's parameters
  void CCProcessor::updateMarlinProcessors(){
  StringVec lcioInType, lcioOutType, value, ptype;
  string name, type;

  // skip if it does not contain parameters or is not installed
  if( hasParameters() && isInstalled() ){

  //ptype.push_back( _type );
  //_param->add( "ProcessorType", ptype );
	
  // loop over all collections of this processor
  // write every collection back to string parameters
  for( unsigned int i=0; i<getCols(INPUT).size(); i++ ){
  name = getCols(INPUT)[i]->getPName();
  type = getCols(INPUT)[i]->getType();
  lcioInType.push_back( name );
  lcioInType.push_back( type );
  value.clear();
  value.push_back( getCols(INPUT)[i]->getValue() );
  _param->add( name, value );
  }
  _param->add( INPUT , lcioInType );
																		     
  for( unsigned int i=0; i<_outCols.size(); i++ ){
  name = _outCols[i]->getPName();
  type = _outCols[i]->getType();
  lcioOutType.push_back( name );
  lcioOutType.push_back( type );
  value.clear();
  value.push_back(_outCols[i]->getValue());
  _param->add( name, value );
  }
  _param->add( OUTPUT , lcioOutType );
																		     
  //update Marlin Processor's parameters
  _proc->setProcessorParameters( _param );
  }
  }
  */

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

  void CCProcessor::createMarlinProc(){
    //check if marlin has an installed processor of this type
    Processor* pp = ProcessorMgr::instance()->getProcessor( _type );
    //checks if marlin has this processor type installed
    if( pp ){
      //create a new marlin processor of this type
      //ProcessorMgr::instance()->addActiveProcessor(_type, _name, _param);
      //ProcessorMgr::instance()->init();
      //_proc = ProcessorMgr::instance()->getActiveProcessor(_name);
      /*
	StringParameters* p = _proc->parameters();
        cout << *p << endl;
      */
      //_proc->printDescriptionXML();
      //_proc->printParameters();

      _proc = pp->newProcessor();
      //_proc->baseInit();
      if( hasParameters() ){
	//set processor's parameters to the same ones as this processor
	_proc->setProcessorParameters( _param );
	/*
	  StringParameters* p = _proc->parameters();
	  cout << *p << endl;
	  _proc->printDescriptionXML();
	  _proc->printParameters();
	*/
      }
	
      clearError( NOT_INSTALLED );
    }
    //this processor type is not installed
    else{
      setError( NOT_INSTALLED );
    }
  }

} // end namespace marlin
