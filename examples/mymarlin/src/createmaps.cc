#include "lcio.h"

#include "IO/LCWriter.h"
#include "EVENT/LCIO.h"

#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "UTIL/LCTime.h"

#include "ChannelToCellID.h"

#include "lccd.h"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std ;
using namespace lcio ;

static const int NRUN   = 10 ;
static const int NEVENT = 10 ;  // events
static const int NCELL  = 5000 ;  // calorimeter cells

// the start data for this simulation
static int YEAR  = 2005 ; 
static int MONTH = 2 ; 
static int DAY  = 23 ; 
static int HOUR  = 0 ; 
static int MIN   = 0 ; 
static int SEC   = 0 ; 

/** Creates random channelID to cellID mappings and stores them in the 
 *  database and in LCIO files.
 */

int main(int argc, char** argv ){


  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;

  lccd::DBInterface db( lccd::getDBInitString(), "/calo/channelmap" , true ) ;

  //----- loop over runs ------------
  for(int rn=0;rn<NRUN;rn++){
    

    // ------- create a new random mapping for the run: ----------------------
    LCCollectionVec* condVec = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;
    for(int i=0;i<NCELL;i++){
      
      condVec->addElement( new ChannelToCellID( rand(), i ) ) ;
    }

    // ---- fill a conditions map with inverse lookup for creating the raw hits
     lccd::ConditionsMap<int,ChannelToCellID> chMap( &ChannelToCellID::getCellID  ) ;
     chMap.conditionsChanged( condVec ) ;
//      chMap.print(std::cout ) ;

    // ---- and store it in the database -----
    LCTime runStart( YEAR, MONTH, DAY, HOUR + rn , MIN , SEC ) ;
    LCTime farFuture( 2045,1, 1, 0 , 42 , 0 ) ;
    

    std::cout << " writing conditions data for timestamps: "
	      << runStart.timeStamp() << " - " 
	      <<  farFuture.timeStamp() 
	      << std::endl ;

    db.storeCollection( runStart.timeStamp(), farFuture.timeStamp(), 
			condVec , "random channel mapping" ) ;
    

    // create a simple file with the conditions data:
    db.createSimpleFile( runStart.timeStamp() , "" ) ;
    
    //------------------------------------------------------------------------


    stringstream fn ; 
    fn << "run" << setfill('0') << setw(4) << rn  
       << "_rawcalo.slcio" ;

    lcWrt->open( fn.str() , LCIO::WRITE_NEW )  ;

    
    LCRunHeaderImpl* runHdr = new LCRunHeaderImpl ; 
    runHdr->setRunNumber( rn ) ;
    
    string detName("Calo")  ;
    runHdr->setDetectorName( detName ) ;
    
    stringstream description ; 
    description << " run: " << rn <<" for testing conditions data with random channel maps" ;
    runHdr->setDescription( description.str()  ) ;
 
    lcWrt->writeRunHeader( runHdr ) ;


    // ---- eventLoop ------------------------------------------
    for(int ev=0;ev<NEVENT;ev++){
      
      LCEventImpl*  evt = new LCEventImpl() ;
      
      evt->setRunNumber(  rn   ) ;
      evt->setEventNumber( ev ) ;
      LCTime now ;
      
      long64 evtTimeStamp = runStart.timeStamp() + 1000000LL  ;// one event per ms
      evt->setTimeStamp( evtTimeStamp  ) ;  
      evt->setDetectorName( detName ) ;
      
      // create and add some cell hits
      LCCollectionVec* calVec = new LCCollectionVec( LCIO::RAWCALORIMETERHIT )  ;
      for(int i=0;i<NCELL;i++){
	RawCalorimeterHitImpl* hit = new RawCalorimeterHitImpl ;


	hit->setCellID0(  chMap.find( i ).getChannelID()  ) ;
	hit->setAmplitude( i ) ;
	calVec->addElement( hit ) ;
      }
      evt->addCollection( calVec , "RawCaloHits" ) ;


      // for the first event we add the channel map to the event
      if( ev==0 ) {

	// note: we need to create a new channelmap here because the conditions 
	// collection is transient and  won't be written to the event
	LCCollectionVec* col = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;
	for(int i=0;i<NCELL;i++){
	  col->addElement( new ChannelToCellID( chMap.find( i ).getChannelID() , i ) ) ;
	}

	evt->addCollection( col, "channelmap" ) ;
      }

      lcWrt->writeEvent( evt ) ;
      delete evt ;
    }
    delete runHdr ;
    delete condVec ;
    lcWrt->close() ;
  } // end run
  delete lcWrt ;
  
  // finally create a database file for this folder
  db.createDBFile() ;

}
