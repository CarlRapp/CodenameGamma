#include "MySkinnedData.h"


Keyframe::Keyframe()
	: TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f),
	Scale(1.0f, 1.0f, 1.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Keyframe::~Keyframe()
{
}

PosKeyframe::PosKeyframe()
	: TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f)
{
}

PosKeyframe::~PosKeyframe()
{
}

ScaleKeyframe::ScaleKeyframe()
	: TimePos(0.0f),
	Scale(1.0f, 1.0f, 1.0f)
{
}

ScaleKeyframe::~ScaleKeyframe()
{
}

RotationKeyframe::RotationKeyframe()
	: TimePos(0.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

RotationKeyframe::~RotationKeyframe()
{
}

float BoneAnimation::GetStartTime()const
{
	// Keyframes are sorted by time, so first keyframe gives start time.
	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime()const
{
	// Keyframes are sorted by time, so last keyframe gives end time.
	float f = Keyframes.back().TimePos;

	return f;
}

void BoneAnimation::Interpolate(float t, XMFLOAT4X4& M)const
{
	if( t <= Keyframes.front().TimePos )
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if( t >= Keyframes.back().TimePos )
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else
	{
		for(UINT i = 0; i < Keyframes.size()-1; ++i)
		{
			if( t >= Keyframes[i].TimePos && t <= Keyframes[i+1].TimePos )
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i+1].TimePos - Keyframes[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i+1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&Keyframes[i+1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&Keyframes[i+1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}

float AnimationClip::GetClipStartTime()const
{
	// Find smallest start time over all bones in this clip.
	float t = MathHelper::Infinity;
	for(UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for(UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const
{
	for(UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

void AnimationClip::Interpolate(float t, std::vector<int>& bones, std::vector<XMFLOAT4X4>& boneTransforms)const
{
	//for(UINT i = firstBone; i < BoneAnimations.size(); ++i)
	for (int i = 0; i < bones.size(); ++i)
	{
		BoneAnimations[bones[i]].Interpolate(t, boneTransforms[bones[i]]);
	}
}

float SkinnedData::GetClipStartTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipEndTime();
}

UINT SkinnedData::BoneCount()const
{
	return mBoneHierarchy.size();
}

void SkinnedData::Set(std::vector<int>& boneHierarchy, 
		              std::vector<XMFLOAT4X4>& boneOffsets,
		              std::map<std::string, AnimationClip>& animations,
					  std::map<std::string, int>		NameToBoneIndex,
					  std::map<int, std::string>		BoneIndexToName,
					  std::map<std::string, Joint>		Joints)
{
	mBoneHierarchy = boneHierarchy;
	mBoneOffsets   = boneOffsets;
	mAnimations    = animations;

	mNameToBoneIndex = NameToBoneIndex;
	mBoneIndexToName = BoneIndexToName;
	mJoints			 = Joints;
}
 
void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos,  std::vector<XMFLOAT4X4>& finalTransforms)const
{
	UINT numBones = mBoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// Interpolate all the bones of this clip at the given time instance.
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// Traverse the hierarchy and transform all the bones to the root space.
	//

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	// The root bone has index 0.  The root bone has no parent, so its toRootTransform
	// is just its local bone transform.
	toRootTransforms[0] = toParentTransforms[0];

	// Now find the toRootTransform of the children.
	for(UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = mBoneHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// Premultiply by the bone offset transform to get the final transform.
	for(UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));

		//XMStoreFloat4x4(&finalTransforms[i], toRoot);

		//XMStoreFloat4x4(&finalTransforms[i], XMMatrixScaling(1,1,1));
		//XMMATRIX globalInverseTransform = XMLoadFloat4x4(&mGlobalInverseTransform);

		//XMMATRIX temp = XMMatrixMultiply(offset, toRoot);
		//XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(globalInverseTransform, temp));
	}
}

bool SkinnedData::IsChildOf(int child, int parent)
{
	int bone =  child;
	while (bone != -1)
	{
		bone = mBoneHierarchy[bone];

		if (bone == parent)
			return true;
	}
	return false;
}

void SkinnedData::GetChildrenBones(int boneIndex, std::vector<int>& bones)
{
	for (int i = boneIndex; i < mBoneHierarchy.size(); ++i)
	{
		if (IsChildOf(i, boneIndex))
			bones.push_back(i);
	}
}

void SkinnedData::GetFinalTransforms(std::vector<Animation*>& animations, std::vector<XMFLOAT4X4>& finalTransforms)
{
	int numAnimations = animations.size();	//Vi har inga animationer
	if (numAnimations == 0)
		return;
	//else if (numAnimations == 1 && animations[0]->numKeyFrames == 1) //Vi har bara en pose. Inget annat.
	//	return;

	UINT numBones = mBoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);


	std::vector<std::vector<int>> bonesPerAnimation;
	for (int i = 0; i < animations.size(); ++i)
	{
		Animation* animation	= animations[i];
		int FirstBone = GetAnimationFirstBone(animation->ClipName);
		std::vector<int> bones;
		bones.push_back(FirstBone);
		GetChildrenBones(FirstBone, bones);
		bonesPerAnimation.push_back(bones);	
	}

	for (int i = bonesPerAnimation.size() - 1; i >= 0; --i)
	{
		for each (int bone in bonesPerAnimation[i])
		{
			for (int j = i - 1; j >= 0; --j)
			{

				for (int k = 0; k < bonesPerAnimation[j].size(); ++k)
				{
					if (bonesPerAnimation[j][k] == bone)
					{
						bonesPerAnimation[j].erase(bonesPerAnimation[j].begin() + k);
						break;
					}
				}
			}
		}
	}

	for (int i = 0; i < animations.size(); ++i)
	{
		auto clip = mAnimations.find(animations[i]->ClipName);
		clip->second.Interpolate(animations[i]->TimePos, bonesPerAnimation[i], toParentTransforms);
	}

	//
	// Traverse the hierarchy and transform all the bones to the root space.
	//

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	// The root bone has index 0.  The root bone has no parent, so its toRootTransform
	// is just its local bone transform.
	toRootTransforms[0] = toParentTransforms[0];

	// Now find the toRootTransform of the children.
	for(UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = mBoneHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// Premultiply by the bone offset transform to get the final transform.
	for(UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));

		//XMStoreFloat4x4(&finalTransforms[i], toRoot);

		//XMStoreFloat4x4(&finalTransforms[i], XMMatrixScaling(1,1,1));
		//XMMATRIX globalInverseTransform = XMLoadFloat4x4(&mGlobalInverseTransform);

		//XMMATRIX temp = XMMatrixMultiply(offset, toRoot);
		//XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(globalInverseTransform, temp));
	}
}
/*
void SkinnedData::GetFinalTransforms(const std::string& clipName,  std::vector<XMFLOAT4X4>& finalTransforms)const
{
	UINT numBones = mBoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// Interpolate all the bones of this clip at the given time instance.
	auto clip = mPoses.find(clipName);

	for (int i = 0; i < numBones; ++i)
	{
		Keyframe kFrame = clip->second.BoneAnimations[i].Keyframes[0];

		XMVECTOR S = XMLoadFloat3(&kFrame.Scale);
		XMVECTOR P = XMLoadFloat3(&kFrame.Translation);
		XMVECTOR Q = XMLoadFloat4(&kFrame.RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&toParentTransforms[i], XMMatrixAffineTransformation(S, zero, Q, P));
	}

	//
	// Traverse the hierarchy and transform all the bones to the root space.
	//

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	// The root bone has index 0.  The root bone has no parent, so its toRootTransform
	// is just its local bone transform.
	toRootTransforms[0] = toParentTransforms[0];

	// Now find the toRootTransform of the children.
	for(UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = mBoneHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// Premultiply by the bone offset transform to get the final transform.
	for(UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));

		//XMStoreFloat4x4(&finalTransforms[i], toRoot);

		//XMStoreFloat4x4(&finalTransforms[i], XMMatrixScaling(1,1,1));
		//XMMATRIX globalInverseTransform = XMLoadFloat4x4(&mGlobalInverseTransform);

		//XMMATRIX temp = XMMatrixMultiply(offset, toRoot);
		//XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(temp, globalInverseTransform));
	}
}
*/
void SkinnedData::CreateClip(std::string name, int firstFrame, int lastFrame, float TimeScale, int FirstBone)
{
	if (mAnimations.empty())
		return;
	AnimationClip all = mAnimations["ALL"];
	AnimationClip clip;
	clip.FirstBone = FirstBone;
	float offset = all.BoneAnimations[0].Keyframes[firstFrame].TimePos;

	TimeScale = 1.0f / TimeScale;

	if (lastFrame > firstFrame)
	{
		for (UINT i = 0; i < all.BoneAnimations.size(); i++)
		{
			BoneAnimation boneAni;
			for (int j = firstFrame; j <= lastFrame; ++j)
			{
				Keyframe kframe = all.BoneAnimations[i].Keyframes[j];

				kframe.TimePos -= offset;
				kframe.TimePos *= TimeScale;

				boneAni.Keyframes.push_back(kframe);
			}
			clip.BoneAnimations.push_back(boneAni);
		}
	}

	else if (lastFrame < firstFrame)
	{
		for (UINT i = 0; i < all.BoneAnimations.size(); i++)
		{
			BoneAnimation boneAni;
			for (int j = firstFrame; j >= lastFrame; --j)
			{
				Keyframe kframe = all.BoneAnimations[i].Keyframes[j];

				kframe.TimePos = abs( kframe.TimePos - offset );
				kframe.TimePos *= TimeScale;

				boneAni.Keyframes.push_back(kframe);
			}
			clip.BoneAnimations.push_back(boneAni);
		}
	}

	else
		return;

	clip.numKeyFrames = clip.BoneAnimations[0].Keyframes.size();
	mAnimations.insert(std::pair<std::string, AnimationClip>(name, clip));
}

void SkinnedData::CreateClip(std::string name1, std::string name2, std::string name3, float TimeScale, int FirstBone)
{
	if (mAnimations.empty() || !HasAnimation(name2) || !HasAnimation(name3))
		return;
	AnimationClip clip1 = mAnimations[name2];
	AnimationClip clip2 = mAnimations[name3];
	AnimationClip clip;

	clip.FirstBone = FirstBone;

	float offset = GetClipEndTime(name2) * TimeScale;

	TimeScale = 1.0f / TimeScale;

	for (UINT i = 0; i < clip1.BoneAnimations.size(); i++)
	{
		BoneAnimation boneAni;
		for (int j = 0; j < clip1.numKeyFrames; ++j)
		{
			Keyframe kframe = clip1.BoneAnimations[i].Keyframes[j];
			kframe.TimePos *= TimeScale;
			boneAni.Keyframes.push_back(kframe);
		}

		for (int j = 0; j < clip2.numKeyFrames; ++j)
		{
			Keyframe kframe = clip2.BoneAnimations[i].Keyframes[j];
			kframe.TimePos *= TimeScale;
			kframe.TimePos += offset;
			boneAni.Keyframes.push_back(kframe);
		}

		clip.BoneAnimations.push_back(boneAni);
	}
	/*
	for (UINT i = 0; i < clip2.BoneAnimations.size(); i++)
	{
		for (int j = 0; j < clip2.numKeyFrames; ++j)
		{
			Keyframe kframe = clip2.BoneAnimations[i].Keyframes[j];
			kframe.TimePos += offset;
			kframe.TimePos *= TimeScale;
			clip.BoneAnimations[i].Keyframes.push_back(kframe);
		}
	}
	*/
	clip.numKeyFrames = clip.BoneAnimations[0].Keyframes.size();
	mAnimations.insert(std::pair<std::string, AnimationClip>(name1, clip));
}

void SkinnedData::CreatePose(std::string name, int frame, int FirstBone)
{
	if (mAnimations.empty())
		return;

	AnimationClip all = mAnimations["ALL"];

	AnimationClip clip;
	clip.FirstBone = FirstBone;

	float offset = all.BoneAnimations[0].Keyframes[frame].TimePos;

	for (UINT i = 0; i < all.BoneAnimations.size(); i++)
	{
		BoneAnimation boneAni;
		Keyframe kframe = all.BoneAnimations[i].Keyframes[frame];
		kframe.TimePos -= offset;
		boneAni.Keyframes.push_back(kframe);
		clip.BoneAnimations.push_back(boneAni);
	}
	clip.numKeyFrames = clip.BoneAnimations[0].Keyframes.size();
	mAnimations.insert(std::pair<std::string, AnimationClip>(name, clip));	
}
