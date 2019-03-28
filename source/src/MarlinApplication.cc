#include "marlin/MarlinApplication.h"

// -- lcio headers
#include <IO/LCEventListener.h>
#include <IO/LCRunListener.h>
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCRunHeader.h"
#include "Exceptions.h"

namespace marlin {
  
  namespace detail {
    
    /**
     *  @brief  MarlinLCListener class
     *  Implement LCEvent and LCRunHeader listeners to use 
     *  in combination with IO::LCReader class.
     *  Forward calls to the application scheduler
     */
    class MarlinLCListener : public IO::LCEventListener, public IO::LCRunListener {
    public:
      /**
       *  @brief  Constructor
       * 
       *  @param  scheduler the scheduler to forward calls
       */
      MarlinLCListener( Scheduler &scheduler ) :
        _scheduler(scheduler) {
        /* nop */
      }
      
      void processEvent(EVENT::LCEvent * evt) {
        _scheduler.processEvent( evt ) ;
      }
      
      void modifyEvent(EVENT::LCEvent * evt) {
        _scheduler.modifyEvent( evt ) ;
      }
      
      void processRunHeader(EVENT::LCRunHeader * rh) {
        _scheduler.processRunHeader( rh ) ;
      }

      void modifyRunHeader(EVENT::LCRunHeader * rh) {
        _scheduler.modifyRunHeader( rh ) ;
      }
      
    private:
      /// The application scheduler
      Scheduler          &_scheduler ;
    };
  }
  
  void MarlinApplication::runApplication() {
    // Create and register a listener
    detail::MarlinLCListener listener( _scheduler ) ;
    _lcReader->registerLCRunListener( &listener ) ;
    _lcReader->registerLCEventListener( &listener ) ;
    // process data from LCIO files !
    bool rewind = true ;
    while ( rewind ) {
      rewind = false ;
      _lcReader->open( _lcioInputFiles ) ;
      // skip events if requested
      if ( _skipNEvents > 0 ) {
        logger()->log<WARNING>() << " --- Marlin app - will skip first " << _skipNEvents << " event(s)" << std::endl ;
        _lcReader->skipNEvents( _skipNEvents ) ;
      }
      try {
        if( _maxRecord > 0 ){
          try {
            _lcReader->readStream( _maxRecord ) ;
          }
          catch( IO::EndOfDataException &e ) {
            logger()->log<WARNING>() << e.what() << std::endl ;
          }
        } 
        else {
          _lcReader->readStream() ;
        }
      }
      catch( StopProcessingException &e ) {
        logger()->log<ERROR>() << std::endl
            << " **********************************************************" << std::endl
            << " *                                                        *" << std::endl
            << " *   Stop of EventProcessing requested by processor :     *" << std::endl
            << " *                  "  << e.what()                           << std::endl
            << " *     will call end() method of all processors !         *" << std::endl
            << " *                                                        *" << std::endl
            << " **********************************************************" << std::endl
            << std::endl ;
      } 
      catch( RewindDataFilesException &e ) {
        rewind = true ;
        logger()->log<MESSAGE>() << std::endl
            << " **********************************************************" << std::endl
            << " *                                                        *" << std::endl
            << " *   Rewind data files requested by processor :           *" << std::endl
            << " *                  "  << e.what()                           << std::endl
            << " *     will rewind to beginning !                         *" << std::endl
            << " *                                                        *" << std::endl
            << " **********************************************************" << std::endl
            << std::endl ;
      }
      _lcReader->close() ;
    }
    // terminate
    _scheduler.end() ;
    _lcReader->removeLCEventListener( &listener ) ;
    _lcReader->removeLCRunListener( &listener ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void MarlinApplication::init() {
    _scheduler.init( this ) ;
    // get settings from global section
    auto globals = globalParameters() ;
    globals->getStringVals( "LCIOInputFiles" , _lcioInputFiles ) ;
    if ( _lcioInputFiles.empty() ) {
      logger()->log<ERROR>() << "No LCIO iput files found in </global> section of steering file" << std::endl ;
      throw Exception( "No LCIO iput files found in </global> section of steering file" ) ;
    }
    _maxRecord = globals->getIntVal( "MaxRecordNumber" ) ;
    _skipNEvents = globals->getIntVal( "SkipNEvents" ) ;
    auto allowModify = ( globals->getStringVal( "AllowToModifyEvent" ) == "true" ) ;
    EVENT::StringVec readCollectionNames {} ;
    globals->getStringVals( "LCIOReadCollectionNames" , readCollectionNames ) ;
    // warning !!!
    if ( allowModify ) {
      logger()->log<WARNING>()  
        << " ******************************************************************************* \n" 
        << " *    AllowToModifyEvent is set to 'true'                                      * \n"
        << " *    => all processors can modify the input event in processEvent() !!        * \n"
        << " *        consider setting this flag to 'false'                                * \n"
        << " *        unless you really need it...                                         * \n"
        << " *    - if you need a processor that modifies the input event                  * \n"
        << " *      please implement the EventModifier interface and use the modifyEvent() * \n"
        << " *      method for this                                                        * \n"
        << " ******************************************************************************* \n" 
        << std::endl ;
    }
    // configure the LCReader
    _lcReader = std::shared_ptr<IO::LCReader>( LCFactory::getInstance()->createLCReader() ) ;
    if ( not readCollectionNames.empty() ) {
      logger()->log<WARNING>()
        << " *********** Parameter LCIOReadCollectionNames given - will only read the following collections: **** " 
        << std::endl ;
      for( auto collection : readCollectionNames ) {
  	    logger()->log<WARNING>()  << "     " << collection << std::endl ;
  	  } 
  	  logger()->log<WARNING>() 
        << " *************************************************************************************************** " << std::endl ;
      _lcReader->setReadCollectionNames( readCollectionNames ) ;
    }
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
