#include "Enemy.h"
#include <DxLib.h>

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltaTime)
{
	switch (state_)
	{
	case State::Idle:
		Timer += deltaTime;
		if (Timer >= Idtimer)
		{
			state_ = State::Telegraph;
			Timer = 0.0f;
		}
		break;
	case State::Telegraph:
		Timer += deltaTime;
		if (Timer >= Tetimer)
		{
			state_ = State::Attack;
			Timer = 0.0f;
		}
		break;
	case State::Attack:
		Timer += deltaTime;
		if (Timer >= Attimer)
		{
			state_ = State::Recovery;
			Timer = 0.0f;
		}
		break;
	case State::Recovery:
		Timer += deltaTime;
		if (Timer >= Retimer)
		{
			state_ = State::Idle;
			Timer = 0.0f;
		}
		break;
	}
}

void Enemy::Draw()
{
	switch (state_)
	{
	case State::Idle:
		DrawString(0, 0, "Idle", GetColor(255, 255, 255), 0);
		break;
	case State::Telegraph:
		DrawString(0, 0, "Telegraph", GetColor(255, 255, 255), 0);
		break;
	case State::Attack:
		DrawString(0, 0, "Attack", GetColor(255, 255, 255), 0);
		break;
	case State::Recovery:
		DrawString(0, 0, "Recovery", GetColor(255, 255, 255), 0);
		break;
	}
}
