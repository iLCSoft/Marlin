#ifndef ErrorOfSigma_h
#define ErrorOfSigma_h 1

namespace marlin{

  /** Small helper class that computes the lower and upper error of
   *  sigma assuming a normal distribution, i.e. sigma has been computed
   *  as sigma =  1. / (n-1) * SUM_i_n( x_i - a_i )**2.
   *
   *  @author F. Gaede, DESY
   *  @version $Id: ErrorOfSigma.h,v 1.2 2005-10-11 12:56:28 gaede Exp $ 
   */
  class ErrorOfSigma {
    
  public:
    
    /** C'tor takes the number of measured values.
     */
    ErrorOfSigma( unsigned n ) ;
    
    /** Virtual d'tor.*/
    virtual ~ErrorOfSigma() {} 
    
    /** The lower error of sigma
     */
    double lowerError( double sigma ) ;
    
    /** The upper error of sigma
     */
    double upperError( double sigma ) ;
    
  protected:
    
    /** Returns the chisquared value with P(chisquared) == 0.84
     */
    virtual double getChiSquaredPlus() ;
    
    /** Returns the chisquared value with P(chisquared) == 0.16
     */
    virtual double getChiSquaredMinus() ;
    
    /** The number of degrees of freedom */
    unsigned _n ;
  } ;
  


} // end namespace 

#endif



