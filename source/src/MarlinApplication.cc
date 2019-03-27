#include "marlin/MarlinApplication.h"

namespace marlin {
  
  void MarlinApplication::run() {
    
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinApplication::init() {
    
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinApplication::printUsage() const {
    logger()->log<MESSAGE>() << " Usage: Marlin [OPTION] [FILE]..." << std::endl 
        << "   runs a Marlin application " << std::endl 
        << std::endl 
        << " Running the application with a given steering file:" << std::endl 
        << "   Marlin steer.xml   " << std::endl 
        << std::endl 
        << "   Marlin [-h/-?]             \t print this help information" << std::endl 
        << "   Marlin -x                  \t print an example steering file to stdout" << std::endl 
        << "   Marlin -c steer.xml        \t check the given steering file for consistency" << std::endl 
        << "   Marlin -u old.xml new.xml  \t consistency check with update of xml file"  << std::endl
        << "   Marlin -d steer.xml flow.dot\t create a program flow diagram (see: http://www.graphviz.org)" << std::endl 
        << std::endl 
        << " Example: " << std::endl 
        << " To create a new default steering file from any Marlin application, run" << std::endl 
        << "     Marlin -x > mysteer.xml" << std::endl 
        << " and then use either an editor or the MarlinGUI to modify the created steering file " << std::endl 
        << " to configure your application and then run it. e.g. : " << std::endl 
        << "     Marlin mysteer.xml > marlin.out 2>&1 &" << std::endl << std::endl
        << " Dynamic command line options may be specified in order to overwrite individual steering file parameters, e.g.:" << std::endl 
        << "     Marlin --global.LCIOInputFiles=\"input1.slcio input2.slcio\" --global.GearXMLFile=mydetector.xml" << std::endl 
        << "            --MyLCIOOutputProcessor.LCIOWriteMode=WRITE_APPEND --MyLCIOOutputProcessor.LCIOOutputFile=out.slcio steer.xml" << std::endl << std::endl
        << "     NOTE: Dynamic options do NOT work together with Marlin options (-x, -f) nor with the MarlinGUI" << std::endl
        << std::endl ;
  }

} // namespace marlin
