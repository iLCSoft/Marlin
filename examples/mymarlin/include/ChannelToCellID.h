#ifndef ChannelToCellID_h
#define ChannelToCellID_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 2
#define NFLOAT 0
#define NDOUBLE 0

#define CH_INDEX 0
#define ID_INDEX 1

using namespace lcio ;

class ChannelToCellID ;

/** Example for a simple calibration class that maps electronic channel id to cellid.
 *  Based on the LCFixedObject template.
 */
class ChannelToCellID : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
public: 
  
  /** Convenient c'tor.
   */
  ChannelToCellID(int channelID, int cellID ) {

    obj()->setIntVal( CH_INDEX , channelID  ) ;
    obj()->setIntVal( ID_INDEX , cellID  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  ChannelToCellID(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

  /** Important for memory handling*/
  virtual ~ChannelToCellID() { /* no op*/  }
  

  // the class interface:
  int getChannelID()  { return obj()->getIntVal( CH_INDEX ) ;  } 
  int getCellID()  { return obj()->getIntVal( ID_INDEX ) ;  } 
  
  void print(  std::ostream& os ) ;
  
  // -------- need to implement abstract methods from LCGenericObject

  const std::string getTypeName() const { 
    return"ChannelToCellID" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:channelID,i:cellID" ;
  }

}; // class


#endif 
//=============================================================================
