#ifndef MARLIN_CONCURRENCY_EVENT_h
#define MARLIN_CONCURRENCY_EVENT_h 1

// -- std headers
#include <map>
#include <string>

// -- lcio headers
#include <LCRTRelations.h>

// -- marlin headers
#include "marlin/concurrency/Internal.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Event class
     *  Hold a user event and provide extension mechanism to attach
     *  runtime object to an event
     *  @see lcrtrel::LCRTRelations
     */
    template <typename T>
    class Event : lcrtrel::LCRTRelations {
    public:
      /**
       *  @brief  Constructor with user event
       *
       *  @param  event the user event to hold
       */
      Event( std::shared_ptr<T> event ) ;

      /**
       *  @brief  Get the user event
       */
      std::shared_ptr<T> event() const;

    private:
      /// The user event to hold
      std::shared_ptr<T>      _event {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    Event<T>::Event( std::shared_ptr<T> evt ) :
      _event(evt) {

    }

    //--------------------------------------------------------------------------

    template <typename T>
    std::shared_ptr<T> Event<T>::event() const {
      return _event ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
