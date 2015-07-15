#ifndef NeroProducer_Bambu_BaseFiller_h
#define NeroProducer_Bambu_BaseFiller_h

#include "MitAna/DataCont/interface/BaseCollection.h"

#include <functional>

namespace mithep {
  namespace nero {

    typedef std::function<mithep::BaseCollection*(char const*)> ProductGetter;

    class BaseFiller {
    public:
      BaseFiller() {}
      virtual ~BaseFiller() {}

      void setGetter(ProductGetter _getter) { getter_ = _getter; }
      virtual void fill() = 0;

    protected:
      template<class T>
      T* getProduct(char const* name) const;

    private:
      ProductGetter getter_;
    };

  }
}

template<class T>
T*
mithep::nero::BaseFiller::getProduct(char const* _name) const
{
  return dynamic_cast<T*>(getter_(_name));
}

#endif
