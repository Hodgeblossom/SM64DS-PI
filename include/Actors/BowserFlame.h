#pragma once

struct BowserFlame : Enemy
{
	static constexpr u16 staticActorID = 0x118;

	WithMeshClsn wmClsn;
	u32 unk2cc;
	MovingCylinderClsn cylClsn;
	ShadowModel shadow;
	u32 unk32c;
	u32 unk330;
	u32 unk334;
	u32 unk338;
	u32 unk33c;
	u32 unk340;
	u32 unk344;
	u32 unk348;
	u32 unk34c;
	u32 unk350;
	u32 unk354;
	u32 unk358;
	u32 currState;
	u32 unk360;
	u32 unk364;
	u32 unk368;
	u32 unk36c;
	u32 unk370;
	u32 unk374;
	u32 unk378;
	u32 unk37c;
	u32 unk380;
	u32 unk384;
	u32 unk388;
};

static_assert(sizeof(BowserFlame) == 0x38c);
