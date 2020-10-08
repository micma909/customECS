#include <stdio.h>
#include <iostream>

#include "EntityLogic.h"
#include "ComponentLogic.h"

#include "linalg.h"

using namespace linalg::aliases;

struct SimpleTransformComponent
{
	float3 pos;
};

int main(int argc, const char* argv[])
{
	ECS::EntityLogic::Init();

	std::vector<ECS::Entity*> entities(10);

	ECS::EntityLogic::InitEntities(entities);

	for (auto& entity : entities)
	{
		SimpleTransformComponent transform;
		transform.pos = float3(0, 0, 0);

		entity->AddComponent(transform);
	}

	for (auto& entity : entities)
	{
		SimpleTransformComponent& trans = entity->GetComponent<SimpleTransformComponent>();
		std::cout << trans.pos[0] << " " << trans.pos[1] << " " << trans.pos[2] << std::endl;
	}


	std::cout << "Hello ECS!" << std::endl;
}