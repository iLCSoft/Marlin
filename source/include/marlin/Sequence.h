#ifndef MARLIN_SEQUENCE_h
#define MARLIN_SEQUENCE_h 1

// -- std headers
#include <vector>
#include <unordered_set>
#include <memory>
#include <map>
#include <mutex>
#include <utility> // pair
#include <ctime>

// lcio forward declaration
namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {
  
  class Application ;
  class SequenceItem ;
  class Processor ;
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  SequenceItem class
   *  Handle a processor pointer and call Processor::processEvent in a
   *  critical section if configured accordingly.
   */
  class SequenceItem {
  public:
    using ClockPair = std::pair<clock_t, clock_t> ;
    ~SequenceItem() = default ;
    
    /**
     *  @brief  Constructor
     *
     *  @param  proc a pointer on a processor
     *  @param  lock whether to use a lock
     */
    SequenceItem( std::shared_ptr<Processor> proc, bool lock = false ) ;
    
    /**
     *  @brief  Process the run header
     * 
     *  @param  rhdr the run header to process
     */
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Modify the run header
     * 
     *  @param  rhdr the run header to modify
     */
    void modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;
    
    /**
     *  @brief  Call Processor::processEvent. Lock if the mutex has been initialized.
     *  Call time is returned in a pair as :
     *   - first : the total time taking into account the mutex waiting time
     *   - second : the time spent on process event only 
     * 
     *  @param  event the event to process
     */
    ClockPair processEvent( std::shared_ptr<EVENT::LCEvent> event ) ;
    
    /**
     *  @brief  Call Processor::modifyEvent. Lock if the mutex has been initialized
     *  Call time is returned in a pair as :
     *   - first : the total time taking into account the mutex waiting time
     *   - second : the time spent on process event only 
     * 
     *  @param  event the event to modify 
     */
    ClockPair modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) ;
    
    /**
     *  @brief  Get the processor instance
     */
    std::shared_ptr<Processor> processor() const ;
    
    /**
     *  @brief  Get the processor name
     */
    const std::string &name() const ;
    
  private:
    ///< The processor instance
    std::shared_ptr<Processor>     _processor {nullptr} ;
    ///< The mutex instance
    std::shared_ptr<std::mutex>    _mutex {nullptr} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  ClockMeasure struct
   *  Holds clock measurement data for processors
   */
  struct ClockMeasure {
    /// The total time spent by the application on processEvent() and modifyEvent() calls
    double   _appClock {0.} ;
    /// The time spent by the processor on processEvent() and modifyEvent() calls
    double   _procClock {0.} ;
    /// The event counter
    int      _counter {0} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  Sequence class
   *  A sequence is a list of processors wrapped in SequenceItem objects.
   *  SequenceItem objects allow for calling processor methods in a critical 
   *  section. This methods are :
   *   - Processor::processEvent()
   *   - Processor::modifyEvent()
   *   - Processor::processRunHeader()
   *   - Processor::modifyRunHeader()
   *  Sequence provides a factory method createItem() to create items as 
   *  shared pointer making it possible to share items between multiple sequence.
   *  To allow a proper management of multiple sequences in an application, see
   *  the SuperSequence class.  
   */
  class Sequence {
  public:
    using Container = std::vector<std::shared_ptr<SequenceItem>> ;
    using Index = Container::size_type ;
    using SizeType = Container::size_type ;
    using ClockMeasureMap = std::map<std::string, ClockMeasure> ;
    using SkippedEventMap = std::map<std::string, int> ;
    
  public:
    Sequence() = default ;
    ~Sequence() = default ;
    Sequence &operator=(const Sequence &) = delete ;
    Sequence(const Sequence &) = delete ;
    
  public:
    /**
     *  @brief  Create a sequence item. The item is not added.
     *  Use addItem() to add it.
     *
     *  @param  processor a processor pointer
     *  @param  lock whether to lock on processEvent/modifyEvent calls
     */
    std::shared_ptr<SequenceItem> createItem( std::shared_ptr<Processor> processor, bool lock ) const ;
    
    /**
     *  @brief  Add an item to the sequence
     *  
     *  @param  item the item to add
     */
    void addItem( std::shared_ptr<SequenceItem> item ) ;
    
    /**
     *  @brief  Get a sequence item at the specified index 
     * 
     *  @param  index the lookup index
     */
    std::shared_ptr<SequenceItem> at( Index index ) const ;
    
    /**
     *  @brief  Get the number of items in the sequence
     */
    SizeType size() const ;
    
    /**
     *  @brief  Process the run header. Call processRunHeader() for each item in the sequence
     * 
     *  @param  rhdr the run header to process
     */
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Modify the run header. Call modifyRunHeader() for each item in the sequence
     * 
     *  @param  rhdr the run header to modify
     */
    void modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;
    
    /**
     *  @brief  Process the event. Call processEvent() for each item in the sequence
     * 
     *  @param  event the event to process
     */
    void processEvent( std::shared_ptr<EVENT::LCEvent> event ) ;

    /**
     *  @brief  Modify the event. Call modifyEvent() for each item in the sequence
     * 
     *  @param  event the event to modify
     */
    void modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) ;
    
    /**
     *  @brief  Generate a clock measure summary of all items
     */
    ClockMeasure clockMeasureSummary() const ;
    
  private:
    ///< The sequence items (processor list)
    Container                       _items {} ;
    ///< The processor clock measurements
    ClockMeasureMap                 _clockMeasures {} ;
    ///< The map of skipped events
    SkippedEventMap                 _skipEventMap {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  SuperSequence class
   *  Manages a fixed list of Sequence objects.
   */
  class SuperSequence {
  private:
    struct ItemEqual {
      using Item = std::shared_ptr<SequenceItem> ;
      bool operator()( const Item& lhs, const Item& rhs ) const {
        return lhs->processor() == rhs->processor() ;
      }
    };
    
    using Sequences = std::vector<std::shared_ptr<Sequence>> ;
    using Index = Sequences::size_type ;
    using SizeType = Sequences::size_type ;
    using SequenceItemList = std::unordered_set<
      std::shared_ptr<SequenceItem>,
      std::hash<std::shared_ptr<SequenceItem>>,
      ItemEqual> ;
    
  public:
    SuperSequence() = delete ;
    ~SuperSequence() = default ;
    SuperSequence(const SuperSequence &) = delete ;
    SuperSequence &operator=(const SuperSequence &) = delete ;
    
    /**
     *  @brief  Constructor
     * 
     *  @param nseqs the number of sequences to manage
     */
    SuperSequence( std::size_t nseqs ) ;
    
    /**
     *  @brief  Get the sequence at the given index
     *
     *  @param  index the sequence index
     */
    std::shared_ptr<Sequence> sequence( Index index ) const ;
    
    /**
     *  @brief  Get the number of sequences
     */
    SizeType size() const ;
    
    /**
     *  @brief  Call Processor::baseInit(app) for all processors 
     *
     *  @param  app the application in which the processors run
     */
    void init( const Application *app ) ;
    
    /**
     *  @brief  Process the run header. Call processRunHeader() for each item in the sequence
     * 
     *  @param  rhdr the run header to process
     */
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Modify the run header. Call modifyRunHeader() for each item in the sequence
     * 
     *  @param  rhdr the run header to modify
     */
    void modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;
    
    /**
     *  @brief  Call Processor::end() for all processors 
     */
    void end() ;
    
  private:
    void buildUniqueList( SequenceItemList &items ) const ;
    
  private:
    ///< The list of sequences
    Sequences                  _sequences {} ;
  };

} // end namespace marlin

#endif
