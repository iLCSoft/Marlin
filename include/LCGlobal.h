#ifndef LCGlobal_h
#define LCGlobal_h 1



/** Simple global class for Marlin.
 *  Holds global parameters.
 */

class LCGlobal{
  
public:
  
  static StringParameters* parameters ;

};

StringParameters* LCGlobal::parameters = 0 ;

#endif
