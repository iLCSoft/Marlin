#include "marlin/Statusmonitor.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"


using namespace lcio ;
using namespace marlin ;


Statusmonitor aStatusmonitor ;


Statusmonitor::Statusmonitor() : Processor("Statusmonitor") {
  
  // modify processor description
  _description = "Statusmonitor prints out information on running Marlin Job: Prints number of runs run and current number of the event. Counting is sequential and not the run or event ID." ;

  registerProcessorParameter("HowOften",
			     "Print the event number every N events",
			     _howOften, 
			     int(10000) ) ;

  
}


void Statusmonitor::init() { 
  streamlog_out(DEBUG) << "INIT CALLED  " << std::endl ;

  // usually a good idea to
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;

}

void Statusmonitor::processRunHeader( LCRunHeader* ) { 
  _nRun++ ;
} 

void Statusmonitor::processEvent( LCEvent *  ) { 

  if (_nEvt % _howOften == 0) {
    streamlog_out(MESSAGE) 
      << " ===== Run  : " << std::setw(7) << _nRun
      << "  Event: " << std::setw(7) << _nEvt << endl;
  }
  _nEvt ++ ;

}



void Statusmonitor::check( LCEvent * ) { 
}


void Statusmonitor::end(){ 
  
  streamlog_out(MESSAGE) << "Statusmonitor::end()  " << name()  << " processed " << _nEvt << " events in " << _nRun << " runs "	    << std::endl ;

}

