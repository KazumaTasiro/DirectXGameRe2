#include "RailCamera.h"

void RailCamera::Initalize()
{
	//���[���h�g�����X�t�H�[���̏�����
	worldTransform_.Initialize();
	//�r���[�v���W�F�N�V�����̏�����
	viewProjection_.Initialize();

	debugText_ = DebugText::GetInstance();

	input_ = Input::GetInstance();
}

void RailCamera::Update()
{
	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 move = { 0,0,0 };
	const float speed = 0.1f;
	const float RotSpeed = 0.05f;
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= speed;
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		move.x += speed;
	}
	if (input_->PushKey(DIK_UP)) {
		move.z += speed;
	}
	else if (input_->PushKey(DIK_DOWN)) {
		move.z -= speed;
	}
	if (input_->PushKey(DIK_R)) {
		worldTransform_.rotation_.y -= RotSpeed;
	}
	else if (input_->PushKey(DIK_T)) {
		worldTransform_.rotation_.y += RotSpeed;
	}
	worldTransform_.translation_ += move;
	Afin(worldTransform_);
	worldTransform_.TransferMatrix();

	viewProjection_.eye = { worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2] };
	//���[���h�O���x�N�g��
	Vector3 forward(0, 0, 1);
	//���[���J�����̉�]�𔽉f
	forward = MatVector(worldTransform_.matWorld_, forward);
	//���_����O���ɓK���ȋ����i�񂾈ʒu�������X
	viewProjection_.target.x = viewProjection_.eye.x + forward.x;
	viewProjection_.target.y = viewProjection_.eye.y + forward.y;
	viewProjection_.target.z = viewProjection_.eye.z + forward.z;

	//���[���h����x�N�g��
	Vector3 up(0, 1, 0);
	//���[���J�����̉�]�𔽉f(���[���J�����̏���x�N�g��)
	viewProjection_.up = MatVector(worldTransform_.matWorld_, up);
	//�r���[�v���W�F�N�V�������X�V
	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();

	//���_�̍��W
	debugText_->SetPos(50, 170);
	debugText_->Printf(
		"eye:(x %f\n,y %f\n,z %f\n)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

}

Vector3 RailCamera::MatVector(Matrix4 matrix4, Vector3 vector3)
{
	Vector3 matVector = { 0,0,0 };

	matVector.x = vector3.x * matrix4.m[0][0] + vector3.y * matrix4.m[1][0] + vector3.z * matrix4.m[2][0];
	matVector.y = vector3.x * matrix4.m[0][1] + vector3.y * matrix4.m[1][1] + vector3.z * matrix4.m[2][1];
	matVector.z = vector3.x * matrix4.m[0][2] + vector3.y * matrix4.m[1][2] + vector3.z * matrix4.m[2][2];

	return matVector;
}

void RailCamera::Afin(WorldTransform& worldTransform_)
{
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matRotX;
	Matrix4 matRotY;
	Matrix4 matRotZ;
	Matrix4 matTrans;
	matScale = {
	worldTransform_.scale_.x,0,0,0,
	0,worldTransform_.scale_.y,0,0,
	0,0,worldTransform_.scale_.z,0,
	0,0,0,1
	};
	matTrans = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	worldTransform_.translation_.x,worldTransform_.translation_.y,worldTransform_.translation_.z,1
	};
	//Z�p��]���̐ݒ�
	matRotZ = {
	cosf(worldTransform_.rotation_.z),sinf(worldTransform_.rotation_.z),0,0,
	-sinf(worldTransform_.rotation_.z),cosf(worldTransform_.rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1
	};
	//X�p��]���̐ݒ�
	matRotX = {
	1,0,0,0,
	0,cosf(worldTransform_.rotation_.x),sinf(worldTransform_.rotation_.x),0,
	0,-sinf(worldTransform_.rotation_.x),cosf(worldTransform_.rotation_.x),0,
	0,0,0,1
	};
	//Y�p��]���̐ݒ�
	matRotY = {
	cosf(worldTransform_.rotation_.y),0,-sinf(worldTransform_.rotation_.y),0,
	0,1,0,0,
	sinf(worldTransform_.rotation_.y),0,cosf(worldTransform_.rotation_.y),0,
	0,0,0,1
	};
	matRot = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	//�e���̉�]�s�������
	matRot *= matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;
	worldTransform_.matWorld_ = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	worldTransform_.matWorld_ *= matScale;
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;
}
