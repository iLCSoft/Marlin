#ifndef MARLIN_ISCHEDULER_h
#define MARLIN_ISCHEDULER_h 1

// -- std headers
#include <memory>
#include <vector>

// lcio forward declaration
namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {

  class Application ;

  /**
   *  @brief  IScheduler interface
   *  Interface for implementing a scheduling algorithm for event processing.
   *  The scheduling can be sequential or parallel (inter-event / intra-event / both).
   *  See implementation classes for more details.
   */
  class IScheduler {
  public:
    virtual ~IScheduler() = default ;

    /**
     *  @brief  Initialize the scheduler with app parameters
     *
     *  @param  app the application in which the scheduler runs
     */
    virtual void init( const Application *app ) = 0 ;
    
    /**
     *  @brief  Process a run header
     *
     *  @param  rhdr the run header to process
     */
    virtual void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) = 0 ;    

    /**
     *  @brief  Push a new event to the scheduler for processing
     *
     *  @param  event the event to push
     */
    virtual void pushEvent( std::shared_ptr<EVENT::LCEvent> event ) = 0 ;

    /**
     *  @brief  Retrieve finished events from the scheduler
     *
     *  @param  events the list of event to retrieve
     */
    virtual void popFinishedEvents( std::vector<std::shared_ptr<EVENT::LCEvent>> &events ) = 0 ;

    /**
     *  @brief  Get the number of free event slots
     */
    virtual std::size_t freeSlots() const = 0 ;
  };

} // end namespace marlin

#endif
