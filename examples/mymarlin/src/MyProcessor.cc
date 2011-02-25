#include "MyProcessor.h"
#include <iostream>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/IHistogram1D.h>
#endif // MARLIN_USE_AIDA


using namespace lcio ;
using namespace marlin ;


MyProcessor aMyProcessor ;


MyProcessor::MyProcessor() : Processor("MyProcessor") {

    // modify processor description
    _description = "MyProcessor does whatever it does ..." ;


    // register steering parameters: name, description, class-variable, default value
    registerInputCollection( LCIO::MCPARTICLE,
            "CollectionName" , 
            "Name of the MCParticle collection"  ,
            _colName ,
            std::string("MCParticle")
    );
}



void MyProcessor::init() { 

    streamlog_out(DEBUG) << "   init called  " << std::endl ;

    // usually a good idea to
    printParameters() ;

    _nRun = 0 ;
    _nEvt = 0 ;

}


void MyProcessor::processRunHeader( LCRunHeader* run) { 

    _nRun++ ;
} 



void MyProcessor::processEvent( LCEvent * evt ) { 


    // this gets called for every event 
    // usually the working horse ...


#ifdef MARLIN_USE_AIDA

    // define a histogram pointer
    static AIDA::ICloud1D* hMCPEnergy ;    

    if( isFirstEvent() ) { 

        hMCPEnergy = AIDAProcessor::histogramFactory(this)->
            createCloud1D( "hMCPEnergy", "energy of the MCParticles", 100 ) ; 

    }
#endif // MARLIN_USE_AIDA

    // old code: do not use this code snippet to get a collection!
    // LCCollection* col = evt->getCollection( _colName ) ;
    // reason: if _colName is a non-existant collection lcio throws an
    //          exception (DataNotAvailableException). However if Marlin is linked
    //          with RAIDA->ROOT _and_ the AIDAProcessor is activated in the steering
    //          file, the lcio exception does not cause the program to stop! This
    //          causes the if-statement below: if( col != NULL){...} to be entered
    //          as col was not previously initialized (or reset) to NULL. This causes
    //          a segfault when using the MCParticle* p pointer in the fill() method
    //          below. This segfault is finally caught by ROOT (showing the stack-trace)
    //          If the previous text doesn't make sense to you, don't worry.. be happy :)
    //
    // new code:

    LCCollection* col = NULL;
    try{
        col = evt->getCollection( _colName );
    }
    catch( lcio::DataNotAvailableException e )
    {
        streamlog_out(WARNING) << _colName << " collection not available" << std::endl;
        col = NULL;
    }

    // this will only be entered if the collection is available
    if( col != NULL ){

        int nMCP = col->getNumberOfElements()  ;

        for(int i=0; i< nMCP ; i++){

            MCParticle* p = dynamic_cast<MCParticle*>( col->getElementAt( i ) ) ;

#ifdef MARLIN_USE_AIDA
            // fill histogram from LCIO data :
            hMCPEnergy->fill( p->getEnergy() ) ;
#endif
        } 
    }



    //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

    streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
        << "   in run:  " << evt->getRunNumber() << std::endl ;



    _nEvt ++ ;
}



void MyProcessor::check( LCEvent * evt ) { 
    // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void MyProcessor::end(){ 

    //   std::cout << "MyProcessor::end()  " << name() 
    // 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
    // 	    << std::endl ;

}

