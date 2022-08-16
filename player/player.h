#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "DebugText.h"
#include "Input.h"
#include "ViewProjection.h"

///<summary>
///���L����
///</summary>
class Player {
public:

	Player();
	~Player();

	///<summary>
	///������
	///</summary>

	void Initialize(Model* model, uint32_t textureHandle);



	///<summary>
	///�X�V
	///</summary>

	void Update();
	void Move();
	void Afin(WorldTransform& worldTransform_);

	///<summary>
	///�`��
	///</summary>

	void Draw(ViewProjection& viewProjection_);

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;

	Input* input_ = nullptr;

	DebugText* debugText_ = nullptr;

	////���[���h�g�����X�t�H�[��
	//WorldTransform worldTransform_;

	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;

};