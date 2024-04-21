#ifndef MAPHELPER_H
#define MAPHELPER_H
#include "Common.h"
#include "Component.h"
#include "PhysicsManager.h"
// #include "ComponentManager.h"
#include <optional>
#include "glm/glm.hpp" // Student : You need to get glm added to your project source code or this line will fail.
					   // Search for your IDE's "include / header directories" option (it's called something different everywhere)
					   // Ensure you have the glm folder listed so your compiler can actually find these glm headers.
static int global_uuid = 0;

// forward delcaration.
class Collision;

enum class Direction
{
    NORTH,  // -y
    SOUTH,  // +y
    EAST,   // +x
    WEST,    // -x
    NONE
};

enum class State
{
	ATTACKING,
	DAMAGED,
	IDLE
};

struct Actor
{
public:
	int uuid;
	std::string actor_name;
	std::map<std::string, Component> components;
	std::map<std::string, Component> components_to_add;
	std::vector<std::string> components_to_remove;
	bool player_is_alive = true;
	bool dont_destroy = false;

	Actor(std::string actor_name) : actor_name(actor_name)
		{
			uuid = global_uuid;
			global_uuid++;
		}

	Actor() {}

	// destructor
	~Actor() 
	{
		// std::cout << "Actor destructor called for " << actor_name << " with uuid of " << uuid << std::endl;
		components.clear();
		components_to_add.clear();
		components_to_remove.clear();
	}

	/************* ACTOR FUNCTIONS *************/
	std::string GetName() const;
	int GetID() const;
	void InjectReference(std::shared_ptr<luabridge::LuaRef> component_ref);
	luabridge::LuaRef GetComponentByKey(const std::string& key);
	luabridge::LuaRef GetComponent(const std::string& type);
	luabridge::LuaRef GetComponents(const std::string& type);
	static inline Actor* FindHelper(const std::string& name);
	static luabridge::LuaRef Find(const std::string& name);
	static luabridge::LuaRef FindAll(const std::string& name);
	luabridge::LuaRef AddComponent(const std::string& component_name);
	void RemoveComponent(const luabridge::LuaRef& table);
	static luabridge::LuaRef Instantiate(const std::string& template_name);
	static void Destroy(Actor* actor);

	// hw8
	luabridge::LuaRef OnCollisionEnter(Collision* collision);
};

#endif