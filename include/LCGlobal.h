#ifndef LCGlobal_h
#define LCGlobal_h 1


namespace marlin{

/** Simple global class for Marlin.
 *  Holds global parameters.
 */
class LCGlobal{
  
public:
  
  static StringParameters* parameters ;

};

StringParameters* LCGlobal::parameters = 0 ;

} // end namespace marlin 
#endif
