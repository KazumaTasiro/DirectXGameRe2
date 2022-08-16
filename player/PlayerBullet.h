#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "DebugText.h"
#include "Input.h"
#include "ViewProjection.h"
#include "Afin.h"

///<summary>
///���L����
///</summary>
class PlayerBullet {
public:
	///<summary>
	///������
	///</summary>

	void Initialize(Model* model, const Vector3& position);;



	///<summary>
	///�X�V
	///</summary>

	void Update();

	///<summary>
	///�`��
	///</summary>

	void Draw(const ViewProjection& viewProjection_);

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;
};