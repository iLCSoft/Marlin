#ifndef MARLIN_CONCURRENCY_EVENT_h
#define MARLIN_CONCURRENCY_EVENT_h 1

// -- std headers
#include <map>
#include <string>

// -- lcio headers
#include <EVENT/LCEvent.h>

// -- marlin headers
#include "marlin/concurrency/Internal.h"
#include "marlin/concurrency/Extensions.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Event class
     */
    class Event {
    public:
      typedef std::shared_ptr<EVENT::LCEvent> EventPtr;

    public:
      /**
       *  @brief  Constructor with user event
       *
       *  @param  event the user event to hold
       */
      Event( EventPtr event ) :
        _event(evt) {
        /* nop */
      }

      /**
       *  @brief  Get the user event
       */
      EVENT::LCEvent* event() {
        return _event.get() ;
      }

      /**
       *  @brief  Get the user event
       */
      const EVENT::LCEvent* event() const {
        return _event.get() ;
      }

      /**
       *  @brief  Get the runtime event extensions
       */
      const Extensions &extensions() const {
        return _extensions ;
      }

      /**
       *  @brief  Get the runtime event extensions
       */
      Extensions &extensions() {
        return _extensions ;
      }

    private:
      /// The user event to hold
      EventPtr      _event {nullptr} ;
      /// The runtime event extensions
      Extensions    _extensions {} ;
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
