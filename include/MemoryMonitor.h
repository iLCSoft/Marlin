#ifndef MemoryMonitor_h
#define MemoryMonitor_h

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>

using namespace lcio ;
using namespace marlin ;

class MemoryMonitor : public Processor {
		
public:
	
	virtual Processor*  newProcessor() { return new MemoryMonitor ; }
	
	MemoryMonitor() ;
	
	// Initialisation - run at the beginning to start histograms, etc.
	virtual void init() ;
	
	// Called at the beginning of every run
	virtual void processRunHeader( LCRunHeader* run ) ;
	
	// Run over each event - the main algorithm
	virtual void processEvent( LCEvent * evt ) ;
	
  // Run at the end of each event
	virtual void check( LCEvent * evt ) ;
	
	// Called at the very end for cleanup, histogram saving, etc.
	virtual void end() ;
	
	
protected:
	
  int _howOften;
	
	// Run and event counters
	int _eventNumber ;
	int _runNumber ;

	
} ;

#endif



