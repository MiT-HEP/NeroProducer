#ifndef BARE_VERTEX_H
#define BARE_VERTEX_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"


class BareVertex : virtual public BareCollection
{
public:
	BareVertex();
	~BareVertex();
	void clear();
	void defineBranches(TTree *t);
	virtual inline string name(){return "BareVertex";};
};
#endif
