Make a parallel version of Marlin. This issue keep track of important features to include or issues to solve for the parallel version of Marlin.

- [ ] Implement simple task scheduling for processor chain
   - Run event processing in parallel
   - Run run header processing serially:
      - wait for all pending task to finish
      - process run header
      - restart file reading and parallel execution of next events
   - Implement specialized version of scheduler for single thread execution to avoid task scheduling overhead
- [ ] Provide service for writing event to file.
   - This can't be done in a processor anymore as writing in parallel would lead to data races.
   - Writer facility could be prepared as an output plugin configured at runtime via XML, e.g:
```xml
<marlin>
  <global>
       <!-- Standard lcio output REC file -->
       <output type="LCIOFile">
         <parameter name="FileName"> my_output_REC.slcio </parameter>
         <parameter name="KeepEventOrdering"> true </parameter>
       </output>
       <!-- Standard lcio output DST file -->
       <output type="LCIOFile">
         <parameter name="FileName"> my_output_DST.slcio </parameter>
         <parameter name="KeepEventOrdering"> true </parameter>
         <parameter name="DropCollectionTypes"> CalorimeterHit </parameter>
       </output>
   </global>
</marlin>
```
- [x] Provide access to Marlin application in processors, e.g:
```cpp
void MyProcessor::init() {
  auto infiles = app().globals().get<std::string>("LCIOInputFiles") ;
}
```
This application will avoid use of singletons such `ProcessorManager`

**Solution**: an instance of a Scheduler class is assigned to the Processor object, not an Application object though.
- [x] Develop a simple/use existing thread safe logging library. It is possible with Marlin to set the verbosity level for each processor independently. This is not possible with streamlog in the parallel version as the verbosity is global in streamlog and the verbosity level has to be thread local to avoid data race condition or unexpected logging behaviour.
   - Modify streamlog ?
   - Re-implement simple version of streamlog but thread safe and allowing for thread local verbosity ?
   - **Solution**: streamlog has been re-implemented with thread safety, but logging initialization is broken and must be changed.
- [x] Random number generation
   - Make sure the random seed generation using event number and run number still works
   - Results based on this random seeds must be re-producible
   - **Solution**: local random seed generator has been re-implemented. Now, the global processor event seeder instance just forward the calls to the local processor instance, which is thread safe. 
- [ ] Histogram/Tuple library
   - Think about how to handle histograms/graphs/tuples in parallel processing.
   - ROOT is not thread safe. Solutions:
      - Provide a wrapper API around main ROOT objects with locks
      - Provide simplified implementation of thread safe histograming library. At end of Marlin application, make a conversion to ROOT objects and write to file.
   - In case the event ordering is important for tuples, provide a mechanism to order (re-order) entries before writing to file
- [ ] Provide a mechanism to load geometry as a global instance
   - Provide global application extensions that users can populate/access at runtime ?
- [ ] Provide mechanism to pass condition together with an event
   - Attach condition to event via an event extension ?
- [ ] Review current XML parser features and issues in order to make a better use of it
- [ ] Allow for different possible source of records, e.g:
   - LCIO file reader
   - StdHep file reader
   - Remote LCIO file
   - Any other file format that can possibly store LCIO event and LCRunHeader objects
This could be implemented as plugins and steered in the XML file, e.g:
```xml
<marlin>
  <global>
    <!-- Standard lcio input file -->
    <input type="LCIOFile">
      <parameter name="FileName"> my_input.slcio </parameter>
    </input>
    <!-- STDHEP input file -->
    <input type="StdHepFile">
      <parameter name="FileName"> my_input.stdhep </parameter>
    </input>
    <!-- Remote lcio input file -->
    <input type="LCIOFile">
      <parameter name="FileUrl"> http://myhostname.org/public/data/my_input.slcio </parameter>
    </input>
    <!-- Why not a database ? -->
    <input type="MySQLDB">
      <parameter name="Host"> http://myhostname.org </parameter>
      <parameter name="User"> ilc </parameter>
      <parameter name="Password"> $ENV{ILC_DB_PASSWORD} </parameter>
      <parameter name="Database"> Proto_Run42 </parameter>  
    </input>
  </global>
</marlin>
```
