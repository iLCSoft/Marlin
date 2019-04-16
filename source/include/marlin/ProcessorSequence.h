#ifndef MARLIN_PROCESSORSEQUENCE_h
#define MARLIN_PROCESSORSEQUENCE_h 1

// -- std headers
#include <vector>
#include <unordered_set>
#include <string>
#include <map>
#include <memory>

// lcio forward declaration
namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {

  class Processor ;
  class EventContext ;

  /**
   *  @brief  ProcessorSequence class
   */
  class ProcessorSequence {
  public:
    /**
     *  @brief  TimeMetadata struct
     *  Handle runtime information of processors
     */
    struct TimeMetadata {
      /// The total processing time of a processor (processEvent and modifyEvent)
      double   _processingTime {0.} ;
      /// The event counter
      int      _eventCounter {0} ;
    };

  public:
    using Container = std::vector<std::shared_ptr<Processor>> ;
    using Iterator = Container::iterator ;
    using ConstIterator = Container::const_iterator ;
    using TimeMap = std::map<std::string, TimeMetadata> ;
    using SkippedEventMap = std::map<const std::string, int> ;

  public:
    ProcessorSequence() = default ;
    ~ProcessorSequence() = default ;
    ProcessorSequence &operator=(const ProcessorSequence &) = delete ;
    ProcessorSequence(const ProcessorSequence &) = delete ;

    // check if the processor exists in the sequence
    bool exists( std::shared_ptr<Processor> processor ) const ;

    // add at the end of the sequence
    void add( std::shared_ptr<Processor> processor ) ;

    // insert processor after the named processor
    void insertAfter( const std::string &afterName, std::shared_ptr<Processor> processor ) ;

    // insert processor after the named processor
    void insertBefore( const std::string &afterName, std::shared_ptr<Processor> processor ) ;

    // remove a processor from the sequence. Return true if the processor has been removed
    bool remove( std::shared_ptr<Processor> processor ) ;

    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    void modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    void processEvent( std::shared_ptr<EventContext> event ) ;

    void modifyEvent( std::shared_ptr<EventContext> event ) ;

    TimeMetadata generateTimeSummary() const ;

  private:
    ConstIterator find( std::shared_ptr<Processor> processor ) const ;
    ConstIterator find( const std::string &name ) const ;

  private:
    /// The processor sequence
    Container           _processors {} ;
  };

} // end namespace marlin

#endif
