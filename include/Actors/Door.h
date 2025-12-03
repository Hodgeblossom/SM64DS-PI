#pragma once

struct Door : Actor
{
	static constexpr u16 staticActorID = 0x161;

	ModelAnim model;
	u32 unk138;
	u32 unk13c;
	u32 unk140;
	u32 unk144;

	virtual s32 InitResources();
	virtual s32 CleanupResources();
	virtual s32 Behavior();
	virtual s32 Render();
	virtual void OnPendingDestroy();
	virtual ~Door();

	static bool StopBowserMessageSound();
	static bool ContinueBowserMessageSound();
};

static_assert(sizeof(Door) == 0x148);
