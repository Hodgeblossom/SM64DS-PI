#pragma once

#include "Stage.h"

extern "C" void ChangeArea(s8 newAreaID);

struct Player : Actor
{
	static constexpr u16 staticActorID = 0xbf;

	enum Characters : u8
	{
		CH_MARIO,
		CH_LUIGI,
		CH_WARIO,
		CH_YOSHI,

		NUM_CHARACTERS,
	};

	enum Animations : u32
	{
		ANIM_ED_KISS_WAIT,
		ANIM_ED_KISS_TO_POSE,
		ANIM_L_ED_AMAZED = ANIM_ED_KISS_TO_POSE,
		ANIM_W_ED_AMAZED_WAIR = ANIM_ED_KISS_TO_POSE,
		ANIM_Y_ED_SHY_WAIT = ANIM_ED_KISS_TO_POSE,
		ANIM_E_DOWN,
		ANIM_SHF_DIE,
		ANIM_S_DIE,
		ANIM_GAS_WAIT,
		ANIM_GAS_DIE,
		ANIM_FALL_DIE,
		ANIM_LIE_END,
		ANIM_LIE_WAIT,
		ANIM_SLEEP_TO_LIE,
		ANIM_SLEEP_END,
		ANIM_SLEEP_WAIT,
		ANIM_SLEEP_START,
		ANIM_DAMAGE_START,
		ANIM_DAMAGE_WAIT,
		ANIM_RETURN_DOWN,
		ANIM_E_SHOCK, // start of arc0.narc
		ANIM_SDOWN,
		ANIM_BKDWN,
		ANIM_SHFDN,
		ANIM_JFDWN,
		ANIM_FIREJMP,
		ANIM_FJPEND,
		ANIM_CARRY_LOST,
		ANIM_STAR_GET,
		ANIM_BROAD_JUMP,
		ANIM_BROAD_JUMP_END,
		ANIM_HANDSTAND_JUMP,
		ANIM_HANDSTAND_WAIT,
		ANIM_HANDSTAND_START,
		ANIM_HANDSTAND_END,
		ANIM_HANG_JUMP_UP,
		ANIM_HANG_WAIT,
		ANIM_HANG_START,
		ANIM_HANG_UP,
		ANIM_TREE_CLIMB,
		ANIM_TREE_POSE,
		ANIM_TREE_WAIT,
		ANIM_TREE_START,
		ANIM_WJMP,
		ANIM_WSLD,
		ANIM_BACK_JUMP,
		ANIM_BACK_JUMP_END,
		ANIM_SQUAT_WAIT,
		ANIM_SQUAT_START,
		ANIM_SQUAT_END,
		ANIM_CARRY_START,
		ANIM_CARRY_THROW,
		ANIM_CARRY_POSE,
		ANIM_CARRY_WALK,
		ANIM_CARRY_WAIT,
		ANIM_RIDE,
		ANIM_RIDE_START,
		ANIM_BRAKE,
		ANIM_BREND,
		ANIM_ATTACK_1,
		ANIM_ATTACK_2,
		ANIM_ATTACK_3,
		ANIM_HIPSR,
		ANIM_HIPAT,
		ANIM_HIPED,
		ANIM_LOST,
		ANIM_RUN,
		ANIM_SLDCT,
		ANIM_SLIP,
		ANIM_SLIPED,
		ANIM_SLPBK,
		ANIM_SLPLA,
		ANIM_TRNED,
		ANIM_TURN,
		ANIM_WAIT,
		ANIM_WALK,
		ANIM_FLY_POSE,
		ANIM_JUMP_TO_FLY,
		ANIM_ROLL_JUMP,
		ANIM_ROLL_JUMP_END,
		ANIM_TJMP1,
		ANIM_TJMP2,
		ANIM_2JMP1,
		ANIM_2JMP2,
		ANIM_2JMPED,
		ANIM_JMPED,
		ANIM_JUMP,
		ANIM_LAND,
		ANIM_LAEND,
		ANIM_ROLL,
		ANIM_MONKEY_WAIT_L,
		ANIM_MONKEY_WAIT_R,
		ANIM_MONKEY_L_TO_R,
		ANIM_MONKEY_R_TO_L,
		ANIM_MONKEY_START,
		ANIM_WALL_WALK_L,
		ANIM_WALL_WALK_R,
		ANIM_WALL_WAIT,
		ANIM_SPIN_JUMP,
		ANIM_SPIN_JUMP_END,
		ANIM_SOFT_STEP,
		ANIM_CRAWL_END,
		ANIM_CRAWL,
		ANIM_CRAWL_START,
		ANIM_PUSH,
		ANIM_SLIDING_KICK_END,
		ANIM_SLIDING_KICK,
		ANIM_ROLL_KICK,
		ANIM_SFBDN,
		ANIM_SFFDN,
		ANIM_PUTON_CAP,
		ANIM_Y_EAT,
		ANIM_W_GSWING_START = ANIM_Y_EAT,
		ANIM_Y_EAT_OUT,
		ANIM_W_GSWING_POSE = ANIM_Y_EAT_OUT,
		ANIM_Y_EAT_FAIL,
		ANIM_W_GSWING_THROW = ANIM_Y_EAT_FAIL,
		ANIM_Y_EAT_SUCCESS,
		ANIM_W_GSWING_END = ANIM_Y_EAT_SUCCESS,
		ANIM_Y_LAY_EGG,
		ANIM_STRUGGLE, // end of arc0.narc
		ANIM_SHOOT_TO_FLY,
		ANIM_SHOOT,
		ANIM_SDROP_LEG,
		ANIM_SDROP_LEG_WAIT,
		ANIM_SDROP_LEG_END,
		ANIM_SDROP_HIP_END,
		ANIM_SDROP_HIP,
		ANIM_SDROP_HIP_WAIT,
		ANIM_SDROP_HEAD_END,
		ANIM_SDROP_HEAD_WAIT,
		ANIM_SDROP_HEAD,
		ANIM_SDROP_WAIT,
		ANIM_SDROP_WALK,
		ANIM_P_KOOPA_THROW,
		ANIM_P_KOOPA_H_WAIT,
		ANIM_P_KOOPA_HOLD,
		ANIM_FALL_SAND,
		ANIM_SWIM_DIE,
		ANIM_DROWN_DIE,
		ANIM_RETURN_STAR,
		ANIM_N_CARRY_START,
		ANIM_N_CARRY_WAIT,
		ANIM_N_CARRY_WALK,
		ANIM_N_CARRY_POSE,
		ANIM_N_CARRY_PUT,
		ANIM_N_CARRY_LOST,
		ANIM_DOOR_PUSH,
		ANIM_DOOR_PULL,
		ANIM_H_CARRY_START,
		ANIM_H_CARRY_WALK,
		ANIM_H_CARRY_WAIT,
		ANIM_H_CARRY_THROW,
		ANIM_STAR_OPEN,
		ANIM_KEY_OPEN,
		ANIM_KEY_GET,
		ANIM_COLD_WAIT_START,
		ANIM_COLD_WAIT,
		ANIM_COLD_WAIT_END,
		ANIM_PUTON_T_CAP,
		ANIM_FIRST_CHANGE,
		ANIM_SCALE_UP,
		ANIM_SU_JUMP_END,
		ANIM_SU_JUMP,
		ANIM_SU_WALK_END,
		ANIM_SU_WALK,
		ANIM_SU_WALK_START,
		ANIM_SU_WAIT,
		ANIM_SU_HIPED,
		ANIM_SU_HIPAT,
		ANIM_SU_HIPSR,
		ANIM_SU_RUN,
		ANIM_RETURN_NOCAP,
		ANIM_SWIM,
		ANIM_SWIM_IN,
		ANIM_SWIM_WAIT,
		ANIM_PADDLE_1,
		ANIM_PADDLE_2,
		ANIM_PADDLE_END,
		ANIM_SWIM_STAR_GET,
		ANIM_SWIM_DAMAGE,
		ANIM_SWIM_ATTACK,
		ANIM_SWIM_CARRY,
		ANIM_PADDLE_CARRY,
		ANIM_SWIM_THROW,
		ANIM_OP_GO,
		ANIM_L_OP_NOD = ANIM_OP_GO,
		ANIM_W_OP_NOD = ANIM_OP_GO,
		ANIM_Y_OP_LIE_TO_SIT = ANIM_OP_GO,
		ANIM_OP_WAIT,
		ANIM_Y_OP_SIT_TO_STAND = ANIM_OP_WAIT,
		ANIM_Y_OP_SIT_WAIT,
		ANIM_L_OP_TURN = ANIM_Y_OP_SIT_WAIT,
		ANIM_W_OP_TURN = ANIM_Y_OP_SIT_WAIT,
		ANIM_Y_OP_STAND_WAIT,
		ANIM_L_OP_LOOK_WARIO = ANIM_Y_OP_STAND_WAIT,
		ANIM_L_OP_LOOK_LUIGI = ANIM_Y_OP_STAND_WAIT,
		ANIM_W_OP_POINT,
		ANIM_ED_WAIT,
		ANIM_ED_LOOK,
		ANIM_ED_LOOK_WAIT,
		ANIM_ED_LOOK_END,
		ANIM_ED_NOD,
		ANIM_ED_PUTUP_WAIT,
		ANIM_ED_LOOKUP_WAIT = ANIM_ED_PUTUP_WAIT,
		ANIM_ED_CAP_OFF,
		ANIM_ED_CAP_OFF_WAIT,
		ANIM_Y_ED_SHY_TO_WAIT = ANIM_ED_CAP_OFF_WAIT,
		ANIM_ED_WAVE,
		ANIM_ED_FLY_START,
		ANIM_Y_ED_LOOK_CAM = ANIM_ED_FLY_START,
		ANIM_ED_FLY,
		ANIM_Y_ED_TURN = ANIM_ED_FLY,

		NUM_ANIMS,
	};

	enum TalkStates
	{
		TK_NOT         = -1,
		TK_START       =  0,
		TK_TALKING     =  1, // +0x6e3 == anything but 3, 5, or 7
		TK_POST_CHOICE =  2, // +0x6e3 == 3; After selecting a dialog option
		TK_UNK3        =  3  // +0x6e3 == 5 or 7
	};

	enum HurtStates
	{
		HT_NOT     = -1,
		HT_START   =  0,
		HT_TALKING =  1,
		HT_UNK2    =  2,
		HT_UNK3    =  3,
		HT_UNK4    =  4,
		HT_UNK5    =  5,
		HT_UNK6    =  6,
		HT_UNK7    =  7,
	};

	enum Flags2
	{


		F2_CAMERA_ZOOM_IN     = 1 << 2,
		F2_TELEPORT           = 1 << 3,



		F2_RESET_POSITION     = 1 << 7,


		F2_EXIT_LEVEL_IF_DEAD = 1 << 10,
		F2_NO_CONTROL         = 1 << 11,
		F2_START_FLOWER_POWER = 1 << 12
	};

	struct State
	{
		bool(Player::* init)();
		bool(Player::* main)();
		bool(Player::* cleanup)();
	}
	static
		ST_LEDGE_HANG,
		ST_LEDGE_GRAB,
		ST_CEILING_GRATE,
		ST_YOSHI_POWER, // tongue, spitting, throwing egg, breathing fire
		ST_SWALLOW,
		ST_IN_YOSHI_MOUTH,
		ST_SMALL_LAUNCH_UP, // never set? (1 call)
		ST_HURT,
		ST_HURT_WATER,
		ST_ELECTROCUTE,
		ST_BURN_FIRE,
		ST_BURN_LAVA,
		ST_DEAD_HIT,
		ST_DEAD_PIT,
		ST_WALK,
		ST_WAIT,
		ST_GRABBED,
		ST_TURN_AROUND,
		ST_JUMP,
		ST_FALL,
		ST_THROWN,
		ST_SIDE_FLIP,
		ST_SLIDE_KICK_RECOVER,
		ST_FLY,
		ST_NO_CONTROL, // includes caps
		ST_OWL,
		ST_DEBUG_FLY,
		ST_WIND_CARRY,
		ST_BALLOON,
		ST_TELEPORT,
		ST_TORNADO_SPIN,
		ST_CANNON,
		ST_SQUISH,
		ST_SHELL,
		ST_STOMACH_SLIDE,
		ST_BUTT_SLIDE,
		ST_DIZZY_STARS,
		ST_HOLD_LIGHT,
		ST_BONK,
		ST_HOLD_HEAVY,
		ST_WALL_SLIDE,
		ST_BOWSER_EARTHQUAKE,
		ST_WALL_JUMP,
		ST_SLOPE_JUMP,
		ST_STUCK_IN_GROUND,
		ST_LAND,
		ST_ON_WALL,
		ST_SPIN,
		ST_TALK,
		ST_CRAZED_CRATE,
		ST_WAIT_QUICKSAND,
		ST_LEVEL_ENTER,
		ST_JUMP_QUICKSAND,
		ST_CROUCH,
		ST_RESPAWN,
		ST_CRAWL,
		ST_BACK_FLIP,
		ST_OPENING_WAKE_UP,
		ST_LONG_JUMP,
		ST_PUNCH_KICK,
		ST_ENDING_FLY,
		ST_GROUND_POUND,
		ST_DIVE,
		ST_THROW,
		ST_SPIN_BOWSER,
		ST_SWING_PLAYER,
		ST_SWEEP_KICK,
		ST_SLIDE_KICK,
		ST_FIRST_PERSON,
		ST_NULL, // all its functions are "return 1,"
		ST_SWIM,
		ST_WATER_JUMP,
		ST_METAL_WATER_GROUND,
		ST_METAL_WATER_WATER,
		ST_CLIMB,
		ST_HEADSTAND,
		ST_POLE_JUMP,
		ST_HEADSTAND_JUMP;

	u32 unk0d4;
	u32 unk0d8;
	ModelAnim2* bodyModels[5]; //the fifth one is the Metal Wario model
	ModelAnim balloonModel;
	Model* headModels[4]; //Yoshi's is a ModelAnim
	Model* headNoCapModels[4]; //Yoshi's is the mouth-is-full model
	ModelAnim wings;
	u32 unk1d8;
	TextureSequence texSeq1dc;
	TextureSequence texSeq1f0;
	TextureSequence texSeq204;
	TextureSequence texSeq218;
	MaterialChanger matChg22c;
	MaterialChanger matChg240;
	TextureSequence texSeq254;
	TextureSequence texSeq268;
	char* unk27c[4];
	char* unk28c[4];
	char* unk29c[4];
	ShadowModel shadow;
	MovingCylinderClsnWithPos cylClsn;
	MovingCylinderClsnWithPos cylClsn2;
	Actor* shellPtr;
	Actor* actorInHands;
	u32 unk35c;
	Actor* actorInMouth;
	u32 unk364;
	ActorBase* speaker;
	u32 unk36c;
	State* currState;
	State* prevState;
	State* nextState;
	CylinderClsn* climbedCylClsn; // null if the player isn't climbing on anything
	WithMeshClsn wmClsn;
	Vector3 unk53c;
	Vector3 unk548; //mirrors the player's position?
	u32 unk554;
	u32 unk558;
	u32 unk55c;
	Vector3 wallNormal;
	u32 unk56c;
	u32 unk570;
	u32 unk574;
	char* unk578;
	char* unk57c;
	u32 unk580;
	u32 unk584;
	Actor** eggPtrArr;
	u32 unk58c;
	u32 unk590;
	u32 unk594;
	u32 unk598;
	u32 unk59c;
	u32 unk5a0;
	u32 unk5a4;
	u32 unk5a8;
	u32 unk5ac;
	u32 unk5b0;
	u32 unk5b4;
	u32 unk5b8;
	Matrix4x3 unkMat5bc;
	Matrix4x3 unkMat5ec;
	u32 unk61c;
	u32 unk620;
	u32 playLongUniqueID;
	u32 unk628;
	u32 unk62c;
	u32 unk630;
	u32 unk634;
	u32 unk638;
	u32 animID;
	Fix12i unk640;
	Fix12i floorY;
	u32 unk648;
	u32 unk64c;
	u32 unk650;
	u32 unk654;
	u32 floorTracID;
	u32 floorCamBehavID;
	u32 floorViewID;
	u32 floorBehavID;
	u32 unk668;
	u32 floorTexID;
	u32 floorWindID;
	u32 unk674;
	u32 unk678;
	u32 unk67c;
	u32 unk680;
	Fix12i jumpPeakHeight; // 0x684
	union { s32 msgID; Fix12i yPosOnPole; /* zero at the bottom of the pole */ };
	Fix12i yVisualOffset;
	Fix12i unk690;
	Fix12i unk694;
	u32 unk698;
	s16 spinningAngularVelY; // used for at least turning on poles and spinning Bowser
	s16 unk69e;
	u16 visibilityCounter; // the player is visible when this is even (except when the player is electrocuted the second bit is checked instead)
	s16 unk6a2;
	u16 stateTimer; // sleep, run charge, etc.
	u16 noControlTimer;
	u16 unk6a8; // underwater splash bubbles, tree particles, jump related, etc.
	u16 nextPunchKickDelay;
	u16 walkRunDecelTimer;
	s16 featherCapTimeRemaining; // 0x6AE
	u16 unk6b0;
	u16 squishedTimer;
	u16 unk6b4;
	u16 unk6b6; // climbing related
	u16 unk6b8;
	u16 unk6ba;
	u16 unk6bc;
	u16 powerupTimer;
	u16 balloonTimer;
	u16 superMushroomTimer;
	u16 unk6c4;
	u16 yoshiSwallowTimer;
	u16 warpTimer;
	u16 unk6ca;
	s16 unk6cc;
	s16 flags2; // 0x6ce && 0x6cf
	s16 megaDestroyCounter;
	s16 inputAngle;
	s16 unk6d4;
	s16 unk6d6;
	u8 playerID; // always 0 in single player mode
	u8 realChar; // probably
	u8 unk6da;
	u8 renderedChar;
	u8 prevHatChar; // 0x6DC
	u8 currHatChar; // 0x6DD
	bool isInAir;
	bool landingSoundPlayed;
	u8 unk6e0;
	u8 currJumpNumber; // 0x6E1: 0 - first, 1 - second, 2 - triple jump, more?
	u8 currPunchKickNumber; // 0x6E2: 0 - first, 1 - second, 2 - kick;
	s8 stateState; // 0x6E3: the current state of the current state. How meta.
	u8 unk6e4;
	u8 canFlutterJump; // 0x6E5: needs to be investigated, certainly used for a good amount more than flutter jumps
	u8 slidingState;
	u8 unk6e7;
	u8 unk6e8;
	u8 currClsnState; // 0x06E9: 0 - not colliding, 1 - standing on ground, 2 - colliding with wall in water, 3 - colliding with wall on land
	s16 unk6ea;
	u32 unk6ec;
	u32 unk6f0;
	bool hasFireInMouth;
	bool opacity;
	bool powerupTimersFrozen;
	u8 unk6f7;
	bool isFireYoshi;       // 0x6f8
	bool isMetalWario;      // 0x6f9
	bool usesMetalModel;    // 0x6fa
	bool isVanishLuigi;     // 0x6fb
	bool usesVanishTexture; // 0x6fc
	bool isBalloonMario;    // 0x6fd
	bool usesBalloonModel;  // 0x6fe
	bool isWingMario;       // 0x6ff
	bool usesWingModel;     // 0x700
	u8 unk701;
	u8 unk702;
	bool isMega;
	u8 unk704;
	u8 unk705;
	bool isUnderwater;
	bool standingInPuddle;
	bool isBeingHurt;
	u8 unk709;
	u8 noControlState; // 0x70a
	u8 unk70b;
	u8 currGroundedState;
	bool isLongFalling;
	u8 onSlopedSurface;
	u8 unk70f;
	s16 unk710;
	bool isInAirIsh; // 0x712
	bool isTangible;
	bool unk714;
	u8 unk715;
	bool isIntangibleToMesh;
	u8 unk717;
	u16 unkFlags;
	bool hasNoCap;
	bool quickSandJump;
	u32 unk71c;
	u32 unk720;
	u32 unk724;
	u32 unk728;
	u32 unk72c;
	u32 unk730;
	u32 unk734;
	u32 unk738;
	s16 toonStateAndFlag; //8 possible states, 0x8000 is the invincible-and-can't-collect-caps flag
	s16 unk73e;
	Fix12i toonIntensity;
	Vector3 unk744;
	u32 unk750;
	u32 unk754;
	u32 unk758;
	s16 unk75c;
	s16 spineAngleOffsY; // is added to bodyModels[GetBodyModelID(param1 & 0xff, false)]->data.bones[8].rot.y
	s16 spineAngleOffsZ; // is added to bodyModels[GetBodyModelID(param1 & 0xff, false)]->data.bones[8].rot.z
	u8 unk762;
	u8 unk763;
	u32 unk764;
	
	static SharedFilePtr* ANIM_PTRS[s32{NUM_ANIMS} * NUM_CHARACTERS];
	static_assert(s32{NUM_ANIMS} * NUM_CHARACTERS == 0x308);

	Player();

	virtual s32 InitResources() override;
	virtual s32 CleanupResources() override;
	virtual s32 Behavior() override;
	virtual s32 Render() override;
	virtual void OnPendingDestroy() override;
	virtual ~Player() override;
	virtual u32 OnYoshiTryEat() override;
	
	void IncMegaKillCount();
	void SetNewHatCharacter(u32 character, u32 arg1, bool makeSfx);
	void SetRealCharacter(u32 character);
	void TurnOffToonShading(u32 character);
	
	bool IsInState(const State& state);
	bool WasPreviousState(const State& state);
	bool IsDiving();
	bool IsInsideOfCannon();
	bool IsCollectingCap();
	bool HasNoCap();
	void Unk_020c6a10(u32 arg0);
	bool Unk_020c4f40(u16 newUnk6a6);
	s32 Unk_020ca8f8();
	bool Unk_020c9e5c(u8 arg0);
	bool Unk_020ca488(); // calls Unk_020c9e5c(0xb);
	bool Unk_020ca150(u8 arg0);
	u32 GetBodyModelID(u32 character, bool checkMetalStateInsteadOfMetalModel) const;
	ModelAnim2* GetBodyModel();
	void SetAnim(u32 animID, s32 flags, Fix12i animSpeed, u32 startFrame);
	void UpdateAnim();
	bool ShowMessage(ActorBase& speaker, u32 msgIndex, const Vector3* lookAt, u32 arg3, u32 arg4);
	bool ShowMessage2(ActorBase& speaker, u32 msgIndex, const Vector3* lookAt, u32 arg3, u32 arg4);
	bool StartTalk(ActorBase& speaker, bool noButtonNeeded); //true iff the talk actually started.
	s32 GetTalkState();
	bool HasFinishedTalking();
	s32 GetHurtState();
	bool IsOnShell(); // if not on shell, reset shell ptr
	bool IsEnteringLevel(); // entering entrance, not entering exit
	bool IsBeingShotOutOfCannon();
	bool CanPause();
	void Burn();
	void Shock(u32 damage);
	void RegisterEggCoinCount(u32 numCoins, bool includeSilverStar, bool includeBlueCoin);
	//speed is multiplied by constant at 0x020ff128+charID*2 and divided by 50 (? could be 25, could be 100).
	void Hurt(const Vector3& source, u32 damage, Fix12i speed, u32 arg4, u32 presetHurt, u32 spawnOuchParticles); // speed is multiplied by constant at 0x020ff128+charID*2 and divided by 50 (? could be 25, could be 100).
	void Heal(s32 health);
	s32 GetHealth();
	void Bounce(Fix12i bounceInitVel);
	void SpinBounce(Fix12i bounceInitVel);
	bool ChangeState(Player::State& state);
	bool JumpIntoBooCage(Vector3& cagePos);
	bool EnterWhirlpool();
	void BlowAway(s16 dir);
	bool IsInAirAndAirIsh();
	bool CanWarp();
	bool IsStateEnteringLevel();
	bool SetNoControlState(u8 arg0, s32 messageID, u8 arg2);
	bool TryTalkToKeyDoor();
	bool TryTalkToDoor(u8 arg0);
	void OpenBigDoor();
	bool CanEnterDoor(u8 newStateState);
	bool TryExitWhiteDoorWithStar();
	bool TryExitCharacterDoorWithIntro();
	bool IsOpeningDoorWithStar();
	bool TryEnterStarDoor(Vector3& doorPos, s16 doorAng);
	void PlayMammaMiaSound();
	bool TryGrab(Actor& actor);
	bool DropActor();
	bool FinishedAnim();

	void Unk_020bf13c();
	void UpdateSwimmingClsn(CylinderClsn& cylClsn);
	void SlidingDust();
	void PlayerLandingDust();
	void SetStomachOrButtSlide(u8 slideCondition);
	bool ShouldSlide();
	bool DecelerateSlide(Fix12i minSlideSpeed);
	bool CheckHitWall(u8 unitsForward, u8 unitsAbove);
	bool CheckSideStep(s16 wallAngle);
	bool SetWallSlideOrBounceBack(s16 wallAngle);
	s32 ApplySlopeTransform();
	void InitBonk(s16 bonkAngle);
	bool ShouldBonk(s16 wallAngle);
	bool CheckBonkOrWallSlide();
	void UpdateFloorCollision();
	void PlayJumpLandSound();
	void WadingRipples(Fix12i speedToCompare);
	void UpdateCameraZoom();
	bool IsHangingFromCeiling();
	bool TryLedgeHang(Fix12i maxGrabHeight, bool facingAway);
	bool ShouldLedgeHang();
	void SetPunchKickAttack(u8 punchKickNumber);
	bool CheckLedgeHangOrGrab();
	bool NotAboveFloor();
	bool SetCrouchJumpAction();
	bool SetCrouchAttackAction();
	bool SetLandingState(u8 stateCondition);
	bool CheckHoldingActor();
	bool CheckYoshiSwallow();
	void Unk_020d8158();
	bool CheckJumpOnPlayer();
	void UpdatePlayerScale();
	void InitGroundPoundCylClsn2();
	void InitPunchKickCylClsn2();
	void AdjustSlideAngle();
	bool CheckGroundPoundPlayer(); //Multiplayer only
	s32 SetDiveOrKick();
	bool IsFlying();
	void SetJumpLandingAnim();
	bool ShouldUseCrazedCrate(Actor* actor);
	void PlayBackflipLandVoice();
	bool SetMidairAction();
	bool ShouldGetStuckInGround();
	u8 GetLandingType();
	void UpdatePlayerModel();
	static bool CheckJumpOnActor(WithMeshClsn& wmClsn, Actor& groundpounder);
	static bool CheckShotIntoActor(WithMeshClsn& wmClsn, Actor& groundpounder);
	static bool CheckGroundPoundOnActor(WithMeshClsn& wmClsn, Actor& groundpounder);
	
	bool IsInAnim(u32 animID);
	bool IsFrontSliding();
	bool LostGrabbedObject();

	s32 CallKuppaScriptInstruction(char* instruction, s16 minFrame, s16 maxFrame);

	void InitWingFeathers(bool setPowerUpFlag);
	void InitBalloonMario();
	void InitVanishLuigi();
	void InitMetalWario();
	void InitFireYoshi();

	void CleanupWingFeathers();
	void CleanupSuperMushroom();
	void CleanupMetalWario();
	void CleanupVanishLuigi();
	void CleanupKoopaShell();
	void CleanupFireYoshi();
	void CleanupBalloonMario();

	s32 St_LedgeHang_Init();
	s32 St_LedgeHang_Main();
	s32 St_LedgeHang_Cleanup();
	s32 St_LedgeGrab_Init();
	s32 St_LedgeGrab_Main();
	s32 St_CeilingGrate_Init();
	s32 St_CeilingGrate_Main();
	s32 St_YoshiPower_Init();
	s32 St_YoshiPower_Main();
	s32 St_YoshiPower_Cleanup();
	s32 St_Swallow_Init();
	s32 St_Swallow_Main();
	s32 St_InYoshiMouth_Init();
	s32 St_InYoshiMouth_Main();
	s32 St_InYoshiMouth_Cleanup();
	s32 St_SmallLaunchUp_Init();
	s32 St_SmallLaunchUp_Main();
	s32 St_Hurt_Init();
	s32 St_Hurt_Main();
	s32 St_Hurt_Cleanup();
	s32 St_HurtWater_Init();
	s32 St_HurtWater_Main();
	s32 St_Electrocute_Init();
	s32 St_Electrocute_Main();
	s32 St_BurnFire_Init();
	s32 St_BurnFire_Main();
	s32 St_BurnLava_Init();
	s32 St_BurnLava_Main();
	s32 St_DeadHit_Init();
	s32 St_DeadHit_Main();
	s32 St_DeadPit_Init();
	s32 St_DeadPit_Main();
	s32 St_Walk_Init();
	s32 St_Walk_Main();
	s32 St_Wait_Init();
	s32 St_Wait_Main();
	s32 St_Wait_Cleanup();
	s32 St_Grabbed_Init();
	s32 St_Grabbed_Main();
	s32 St_Grabbed_Cleanup();
	s32 St_TurnAround_Init();
	s32 St_TurnAround_Main();
	s32 St_Jump_Init();
	s32 St_Jump_Main();
	s32 St_Fall_Init();
	s32 St_Fall_Main();
	s32 St_Thrown_Init();
	s32 St_Thrown_Main();
	s32 St_Thrown_Cleanup();
	s32 St_SideFlip_Init();
	s32 St_SideFlip_Main();
	s32 St_SlideKickRecover_Init();
	s32 St_SlideKickRecover_Main();
	s32 St_Fly_Init();
	s32 St_Fly_Main();
	s32 St_NoControl_Init();
	s32 St_NoControl_Main();
	s32 St_NoControl_Cleanup();
	s32 St_Owl_Init();
	s32 St_Owl_Main();
	s32 St_Owl_Cleanup();
	s32 St_DebugFly_Init();
	s32 St_DebugFly_Main();
	s32 St_WindCarry_Init();
	s32 St_WindCarry_Main();
	s32 St_Balloon_Init();
	s32 St_Balloon_Main();
	s32 St_Balloon_Cleanup();
	s32 St_Teleport_Init();
	s32 St_Teleport_Main();
	s32 St_TornadoSpin_Init();
	s32 St_TornadoSpin_Main();
	s32 St_Cannon_Init();
	s32 St_Cannon_Main();
	s32 St_Cannon_Cleanup();
	s32 St_Squish_Init();
	s32 St_Squish_Main();
	s32 St_Squish_Cleanup();
	s32 St_Shell_Init();
	s32 St_Shell_Main();
	s32 St_Shell_Cleanup();
	s32 St_StomachSlide_Init();
	s32 St_StomachSlide_Main();
	s32 St_ButtSlide_Init();
	s32 St_ButtSlide_Main();
	s32 St_DizzyStars_Init();
	s32 St_DizzyStars_Main();
	s32 St_DizzyStars_Cleanup();
	s32 St_HoldLight_Init();
	s32 St_HoldLight_Main();
	s32 St_HoldLight_Cleanup();
	s32 St_Bonk_Init();
	s32 St_Bonk_Main();
	s32 St_HoldHeavy_Init();
	s32 St_HoldHeavy_Main();
	s32 St_WallSlide_Init();
	s32 St_WallSlide_Main();
	s32 St_BowserEarthquake_Init();
	s32 St_BowserEarthquake_Main();
	s32 St_WallJump_Init();
	s32 St_WallJump_Main();
	s32 St_SlopeJump_Init();
	s32 St_SlopeJump_Main();
	s32 St_StuckInGround_Init();
	s32 St_StuckInGround_Main();
	s32 St_Land_Init();
	s32 St_Land_Main();
	s32 St_OnWall_Init();
	s32 St_OnWall_Main();
	s32 St_Spin_Init();
	s32 St_Spin_Main();
	s32 St_Spin_Cleanup();
	s32 St_Talk_Init();
	s32 St_Talk_Main();
	s32 St_Talk_Cleanup();
	s32 St_CrazedCrate_Init();
	s32 St_CrazedCrate_Main();
	s32 St_CrazedCrate_Cleanup();
	s32 St_WaitQuicksand_Init();
	s32 St_WaitQuicksand_Main();
	s32 St_LevelEnter_Init();
	s32 St_LevelEnter_Main();
	s32 St_LevelEnter_Cleanup();
	s32 St_JumpQuicksand_Init();
	s32 St_JumpQuicksand_Main();
	s32 St_Crouch_Init();
	s32 St_Crouch_Main();
	s32 St_Respawn_Init();
	s32 St_Respawn_Main();
	s32 St_Crawl_Init();
	s32 St_Crawl_Main();
	s32 St_BackFlip_Init();
	s32 St_BackFlip_Main();
	s32 St_OpeningWakeUp_Init();
	s32 St_OpeningWakeUp_Main();
	s32 St_LongJump_Init();
	s32 St_LongJump_Main();
	s32 St_PunchKick_Init();
	s32 St_PunchKick_Main();
	s32 St_EndingFly_Init();
	s32 St_EndingFly_Main();
	s32 St_GroundPound_Init();
	s32 St_GroundPound_Main();
	s32 St_GroundPound_Cleanup();
	s32 St_Dive_Init();
	s32 St_Dive_Main();
	s32 St_Throw_Init();
	s32 St_Throw_Main();
	s32 St_BowserSpin_Init();
	s32 St_BowserSpin_Main();
	s32 St_BowserSpin_Cleanup();
	s32 St_SwingPlayer_Init();
	s32 St_SwingPlayer_Main();
	s32 St_SwingPlayer_Cleanup();
	s32 St_SweepKick_Init();
	s32 St_SweepKick_Main();
	s32 St_SlideKick_Init();
	s32 St_SlideKick_Main();
	s32 St_FirstPerson_Init();
	s32 St_FirstPerson_Main();
	s32 St_FirstPerson_Cleanup();
	s32 St_Null_Init();
	s32 St_Null_Main();
	s32 St_Null_Cleanup();
	s32 St_Swim_Init();
	s32 St_Swim_Main();
	s32 St_Swim_Cleanup();
	s32 St_WaterJump_Init();
	s32 St_WaterJump_Main();
	s32 St_MetalWaterGround_Init();
	s32 St_MetalWaterGround_Main();
	s32 St_MetalWaterGround_Cleanup();
	s32 St_MetalWaterWater_Init();
	s32 St_MetalWaterWater_Main();
	s32 St_MetalWaterWater_Cleanup();
	s32 St_Climb_Init();
	s32 St_Climb_Main();
	s32 St_Climb_Cleanup();
	s32 St_Headstand_Init();
	s32 St_Headstand_Main();
	s32 St_Headstand_Cleanup();
	s32 St_PoleJump_Init();
	s32 St_PoleJump_Main();
	s32 St_HeadstandJump_Init();
	s32 St_HeadstandJump_Main();

	template<class ID>
	bool CustomStateFunc();

	void ChangeArea(s32 newAreaID)
	{
		areaID = newAreaID;
		::ChangeArea(newAreaID);
	}

	bool IsWarping() const
	{
		return currState == &ST_NO_CONTROL && stateState == 6;
	}
};

static_assert(sizeof(Player) == 0x768, "sizeof(Player) is incorrect!");
static_assert(Player::NUM_ANIMS == 194, "Player::NUM_ANIMS is incorrect!");
static_assert(Player::NUM_CHARACTERS == 4, "Player::NUM_CHARACTERS is incorrect!");
