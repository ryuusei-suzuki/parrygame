#pragma once
//Šî’êƒNƒ‰ƒX‚Å‚·
class Scene
{public:
	virtual ~Scene() = default;
	//ƒ‹[ƒ‹
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
};
