#include "NeroProducer/Nero/interface/NeroTrigger.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroTrigger::NeroTrigger() : BareTrigger()
{
}

NeroTrigger::~NeroTrigger(){
}



int NeroTrigger::analyze(const edm::Event& iEvent){

	if ( mOnlyMc  ) return 0;

	iEvent.getByToken(token,handle);
	iEvent.getByToken(prescale_token,prescale_handle);

	 const pat::helper::TriggerMatchHelper matchHelper; 

	 triggerFired   ->resize( triggerNames->size(),0);
	 triggerPrescale->resize( triggerNames->size(),0);

	 if ( handle.isValid() and not handle.failedToGet() ) {
		edm::RefProd<edm::TriggerNames> trigNames( &(iEvent.triggerNames( *handle )) );
		int ntrigs = (int)trigNames->size();
		for (int i = 0; i < ntrigs; i++) {
			string name = trigNames->triggerName(i);
			for(unsigned int j=0;j< triggerNames->size() ;++j)	
				{
				if (name.find( (*triggerNames)[j]) != string::npos)
					(*triggerFired)[j] = 1;
					(*triggerPrescale)[j] = prescale_handle -> getPrescaleForIndex(i) ;

				} // my trigger end
		} // trigger loop
	 	} // handle is valid

	 // ---- TRIGGER MATCHING ---
	 // example:
	 // 	// -- python
	 //     muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
	 //     muonMatch2    = cms.string( 'muonTriggerMatchHLTMuons2' ),
	 //     elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
	 //      triggerEvent = cms.InputTag( "patTriggerEvent" )
	 //      // -- c++
	 // edm::Handle<pat::TriggerEvent> triggerEvent;
	 //  for (unsigned int j = 0; j < muons->size(); ++j){
	 //  const pat::TriggerObjectRef trigRef( matchHelper.triggerMatchObject( muons,j,muonMatch_, iEvent, *triggerEvent ) );
	 //  if ( trigRef.isAvailable() && trigRef.isNonnull() ) { isMatched =1};
	 //  }
	return 0;
}
