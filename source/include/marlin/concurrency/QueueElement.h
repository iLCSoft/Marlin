#ifndef MARLIN_CONCURRENCY_QUEUEELEMENT_h
#define MARLIN_CONCURRENCY_QUEUEELEMENT_h 1

// -- std headers
#include <memory>
#include <future>

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  QueueElement class
     *  A template queue element used in the thread pool.
     *  The IN type represent the actual data type pushed by the
     *  user in the thread pool queue and the OUT type the expected
     *  output stored in the future object returned by calling push()
     */
    template <typename IN, typename OUT>
    class QueueElement {
    public:
      // no copy
      QueueElement() = default ;
      QueueElement( const QueueElement<IN,OUT> & ) = delete ;
      QueueElement &operator=( const QueueElement<IN,OUT> & ) = delete ;

      /**
       *  @brief  Constructor with input data
       *
       *  @param  input user input data
       */
      QueueElement( IN && input ) :
        _input(input) {
        /* nop */
      }

      /**
       *  @brief  Move constructor
       */
      QueueElement( QueueElement<IN,OUT> &&rhs ) {
        *this = std::move(rhs) ;
      }

      /**
       *  @brief  Move assignement operator
       */
      QueueElement &operator=( QueueElement<IN,OUT> &&rhs ) {
        _promise = std::move(rhs._promise) ;
        _input = std::move(rhs._input) ;
        return *this ;
      }
      
      /**
       *  @brief  Get the output promise
       */
      std::shared_ptr<std::promise<OUT>> promise() const {
        return _promise ;
      }

      /**
       *  @brief  Set the value to be retrieved in the future variable
       *
       *  @param  output the output data to retrieve in the future object
       */
      void setValue( OUT && output ) {
        _promise->set_value( output ) ;
      }

      /**
       *  @brief  Take the input data. Note that this moves the input data
       *  and thus invalidate the stored input data of the queue element
       */
      IN takeInput() {
        return std::move(_input) ;
      }

    private:
      ///< The promise for getting the output data
      std::shared_ptr<std::promise<OUT>>    _promise {std::make_shared<std::promise<OUT>>()} ;
      ///< The input data provided by the user
      IN                                    _input {} ;
    };

    template <typename OUT>
    class QueueElement<void, OUT> {
    public:
      // no copy
      QueueElement( const QueueElement<void,OUT> & ) = delete ;
      QueueElement &operator=( const QueueElement<void,OUT> & ) = delete ;
      QueueElement() = default ;
      QueueElement( QueueElement<void,OUT> &&rhs ) { *this = std::move(rhs) ; }
      QueueElement &operator=( QueueElement<void,OUT> &&rhs ) { _promise = std::move(rhs._promise) ; return *this ; }
      std::shared_ptr<std::promise<OUT>> promise() const { return _promise ; }
      void setValue( OUT && output ) { _promise->set_value( output ) ; }
    private:
      std::shared_ptr<std::promise<OUT>>    _promise {std::make_shared<std::promise<OUT>>()} ;
    };

    template <typename IN>
    class QueueElement<IN, void> {
    public:
      // no copy
      QueueElement() = default ;
      QueueElement( const QueueElement<IN,void> & ) = delete ;
      QueueElement &operator=( const QueueElement<IN,void> & ) = delete ;
      QueueElement( IN && input ) : _input(input) {}
      QueueElement( QueueElement<IN,void> &&rhs ) { *this = std::move(rhs) ; }
      QueueElement &operator=( QueueElement<IN,void> &&rhs ) {
        _promise = std::move(rhs._promise) ;
        _input = std::move(rhs._input) ;
        return *this ;
      }
      std::shared_ptr<std::promise<void>> promise() const { return _promise ; }
      void setValue() { _promise->set_value() ; }
      IN takeInput() { return std::move(_input) ; }
    private:
      std::shared_ptr<std::promise<void>>    _promise {std::make_shared<std::promise<void>>()} ;
      IN                                     _input {} ;
    };

    template <>
    class QueueElement<void, void> {
    public:
      // no copy
      QueueElement( const QueueElement<void,void> & ) = delete ;
      QueueElement &operator=( const QueueElement<void,void> & ) = delete ;
      QueueElement() = default ;
      QueueElement( QueueElement<void,void> &&rhs ) { *this = std::move(rhs) ; }
      QueueElement &operator=( QueueElement<void,void> &&rhs ) { _promise = std::move(rhs._promise) ; return *this ; }
      std::shared_ptr<std::promise<void>> promise() const { return _promise ; }
      void setValue() { _promise->set_value() ; }
    private:
      std::shared_ptr<std::promise<void>>    _promise {std::make_shared<std::promise<void>>()} ;
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
