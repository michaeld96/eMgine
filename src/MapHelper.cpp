#include "MapHelper.h"
#include "ComponentManager.h"
#include "SceneManager.h"
#include "RigidBody.h"
// #include "LuaPrint.h"


std::string Actor::GetName() const
{ 
    return actor_name;
}

int Actor::GetID() const
{
    return uuid;
}

void Actor::InjectReference(std::shared_ptr<luabridge::LuaRef> component_ref)
{
    (*component_ref)["actor"] = this;
}

luabridge::LuaRef Actor::GetComponentByKey(const std::string& key)
{
    if (components.empty())
    {
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
    auto itr = components.find(key);
    if (itr != components.end())
    {
        return *(itr->second.lua_table);
    }
    else
    {
        // returning nil if the component is not found.
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
}

luabridge::LuaRef Actor::GetComponent(const std::string& type)
{
    if (!components.empty())
    {
        for (auto& [key, value] : components)
        {
            if (value.type == type && value.enabled)
            {
                return luabridge::LuaRef(*value.lua_table.get());
            }
        }
    }
    return luabridge::LuaRef(ComponentManager::_lua_state);
}

luabridge::LuaRef Actor::GetComponents(const std::string& type)
{
    luabridge::LuaRef components_table = luabridge::newTable(ComponentManager::_lua_state);
    int index = 1; // lua tables are 1 indexed.
    for (auto& [key, value] : components)
    {
        if (value.type == type)
        {
            // components_table.append(*value.lua_table);
            components_table[index++] = *value.lua_table;
        }
    }
    if (index == 1)
    {
        // returning nil if the component is not found.
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
    return components_table;
}

Actor* Actor::FindHelper(const std::string& actor_with_same_name)
{
    // loop thru all actors until we find the first one with the same name.
    for (auto& actor : SceneManager::imported_actors)
    {
        // HACKY!!
        if (actor->actor_name == actor_with_same_name && actor->player_is_alive)
        {
            return actor.get();
        }
    }
    // loop thru actors_that_need_to_be_added and see if we find the actor
    for (auto& actor : SceneManager::actors_that_need_to_be_added)
    {
        if (actor->actor_name == actor_with_same_name)
        {
            return actor.get();
        }
    }
    // return a nil value if no actor was found
    return nullptr;
}

luabridge::LuaRef Actor::Find(const std::string& actor_with_same_name)
{
    Actor* actor = FindHelper(actor_with_same_name);
    // std::shared_ptr<Actor> actor = FindHelper(actor_with_same_name);
    if (actor != nullptr)
    {
        return luabridge::LuaRef(ComponentManager::_lua_state, actor);
    }
    // return a nil value if no actor was found
    return luabridge::LuaRef(ComponentManager::_lua_state);
}

luabridge::LuaRef Actor::FindAll(const std::string& actor_with_same_name)
{
    luabridge::LuaRef actors_table = luabridge::newTable(ComponentManager::_lua_state);
    int index = 1; // lua tables are 1 indexed.
    for (auto& actor : SceneManager::imported_actors)
    {
        if (actor->actor_name == actor_with_same_name)
        {
            // actors_table[index++] = &actor;
            actors_table[index++] = actor.get();
        }
        // check actors_that_need_to_be_added
        else
        {
            for (auto& actor : SceneManager::actors_that_need_to_be_added)
            {
                if (actor->actor_name == actor_with_same_name)
                {
                    actors_table[index++] = actor.get();
                }
            }
        }
    }
    if (index == 1)
    {
        // returning nil if the actor is not found.
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
    return actors_table;
}

luabridge::LuaRef Actor::AddComponent(const std::string& component_name)
{
    if (component_name == "Rigidbody")
    {
        // create rigidbody
        Component new_component;
        new_component.type = "Rigidbody";
        RigidBody* rigidbody = new RigidBody();
        new_component.enabled = true;
        // have luaref to the rigidbody
        luabridge::LuaRef luaRigidBodyRef = luabridge::LuaRef(ComponentManager::_lua_state, rigidbody);
        // make table
        new_component.lua_table = std::make_shared<luabridge::LuaRef>(luaRigidBodyRef);
        // generate key
        std::string key = ComponentManager::generate_component_key();
        components_to_add.insert({key, new_component});
        // return the new component
        return *new_component.lua_table;
    }
    else
    {
        // create a new component
        Component new_component;
        // load in base table
        luabridge::LuaRef base_table = *ComponentManager::_component_tables[component_name];
        // create a new table
        new_component.lua_table = std::make_shared<luabridge::LuaRef>(luabridge::newTable(ComponentManager::_lua_state));
        // mark type
        new_component.type = component_name;
        // make enabled
        (*new_component.lua_table)["enabled"] = true;
        (*new_component.lua_table)["type"] = component_name;
        std::string key = ComponentManager::generate_component_key();
        (*new_component.lua_table)["key"] = key;
        // establish inheritance
        ComponentManager::establish_inheritance(*new_component.lua_table, base_table);
        // inject reference
        InjectReference(new_component.lua_table);
        // add to components_to_add
        components_to_add.insert({key, new_component});
        // return the new component
        return *new_component.lua_table;
    }
}

void Actor::RemoveComponent(const luabridge::LuaRef& table)
{
    // get the name of the table
    std::string component_key = table["key"].cast<std::string>();
    // find the component in the components map
    if (!component_key.empty())
    {
        for (auto& [key, value] : components)
        {
            if (key == component_key)
            {
                // make enabled false (just in case).
                table["enabled"] = false;
                // make enabled false.
                value.enabled = false;
                // push name to components_to_remove
                components_to_remove.push_back(component_key);
                return;
            }
        }
        return;
    }
    else 
    {
        // check if component is rigidbody.
        RigidBody* rb = table.cast<RigidBody*>();
        if (rb != nullptr)
        {
            // see if this is the rigidbody that needs to be deleted.
            for (auto& [key, value] : components)
            {
                if (key == rb->key)
                {
                    components_to_remove.push_back(rb->key);
                    return;
                }
            }
        }
    }
}

// Actor* Actor::Instantiate(const std::string& template_name)
luabridge::LuaRef Actor::Instantiate(const std::string& template_name)
{
    std::shared_ptr<Actor> new_actor_shared = SceneManager::load_in_template(template_name);
    SceneManager::actors_that_need_to_be_added.push_back(new_actor_shared);
    return luabridge::LuaRef(ComponentManager::_lua_state, new_actor_shared.get());
}

void Actor::Destroy(Actor* actor)
{
    if (SceneManager::imported_actors.empty())
    {
        return;
    }
    // Check to see if actor exists
    auto itr = std::find_if(SceneManager::imported_actors.begin(), SceneManager::imported_actors.end(),
    [actor](const std::shared_ptr<Actor>& ptr) { return ptr->uuid == actor->uuid; });

    auto itr2 = std::find_if(SceneManager::actors_that_need_to_be_added.begin(), SceneManager::actors_that_need_to_be_added.end(),
    [actor](const std::shared_ptr<Actor>& ptr) { return ptr.get() == actor; });

    // If actor does not exist, return.
    if (itr == SceneManager::imported_actors.end() && itr2 == SceneManager::actors_that_need_to_be_added.end())
    {
        return;
    }

    // If actor does exist, add to actors_that_need_to_be_destroyed.
    bool need_to_erase_itr2 = false;
    if (itr != SceneManager::imported_actors.end())
    {
        SceneManager::actors_that_need_to_be_destroyed.push_back(itr->get()->uuid);
    }
    else if (itr2 != SceneManager::actors_that_need_to_be_added.end())
    {
        need_to_erase_itr2 = true;
    }

    // No lifecycle functions should run after this.
    // Loop through components and make them all disabled.
    for (auto& [key, value] : actor->components)
    {
        (*value.lua_table)["enabled"] = false;
        value.enabled = false;
    }
    if (need_to_erase_itr2)
    {
        SceneManager::actors_that_need_to_be_added.erase(itr2);
    }

    // Additional cleanup if needed
    actor->player_is_alive = false;
}
