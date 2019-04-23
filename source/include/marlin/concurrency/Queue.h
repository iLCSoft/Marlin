#ifndef MARLIN_CONCURRENCY_QUEUE_h
#define MARLIN_CONCURRENCY_QUEUE_h 1

// -- std headers
#include <mutex>
#include <utility>
#include <queue>

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Queue class.
     *  A simplified thread safe queue container.
     *  Support maximum size setting.
     *  The type T must implement move assignement
     */
    template <
      typename T,
      class = typename std::enable_if<std::is_move_assignable<T>::value>::type>
    class Queue {
    public:
      Queue() = default ;
      ~Queue() = default ;
      Queue(const Queue&) = delete ;
      Queue& operator=(const Queue&) = delete ;

      /**
       *  @brief  Constructor
       *
       *  @param  maxsize the maximum queue size
       */
      Queue( std::size_t maxsize ) {
        _maxSize = maxsize ;
      }

      /**
       *  @brief  Push a value to the queue.
       *  WARNING: On success, the element is moved in the queue 
       *  container, else it is not !
       *
       *  @param  value the value to push
       */
      bool push(T &value) {
        std::unique_lock<std::mutex> lock(_mutex) ;
        if( _queue.size() >= _maxSize ) {
          return false ;
        }
        _queue.push( std::move(value) ) ;
        return true ;
      }

      /**
       *  @brief  Pop and get the front element in the queue.
       *  The queue type must support move operation
       *
       *  @param  value the value to receive
       */
      bool pop( T & value ) {
        std::unique_lock<std::mutex> lock(_mutex) ;
        if( _queue.empty() ) {
          return false ;
        }
        value = std::move(_queue.front()) ;
        _queue.pop() ;
        return true ;
      }

      /**
       *  @brief  Whether the queue is empty
       */
      bool empty() const {
        std::unique_lock<std::mutex> lock(_mutex) ;
        return _queue.empty() ;
      }

      /**
       *  @brief  Get the maximum queue size
       */
      std::size_t maxSize() const {
        std::unique_lock<std::mutex> lock(_mutex) ;
        return _maxSize ;
      }

      /**
       *  @brief  Set the maximum queue size.
       *  Note that the queue is NOT resized if the new value is smaller
       *  than the old size. The value of the old max size is returned
       *
       *  @param  maxsize the maximum queue size to set
       */
      std::size_t setMaxSize( std::size_t maxsize ) {
        std::unique_lock<std::mutex> lock(_mutex) ;
        std::swap( _maxSize, maxsize ) ;
        return maxsize ;
      }

      /**
       *  @brief  Check whether the queue has reached the maximum allowed size
       */
      bool isFull() const {
        std::unique_lock<std::mutex> lock(_mutex) ;
        return (_queue.size() >= _maxSize) ;
      }

      /**
       *  @brief  Clear the queue
       */
      void clear() {
        std::unique_lock<std::mutex> lock(_mutex) ;
        while( not _queue.empty() ) {
          _queue.pop() ;
        }
      }

      /**
       *  @brief  Get the number of free slots in the queue
       */
      std::size_t freeSlots() const {
        std::unique_lock<std::mutex> lock(_mutex) ;
        return (_queue.size() >= _maxSize ? 0 : (_maxSize - _queue.size())) ;
      }

    private:
      /// The underlying queue object
      std::queue<T>              _queue {} ;
      /// The synchronization mutex
      mutable std::mutex         _mutex {} ;
      /// The maximum size of the queue
      std::size_t                _maxSize {std::numeric_limits<std::size_t>::max()} ;
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
