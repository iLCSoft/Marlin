#include "TestProcessorEventSeeder.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/Logging.h"
#include "marlin/ProcessorApi.h"

#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

using namespace lcio ;
using namespace marlin ;


TestProcessorEventSeeder aTestProcessorEventSeeder ;


TestProcessorEventSeeder::TestProcessorEventSeeder() : Processor("TestProcessorEventSeeder") {

  // modify processor description
  _description = "TestProcessorEventSeeder test the uniquie event seeding functionality of the ProcessorEventSeeder service in Marlin" ;

  _nRun = -1 ;
  _nEvt = -1 ;

}


void TestProcessorEventSeeder::init() {

  ProcessorApi::registerForRandomSeeds( this ) ;

  _nRun = 0 ;
  _nEvt = 0 ;
}


void TestProcessorEventSeeder::processRunHeader( EVENT::LCRunHeader* ) {

  ++_nRun ;

}

void TestProcessorEventSeeder::processEvent( EVENT::LCEvent * evt ) {


  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
		       << "   in run:  " << evt->getRunNumber()
		       << std::endl ;

  unsigned int seed = ProcessorApi::getRandomSeed( this, evt ) ;

  streamlog_out( DEBUG ) << "seed set to "
			    << seed
			    << " for event "
			    << evt->getEventNumber()
			    << std::endl;

  try{
    ProcessorApi::registerForRandomSeeds( this ) ;
  }
  catch( lcio::Exception ) {
    log<ERROR>() << name() << " failed to register processor to event seed generator (TEST is OK)" << std::endl ;
  }

  unsigned long long runnum_and_eventnum = evt->getRunNumber() ;
  runnum_and_eventnum = runnum_and_eventnum << 32  ;

  runnum_and_eventnum += evt->getEventNumber() ;

  std::map< unsigned long long, unsigned int>::iterator it ;

  it = _seeds.find(runnum_and_eventnum);

  if( it == _seeds.end() ){
    _seeds[runnum_and_eventnum] = seed ;
  }
  else {

    if( seed != _seeds[runnum_and_eventnum] ) {
      streamlog_out(ERROR) << " Seeds don't match for"
			   << " run " <<   evt->getRunNumber()
			   << " event " << evt->getEventNumber()
			   << " old seed = " << _seeds[runnum_and_eventnum]
			   << " new seed = " << seed
			   << std::endl ;
    }
    else{
      streamlog_out(DEBUG) << " Seeds match for"
			   << " run " <<   evt->getRunNumber()
			   << " event " << evt->getEventNumber()
			   << " old seed = " << _seeds[runnum_and_eventnum]
			   << " new seed = " << seed
			   << std::endl ;

    }

  }

//  if( _nEvt > 10 ) {

  //  streamlog_out(ERROR)  << " processEvent LCEvent::eventNumber not modified ! "  << std::endl;

  //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

  ++_nEvt ;

}



void TestProcessorEventSeeder::check( LCEvent * ) {
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void TestProcessorEventSeeder::end(){

  streamlog_out(MESSAGE4) << name()
			  << " random seeds received for " << _nEvt << " events in " << _nRun << " run "
			  << std::endl ;

}
