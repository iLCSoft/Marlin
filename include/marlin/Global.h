#ifndef Global_h
#define Global_h 1


namespace marlin{

  class StringParameters ;

  /** Simple global class for Marlin.
   *  Holds global parameters.
   */
  class Global{
    
  public:
    
    static StringParameters* parameters ;
    
  };
  
  
} // end namespace marlin 
#endif
