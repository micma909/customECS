#pragma once
#include <queue>
#include <array>
#include <cassert>

#include "Definitions.h"
#include "ComponentLogic.h"


namespace ECS
{
	// moving private members out to avoid "unresolved ext symbol" 
	static std::queue<EntityId> availableEntitiesQ;
	static std::array<Signature, MAX_ENTITIES> signatures;
	static uint32_t nrEntitiesAlive;

	class Entity
	{
	public:
		template<typename T>
		void AddComponent(T component)
		{
			// Register in, and Add component
			if (!ComponentLogic::IsRegistered<T>())
				ComponentLogic::RegisterComponent<T>();
			ComponentLogic::AddComponent<T>(id, component);
		}

		template<typename T>
		T& GetComponent()
		{
			return ComponentLogic::GetComponent<T>(id);
		}

		~Entity()
		{
			ComponentLogic::EntityDestroyed(id);
		}

	private:
		friend class EntityLogic;
		Entity() : id(-1)
		{};

		EntityId id;
	};

	class EntityLogic
	{
	public:
		static void Init()
		{
			for (EntityId ent = 0; ent < MAX_ENTITIES; ++ent)
			{
				availableEntitiesQ.push(ent);
			}
		}

		static void InitEntities(Entity* entity)
		{
			entity = new Entity();
			entity->id = AssignEntityID();
		}

		static void InitEntities(std::vector<Entity*>& entites)
		{
			for (int i = 0; i < entites.size(); i++)
			{
				entites[i] = new Entity();
				entites[i]->id = AssignEntityID();
			}
		}

		static void DestroyEntity(EntityId entity)
		{
			assert(entity < MAX_ENTITIES && "Entity out of range.");

			// memset 0 on entity's signature
			signatures[entity].reset();

			availableEntitiesQ.push(entity);

			--nrEntitiesAlive;
		}

		static void SetSignature(EntityId entity, Signature signature)
		{
			assert(entity < MAX_ENTITIES && "Entity out of range.");

			signatures[entity] = signature;
		}

		static Signature GetSignature(EntityId entity)
		{
			return signatures[entity];
		}

	private:

		static EntityId AssignEntityID()
		{
			EntityId ID = availableEntitiesQ.front();

			availableEntitiesQ.pop();
			nrEntitiesAlive++;

			return ID;
		}
	};
}