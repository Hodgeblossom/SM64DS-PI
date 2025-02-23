#pragma once

struct SpikeBomb : Actor
{
	static constexpr u16 staticActorID = 0x11c;
	static SharedFilePtr modelFile; // 0x0211b1c4

	Model model;
	MovingCylinderClsnWithPos cylClsn;
	u32 unk164;
	u32 unk168;
	u32 unk16c;
	u32 stateID;
	Vector3 savedPos;
	Fix12i unk180;
	Fix12i detonationRadius;
	u32 otherBombUniqueIDs[8];
	u32 unk1a8;
	u16 unk1ac;
	u16 unk1ae;

	void Recover();
};

static_assert(sizeof(SpikeBomb) == 0x1b0);
