#pragma once
#include <unordered_map>
#include <memory>
#include <cassert>

#include "Definitions.h"
//#include "ComponentArray.h"


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


	// moving private members out to avoid "unresolved ext symbol" 
	static std::unordered_map<const char*, ComponentType> componentTypes;
	static std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays;
	static ComponentType nextComponentType;

	template<typename T>
	static std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(componentArrays.find(typeName) != componentArrays.end() &&
			"Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}

	class ComponentLogic
	{
	private:
		
	// Why cant I put my members here? Then its "unresolved external symbol" 
    // Not sure what to put here at all...

	public:

		template<typename T> 
		static bool IsRegistered()
		{
			return (componentTypes.find(typeid(T).name()) != componentTypes.end());
		}

		template<typename T>
		static void RegisterComponent()
		{
			const char* typeName = typeid(T).name();

			assert(componentTypes.find(typeName) == componentTypes.end() &&
				"Registering component type more than once.");

			// Add this component type to the component type map
			componentTypes.insert({ typeName, nextComponentType });

			// Create a ComponentArray pointer and add it to the component arrays map
			componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

			// Increment the value so that the next component registered will be different
			++nextComponentType;
		}

		template<typename T>
		static ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();

			assert(componentTypes.find(typeName) != componentTypes.end() &&
				"Component not registered.");

			return componentTypes[typeName];
		}

		template<typename T>
		static void AddComponent(EntityId entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		static void RemoveComponent(EntityId entity)
		{
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		static T& GetComponent(EntityId entity)
		{
			return GetComponentArray<T>()->GetData(entity);
		}

		template<typename T>
		static std::shared_ptr<ComponentArray<T>> GetAllComponents()
		{
			return GetComponentArray<T>();
		}

		static void EntityDestroyed(EntityId entity)
		{
			for (auto const& pair : componentArrays)
			{
				auto const& component = pair.second;
				component->EntityDestroyed(entity);
			}
		}
	};
}
