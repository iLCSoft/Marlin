#ifndef Global_h
#define Global_h 1


namespace marlin{

/** Simple global class for Marlin.
 *  Holds global parameters.
 */
class Global{
  
public:
  
  static StringParameters* parameters ;

};

StringParameters* Global::parameters = 0 ;

} // end namespace marlin 
#endif
