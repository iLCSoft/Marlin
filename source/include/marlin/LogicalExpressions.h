#ifndef LogicalExpressions_h
#define LogicalExpressions_h 1

#include <map>
#include <string>
#include <list>
#include <vector>
#include <ostream>

typedef std::map< const std::string, std::string > ConditionsMap ;
typedef std::map< const std::string, bool > ResultMap ;


namespace marlin{


  /** Helper struct for LogicalExpression.
   *
   *  @author F. Gaede, DESY
   *  @version $Id: LogicalExpressions.h,v 1.3 2005-10-11 12:56:28 gaede Exp $
   */
  struct Expression{

    Expression() : Operation( AND ), isNot( false ), Value("") {
    }
    enum Operator{ OR, AND } ;

    Operator Operation ;
    bool isNot ;
    std::string Value ;
  };

  std::ostream& operator<< (  std::ostream& s,  Expression& e ) ;

  /** Helper class for LogicalExpressions that splits the expression into
   *  subexpressions - needs to be apllied iteratively.
   *
   */
  class Tokenizer{

    enum state{
      newtoken,
      parenthesis,
      operation
    } ;

    std::vector< Expression >& _tokens ;
    char _last ;
    bool needToken ;
    int openPar ;
    int closedPar ;
    state _state ;

  public:

    Tokenizer( std::vector< Expression >& tokens ) : _tokens(tokens) , _last(0), needToken(true)
						   , openPar(0), closedPar(0) ,
						     _state( newtoken ) {
    }


    /** Save the current character and change state if necessary */
    void operator()(const char& c) {

      if( c != ' ' && c != '\t'  ) {  // ignore whitespaces and tabs

 	if( c == '(' ) ++openPar ;

 	if( c == ')' ) ++closedPar ;

	switch( _state ){

	case( newtoken ):

	  if( needToken ){
	    _tokens.push_back( Expression() ) ; // create a new object
	    needToken = false ;
	  }
	  if( c == '!' )
 	    _tokens.back().isNot = true ;

	  else if( c == '(' )
	    _state =  parenthesis ;

	  else {
	    _tokens.back().Value += c ;
	    _state = operation ;
	  }
	  break ;

	case( parenthesis ):

	  if( closedPar == openPar ) {

	    _state = operation ;

	  } else {

	    _tokens.back().Value += c ;
	  }
	  break ;

	case( operation ): // need to accumulate values until && or ||

	  if( c == '&' || c=='|' ){

	    if ( c == '&' && _last == '&' ) {
	      _tokens.push_back( Expression() ) ; // create a new object
	      _tokens.back().Operation = Expression::AND ;
	      _state = newtoken ;
	    }
	    if ( c == '|' && _last == '|' ) {
	      _tokens.push_back( Expression() ) ; // create a new object
	      _tokens.back().Operation = Expression::OR ;
	      _state = newtoken ;
 	    }

	  }  else {

	    _tokens.back().Value += c ;
	  }

	  break ;
	}

      }
      _last = c ;
    }

    ~Tokenizer(){
    }

    std::vector<Expression> & result()  {

      return _tokens ;

    }
  };

  /** Helper class that holds named boolean values and named conditions that are expressions
   * of these values and computes the corresponding truth values.
   */
  class LogicalExpressions {

  public:

    /** C'tor.
     */
    LogicalExpressions() ;

    /** Virtual d'tor.*/
    virtual ~LogicalExpressions() {}

    /** Add a new named logical expression formed out of [!,(,&&,||,),value], e.g.<br>
     *  ( A && ( B || !C ) ) || ( !B && D )
     */
    void addCondition( const std::string& name, const std::string& expression ) ;

    /** Clear all boolean values */
    void clear() ;

    /** True if the named condition (stored with addCondition) is true with the current values */
    bool conditionIsTrue( const std::string& name ) const ;

    /** True if the given expression  is true with the current values */
    bool expressionIsTrue( const std::string& expression ) const ;

    /** Set the the boolean value for the given key*/
    void setValue( const std::string& key, bool val ) ;


  protected:

    /** helper function for finding return values, that actually have been set by their corresponding processor - throws exception if not set */
    bool getValue( const std::string& key ) const ;

    ConditionsMap _condMap{};
    ResultMap _resultMap{};

  } ;
} // end namespace

#endif
