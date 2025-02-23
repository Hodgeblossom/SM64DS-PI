#pragma once

struct BowserTail : Actor
{
	static constexpr u16 staticActorID = 0x116;

	MovingCylinderClsn cylClsn; // 0xd4
	u32 unk108;
	u32 unk10c;
	u32 unk110;
	u32 unk114;
};

static_assert(sizeof(BowserTail) == 0x118);

struct Bowser : Actor
{
	static constexpr u16 staticActorID = 0x117;

	ModelAnim modelAnim; // 0xd4
	TextureSequence texSeq; // 0x138
	WithMeshClsn wmClsn; // 0x14c
	ShadowModel shadow; // 0x308
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
	u32 unk35c;
	MovingCylinderClsnWithPos cylClsn; // 0x360
	Actor* player;
	Actor* alsoPlayer; // ???
	u32 unk3a8;
	u32 unk3ac;
	Vector3 spawnPos;
	u32 unk3bc;
	u32 unk3c0;
	u32 unk3c4;
	Vector3 savedPos;
	u32 unk3d4;
	u32 unk3d8;
	u32 unk3dc;
	u32 unk3e0;
	u32 unk3e4;
	u32 unk3e8;
	Fix12i horzDistToPlayer;
	u32 unk3f0;
	Fix12i horzDistToOrigin;
	Fix12i animSpeed;
	u16 unk3fc;
	u16 bounceCounter;
	u32 unk400;
	u16 unk404;
	s16 horzAngleToPlayer;
	s16 horzAngleToOrigin;
	u16 unk40a;
	u32 subStateID;
	u32 stateID;
	u8 unk414;
	u8 unk415;
	u8 unk416;
	u8 unk417;
	u32 flags2;
	u8 alpha; // from 0 to 255
	u8 unk41d;
	u8 health; // the number of explosions Bowser can take before being defeated
	u8 unk41f;
	u16 unk420;
	u8 unk422;
	u8 unk423;
	u8 unk424;
	u8 unk425;
	u8 unk426;
	u8 unk427;
	u32 unk428;
	Vector3 camLookAt;
	Vector3 camPos;
	u32 unk444;
	u32 unk448;
	u32 unk44c;
	u32 unk450;

	bool CheckSpikeBombClsn();
};

static_assert(sizeof(Bowser) == 0x454);
