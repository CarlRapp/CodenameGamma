#ifndef MYSKINNEDDATA_H
#define MYSKINNEDDATA_H

#include "..\..\..\stdafx.h"
#include <map>
#include "Mesh.h"
#include "Vertex.h"

#include <vector>

using namespace std;

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
	/*
	std::vector<PosKeyframe> PosKeyframes; 
	std::vector<ScaleKeyframe> ScaleKeyframes; 
	std::vector<RotationKeyframe> RoatationKeyframes; 
	*/
};

struct AnimationNode
{
	std::vector<std::string> bones;
};

struct AnimationData
{
	string	clipName;
	bool	loop;

	AnimationData()
	{
		clipName = "";
		loop = false;
	}
};

struct Animation
{
	std::string	ClipName;
	float	TimePos;
	float	AnimationSpeed;
	bool	Loop;
	int		numKeyFrames;

	Animation* nextAnimation;

	Animation()
	{
		TimePos			= 0.0f;
		AnimationSpeed	= 1.0f;
		nextAnimation	= 0;
	}

	bool operator==(Animation B)
	{
		return ClipName == B.ClipName;
	}
};

///<summary>
/// Examples of AnimationClips are "Walk", "Run", "Attack", "Defend".
/// An AnimationClip requires a BoneAnimation for every bone to form
/// the animation clip.    
///</summary>
struct AnimationClip
{
	int	  FirstBone;
	int	  numKeyFrames;

	float GetClipStartTime()const;
	float GetClipEndTime()const;

    void Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const;
	void Interpolate(float t, std::vector<int>& bones, std::vector<XMFLOAT4X4>& boneTransforms)const;

    std::vector<BoneAnimation> BoneAnimations; 				//ta bort
	//std::map<std::string, BoneAnimation> BoneAnimations;	//använd

	AnimationClip()
	{
		FirstBone = 0;
	}

};

/*
struct Pose
{
    std::vector<BoneAnimation> BoneAnimations;
};
*/

struct Joint
{
	XMFLOAT3 position;
	int parent;
};


class SkinnedData
{
private:
    // Gives parentIndex of ith bone.
	std::vector<int> mBoneHierarchy;

	std::vector<XMFLOAT4X4> mBoneOffsets;
   
	std::map<std::string, AnimationClip>	mAnimations;
	//std::map<std::string, Pose>				mPoses;

	std::map<std::string, int>				mNameToBoneIndex;
	std::map<int, std::string>				mBoneIndexToName;
	std::map<std::string, Joint>			mJoints;

	bool IsChildOf(int child, int parent);
	void GetChildrenBones(int boneIndex, std::vector<int>& bones);

public:

	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	bool  HasAnimation(std::string name)
	{
		return (mAnimations.find(name) != mAnimations.end() && !mBoneHierarchy.empty());
	}

	int	  GetAnimationFirstBone(std::string name)
	{
		if (mAnimations.find(name) != mAnimations.end())
		{
			return mAnimations[name].FirstBone;
		}
		return -1;
	}

	int   GetNumberOfKeyFrames(std::string name)
	{
		if (mAnimations.find(name) != mAnimations.end())
		{
			return mAnimations[name].numKeyFrames;
		}
		return 0;
	}

	bool  HasAnimations()
	{
		return (!mAnimations.empty() && !mBoneHierarchy.empty());
	}
	/*
	bool  HasPose(std::string name)
	{
		return mPoses.find(name) != mPoses.end();
	}
	*/
	AnimationClip* GetAnimation(std::string name) 
	{ 
		if (HasAnimation(name))
			return &mAnimations[name];
		else
			return NULL;
	}

	std::string GetBoneName(UINT i)
	{
		if (i < mBoneIndexToName.size())
			return 	mBoneIndexToName[i];
		return "";
	}

	void Set(
		std::vector<int>& boneHierarchy, 
		std::vector<XMFLOAT4X4>& boneOffsets,
		std::map<std::string, AnimationClip>& animations,
		std::map<std::string, int>		NameToBoneIndex,
		std::map<int, std::string>		BoneIndexToName,
		std::map<std::string, Joint>	Joints);

	 // In a real project, you'd want to cache the result if there was a chance
	 // that you were calling this several times with the same clipName at 
	 // the same timePos.
    void GetFinalTransforms(const std::string& clipName, float timePos, 
		 std::vector<XMFLOAT4X4>& finalTransforms)const;

	void GetFinalTransforms(std::vector<Animation*>& animations, 
		 std::vector<XMFLOAT4X4>& finalTransforms);

	//void GetFinalTransforms(const std::string& clipName, 
	//	 std::vector<XMFLOAT4X4>& finalTransforms)const;

	void CreateClip(std::string, int firstFrame, int lastFrame, float TimeScale, int FirstBone);
	void CreateClip(std::string name1, std::string name2, std::string name3, float TimeScale, int FirstBone);
	void CreatePose(std::string, int frame, int FirstBone);

	std::vector<BoundingOrientedBox> CreateBoneBoxes(std::vector<Vertex::PosNormalTexTanSkinned>& vertices)
	{
		std::vector<BoundingOrientedBox> result;

		int numBones = BoneCount();

		if (numBones > 0)
		{
			if (mAnimations.size() > 0)
			{
				//std::vector<XMFLOAT4X4> finalTransforms(BoneCount());
				//GetFinalTransforms("ALL", 0.0f, finalTransforms);			

				for (int i = 0; i < numBones; ++i)
				{
					std::vector<Vertex::PosNormalTexTanSkinned> vertexPart;

					for each (Vertex::PosNormalTexTanSkinned v in vertices)
					{
						if (v.BoneIndex[0] == i || v.BoneIndex[1] == i || v.BoneIndex[2] == i || v.BoneIndex[3] == i)						
							vertexPart.push_back(v);						
					}
					BoundingBox AABB;
					BoundingOrientedBox OBB;
					BoundingBox::CreateFromPoints(AABB, vertexPart.size(), &vertexPart[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));
					BoundingOrientedBox::CreateFromBoundingBox(OBB, AABB);

					//BoundingOrientedBox::CreateFromPoints(OBB, vertexPart.size(), &vertexPart[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));

					//OBB.Transform(OBB, XMLoadFloat4x4(&finalTransforms[i]));

					result.push_back(OBB);
				}
			}
		}
		/*
		else
		{
			BoundingOrientedBox OBB;
			BoundingBox::CreateFromPoints(AABB, vertices.size(), &vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));
			BoundingOrientedBox::CreateFromBoundingBox(OBB, AABB);

			result.push_back(OBB);
		}

		*/

		return result;
	}

	void GetJointData(std::string name, int& bone, XMFLOAT3& pos)
	{
		pos = XMFLOAT3(0,0,0);

		//XMFLOAT4 tempPos = XMFLOAT4(0,0,0,1);

		if (mJoints.find(name) != mJoints.end())
		{
			Joint joint = mJoints[name];
			pos = joint.position;

			if (mNameToBoneIndex.find(name) != mNameToBoneIndex.end())
				bone = mNameToBoneIndex[name];
			else
				bone =  joint.parent;
		}

		else
			bone = -1;
	}

	XMFLOAT4X4 mGlobalInverseTransform;



};
 
#endif // SKINNEDDATA_H