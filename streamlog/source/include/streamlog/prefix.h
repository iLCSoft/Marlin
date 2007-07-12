// -*- mode: c++;
#ifndef prefix_h
#define prefix_h

#include <sstream>

namespace streamlog{

  class logscope ;
  class logstream ;


  /** Base class for log message prefix formating. Subclasses need to define 
   *  std::string operator()() using this->_levelName and this->_name.
   * 
   *  @author F. Gaede, DESY
   *  @version $Id: prefix.h,v 1.1.1.1 2007-07-12 17:14:48 gaede Exp $
   */
  class prefix_base{
  
    friend class logscope ;
    friend class logstream ;

  protected:
    std::string _name ;
    std::string _levelName ;
  
  public:
    prefix_base() ;
    virtual ~prefix_base() ;
 
    virtual std::string operator()() =0 ;
    
    void setLevelName( const std::string& lName) { _levelName = lName ; } 
  
  };

  /** Default log message prefix:  [ LevelName "ScopeName"].
   * 
   *  @author F. Gaede, DESY
   *  @version $Id: prefix.h,v 1.1.1.1 2007-07-12 17:14:48 gaede Exp $
   */

  class prefix : public prefix_base{
  public:
    virtual std::string operator()()  {
      std::stringstream ss ;
      ss << "[ " << this->_levelName << " \"" << this->_name << "\"] " ;
      return ss.str() ;
    }

  };



}
#endif
