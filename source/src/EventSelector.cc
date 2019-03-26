#include "marlin/EventSelector.h"

#include <iostream>

// -- marlin headers
#include <marlin/Logging.h>

using namespace lcio ;
using namespace marlin ;


EventSelector aEventSelector ;


EventSelector::EventSelector() : Processor("EventSelector") {

  // modify processor description
  _description = "EventSelector returns true if given event was specified in EventList" ;

  IntVec evtsExample ;
  // evtsExample.push_back( 42 );
  // evtsExample.push_back( 1024 );


  registerProcessorParameter( "EventList" ,
			      "event list - pairs of Eventnumber RunNumber"  ,
			      _evtList ,
			      evtsExample ) ;
  _nEvt = -1;
  _nRun = -1;
}


void EventSelector::init() {

  // usually a good idea to
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;


  unsigned nEvts = _evtList.size() ;

  for( unsigned i=0 ; i < nEvts ; i+=2 ) {

    _evtSet.insert( std::make_pair( _evtList[i] , _evtList[ i+1 ] ) ) ;
  }
}

void EventSelector::processRunHeader( LCRunHeader* ) {

  _nRun++ ;
}


void EventSelector::modifyEvent( LCEvent *evt ) {
  processEvent( evt );
}

void EventSelector::processEvent( LCEvent * evt ) {

  // if no events specifiec - always return true
  if( _evtList.size() == 0 ) {
    setReturnValue( true ) ;
    return ;
  }

  SET::iterator it = _evtSet.find( std::make_pair( evt->getEventNumber() , evt->getRunNumber() ) ) ;

  bool isInList = it != _evtSet.end() ;


  //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

  log<DEBUG>() << "   processing event: " << evt->getEventNumber()
		       << "   in run:  " << evt->getRunNumber()
		       << " - in event list : " << isInList
		       << std::endl ;

  setReturnValue( isInList ) ;


  _nEvt ++ ;
}



void EventSelector::check( LCEvent *  ) {
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void EventSelector::end(){

//   std::cout << "EventSelector::end()  " << name()
// 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
// 	    << std::endl ;

}
