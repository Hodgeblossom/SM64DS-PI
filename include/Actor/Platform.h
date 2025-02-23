#pragma once

struct Platform : Actor
{
	Model model;             // 0xd4
	MovingMeshCollider clsn; // 0x124
	Matrix4x3 clsnNextMat;   // 0x2ec
	u16 unk31c;              // 0x31c

	Platform();
	virtual ~Platform() override;
	virtual void Kill();

	void KillByMegaChar(Player& player);
	bool UpdateKillByMegaChar(s16 rotSpeedX, s16 rotSpeedY, s16 rotSpeedZ, Fix12i speed); //true if killed by mega char
	void UpdateClsnPosAndRot(); //make sure the mesh collider is at 0x124 first! Also, call this after updating the model's matrix
	void UpdateModelPosAndRotY(); //make sure the model is at 0x0d4 first!

	// Enables collision if in range and disables it otherwise.
	// If clsnRange        == 0._f, Actor::rangeAsr3 << 3 is used instead.
	// If clsnRangeOffsetY == 0._f, Actor::rangeOffsetY   is used instead.
	bool IsClsnInRange(Fix12i clsnRange, Fix12i clsnRangeOffsetY);

	// Enables collision if on screen and in range, or if on screen and clsnRange == 0._f.
	// Otherwise disables the collision.
	// If clsnRangeOffsetY == 0._f, Actor::rangeOffsetY is used instead.
	bool IsClsnInRangeOnScreen(Fix12i clsnRange, Fix12i clsnRangeOffsetY);
};

struct PlatformRes
{
	SharedFilePtr* modelFile;
	SharedFilePtr* clsnFile;
	CLPS_Block* clpsBlock;
};

static_assert(sizeof(Platform) == 0x320);