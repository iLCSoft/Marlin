#ifndef MARLIN_CONCURRENCY_PROCESSORFACTORY_h
#define MARLIN_CONCURRENCY_PROCESSORFACTORY_h 1

// -- std headers
#include <memory>
#include <string>

// -- marlin headers
#include <marlin/concurrency/Processor.h>

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  ProcessorFactory class.
     *  Abstract factory for processor creation
     */
    class ProcessorFactory {
    public:
      /**
       *  @brief  Destructor
       */
      virtual ~ProcessorFactory() { /* nop */ }

      /**
       *  @brief  Factory method to create a processor
       */
      virtual std::shared_ptr<Processor> create( const std::string &name ) const = 0 ;
    };

    /**
     *  @brief  ProcessorFactoryT class
     *  Concrete template implementation of processor factory
     */
    template <typename T>
    class ProcessorFactoryT final : public ProcessorFactory {
    public:
      /**
       *  @brief  Factory method to create a processor
       */
      template <typename std::enable_if<std::is_base_of<Processor, T>::value, Processor>::type>
      inline std::shared_ptr<Processor> create( const std::string &name ) const {
        std::shared_ptr<Processor> proc = std::make_shared<T>() ;
        proc->setName( name ) ;
        return name ;
      }
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
