#pragma once

struct BootScene : Scene // internal name: dScBoot
{
	static constexpr u16 staticActorID = 0;

	u32 unk50;
	u32 unk54;

	virtual ~BootScene();
};
