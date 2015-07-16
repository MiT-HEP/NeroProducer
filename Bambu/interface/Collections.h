#ifndef NeroProducer_Bambu_Collections_h
#define NeroProducer_Bambu_Collections_h

// probably better to have this in Core and not under mithep

namespace mithep {
  namespace nero {

    enum Collection {
      kEvent,
      kJets,
      kFatJets,
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

  }
}

#endif
