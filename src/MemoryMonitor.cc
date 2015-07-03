/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
#include "MemoryMonitor.h"

#include "sys/types.h"

#if APPLE
#include "sys/sysctl.h"
#include <mach/mach.h>

#else 
#include "sys/sysinfo.h"
#endif

using namespace lcio ;
using namespace marlin ;
using namespace std ;

MemoryMonitor aMemoryMonitor;


MemoryMonitor::MemoryMonitor() : Processor("MemoryMonitor") {
	
	// modify processor description
	_description = "Simple processor to print out the memory consumption at defined intervals" ;

  registerProcessorParameter("howOften",
                             "Print Event Number Every N Events",
                             _howOften, 
                             int(1) ) ;
}


void MemoryMonitor::init() {
	
	// Print the initial parameters
	printParameters() ;
	
	// Reset counters
	_runNumber = 0 ;
	_eventNumber = 0 ;

	
}


void MemoryMonitor::processRunHeader( LCRunHeader* run) {
	_runNumber++ ;
}

void MemoryMonitor::processEvent( LCEvent* evt ) {
	

  if (_eventNumber % _howOften == 0) {

#if APPLE
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    
    if (KERN_SUCCESS != task_info(mach_task_self(),
      TASK_BASIC_INFO, (task_info_t)&t_info, 
                                  &t_info_count))
    {
      streamlog_out(ERROR) << "Problem accessing system information from  MacOS X!"<< std::endl ;
    }
    
    streamlog_out(MESSAGE) << " Processed event  "<<_eventNumber
    << " Resident size is: "<< t_info.resident_size<<" virtual size: "<<t_info.virtual_size
    << std::endl ;
    
#else
    struct sysinfo memInfo;

    sysinfo (&memInfo);

    unsigned long physMemUsed = memInfo.totalram - memInfo.freeram;
    //Multiply in next statement to avoid int overflow on right hand side...   


    streamlog_out(MESSAGE) << " Processed event  "<<_eventNumber
                          << " Physical memory in use: "<<physMemUsed
                          << std::endl ;
#endif
  }
  // Increment the event number
	_eventNumber++ ;
	
}

void MemoryMonitor::check( LCEvent * evt ) {
	// nothing to check here - could be used to fill checkplots in reconstruction processor
}


void MemoryMonitor::end(){

  streamlog_out(MESSAGE) << "MemoryMonitor::end()  " << name()  << " processed " << _eventNumber << " events in " << _runNumber << " runs "
  << std::endl ;
	
	
	
}