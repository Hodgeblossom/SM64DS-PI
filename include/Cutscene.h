#ifndef SM64DS_CUTSCENE_INCLUDED
#define SM64DS_CUTSCENE_INCLUDED

#include "Math.h"
#include <cstdint>
#include <concepts>
#include <array>
#include <bit>

struct Camera;
struct Player;
struct FaderWipe;

template<class T>
using KS_MemberFuncPtr = s32(T::*)(char* params, s16 minFrame, s16 maxFrame);

extern "C"
{
	bool RunKuppaScript(char* address);
	void EndKuppaScript();
	void ProcessKuppaScript();
	void ResetKuppaScript();
	bool ContinueKuppaScriptIfNecessary();

	void StartIntroCutscene();
	void DisableSoundPlayersForCredits();

	// The array of player functions (resp. camera functions) is initialized when the first player
	// instruction (resp. camera instruction) is run. Functions in both arrays usually return 1.

	extern KS_MemberFuncPtr<Player> KS_PLAYER_FUNCTIONS[14];
	extern KS_MemberFuncPtr<Camera> KS_CAMERA_FUNCTIONS[39];

	extern u32 KS_PLAYER_IDS[4];
	extern u32 CUTSCENE_OBJECT_IDS[16];
	extern u32 KS_FRAME_COUNTER;
	extern u8 KS_NUM_PLAYERS;
	extern FaderWipe* KS_FADER;

	extern char* RUNNING_KUPPA_SCRIPT; // nullptr if no script is running
	extern char* SAVED_KUPPA_SCRIPT;

	extern char INTRO_CUTSCENE[];
	extern char FALL_IN_FRONT_OF_CASTLE_CUTSCENE[];
	extern char BOB_CREDITS_CUTSCENE[];
}

template<std::size_t size, class... Initializers> requires(size > 0)
struct KuppaScript
{
	std::array<char, size> data;

	consteval explicit KuppaScript(const decltype(data)& data):
		data(data)
	{}

	constexpr       char* Raw()       { return data.data(); }
	constexpr const char* Raw() const { return data.data(); }

	bool Run() &
	{
		(Initializers{}(data.data()), ...);

		return RunKuppaScript(data.data());
	}

	static void Stop() { EndKuppaScript(); }

	bool IsRunning() const { return RUNNING_KUPPA_SCRIPT == data.data(); }
};

namespace KuppaScriptImpl {

template<u8 id>
struct CharID_Type
{
	static constexpr u8 instructionID = id;
};

template<class T>
concept CharID = std::same_as<T, CharID_Type<T::instructionID>>;

template<template<std::size_t> class>
struct DefaultCharImpl { using Type = void; };

template<template<std::size_t> class Compiler>
using DefaultChar = DefaultCharImpl<Compiler>::Type;

template<class T, std::size_t s1, std::size_t s2>
consteval auto operator+(const std::array<T, s1>& a1, const std::array<T, s2>& a2)
{
	std::array<T, s1 + s2> res;

	for (std::size_t i = 0; i < a1.size();++i)
		res[i] = a1[i];

	for (std::size_t i = 0; i < a2.size();++i)
		res[a1.size() + i] = a2[i];

	return res;
}

consteval auto ToByteArray(const auto&... args)
{
	if constexpr (sizeof...(args) == 0)
		return std::array<char, 0> {};
	else
		return (std::bit_cast<std::array<char, sizeof(args)>>(args) + ...);
}

template<
	template<std::size_t scriptSize, class... Initializers> class DerivedCompiler,
	std::size_t scriptSize,
	class... Initializers
>
class BaseScriptCompiler
{
	const std::array<char, scriptSize> precedingScript;

public:
	template<std::size_t paramListSize, class... NewInitializers>
	struct PendingInstruction
	{
		u8 id;
		std::array<char, scriptSize> precedingScript;
		std::array<char, paramListSize> params;

		static constexpr std::size_t size = 6 + paramListSize;
		static_assert(size < 256, "A Kuppa Script command is too long");

		consteval auto operator()(s16 minFrame, s16 maxFrame) const
		{
			return DerivedCompiler<scriptSize + size, Initializers..., NewInitializers...>
			{
				precedingScript
				+ std::to_array<char>({size, id})
				+ ToByteArray(minFrame, maxFrame)
				+ params
			};
		}

		consteval auto operator()(s16 frame) const
		{
			return operator()(frame, frame);
		}
	};

	consteval BaseScriptCompiler() requires(scriptSize == 0) = default;

	consteval BaseScriptCompiler(const decltype(precedingScript)& precedingScript) requires(scriptSize > 0):
		precedingScript(precedingScript)
	{}

	consteval auto End()
	{
		return KuppaScript<scriptSize + 1, Initializers...>{precedingScript + std::to_array<char>({0})};
	}

	template<u8 id, class... NewInitializers>
	consteval auto Instruction(const auto&... args) -> PendingInstruction<(sizeof(args) + ... + 0), NewInitializers...>
	{
		return {id, precedingScript, ToByteArray(args...)};
	}

	template<CharID Char, u8 subID, class... NewInitializers>
	consteval auto PlayerInstruction(const auto&... args)
	{
		return Instruction<Char::instructionID, NewInitializers...>(subID, args...);
	}

	template<u8 subID, class... NewInitializers>
	consteval auto CamInstruction(const auto&... args)
	{
		return Instruction<4, NewInitializers...>(subID, args...);
	}

	// Set camera target position (coordinates in fxu)
	consteval auto SetCamTarget(Vector3_16 target)
	{
		return CamInstruction<0>(target);
	}

	consteval auto SetCamTarget(s16 x, s16 y, s16 z)
	{
		return SetCamTarget(Vector3_16{x, y, z});
	}

	// Set camera position (coordinates in fxu)
	consteval auto SetCamPos(Vector3_16 pos)
	{
		return CamInstruction<1>(pos);
	}

	consteval auto SetCamPos(s16 x, s16 y, s16 z)
	{
		return SetCamPos(Vector3_16{x, y, z});
	}

	// Set camera target position and position (coordinates in fxu)
	consteval auto SetCamTargetAndPos(Vector3_16 target, Vector3_16 pos)
	{
		return CamInstruction<2>(target, pos);
	}

	consteval auto SetCamTargetAndPos(s16 tx, s16 ty, s16 tz, s16 px, s16 py, s16 pz)
	{
		return SetCamTargetAndPos(Vector3_16{tx, ty, tz}, Vector3_16{px, py, pz});
	}

	// Set camera FOV modifier (not sure if it's exactly the FOV, might be FOV/2)
	consteval auto SetCamFOV(u16 fovModifier)
	{
		return CamInstruction<3>(fovModifier);
	}

	// Adjust camera FOV modifier
	consteval auto AdjustCamFOV(u16 targetModifier, u16 speed)
	{
		return CamInstruction<4>(targetModifier, speed);
	}

	// Adjust screen size from full size to target values (uses minFrame and maxFrame for gradient)
	consteval auto AdjustCamScreenSize(u8 left, u8 bottom, u8 right, u8 top)
	{
		return CamInstruction<5>(left, bottom, right, top);
	}

	// Weird cubic interpolation?
	consteval auto UnkCubicInterpolation(u32 unk0, u32 unk1) // pointers?
	{
		return CamInstruction<6>(unk0, unk1);
	}

	// Set a stored value presumably used by the camera
	consteval auto SetStoredFix12(Fix12i newValue)
	{
		return CamInstruction<13>(newValue);
	}

	// The same as above but the type is an int for backwards compatibility
	consteval auto SetStoredFix12(s32 newValue)
	{
		return CamInstruction<13>(newValue);
	}

	consteval auto AdjustStoredFix12(Fix12i target, Fix12i speed)
	{
		return CamInstruction<14>(target, speed);
	}

	// The same as above but types are int for backwards compatibility
	consteval auto AdjustStoredFix12(s32 target, s32 speed)
	{
		return CamInstruction<14>(target, speed);
	}

	// Adjust camera target position via exponential decay
	consteval auto AdjustCamTargetDec(
		Vector3_16 dest,    // the destination vector of the target position
		Vector3_16 approach // approach factors
	)
	{
		return CamInstruction<15>(dest, approach);
	}

	consteval auto AdjustCamTargetDec(
		Vector3_16 dest,       // the destination vector of the target position
		s16 ax, s16 ay, s16 az // approach factors
	)
	{
		return AdjustCamTargetDec(dest, Vector3_16{ax, ay, az});
	}

	consteval auto AdjustCamTargetDec(
		Vector3_16 dest, // the destination vector of the target position
		s16 approach   // single approach factor
	)
	{
		return AdjustCamTargetDec(dest, approach, approach, approach);
	}

	consteval auto AdjustCamTargetDec(
		s16 dx, s16 dy, s16 dz, // the destination coordinates of the target position
		s16 ax, s16 ay, s16 az  // approach factors
	)
	{
		return AdjustCamTargetDec(Vector3_16{dx, dy, dz}, Vector3_16{ax, ay, az});
	}

	consteval auto AdjustCamTargetDec(
		s16 dx, s16 dy, s16 dz, // the destination coordinates of the target position
		s16 approach            // single approach factor
	)
	{
		return AdjustCamTargetDec(Vector3_16{dx, dy, dz}, approach);
	}

	// Adjust camera position via exponential decay
	consteval auto AdjustCamPosDec(
		Vector3_16 dest,    // the destination vector of the target position
		Vector3_16 approach // approach factors
	)
	{
		return CamInstruction<16>(dest, approach);
	}

	consteval auto AdjustCamPosDec(
		Vector3_16 dest,       // the destination vector of the target position
		s16 ax, s16 ay, s16 az // approach factors
	)
	{
		return AdjustCamPosDec(dest, Vector3_16{ax, ay, az});
	}

	consteval auto AdjustCamPosDec(
		Vector3_16 dest, // the destination vector of the target position
		s16 approach   // single approach factor
	)
	{
		return AdjustCamPosDec(dest, approach, approach, approach);
	}

	consteval auto AdjustCamPosDec(
		s16 dx, s16 dy, s16 dz, // the destination coordinates of the position
		s16 ax, s16 ay, s16 az  // approach factors
	)
	{
		return AdjustCamPosDec(Vector3_16{dx, dy, dz}, Vector3_16{ax, ay, az});
	}

	consteval auto AdjustCamPosDec(
		s16 dx, s16 dy, s16 dz, // the destination coordinates of the position
		s16 approach            // single approach factor
	)
	{
		return AdjustCamPosDec(Vector3_16{dx, dy, dz}, approach);
	}

	// Set stored angle toward pause view position and the stored shorts to 0
	consteval auto ResetCamToPause()
	{
		return CamInstruction<17>();
	}

	// Adjust camera target position to offset from owner via exponential decay
	consteval auto SetCamTargetRelativeDec(Vector3_16 offset, u8 approachFactorLsl8)
	{
		return CamInstruction<18>(offset, approachFactorLsl8);
	}

	consteval auto SetCamTargetRelativeDec(s16 x, s16 y, s16 z, u8 approachFactorLsl8)
	{
		return SetCamTargetRelativeDec(Vector3_16{x, y, z}, approachFactorLsl8);
	}

	// Adjust camera target position to offset from owner rotated by owner's facing angle via exponential decay
	consteval auto AdjustCamByOwnerAngleDec(Vector3_16 offset, u8 approachFactorLsl8)
	{
		return CamInstruction<19>(offset, approachFactorLsl8);
	}

	consteval auto AdjustCamByOwnerAngleDec(s16 x, s16 y, s16 z, u8 approachFactorLsl8)
	{
		return AdjustCamByOwnerAngleDec(Vector3_16{x, y, z}, approachFactorLsl8);
	}

	// Adjust camera position and angles relative to owner position via exponential decay
	consteval auto CamPosAngleRelativeDec(
		u16 targetDist,           // target distance (in fxu)
		u8  approachFactorLsl8,   // approach factor multiplied by 256
		s16 targetVertAngle,      // target vertical angle
		u8  invVertAngleApproach, // 1 divided by the vertical angle's approach factor (Set to 0 to not change vertical angle)
		s16 targetHorzAngle,      // Target horizontal angle (Set to -1 to use CAMERA->targetAngle),
		u8  invHorzAngleApproach  // 1 divided by the horizontal angle's approach factor (Set to 0 to not change horizontal angle)
	)
	{
		return CamInstruction<20>(
			targetDist, approachFactorLsl8,
			targetVertAngle, invVertAngleApproach,
			targetHorzAngle, invHorzAngleApproach
		);
	}

	// Spin camera target position around camera position
	// Sets a stored vector to the target position on the first frame
	consteval auto SpinCamTarget(s16 vertAngularSpeed, s16 horzAngularSpeed)
	{
		return CamInstruction<21>(vertAngularSpeed, horzAngularSpeed);
	}

	// Spin the camera position around its owner position
	consteval auto SpinCamAroundOwnerPos(s32 speed, s16 vertAngularSpeed, s16 horzAngularSpeed)
	{
		return CamInstruction<22>(speed, vertAngularSpeed, horzAngularSpeed);
	}

	// Adjust FOV modifier via exponential decay (invApproachFactor = 1 / approachFactor)
	consteval auto AdjustCamFOVDec(u16 targetModifier, u8 invApproachFactor)
	{
		return CamInstruction<23>(targetModifier, invApproachFactor);
	}

	// Adjust camera FOV if new value is greater than the old value
	consteval auto AdjustCamFOVIfBigger(
		u16 newModifier,
		s16 fovSpeedTowards0,
		s16 fovOscillationAngularSpeed
	)
	{
		return CamInstruction<24>(
			newModifier,
			fovSpeedTowards0,
			fovOscillationAngularSpeed
		);
	}

	// Not actually a nop, but something to do with angles
	consteval auto NopCam()
	{
		return CamInstruction<25>();
	}

	// Approach a distance of 800 fxu (approach factor = 1/20),
	// 33.75° above the player (approach factor = 1/16), with the
	// horizontal angle being the stored angle (approach factor = 1/8)
	consteval auto CamApproachPlayerFromTop()
	{
		return CamInstruction<26>();
	}

	// Set camera target position and position to rotated offset from owner (in fxu)
	consteval auto SetCamTargetAndPosRotatedFromOwner(Vector3_16 target, Vector3_16 pos)
	{
		return CamInstruction<27>(target, pos);
	}

	consteval auto SetCamTargetAndPosRotatedFromOwner(s16 tx, s16 ty, s16 tz, s16 px, s16 py, s16 pz)
	{
		return SetCamTargetAndPosRotatedFromOwner(Vector3_16{tx, ty, tz}, Vector3_16{px, py, pz});
	}

	// Waits for a button or touchscreen input and returns to the title screen (previously named "Wifi" for some reason)
	consteval auto WaitAndLoadTitleScreen() { return Instruction<5>(); }

	// Change cutscene script (by raw address)
	consteval auto ChangeScript(u32 newScriptAddress)
	{
		return Instruction<6>(newScriptAddress);
	}

	// Change music
	consteval auto ChangeMusic(u32 musicID)
	{
		return Instruction<7>(musicID);
	}

	// Play sound from SSAR 1 (player voices). [].
	consteval auto PlaySoundSSAR1(u32 soundID)
	{
		return Instruction<8>(soundID);
	}
	
	// Play sound from SSAR 2 (system)
	consteval auto PlaySoundSSAR2(u32 soundID)
	{
		return Instruction<9>(soundID);
	}
	
	// Display a message
	consteval auto DisplayMessage(u16 messageID)
	{
		return Instruction<10>(messageID);
	}

	// Change level (with raw cutscene address)
	consteval auto ChangeLevel(
		u8  newLevelID,
		u8  entranceID,
		u8  starID,
		u32 cutsceneAddress = 0 // 0 if none
	)
	{
		return Instruction<11>(
			newLevelID, entranceID, starID,
			cutsceneAddress
		);
	}

	// Fade to white
	consteval auto FadeToWhite() { return Instruction<12>(); }
	
	// Fade from white
	consteval auto FadeFromWhite() { return Instruction<13>(); }
	
	// Fade to black
	consteval auto FadeToBlack() { return Instruction<14>(); }
	
	// Fade from white, but broken
	consteval auto FadeFromWhiteBroken() { return Instruction<15>(); }
	
	// Fade to black, then fade from black
	consteval auto FadeToBlackAndBack() { return Instruction<16>(); }
	
	// Enables the "Ambient Sound Effects" objects by storing 0 at 0x02110aec (previously named "STZ")
	consteval auto EnableAmbientSoundEffects() { return Instruction<17>(); }

	// Set position and Y-angle (both model and motion angle)
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto SetPlayerPosAndAngleY(Vector3_16 pos, s16 angleY)
	{
		return PlayerInstruction<Char, 0>(pos, angleY);
	}

	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto SetPlayerPosAndAngleY(s16 xPos, s16 yPos, s16 zPos, s16 angleY)
	{
		return SetPlayerPosAndAngleY<Char>(Vector3_16{xPos, yPos, zPos}, angleY);
	}

	// Send input to move the player to a target position (full magnitude: 0x1000)
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto SendPlayerInput(Vector3_16 pos, s16 inputMagnitude)
	{
		return PlayerInstruction<Char, 1>(pos, inputMagnitude);
	}

	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto SendPlayerInput(s16 xPos, s16 yPos, s16 zPos, s16 inputMagnitude)
	{
		return SendPlayerInput<Char>(Vector3_16{xPos, yPos, zPos}, inputMagnitude);
	}

	// Send input to move the player to a target position (full magnitude: 1._fs)
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto SendPlayerInput(Vector3_16 pos, Fix12s inputMagnitude = 1._fs)
	{
		return PlayerInstruction<Char, 1>(pos, inputMagnitude);
	}

	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto SendPlayerInput(s16 xPos, s16 yPos, s16 zPos, Fix12s inputMagnitude = 1._fs)
	{
		return SendPlayerInput<Char>(Vector3_16{xPos, yPos, zPos}, inputMagnitude);
	}

	// Orr player flags with 0x24000000 (previously named OrrPlayerFlags)
	template<class Char = DefaultChar<DerivedCompiler>>
	consteval auto ActivatePlayer()
	{
		return PlayerInstruction<Char, 2>();
	}

	// Make a player lie down
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto MakePlayerLieDown()
	{
		return PlayerInstruction<Char, 3>();
	}

	// Player character voice
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto PlayPlayerVoice(u32 soundID)
	{
		return PlayerInstruction<Char, 4>(soundID);
	}

	// Play sound from SSAR0
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto PlayerPlaySoundSSAR0(u32 soundID)
	{
		return PlayerInstruction<Char, 5>(soundID);
	}

	// Play sound from SSAR3
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto PlayerPlaySoundSSAR3(u32 soundID)
	{
		return PlayerInstruction<Char, 6>(soundID);
	}

	// Press and hold buttons
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto PlayerHoldButtons(u16 buttons)
	{
		return PlayerInstruction<Char, 7>(buttons);
	}

	// Drop the player with a speed of 32 fxu/frame and give him wings for 408 frames
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto GivePlayerWingsAndDrop()
	{
		return PlayerInstruction<Char, 8>();
	}

	// Hurt the player with an imaginary source 80 fxu away
	// If the player is Luigi, spawn ouch stars as well. [DirectionOfSource].
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto HurtPlayer(s16 directionOfSource)
	{
		return PlayerInstruction<Char, 9>(directionOfSource);
	}

	// A weird cap animation
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto AnimatePlayerCap(u8 state)
	{
		return PlayerInstruction<Char, 10>(state);
	}

	// Turn the player via exponential decay. [NewAngle].
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto TurnPlayerDec(s16 newAngleY)
	{
		return PlayerInstruction<Char, 11>(newAngleY);
	}

	// Make the player move forward at a certain speed (does not change animation)
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto MovePlayerForward()
	{
		return PlayerInstruction<Char, 12>();
	}
	
	// Kill the player
	template<CharID Char = DefaultChar<DerivedCompiler>>
	consteval auto KillPlayer()
	{
		return PlayerInstruction<Char, 13>();
	}
};

template<std::size_t scriptSize = 0>
class VanillaScriptCompiler : BaseScriptCompiler<VanillaScriptCompiler, scriptSize> {};

struct DefaultCompilerTag {};

template<DefaultCompilerTag>
struct DefaultScriptCompiler
{
	using Type = VanillaScriptCompiler<>;
};

template<template<DefaultCompilerTag> class T = DefaultScriptCompiler>
consteval T<{}>::Type NewScript()
{
	return {};
}

} // namespace KuppaScriptImpl

using KuppaScriptImpl::NewScript;

template<const auto scriptCompiler>
inline bool Run()
{
	static constinit auto script = scriptCompiler.End();

	return script.Run();
}

using Mario = KuppaScriptImpl::CharID_Type<0>;
using Luigi = KuppaScriptImpl::CharID_Type<1>;
using Wario = KuppaScriptImpl::CharID_Type<2>;
using Yoshi = KuppaScriptImpl::CharID_Type<3>;

#endif // SM64DS_CUTSCENE_INCLUDED
