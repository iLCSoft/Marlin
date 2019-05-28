

# Writing your first Marlin Processor

New to Marlin ? You want to get started with a simple `Processor` example ? The is the perfect place to start then. A `Processor` is Marlin plugin that implement the processing of LCIO `LCEvent` and `LCRunHeader` objects. The processor can interact with this event to get data, so called `LCCollection` objects, or create and store data in the event. To implement a `Processor`, you must inherit from the `marlin::Processor` class and re-implement a few virtual callback functions.

Let's start with the simplest Processor implementation:

```cpp
// MyProcessor.cc
// -- marlin headers
#include <marlin/Processor.h>
// -- lcio headers
#include <EVENT/LCEvent.h>

class MyProcessor : public marlin::Processor {
public:
  MyProcessor() = default ;
  marlin::Processor *newProcessor() const override ;
  void processEvent( EVENT::LCEvent *event ) override ;
};

marlin::Processor *MyProcessor::newProcessor() const {
  return new MyProcessor() ;
}

void MyProcessor::processEvent( EVENT::LCEvent *event ) {
  log<MESSAGE>() << "Processing" <<
  " event no " << event->getEventNumber() <<
  ", run no " << event->getRunNumber() << std::endl ;
}

// plugin declaration
MyProcessor myProcessor ;
```

A few explanations for this code:

- The method `newProcessor()` is factory method used by the framework to create a processor instance when running the `Marlin` application. This is the only required overload when you implement a `Processor`.
- The method `processEvent(event)` is a callback method processed everytime an LCIO event is, for example, read from an LCIO file. Here, we just print the event number and run number using the logging method `log<T>()`, where `T` is the log level (here `MESSAGE`).  

To compile this code, see the next sections on this page.

For more information on the `Processor` API usage, see the [dedicated documentation page](processor-api.md)

# Dealing with CMake in your project



# XML steering file


# Running your processor with Marlin
