#pragma once
#include <unordered_map>
#include "Definitions.h"

namespace ECS
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(EntityId entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void InsertData(EntityId entity, T component)
		{
			assert(this->entityToIndexMap.find(entity) == this->entityToIndexMap.end() &&
				"Same component added more than once.");

			size_t newIndex = this->sizeOfEntries;

			this->entityToIndexMap[entity] = newIndex;
			this->indexToEntityMap[newIndex] = entity;

			componentArray[newIndex] = component;

			++this->sizeOfEntries;
		}

		void RemoveData(EntityId entity)
		{
			assert(this->entityToIndexMap.find(entity) != this->entityToIndexMap.end() &&
				"Component does not exist.");

			// TODO: See if tracking of elements cant be made simpler

			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = this->entityToIndexMap[entity];
			size_t indexOfLastElement = sizeOfEntries - 1;
			this->componentArray[indexOfRemovedEntity] = this->componentArray[indexOfLastElement];

			// Update map to point to moved spot
			EntityId entityOfLastElement = this->indexToEntityMap[indexOfLastElement];
			this->entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			this->indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			this->entityToIndexMap.erase(entity);
			this->indexToEntityMap.erase(indexOfLastElement);

			--this->sizeOfEntries;
		}

		T& GetData(EntityId entity)
		{
			assert(this->entityToIndexMap.find(entity) != this->entityToIndexMap.end() &&
				"Component does not exist.");

			size_t index = this->entityToIndexMap[entity];
			return componentArray[index];
		}

		std::array<T, MAX_ENTITIES>& GetInnerArray()
		{
			return this->componentArray;
		}

		void EntityDestroyed(EntityId entity) override
		{
			// cleanup if entity has been destroyed
			if (this->entityToIndexMap.find(entity) != this->entityToIndexMap.end())
			{
				this->RemoveData(entity);
			}
		}

	private:
		std::array<T, MAX_ENTITIES> componentArray;
		std::unordered_map<EntityId, size_t> entityToIndexMap;
		std::unordered_map<size_t, EntityId> indexToEntityMap;
		size_t sizeOfEntries;
	};

}