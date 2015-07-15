#ifndef NeroProducer_Bambu_BaseConfig_h
#define NeroProducer_Bambu_BaseConfig_h

#include "NeroProducer/Bambu/interface/Collections.h"

#include "TObject.h"

#include <utility>

class BareCollection;

namespace mithep {
  namespace nero {
    class BaseFiller;
  }
}

namespace mithep {
  namespace nero {
    
    class BaseConfig : public TObject {
    public:
      BaseConfig() {}
      virtual ~BaseConfig() {}

      virtual mithep::nero::Collection collection() const = 0;
      virtual std::pair<BareCollection*, BaseFiller*> create() const = 0;
    };

  }
}

#endif
