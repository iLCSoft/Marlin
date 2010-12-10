#ifndef EventModifier_h
#define EventModifier_h 1


namespace EVENT{ class LCEvent ; }

namespace marlin{

  /** Tagging interface for processors that modify the LCIO event.
   *  @author F. Gaede, DESY
   *  @version $Id: EventModifier.h,v 1.2 2007-08-17 11:21:55 gaede Exp $
   */
  class EventModifier {
    
public:
    /** Implement to modify the event */
    virtual void modifyEvent( EVENT::LCEvent* ) = 0 ;

    /** Implement to modify the run header */
    virtual void modifyRunHeader(EVENT::LCRunHeader * rh) {} ;

    /** Return  name of this  event modifier.
     */
    virtual const std::string & name() const = 0 ;

    // /** Return  name of log level name this event modifier.
    //  */
    // virtual const std::string & logLevelName() { 
    //   static const std::string s("") ;
    //   return s ; 
    // }

    virtual ~EventModifier() {}
  };
 
} // end namespace marlin 

#endif
