#ifndef MYSKINNEDDATA_H
#define MYSKINNEDDATA_H

#include "..\..\..\stdafx.h"
#include <map>
#include "Mesh.h"

///<summary>
/// A Keyframe defines the bone transformation at an instant in time.
///</summary>
struct Keyframe
{
	Keyframe();
	~Keyframe();

    float TimePos;
	XMFLOAT3 Translation;
	XMFLOAT3 Scale;
	XMFLOAT4 RotationQuat;
};

struct PosKeyframe
{
	PosKeyframe();
	~PosKeyframe();

    float TimePos;
	XMFLOAT3 Translation;
};

struct ScaleKeyframe
{
	ScaleKeyframe();
	~ScaleKeyframe();

    float TimePos;
	XMFLOAT3 Scale;
};

struct RotationKeyframe
{
	RotationKeyframe();
	~RotationKeyframe();

    float TimePos;
	XMFLOAT4 RotationQuat;
};

///<summary>
/// A BoneAnimation is defined by a list of keyframes.  For time
/// values inbetween two keyframes, we interpolate between the
/// two nearest keyframes that bound the time.  
///
/// We assume an animation always has two keyframes.
///</summary>
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

    void Interpolate(float t, XMFLOAT4X4& M)const;

	std::vector<Keyframe> Keyframes;	//ta bort
	std::vector<PosKeyframe> PosKeyframes; 
	std::vector<ScaleKeyframe> ScaleKeyframes; 
	std::vector<RotationKeyframe> RoatationKeyframes; 
};

struct AnimationNode
{
	std::vector<std::string> bones;
};

///<summary>
/// Examples of AnimationClips are "Walk", "Run", "Attack", "Defend".
/// An AnimationClip requires a BoneAnimation for every bone to form
/// the animation clip.    
///</summary>
struct AnimationClip
{
	float GetClipStartTime()const;
	float GetClipEndTime()const;

    void Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const;

    std::vector<BoneAnimation> BoneAnimations; 				//ta bort
	//std::map<std::string, BoneAnimation> BoneAnimations;	//använd
};

class SkinnedData
{
public:

	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	void Set(
		std::vector<int>& boneHierarchy, 
		std::vector<XMFLOAT4X4>& boneOffsets,
		std::map<std::string, AnimationClip>& animations);

	 // In a real project, you'd want to cache the result if there was a chance
	 // that you were calling this several times with the same clipName at 
	 // the same timePos.
    void GetFinalTransforms(const std::string& clipName, float timePos, 
		 std::vector<XMFLOAT4X4>& finalTransforms)const;

private:
    // Gives parentIndex of ith bone.
	std::vector<int> mBoneHierarchy;
	std::map<std::string, int> mBones;	//använd

	std::vector<XMFLOAT4X4> mBoneOffsets;
   
	std::map<std::string, AnimationClip> mAnimations;
};
 
#endif // SKINNEDDATA_H