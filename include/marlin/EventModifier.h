#ifndef EventModifier_h
#define EventModifier_h 1


namespace EVENT{ class LCEvent ; }

namespace marlin{

  /** Tagging interface for processors that modify the LCIO event.
   *  @author F. Gaede, DESY
   *  @version $Id: EventModifier.h,v 1.1 2007-08-13 10:38:06 gaede Exp $
   */
  class EventModifier {
    
public:
    /** Implement to modify the event */
    virtual void modifyEvent( EVENT::LCEvent* ) = 0 ;

    virtual ~EventModifier() {}
  };
 
} // end namespace marlin 

#endif
