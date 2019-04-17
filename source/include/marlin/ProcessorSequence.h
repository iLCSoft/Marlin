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

    /**
     *  @brief  Whether the processor exists in the sequence 
     * 
     *  @param  processor a processor pointer
     */
    bool exists( std::shared_ptr<Processor> processor ) const ;

    /**
     *  @brief  Add at the end of the sequence
     * 
     *  @param  processor a processor pointer
     */
    void add( std::shared_ptr<Processor> processor ) ;

    /**
     *  @brief  Insert processor after the named processor
     * 
     *  @param  afterName the name of the processor after which to insert the processor
     *  @param  processor the processor to insert
     */
    void insertAfter( const std::string &afterName, std::shared_ptr<Processor> processor ) ;

    /**
     *  @brief  Insert processor before the named processor
     * 
     *  @param  afterName the name of the processor before which to insert the processor
     *  @param  processor the processor to insert
     */
    void insertBefore( const std::string &afterName, std::shared_ptr<Processor> processor ) ;

    /**
     *  @brief  Remove a processor from the sequence. 
     *  Return true if the processor has been removed
     *  
     *  @param  processor [description]
     */
    bool remove( std::shared_ptr<Processor> processor ) ;

    /**
     *  @brief  Process the run header. Call processRunHeader() for each processor in the sequence
     * 
     *  @param  rhdr the run header to process
     */
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Modify the run header. Call modifyRunHeader() for each processor in the sequence
     * 
     *  @param  rhdr the run header to modify
     */
    void modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Process the event. Call processEvent() for each processor in the sequence
     * 
     *  @param  event the event to process
     */
    void processEvent( std::shared_ptr<EVENT::LCEvent> event ) ;

    /**
     *  @brief  Modify the event. Call modifyEvent() for each processor in the sequence
     * 
     *  @param  event the event to modify
     */
    void modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) ;

    /**
     *  @brief  Generate a time summary of all processors
     */
    TimeMetadata generateTimeSummary() const ;

  private:
    ConstIterator find( std::shared_ptr<Processor> processor ) const ;
    ConstIterator find( const std::string &name ) const ;

  private:
    /// The processor sequence
    Container                           _processors {} ;
    /// Execution time measurement for each processor
    TimeMap                             _processorTimes {} ;
    /// Skip event exception map
    SkippedEventMap                     _skipEventMap {} ;
  };

} // end namespace marlin

#endif
