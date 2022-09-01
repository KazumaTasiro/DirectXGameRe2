#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "DebugText.h"

///<summary>
/// ������
///</summary>
class RailCamera {
public:
	///<summary>
	/// ������
	///</summary>
	void Initalize();

	///<summary>
	/// �X�V
	///</summary>
	void Update();

	Vector3 MatVector(Matrix4 matrix4, Vector3 vector3);

	void Afin(WorldTransform& worldTransform_);

	 ViewProjection GetViewProjection() { return viewProjection_; }

	 WorldTransform& GetworldTransform() { return worldTransform_; }

	 void Reset();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;

	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;

	//�L�[����
	Input* input_ = nullptr;

	DebugText* debugText_ = nullptr;
};