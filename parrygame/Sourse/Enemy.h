#pragma once

enum class State
{
	Idle,
	Telegraph,
	Attack,
	Recovery
};

class Enemy
{
public:
	Enemy();
	~Enemy();
	void Update(float deltaTime);
	void Draw();

	State state_ = State::Idle;

	float Timer = 0.0f;
	float Idtimer = 0.7f;
	float Tetimer = 1.0f;
	float Attimer = 0.5f;
	float Retimer = 1.0f;
};
