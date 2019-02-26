#ifndef MARLIN_CONCURRENCY_EXTENSIONS_h
#define MARLIN_CONCURRENCY_EXTENSIONS_h 1

// -- std headers
#include <map>
#include <string>
#include <memory>

// -- marlin headers
#include "marlin/concurrency/Internal.h"
#include "marlin/Exceptions.h"

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  ExtensionBase class
     *  Base class for extension mechanism
     */
    class ExtensionBase {
    protected:
      ExtensionBase() = default ;
      ExtensionBase(const Extension &) = default ;

    public:
      /**
       *  @brief  Destructor
       */
      virtual ~ExtensionBase() = default ;

      /**
       *  @brief  Delete the extension pointer
       */
      virtual void destruct() = 0 ;

      /**
       *  @brief  Get the raw extension pointer
       */
      virtual void *raw() = 0 ;

      /**
       *  @brief  Get the extension 64 bit hash
       */
      virtual unsigned long long hash64() const = 0;

      /**
       *  @brief  Helper function to dynamic cast the extension pointer
       */
      template <typename T>
      const T *dynamic_cast() const { return dynamic_cast<const T*>( raw() ); }

      /**
       *  @brief  Helper function to dynamic cast the extension pointer
       */
      template <typename T>
      T *dynamic_cast() { return dynamic_cast<T*>( raw() ); }

      /**
       *  @brief  Helper function to static cast the extension pointer
       */
      template <typename T>
      const T *static_cast() const { return static_cast<const T*>( raw() ); }

      /**
       *  @brief  Helper function to static cast the extension pointer
       */
      template <typename T>
      T *static_cast() { return static_cast<T*>( raw() ); }
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  OwnedExtension class
     *  Hold an user extension pointer owned by the extension
     */
    template <typename IFACE, typename CONCRETE,
    class = typename std::enable_if<std::is_base_of<IFACE, CONCRETE>::value>::type>
    class OwnedExtension : public ExtensionBase {
    public:
      OwnedExtension(const OwnedExtension &) = default ;
      ~OwnedExtension() = default ;

    public:
      /**
       *  @brief  Constructor
       *
       *  @param  ptr the extension pointer
       *  @warning the pointer is owned by this extension
       */
      OwnedExtension( CONCRETE * ptr ) :
        _ptr( ptr ) {
        /* nop */
      }

    public:
      // from base
      void destruct() { delete _ptr ; }
      void *raw() { return _ptr ; }
      unsigned long long hash64() const { HashHelper::typeHash64<IFACE>() ; }

    private:
      /// User extension pointer (owned by this extension)
      CONCRETE           *_ptr {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  SimpleExtension class
     *  Hold an user extension pointer owned by the user
     */
    template <typename IFACE, typename CONCRETE,
    class = typename std::enable_if<std::is_base_of<IFACE, CONCRETE>::value>::type>
    class SimpleExtension : public ExtensionBase {
    public:
      SimpleExtension(const SimpleExtension &) = default ;
      ~SimpleExtension() = default ;

    public:
      /**
       *  @brief  Constructor
       *
       *  @param  ptr the extension pointer.
       *  @warning The pointer is owned by the caller
       */
      SimpleExtension( CONCRETE * ptr ) :
        _ptr( ptr ) {
        /* nop */
      }

    public:
      // from base
      void destruct() { /* nop */ }
      void *raw() { return _ptr ; }
      unsigned long long hash64() const { HashHelper::typeHash64<IFACE>() ; }

    private:
      /// User extension pointer (owned by the user)
      CONCRETE           *_ptr {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  Extensions class.
     *  Hold a map of extensions uniquely identified using a hash 64 code
     */
    class Extensions {
    public:
      typedef std::shared_ptr<ExtensionBase> ExtensionBasePtr;
      typedef std::map<unsigned long long, std::shared_ptr<ExtensionBase>> ExtensionMap ;

    public:
      Extensions() = default ;
      Extensions(const Extensions&) = delete ;
      Extensions& operator=(const Extensions&) = delete ;

      /**
       *  @brief  Destructor
       */
      ~Extensions() {
        clear() ;
      }

      /**
       *  @brief  Add an user extension
       *
       *  @param  ext   the user extension pointer
       *  @param  owned whether the extension owns the pointer
       */
      template <typename IFACE, typename CONCRETE>
      IFACE *addExtension( CONCRETE *concrete , bool owned = true ) {
        auto hash = HashHelper::typeHash64<IFACE>() ;
        ExtensionBasePtr ext = owned ?
          std::make_shared<OwnedExtension<IFACE, CONCRETE>>( concrete ) :
          std::make_shared<SimpleExtension<IFACE, CONCRETE>>( concrete ) ;
        auto iter = _extensions.find( hash ) ;
        if ( iter != _extensions.end() ) {
          throw Exception( "Extensions::addExtension: extension already registered !" );
        }
        _extensions[ hash ] = ext ;
        return concrete ;
      }

      /**
       *  @brief  Remove an extension
       *
       *  @param  destroy whether to destroy the extension.
       *  @return if the extension is not owned, a pointer on the extension is returned
       */
      template <typename IFACE>
      IFACE *removeExtension( bool destroy = true ) {
        auto hash = HashHelper::typeHash64<IFACE>() ;
        auto iter = _extensions.find( hash ) ;
        if ( iter == _extensions.end() ) {
          throw Exception( "Extensions::removeExtension: no such extension !" ) ;
        }
        IFACE *ptr = iter->second->dynamic_cast<IFACE>() ;
        if ( destroy ) {
          iter->second->destroy() ;
        }
        _extensions.erase( iter ) ;
        return ptr ;
      }

      /**
       *  @brief  Get a registered extension
       */
      template <typename IFACE>
      IFACE *extension() {
        auto hash = HashHelper::typeHash64<IFACE>() ;
        auto iter = _extensions.find( hash ) ;
        if ( iter == _extensions.end() ) {
          throw Exception( "Extensions::extension: no such extension !" ) ;
        }
        return iter->second->dynamic_cast<IFACE*>() ;
      }

      /**
       *  @brief  Clear the extensions map
       */
      void clear() {
        for ( auto iter : _extensions ) {
          iter.second->destroy() ;
        }
        _extensions.clear() ;
      }

    private:
      /// The extensions map
      ExtensionMap          _extensions {} ; //!
    };

  } // end namespace concurrency

} // end namespace marlin

#endif
