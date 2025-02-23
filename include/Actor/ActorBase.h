#pragma once

#include "../Memory.h"

// vtable at 0x02099edc
struct ActorBase // internal name: fBase
{
	enum AliveState
	{
		ALIVE = 1,
		DEAD = 2
	};

	struct SceneNode
	{
		SceneNode* parent;
		SceneNode* firstChild;
		SceneNode* prevSibling;
		SceneNode* nextSibling;
		ActorBase* actor;
		
		SceneNode(); //Calls Reset
		void Reset();
	};

	struct ProcessingListNode
	{
		ProcessingListNode* prev;
		ProcessingListNode* next;
		ActorBase* actor;
		u16 priority;
		u16 priorityCopy;
		
		//void RemoveFromGlobal();	//at 0x020440E8 and 0x02044104, removes if at 0x020A4B68
	};

	enum VirtualFuncSuccess
	{
		VS_FAIL_BEFORE = 0,
		VS_FAIL = 1,
		VS_SUCCESS = 2,
		VS_RETURN_MINUS_1 = 3
	};

	//vTable;
	u32 uniqueID;                   // 0x04
	u32 param1;                     // 0x08
	u16 actorID;                    // 0x0c
	u8 aliveState;                  // 0x0e
	bool shouldBeKilled;            // 0x0f
	u8 unk10;                       // 0x10
	u8 unk11;                       // 0x11
	u8 unk12;                       // 0x12
	u8 unk13;                       // 0x13
	SceneNode sceneNode;            // 0x14
	ProcessingListNode behavNode;   // 0x28
	ProcessingListNode renderNode;  // 0x38
	u8 unk48;                       // 0x48
	u8 unk49;                       // 0x49
	u16 unk4a;                      // 0x4a
	Heap* unkHeap;                  // 0x4c (usually null, may be set in Virtual34 or Virtual38)

	ActorBase();
	virtual s32  InitResources();
	virtual bool BeforeInitResources();
	virtual void AfterInitResources(u32 vfSuccess);
	virtual s32  CleanupResources();
	virtual bool BeforeCleanupResources();
	virtual void AfterCleanupResources(u32 vfSuccess);
	virtual s32  Behavior();
	virtual bool BeforeBehavior();
	virtual void AfterBehavior(u32 vfSuccess);
	virtual s32  Render();
	virtual bool BeforeRender();
	virtual void AfterRender(u32 vfSuccess);
	virtual void OnPendingDestroy();
	virtual bool Virtual34(u32 arg0, u32 arg1);
	virtual bool Virtual38(u32 arg0, u32 arg1);
	virtual bool OnHeapCreated();
	virtual ~ActorBase();

	void* operator new(size_t count); //actor bases have their own heap
	void operator delete(void* ptr) { Memory::Deallocate(ptr, Memory::gameHeapPtr); }

	ActorBase(const ActorBase&) = delete;
	ActorBase(ActorBase&&) = delete;
	ActorBase& operator=(const ActorBase&) = delete;
	ActorBase& operator=(ActorBase&&) = delete;

	s32 Process(s32 (ActorBase::*callback)(), bool (ActorBase::*preCallback)(), void (ActorBase::*postCallback)(u32 vfSuccess));
	void MarkForDestruction();
};

static_assert(sizeof(ActorBase) == 0x50);