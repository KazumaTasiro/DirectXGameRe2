#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "ViewProjection.h"
#include <Afin.h>


///<summary>
///�V��
///</summary>
class Skydome {
public:
	///<summary>
	///������
	///</summary>
	void Initalize(Model* model);

	///<summary>
	///�X�V
	///</summary>
	void Update();

	///<summary>
	///�`��
	///</summary>
	void Draw(ViewProjection viewProjection_);

	void Afin(WorldTransform& worldTransform_);

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;

};