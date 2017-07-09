# Specification and TODO things to get a MT version of Marlin

Many ways to implement a MT version. Here is one of them which is, to me, the simplest way to do it, with minor things to modify to get a first working version.

## MT on processors with queued scheduling :

- init() and end() as usual.
- processors organized in double-linked list. Use a thread pool with a fixed (n_th) number of threads. Processor::processEvent() calls are linked in different threads. The processed event for a single processor is pushed in the queue of the next processor.
- Only n_th processor can process events at the same time.

### Pros and cons
#### Pros :
- No lock for user in their processor since Processor::processEvent() is called one at a time in a single thread.

#### Cons :
- ...

### Gain on performances

For N processors, the processing time with MT is limited by :
- the number of threads
- the processing time of the processor that takes the longest time to process an event.

### Things to adapt to get MT

#### In LCIO :

- In LCReader, LCEventListener : when an event is read from a file, the previous one is deleted.
LCEvent * should be transformed in a shared_ptr&lt;LCEvent> in SIOReader. Best way to do this is to implement a new SIOReader that owns a shared_ptr&lt;LCEvent>

#### In Marlin :

- In Marlin.cc, create a lcio file reader that support shared_ptr and pass the lcReader instance to processor manager singleton. Each time ProcessorManager::processEvent() is called, get the shared_ptr of the current event and store it to avoid event deletion. At end of process event call, remove the shared_ptr instance and thus the event.

- Implement a thread pool for Marlin, but generic enough to be reusable.

- In ProcessorManager, use the thread pool to start n_th threads. Store the processors in a double-linked list, and implement the logic of processor chain.














.
