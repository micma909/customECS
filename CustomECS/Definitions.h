#pragma once
#include <cstdint>
#include <bitset>

namespace ECS
{
	// Entity type alias, unique ID of entity
	using EntityId = std::uint32_t;

	// Size of arrays
	const EntityId MAX_ENTITIES = 5000;

	// Component type alias, unique ID for component
	using ComponentType = std::uint8_t;

	// Size of arrays
	const ComponentType MAX_COMPONENTS = 32;

	// Signature 
	using Signature = std::bitset<MAX_COMPONENTS>;
}