#pragma once

#include "ActorBase.h"

struct ActorDerived : ActorBase // internal name: dBase
{
	virtual void AfterInitResources(u32 vfSuccess) override; // Destroys the actor (ActorBase::MarkForDestruction) on vfunc failure, then calls ActorBase::AfterInitResources(u32)
	virtual ~ActorDerived() override;

	static ActorDerived* Spawn(u32 actorID, ActorBase* parent, u32 param1, u8 category); // parent is often the scene
};

static_assert(sizeof(ActorDerived) == 0x50);
