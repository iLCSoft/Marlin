#ifndef MemoryMonitor_h
#define MemoryMonitor_h

#include "marlin/Processor.h"
#include "lcio.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

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
	
  int _howOften=1;
	
  // Run and event counters
  int _eventNumber=-1;
  int _runNumber=-1;
  
  int parseLine(char* line){
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i-3] = '\0';
    i = atoi(line);
    return i;
  };
  
  //Used only on linux machines
  int getValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];
    
    
    while (fgets(line, 128, file) != NULL){
      if (strncmp(line, "VmRSS:", 6) == 0){
        result = parseLine(line);
        break;
      }
    }
    fclose(file);
    return result;
  };

	
} ;

#endif



