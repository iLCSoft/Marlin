#ifndef MARLIN_CONCURRENCY_QUEUE_h
#define MARLIN_CONCURRENCY_QUEUE_h 1

// -- std headers
#include <deque>
#include <mutex>
#include <algorithm>

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  Queue class.
     *  A thread safe implementation of a queue.
     *  Does not provide the full std::deque interface,
     *  but only useful methods for the thread pool implementation.
     *  Queue objects are not copiable though movable, as it uses a 
     *  mutex internally for thread safety 
     */
    template <typename T>
    class Queue {
    public:
      // typedefs
      typedef std::deque<T>        QueueType ;
      
    public:
      // not copiable but movable 
      Queue() = default ;
      ~Queue() = default ;
      Queue(Queue<T> &&) = default ;
      Queue(const Queue<T> &) = delete ;      
      Queue<T>& operator=(const Queue<T> &) = delete ;
      
      /**
       *  @brief  Push (back) an element to the queue (const reference)
       *  
       *  @param element the element to push
       */
      void push( const T &element ) ;
      
      /**
       *  @brief  Push (back) an element to the queue (move semantic)
       *  
       *  @param element the element to push
       */
      void push( T &&element ) ;
      
      /**
       *  @brief  Pop the front element and returns it by reference.
       *  Returns true if an element has been poped from the queue,
       *  else false in case the queue was empty
       *   
       *  @param  element the element to get by reference
       */
      bool pop( T &element ) ;
      
      /**
       *  @brief  Remove an element from the queue.
       *  Returns true on success, false otherwise.
       *  
       *  @param  element the element to remove
       */
      bool remove( const T &element ) ;
      
      /**
       *  @brief  Whether the queue is empty
       */
      bool empty() const ;

      /**
       *  @brief  Get the queue size
       */
      std::size_t size() const ;

      /**
       *  @brief  Clear the queue
       */
      void clear() ;
      
    private:
      /// The queue mutex
      mutable std::mutex         _mutex {} ;
      /// The wrapped queue
      QueueType                  _queue {} ;
    };
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
    template <typename T>
    inline void Queue<T>::push( const T &element ) {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      _queue.push_back( element ) ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline void Queue<T>::push( T &&element ) {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      _queue.push_back( element ) ;
    }
    
    //--------------------------------------------------------------------------

    template <typename T>
    inline bool Queue<T>::pop( T &element ) {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      if ( _queue.empty() ) {
        return false ;
      }
      element = std::move( _queue.front() ) ;
      _queue.pop_front() ;
      return true ;
    }
    
    //--------------------------------------------------------------------------
    
    template <typename T>
    inline bool Queue<T>::remove( const T &element ) {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      auto iter = std::find( _queue.begin(), _queue.end(), element ) ;
      if ( _queue.end() != iter ) {
        _queue.erase( iter ) ;
        return true ;
      }
      return false ;
    }
    
    //--------------------------------------------------------------------------

    template <typename T>
    inline bool Queue<T>::empty() const {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      return _queue.empty() ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline std::size_t Queue<T>::size() const {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      return _queue.size() ;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    inline void Queue<T>::clear() {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      _queue.clear() ;
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
