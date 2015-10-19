#ifndef NeroProducer_Bambu_BaseFiller_h
#define NeroProducer_Bambu_BaseFiller_h

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

#include "MitAna/DataTree/interface/Particle.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TTree.h"

#include <functional>
#include <stdexcept>

namespace mithep {
  namespace nero {

    class BaseFiller : public TObject {
    public:
      enum Collection {
        kEvent,
        kJets,
        kPuppiJets,
        kAK8Jets,
        kCA15Jets,
        kLeptons,
        kMet,
        kMonteCarlo,
        kPhotons,
        kTaus,
        kTrigger,
        kVertex,
        nEventObjects,
        kAll = nEventObjects,
        nCollections,
        nLumiObjects = nCollections - nEventObjects
      };

      BaseFiller() {}
      virtual ~BaseFiller() {}

      virtual BareCollection* getObject() { return 0; }
      virtual Collection collection() const = 0;

      virtual void setCrossRef(BaseFiller*[]) {}

      virtual void initialize() {} // @SlaveBegin      
      virtual void finalize() {} // @SlaveTerminate
      virtual void notify() {}
      void callBegin();
      void callFill();

      virtual void defineBranches(TTree* tree) { getObject()->defineBranches(tree); };

      typedef std::function<TObject*(char const*)> ProductGetter;
      void setProductGetter(ProductGetter _getter) { getter_ = _getter; }

      void activate() { active_ = true; }
      void disactivate() { active_ = false; }

    protected:
      template<class T>
      T* getSource(char const* name, bool doThrow = true) const;

      void newP4(BareP4&, mithep::Particle const&) const;
      void newP4(TClonesArray&, mithep::Particle const&) const;

      virtual void begin() {} // @BeginRun
      virtual void fill() = 0; // @Process

      bool active_ = true;

    private:
      ProductGetter getter_;
    };

  }
}

inline
void
mithep::nero::BaseFiller::callBegin()
{
  if (!active_)
    return;

  begin();
}

inline
void
mithep::nero::BaseFiller::callFill()
{
  if (!active_)
    return;

  fill();
}

template<class T>
inline
T*
mithep::nero::BaseFiller::getSource(char const* _name, bool _doThrow/* = true*/) const
{
  auto* source = dynamic_cast<T*>(getter_(_name));
  if (!source && _doThrow)
    throw std::runtime_error((TString("ProductNotFound: ") + _name).Data());

  return source;
}

inline
void
mithep::nero::BaseFiller::newP4(BareP4& _p4col, mithep::Particle const& _part) const
{
  newP4(*_p4col.p4, _part);
}

inline
void
mithep::nero::BaseFiller::newP4(TClonesArray& _arr, mithep::Particle const& _part) const
{
  new (_arr[_arr.GetEntriesFast()]) TLorentzVector(_part.Px(), _part.Py(), _part.Pz(), _part.E());
}

#endif
