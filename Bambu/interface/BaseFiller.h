#ifndef NeroProducer_Bambu_BaseFiller_h
#define NeroProducer_Bambu_BaseFiller_h

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"
#include "NeroProducer/Bambu/interface/Collections.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <functional>

namespace mithep {
  namespace nero {

    class BaseFiller : public TObject {
    public:
      BaseFiller() {}
      virtual ~BaseFiller() {}

      virtual BareCollection* getObject() { return 0; }
      virtual mithep::nero::Collection collection() const = 0;

      virtual void initialize() {}
      virtual void setCrossRef(BaseFiller*[]) {}
      virtual void fill() = 0;

      typedef std::function<TObject*(char const*)> ProductGetter;
      void setProductGetter(ProductGetter _getter) { getter_ = _getter; }

    protected:
      template<class T>
      T* getSource(char const* name) const;

      void newP4(BareP4&, double px, double py, double pz, double e) const;
      void newP4(TClonesArray&, double px, double py, double pz, double e) const;

    private:
      ProductGetter getter_;
    };

  }
}

template<class T>
inline
T*
mithep::nero::BaseFiller::getSource(char const* _name) const
{
  return dynamic_cast<T*>(getter_(_name));
}

inline
void
mithep::nero::BaseFiller::newP4(BareP4& _p4col, double _px, double _py, double _pz, double _e) const
{
  newP4(*_p4col.p4, _px, _py, _pz, _e);
}

inline
void
mithep::nero::BaseFiller::newP4(TClonesArray& _arr, double _px, double _py, double _pz, double _e) const
{
  new (_arr[_arr.GetEntriesFast()]) TLorentzVector(_pz, _py, _pz, _e);
}

#endif
