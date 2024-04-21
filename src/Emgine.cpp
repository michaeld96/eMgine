#include "Emgine.h"
#include "ComponentManager.h"
#include "LuaPrint.h"
#include "RigidBody.h"

Emgine::~Emgine()
{

}

GameState Emgine::get_game_state()
{
    return emgine_game_state;
}

void Emgine::set_game_state(GameState in_game_state)
{
    emgine_game_state = in_game_state;
}

void Emgine::process_input(SDL_Renderer* r)
{
    emgine_player_is_moving = false;
    SDL_Event event;
    // input
    while (Helper::SDL_PollEvent498(&event))
    {

        InputManager::process_event(event);
        if (event.type == SDL_QUIT)
        {
            emgine_game_state = GameState::QUIT;
        }
    }
}

void Emgine::load_bad_ending_to_buffer(SDL_Renderer* r)
{
    SDL_Texture* bad_ending_texture = ImageManager::get_texture(ImageManager::bad_ending_img_name);
    SDL_RenderCopy(r, bad_ending_texture, NULL, NULL);
    if (AudioManager::bad_ending_audio_name != std::string("") && !emgine_played_bad_ending_audio)
    {
        AudioManager::halt_audio();
        AudioManager::play_audio(AudioManager::bad_ending_audio_name, 0, 0);
        emgine_played_bad_ending_audio = true;
    }
}

void Emgine::load_good_ending_to_buffer(SDL_Renderer* r)
{
    // get good ending texture.
    SDL_Texture* good_ending_texture = ImageManager::get_texture(ImageManager::good_ending_img_name);
    SDL_RenderCopy(r, good_ending_texture, NULL, NULL);
    if (AudioManager::good_ending_audio_name != std::string("") && !emgine_played_good_ending_audio)
    {
        AudioManager::halt_audio();
        AudioManager::play_audio(AudioManager::good_ending_audio_name, 0, 0);
        emgine_played_good_ending_audio = true;
    }
}

void Emgine::make_player_state_moving()
{
    emgine_player_is_moving = true;   
}

void Emgine::make_player_state_not_moving()
{
    emgine_player_is_moving = false;
}

void Emgine::run_onstart_for_all_actors()
{
    for (auto& actor : SceneManager::imported_actors)
    {
        if (!actor->dont_destroy)
        {
            // Iterate over the components, which are already sorted by the string key
            for (auto& kvp : actor->components) 
            {
                const std::string& key = kvp.first;
                Component& component = kvp.second;

                // Check if the lua_table shared_ptr is valid and if it contains a table
                if (component.lua_table && component.lua_table->isTable()) 
                {
                    // Access the Lua table
                    luabridge::LuaRef& table = *component.lua_table;
                    component.enabled = table["enabled"].cast<bool>();
                    if (component.enabled)
                    {
                        // update key.
                        // table["key"] = key;
                        // Check if the OnStart function exists in the table and is callable
                        if (table["OnStart"].isFunction()) 
                        {
                            try
                            {
                                // Call the OnStart function with the Lua table as an argument
                                table["OnStart"](table);
                            }
                            catch (luabridge::LuaException e)
                            {
                                std::cout << "\033[31m" << actor->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Emgine::onupdate_for_all_actors()
{
    // actor's are already in sorted order.
    for (auto& actor : SceneManager::imported_actors)
    {
        // Iterate over the components, which are already sorted by the string key
        for (auto& kvp : actor->components) 
        {
            const std::string& key = kvp.first;
            Component& component = kvp.second;

            // Check if the lua_table shared_ptr is valid and if it contains a table
            if (component.lua_table && component.lua_table->isTable()) 
            {
                // Access the Lua table
                luabridge::LuaRef& table = *component.lua_table;
                // grab the updated value of "enabled" from the lua table.
                component.enabled = table["enabled"].cast<bool>();
                if (component.enabled)
                {
                    // Check if the OnUpdate function exists in the table and is callable
                    if (table["OnUpdate"].isFunction()) 
                    {
                        try
                        {
                            // Call the OnUpdate function with the Lua table as an argument
                            table["OnUpdate"](table);
                        }
                        catch (luabridge::LuaException e)
                        {
                            std::cout << "\033[31m" << actor->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                        }
                    }
                }
            }
        }
    }
}

void Emgine::onlateupdate_for_all_actors()
{
    // actor's are already in sorted order.
    for (auto& actor : SceneManager::imported_actors)
    {
        
        // see if actor is new by looking into the actors_that_need_on_start_ran vector.
        if (std::find(SceneManager::actors_that_need_on_start_ran.begin(), SceneManager::actors_that_need_on_start_ran.end(), actor) == SceneManager::actors_that_need_on_start_ran.end())
        {
            // Iterate over the components, which are already sorted by the string key
            for (auto& kvp : actor->components) 
            {
                const std::string& key = kvp.first;
                Component& component = kvp.second;

                // Check if the lua_table shared_ptr is valid and if it contains a table
                if (component.lua_table && component.lua_table->isTable()) 
                {
                    // Access the Lua table
                    luabridge::LuaRef& table = *component.lua_table;
                    // hack: we are reaching into the Lua table and pulling out the enable feature.
                    //       for some reason, it is not being written to with on start...
                    component.enabled = table["enabled"].cast<bool>();
                    if (component.enabled)
                    {
                        // Check if the OnLateUpdate function exists in the table and is callablex
                        if (table["OnLateUpdate"].isFunction()) 
                        {
                            try
                            {
                                // Call the OnLateUpdate function with the Lua table as an argument
                                table["OnLateUpdate"](table);
                            }
                            catch (luabridge::LuaException e)
                            {
                                std::cout << "\033[31m" << actor->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Emgine::process_added_components()
{
    for (auto& actor : SceneManager::imported_actors)
    {
        // go thru actors components_to_add which is a map<string, Components>
        for (auto& [add_key, add_component] : actor->components_to_add)
        {
            // we need to call OnStart for this new component if it has one.
            if (add_component.lua_table && add_component.lua_table->isTable()) 
            {
                // Access the Lua table
                luabridge::LuaRef& table = *add_component.lua_table;
                // Check if the OnStart function exists in the table and is callable
                if (table["OnStart"].isFunction()) 
                {
                    table["key"] = add_key;
                    try
                    {
                        // Call the OnStart function with the Lua table as an argument
                        table["OnStart"](table);
                    }
                    catch (luabridge::LuaException e)
                    {
                        std::cout << "\033[31m" << actor->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                    }
                }
            }
            else if (add_component.type == "Rigidbody")
            {
                // deref luaref to get rigidbody
                RigidBody* rb = add_component.lua_table->cast<RigidBody*>();
                // set the properties of the rigidbody.
                
                rb->set_properties(rb->_x,
                                    rb->_y,
                                    rb->_body_type,
                                    rb->_precise,
                                    rb->_gravity_scale,
                                    rb->_density,
                                    rb->_angular_friction,
                                    rb->_rotation,
                                    rb->_has_collider,
                                    rb->_has_trigger,
                                    rb->_collider_type,
                                    rb->_collider_width,
                                    rb->_collider_height,
                                    rb->_collider_radius,
                                    rb->_collider_friction,
                                    rb->_collider_bounciness,
                                    rb->_actor,
                                    rb->_trigger_type,
                                    rb->_trigger_width,
                                    rb->_trigger_height,
                                    rb->_trigger_radius);
            rb->key = add_key;
            // if forces are to be added the rigid body.
            if (rb->_forces_to_be_applied.size() > 0)
            {
                for (auto& force : rb->_forces_to_be_applied)
                {
                    rb->AddForce(force);
                }
            }
            if (rb->_velocities_to_be_applied.size() > 0)
            {
                for (auto& velocity : rb->_velocities_to_be_applied)
                {
                    rb->SetVelocity(velocity);
                }
            }
                
            }
            // add these components to the actors components.
            actor->components[add_key] = add_component;
        }
        // clear the components_to_add map.
        actor->components_to_add.clear();
    }
}

void Emgine::process_removed_components()
{
    for (auto& actor : SceneManager::imported_actors)
    {
        std::vector<std::string> keys_to_remove;
        // go thru all actors components
        for (auto& [key, component] : actor->components)
        {
            // loop thru the components_to_remove vector.
            for (auto& remove_key : actor->components_to_remove)
            {
                if (key == remove_key)
                {
                    // call OnDestroy for this component if it has one.
                    if (component.lua_table && component.lua_table->isTable()) 
                    {
                        // Access the Lua table
                        luabridge::LuaRef& table = *component.lua_table;
                        // Check if the OnDestroy function exists in the table and is callable
                        if (table["OnDestroy"].isFunction()) 
                        {
                            try
                            {
                                // Call the OnDestroy function with the Lua table as an argument
                                table["OnDestroy"](table);
                            }
                            catch (luabridge::LuaException e)
                            {
                                std::cout << "\033[31m" << actor->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                            }
                        }
                    }
                    else if (component.type == "Rigidbody")
                    {
                        // deref luaref to get rigidbody
                        RigidBody* rb = component.lua_table->cast<RigidBody*>();
                        // remove the rigidbody from the physics world.
                        PhysicsManager::physics_world->DestroyBody(rb->body);
                    }
                    keys_to_remove.push_back(key);
                }
            }
        }
        // remove the components from the actors components.
        for (auto& key : keys_to_remove)
        {
            actor->components.erase(key);
        }
        actor->components_to_remove.clear();
    }
}

void Emgine::actors_that_need_to_be_destroyed()
{
    for (auto& uuid : SceneManager::actors_that_need_to_be_destroyed)
    {
        if (SceneManager::imported_actors.size() == 0)
        {
            return;
        }
        // find the actor that needs to be destroyed and remove it from the imported_actors vector.
        auto itr = std::find_if(SceneManager::imported_actors.begin(), SceneManager::imported_actors.end(), [uuid](const std::shared_ptr<Actor>& actor) {
            return actor->uuid == uuid;
        });
        // loop thru components and call OnDestory if it exists.
        if (itr != SceneManager::imported_actors.end())
        {
            for (auto& kvp : (*itr)->components) 
            {
                const std::string& key = kvp.first;
                Component& component = kvp.second;

                // Check if the lua_table shared_ptr is valid and if it contains a table
                if (component.lua_table && component.lua_table->isTable()) 
                {
                    // Access the Lua table
                    luabridge::LuaRef& table = *component.lua_table;
                    // Check if the OnDestroy function exists in the table and is callable
                    if (table["OnDestroy"].isFunction()) 
                    {
                        try
                        {
                            // Call the OnDestroy function with the Lua table as an argument
                            table["OnDestroy"](table);
                        }
                        catch (luabridge::LuaException e)
                        {
                            std::cout << "\033[31m" << (*itr)->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                        }
                    }
                }
                else if (component.type == "Rigidbody")
                {
                    // deref luaref to get rigidbody
                    RigidBody* rb = component.lua_table->cast<RigidBody*>();
                    // remove the rigidbody from the physics world.
                    PhysicsManager::physics_world->DestroyBody(rb->body);
                }
            }
        }
        // erase the actor from the imported_actors vector.
        if (itr != SceneManager::imported_actors.end())
        {
            SceneManager::imported_actors.erase(itr);
        }
        
    }
    SceneManager::actors_that_need_to_be_destroyed.clear();

}

void Emgine::add_new_actors()
{
    for (auto& actor : SceneManager::actors_that_need_to_be_added)
    {
        // Check and call OnStart for each component
        for (auto& [key, component] : actor->components)
        {
            if (component.lua_table && component.lua_table->isTable())
            {
                luabridge::LuaRef& table = *component.lua_table;
                if (table["OnStart"].isFunction())
                {
                    try
                    {
                        table["OnStart"](table);
                    }
                    catch (const luabridge::LuaException& e)
                    {
                        std::cerr << "Error calling OnStart for component " << key << " of actor " << actor->actor_name << ": " << e.what() << std::endl;
                    }
                }
            }
            else 
            {
                // get pointer to rigidbody
                RigidBody* rb = component.lua_table->cast<RigidBody*>();
                // set the properties of the rigidbody.
                rb->set_properties(rb->_x,
                                    rb->_y,
                                    rb->_body_type,
                                    rb->_precise,
                                    rb->_gravity_scale,
                                    rb->_density,
                                    rb->_angular_friction,
                                    rb->_rotation,
                                    rb->_has_collider,
                                    rb->_has_trigger,
                                    rb->_collider_type,
                                    rb->_collider_width,
                                    rb->_collider_height,
                                    rb->_collider_radius,
                                    rb->_collider_friction,
                                    rb->_collider_bounciness,
                                    rb->_actor,
                                    rb->_trigger_type,
                                    rb->_trigger_width,
                                    rb->_trigger_height,
                                    rb->_trigger_radius);
            }
        }

        // Move the actor to imported_actors
        SceneManager::imported_actors.push_back(std::move(actor));
    }

    // Clear the list of actors that need to be added
    SceneManager::actors_that_need_to_be_added.clear();
}