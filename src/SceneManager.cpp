#include "SceneManager.h"
#include "MapHelper.h"
#include "ComponentManager.h"
#include "RigidBody.h"
#include "PhysicsManager.h"
// #include "LuaPrint.h"

void SceneManager::Load(std::string scene_name)
{
    // new scene has been requested
    new_scene_requested = true;
    new_scene_name = scene_name;
    // set all actors that dont have dont_destroy to be destroyed.
    for (auto& actor : imported_actors)
    {
        // check to see if the actor's UUID is already in the list.
        auto itr = std::find_if(actors_that_need_to_be_destroyed.begin(), actors_that_need_to_be_destroyed.end(), 
            [&actor](const auto& uuid) { return uuid == actor->uuid; });
        if (!actor->dont_destroy && itr == actors_that_need_to_be_destroyed.end())
        {
            actors_that_need_to_be_destroyed.push_back(actor->uuid);
        }
    }
}

std::string SceneManager::GetCurrent()
{
    // return the name of the current scene without .scene
    return current_scene;
}

void SceneManager::DontDestroy(Actor* actor)
{
    actor->dont_destroy = true;
}

void SceneManager::load_scene(const std::string& scene_name)
{
    current_scene = scene_name;
    // imported_actors.clear();
    std::string scene_path = "resources/scenes/" + scene_name + ".scene";
    if (std::filesystem::exists(scene_path)) 
    {
        rapidjson::Document scene_document;
        ConfigManager::read_json_file(scene_path, scene_document);
        if (scene_document.HasMember("actors") && scene_document["actors"].IsArray())
        {
            const rapidjson::Value& actors = scene_document["actors"];
            for (rapidjson::SizeType i = 0; i < actors.Size(); ++i)
            {


                std::string name = "";
                std::string view_image = ""; // changed
                std::string view_image_back = ""; 
                double transform_position_x = 0; // changed
                double transform_position_y = 0; // changed
                double transform_scale_x = 1.0; // new 
                double transform_scale_y = 1.0; // new
                double transformation_rotation_degrees = 0.0; // new
                // TODO: load in image and get rect or something.
                double view_pivot_offset_x = -999; // need to update.
                double view_pivot_offset_y = -999; // need to update.
                float vel_x = 0.0;
                float vel_y = 0.0;
                
                std::string nearby_dialogue = "";
                std::string contact_dialogue = "";
                std::optional<int> rendering_order = std::nullopt;
                // hw6
                bool movement_bounce_enabled = false;
                float box_collider_width = 0.0f;
                float box_collider_height = 0.0f;
                float box_trigger_width = 0.0f;
                float box_trigger_height = 0.0f;
                std::string view_image_damage = "";
                std::string view_image_attack = "";
                std::string nearby_dialogue_sfx = "";

                // hw7
                std::string component_name = "";
                Component new_component;
                bool template_has_components = false;
                std::vector<std::pair<std::string, Component>> template_components;

                const rapidjson::Value& actor_data = actors[i];
                // Extract actor properties, using defaults if any are missing
                if (actor_data.HasMember("template") && actor_data["template"].IsString())
                {
                    std::string template_name = actor_data.HasMember("template") ? actor_data["template"].GetString() : "";
                    TemplateManager::check_if_template_exists(template_name);
                    std::string template_path = "resources/actor_templates/" + template_name + ".template";
                    rapidjson::Document template_document;
                    ConfigManager::read_json_file(template_path, template_document);
                    // now fill in the template data.
                    if (template_document.HasMember("name"))
                    {
                        name = template_document["name"].GetString();
                    }
                    if (template_document.HasMember("view_image"))
                    {
                        view_image = template_document["view_image"].GetString();
                    }
                    if (template_document.HasMember("transform_position_x"))
                    {
                        transform_position_x = template_document["transform_position_x"].GetDouble();
                    }
                    if (template_document.HasMember("transform_position_y"))
                    {
                        transform_position_y = template_document["transform_position_y"].GetDouble();
                    }
                    if (template_document.HasMember("vel_x"))
                    {
                        vel_x = template_document["vel_x"].GetFloat();
                    }
                    if (template_document.HasMember("vel_y"))
                    {
                        vel_y = template_document["vel_y"].GetFloat();
                    }
                    if (template_document.HasMember("nearby_dialogue"))
                    {
                        nearby_dialogue = template_document["nearby_dialogue"].GetString();
                    }
                    if (template_document.HasMember("contact_dialogue"))
                    {
                        contact_dialogue = template_document["contact_dialogue"].GetString();
                    }
                    // new hw5
                    if (template_document.HasMember("transform_scale_x"))
                    {
                        transform_scale_x = template_document["transform_scale_x"].GetDouble();
                    }
                    if (template_document.HasMember("transform_scale_y"))
                    {
                        transform_scale_y = template_document["transform_scale_y"].GetDouble();
                    }
                    if (template_document.HasMember("transform_rotation_degrees"))
                    {
                        transformation_rotation_degrees = template_document["transform_rotation_degrees"].GetDouble();
                    }
                    if (template_document.HasMember("view_pivot_offset_x"))
                    {
                        view_pivot_offset_x = template_document["view_pivot_offset_x"].GetDouble();
                    }
                    if (template_document.HasMember("view_pivot_offset_y"))
                    {
                        view_pivot_offset_y = template_document["view_pivot_offset_y"].GetDouble();
                    }
                    if (template_document.HasMember("render_order"))
                    {
                        rendering_order = template_document["render_order"].GetInt();
                    }
                    // hw6
                    if (template_document.HasMember("view_image_back"))
                    {
                        view_image_back = template_document["view_image_back"].GetString();
                    }
                    if (template_document.HasMember("movement_bounce_enabled"))
                    {
                        movement_bounce_enabled = template_document["movement_bounce_enabled"].GetBool();
                    }
                    if (template_document.HasMember("box_collider_width"))
                    {
                        box_collider_width = template_document["box_collider_width"].GetFloat();
                    }
                    if (template_document.HasMember("box_collider_height"))
                    {
                        box_collider_height = template_document["box_collider_height"].GetFloat();
                    }
                    if (template_document.HasMember("box_trigger_width"))
                    {
                        box_trigger_width = template_document["box_trigger_width"].GetFloat();
                    }
                    if (template_document.HasMember("box_trigger_height"))
                    {
                        box_trigger_height = template_document["box_trigger_height"].GetFloat();
                    }
                    if (template_document.HasMember("view_image_damage"))
                    {
                        view_image_damage = template_document["view_image_damage"].GetString();
                    }
                    if (template_document.HasMember("view_image_attack"))
                    {
                        view_image_attack = template_document["view_image_attack"].GetString();
                    }
                    if (template_document.HasMember("nearby_dialogue_sfx"))
                    {
                        nearby_dialogue_sfx = template_document["nearby_dialogue_sfx"].GetString();
                        // load audio into audio hash map
                        AudioManager::load_nearby_dialogue_sfx(nearby_dialogue_sfx);
                    }
                    // Load components
                    if (template_document.HasMember("components") && template_document["components"].IsObject())
                    {
                        const rapidjson::Value& components = template_document["components"];
                        for (auto& component : components.GetObject())
                        {
                            component_name = component.name.GetString();
                            if (component.value.IsObject())
                            {
                                // Component new_component;
                                new_component.type = component.value["type"].GetString();
                                if (new_component.type == "Rigidbody")
                                {
                                    // create rigid body
                                    // RigidBody* rigid_body_component = new RigidBody(physics_manager);
                                    // rigid_body_component->create_body(physics_manager->physics_world);
                                    // new_component.type = "Rigidbody";
                                    // std::cerr << "TODO: IMPLEMENT RIGIDBODY COMPONENT FOR TEMPLATES\n";
                                    // exit(0);
                                }
                                else
                                {
                                    // let's see if this component exists.
                                    check_if_lua_file_exists(new_component.type);
                                    // Load and get the base table from the Lua state
                                    luabridge::LuaRef baseTable = *ComponentManager::_component_tables[new_component.type];
                                    
                                    // Create a new table for the component instance
                                    new_component.lua_table = std::make_shared<luabridge::LuaRef>(luabridge::newTable(ComponentManager::_lua_state));
                                    // std::cout << "before establish inheritance\n";
                                    // printLuaTable(*new_component.lua_table);
                                    ComponentManager::establish_inheritance(*new_component.lua_table, baseTable);
                                    // std::cout << "after establish inheritance\n";

                                    // we will write properties to the lua table.
                                    for (auto& property : component.value.GetObject())
                                    {
                                        if (property.name != "type")
                                        {
                                            std::string property_name = property.name.GetString();
                                            if (property.value.IsString())
                                            {
                                                (*new_component.lua_table)[property_name] = property.value.GetString();
                                            }
                                            else if (property.value.IsFloat())
                                            {
                                                (*new_component.lua_table)[property_name] = property.value.GetDouble();
                                            }
                                            else if (property.value.IsInt())
                                            {
                                                (*new_component.lua_table)[property_name] = property.value.GetInt();
                                            }
                                            else if (property.value.IsBool())
                                            {
                                                (*new_component.lua_table)[property_name] = property.value.GetBool();
                                            }
                                        }
                                    }
                                    // test.
                                    (*new_component.lua_table)["enabled"] = new_component.enabled;
                                    (*new_component.lua_table)["type"] = new_component.type;
                                    (*new_component.lua_table)["key"] = component_name;
                                    // Establish inheritance
                                    // ComponentManager::establish_inheritance(*new_component.lua_table, baseTable);
                                    template_has_components = true;
                                    // push this to the vector of components.
                                    template_components.push_back({component_name, new_component});
                                }
                            }
                        }
                    }

                }
                // Override template properties with actor-specific properties
                if (actor_data.HasMember("name") && actor_data["name"].IsString()) name = actor_data["name"].GetString();

                imported_actors.push_back(std::make_shared<Actor>(name));
                // Checking to see if the template has components. If it does, then we add them from
                // the vector that we pushed them to.
                if (template_has_components)
                {
                    for (auto& component : template_components)
                    {
                        imported_actors.back()->components.insert({component.first, component.second});
                        // call inject reference with this actor.
                        imported_actors.back()->InjectReference(component.second.lua_table);
                    }
                }
                // hw7
                // Now we see if the actor has components.
                if (actor_data.HasMember("components") && actor_data["components"].IsObject())
                {
                    const rapidjson::Value& components = actor_data["components"];
                    for (auto& component : components.GetObject())
                    {
                        std::string local_component_name = component.name.GetString();
                        // check to see if the component exists.
                        auto itr = imported_actors.back()->components.find(local_component_name);
                        // if the component does exists, we will overwrite the properties (this means
                        // that a template was used, but we are overwriting the properties with the
                        // actor's properties.)
                        if (itr != imported_actors.back()->components.end())
                        {
                            // grabbing the Lua reference that talks to the component.
                            Component& existing_component = itr->second;
                            // we will write properties to the lua table.
                            for (auto& property : component.value.GetObject())
                            {
                                if (property.name != "type")
                                {
                                    std::string property_name = property.name.GetString();
                                    if (property.value.IsString())
                                    {
                                        (*existing_component.lua_table)[property_name] = property.value.GetString();
                                    }
                                    else if (property.value.IsFloat())
                                    {
                                        (*existing_component.lua_table)[property_name] = property.value.GetDouble();
                                    }
                                    else if (property.value.IsInt())
                                    {
                                        (*existing_component.lua_table)[property_name] = property.value.GetInt();
                                    }
                                    else if (property.value.IsBool())
                                    {
                                        (*existing_component.lua_table)[property_name] = property.value.GetBool();
                                    }
                                }
                            }
                        }
                        else if (component.value.IsObject())
                        {
                            Component new_component;
                            new_component.type = component.value["type"].GetString();
                            if (new_component.type == "Rigidbody")
                            {
                                // now we need to loop thru the values to fill the constructor for the rigid body.
                                // DEFAULT VALUES! :)
                                float x = 0.0f;
                                float y = 0.0f;
                                std::string body_type = "dynamic";
                                bool precise = true;
                                float gravity_scale = 1.0f;
                                float density = 1.0f;
                                float angular_friction = 0.3f;
                                float rotation = 0.0f;
                                bool has_collider = true;
                                bool has_trigger = true;
                                
                                // collider info:
                                std::string collider_type = "box";
                                float collider_width = 1.0f;
                                float collider_height = 1.0f;
                                float collider_radius = 0.5f;
                                float collider_friction = 0.3f;
                                float collider_bounciness = 0.3f;

                                // more trigger info:
                                std::string trigger_type = "box";
                                float trigger_width = 1.0f;
                                float trigger_height = 1.0f;
                                float trigger_radius = 0.5f;
                                
                                if (component.value.HasMember("x"))
                                {
                                    x = component.value["x"].GetFloat();
                                }
                                if (component.value.HasMember("y"))
                                {
                                    y = component.value["y"].GetFloat();
                                }
                                if (component.value.HasMember("body_type"))
                                {
                                    body_type = component.value["body_type"].GetString();
                                }
                                if (component.value.HasMember("precise"))
                                {
                                    precise = component.value["precise"].GetBool();
                                }
                                if (component.value.HasMember("gravity_scale"))
                                {
                                    gravity_scale = component.value["gravity_scale"].GetFloat();
                                }
                                if (component.value.HasMember("density"))
                                {
                                    density = component.value["density"].GetFloat();
                                }
                                if (component.value.HasMember("angular_friction"))
                                {
                                    angular_friction = component.value["angular_friction"].GetFloat();
                                }
                                if (component.value.HasMember("rotation"))
                                {
                                    rotation = component.value["rotation"].GetFloat();
                                }
                                if (component.value.HasMember("has_collider"))
                                {
                                    has_collider = component.value["has_collider"].GetBool();
                                }
                                if (component.value.HasMember("has_trigger"))
                                {
                                    has_trigger = component.value["has_trigger"].GetBool();
                                }
                                // collider overrides:
                                if (component.value.HasMember("collider_type"))
                                {
                                    collider_type = component.value["collider_type"].GetString();
                                }
                                if (component.value.HasMember("width"))
                                {
                                    collider_width = component.value["width"].GetFloat();
                                }
                                if (component.value.HasMember("height"))
                                {
                                    collider_height = component.value["height"].GetFloat();
                                }
                                if (component.value.HasMember("radius"))
                                {
                                    collider_radius = component.value["radius"].GetFloat();
                                }
                                if (component.value.HasMember("friction"))
                                {
                                    collider_friction = component.value["friction"].GetFloat();
                                }
                                if (component.value.HasMember("bounciness"))
                                {
                                    collider_bounciness = component.value["bounciness"].GetFloat();
                                }

                                // trigger overrides:
                                if (component.value.HasMember("trigger_type"))
                                {
                                    trigger_type = component.value["trigger_type"].GetString();
                                }
                                if (component.value.HasMember("trigger_width"))
                                {
                                    trigger_width = component.value["trigger_width"].GetFloat();
                                }
                                if (component.value.HasMember("trigger_height"))
                                {
                                    trigger_height = component.value["trigger_height"].GetFloat();
                                }
                                if (component.value.HasMember("trigger_radius"))
                                {
                                    trigger_radius = component.value["trigger_radius"].GetFloat();
                                }


                                // create rigid body (TODO: might need to make a vector of shared pointers?)
                                new_component.type = "Rigidbody";
                                new_component.enabled = true;
                                // create the rigid body
                                RigidBody* rigid_body_component = new RigidBody(
                                                                                x,
                                                                                y,
                                                                                body_type,
                                                                                precise,
                                                                                gravity_scale,
                                                                                density,
                                                                                angular_friction,
                                                                                rotation,
                                                                                has_collider,
                                                                                has_trigger,
                                                                                collider_type,
                                                                                collider_width,
                                                                                collider_height,
                                                                                collider_radius,
                                                                                collider_friction,
                                                                                collider_bounciness,
                                                                                imported_actors.back().get(),
                                                                                trigger_type,
                                                                                trigger_width,
                                                                                trigger_height,
                                                                                trigger_radius);
                                rigid_body_component->key = local_component_name;
                                // have lua reference to the rigid body.
                                luabridge::LuaRef luaRigidBodyRef = luabridge::LuaRef(ComponentManager::_lua_state, rigid_body_component);

                                new_component.lua_table = std::make_shared<luabridge::LuaRef>(luaRigidBodyRef);
                                imported_actors.back()->components.insert({local_component_name, new_component});
                            }
                            else
                            {
                                // let's see if this component exists.
                                check_if_lua_file_exists(new_component.type);
                                // Load and get the base table from the Lua state
                                luabridge::LuaRef baseTable = *ComponentManager::_component_tables[new_component.type];

                                // Create a new table for the component instance
                                new_component.lua_table = std::make_shared<luabridge::LuaRef>(luabridge::newTable(ComponentManager::_lua_state));
                                
                                ComponentManager::establish_inheritance(*new_component.lua_table, baseTable);

                                // we will write properties to the lua table.
                                for (auto& property : component.value.GetObject())
                                {
                                    if (property.name != "type")
                                    {
                                        std::string property_name = property.name.GetString();
                                        if (property.value.IsString())
                                        {
                                            (*new_component.lua_table)[property_name] = property.value.GetString();
                                        }
                                        else if (property.value.IsFloat())
                                        {
                                            (*new_component.lua_table)[property_name] = property.value.GetDouble();
                                        }
                                        else if (property.value.IsInt())
                                        {
                                            (*new_component.lua_table)[property_name] = property.value.GetInt();
                                        }
                                        else if (property.value.IsBool())
                                        {
                                            (*new_component.lua_table)[property_name] = property.value.GetBool();
                                        }
                                    }
                                }
                                
                                
                                (*new_component.lua_table)["enabled"] = new_component.enabled;
                                (*new_component.lua_table)["type"] = new_component.type;
                                (*new_component.lua_table)["key"] = local_component_name;
                                // inject actor to be pointer to actor.
                                imported_actors.back()->InjectReference(new_component.lua_table);
                            
                                imported_actors.back()->components.insert({local_component_name, new_component});
                            }
                        }
                    }
                }

            }
        }
    }
    else
    {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }
}
// Actor* SceneManager::load_in_template(const std::string& template_name)
std::shared_ptr<Actor> SceneManager::load_in_template(const std::string& template_name)
 {
    std::string template_path = "resources/actor_templates/" + template_name + ".template";
    if (!std::filesystem::exists(template_path))
    {
        std::cerr << "error: failed to locate template " << template_name;
        exit(0);
    }
    std::string name;
    std::string component_name;
    Component new_component;
    std::vector<std::pair<std::string, Component>> template_components;

    rapidjson::Document template_document;
    ConfigManager::read_json_file(template_path, template_document);
    if (template_document.HasMember("name"))
    {
        name = template_document["name"].GetString();
    }

    std::shared_ptr<Actor> actor = std::make_shared<Actor>(name);

    // Load components
    if (template_document.HasMember("components") && template_document["components"].IsObject())
    {
        const rapidjson::Value& components = template_document["components"];
        for (auto& component : components.GetObject())
        {
            component_name = component.name.GetString();
            if (component.value.IsObject())
            {
                // Component new_component;
                new_component.type = component.value["type"].GetString();
                if (new_component.type == "Rigidbody")
                {
                    // now we need to loop thru the values to fill the constructor for the rigid body.
                    // DEFAULT VALUES! :)
                    float x = 0.0f;
                    float y = 0.0f;
                    std::string body_type = "dynamic";
                    bool precise = true;
                    float gravity_scale = 1.0f;
                    float density = 1.0f;
                    float angular_friction = 0.3f;
                    float rotation = 0.0f;
                    bool has_collider = true;
                    bool has_trigger = true;
                    
                    // collider info:
                    std::string collider_type = "box";
                    float collider_width = 1.0f;
                    float collider_height = 1.0f;
                    float collider_radius = 0.5f;
                    float collider_friction = 0.3f;
                    float collider_bounciness = 0.3f;

                    // more trigger info:
                    std::string trigger_type = "box";
                    float trigger_width = 1.0f;
                    float trigger_height = 1.0f;
                    float trigger_radius = 0.5f;
                    
                    if (component.value.HasMember("x"))
                    {
                        x = component.value["x"].GetFloat();
                    }
                    if (component.value.HasMember("y"))
                    {
                        y = component.value["y"].GetFloat();
                    }
                    if (component.value.HasMember("body_type"))
                    {
                        body_type = component.value["body_type"].GetString();
                    }
                    if (component.value.HasMember("precise"))
                    {
                        precise = component.value["precise"].GetBool();
                    }
                    if (component.value.HasMember("gravity_scale"))
                    {
                        gravity_scale = component.value["gravity_scale"].GetFloat();
                    }
                    if (component.value.HasMember("density"))
                    {
                        density = component.value["density"].GetFloat();
                    }
                    if (component.value.HasMember("angular_friction"))
                    {
                        angular_friction = component.value["angular_friction"].GetFloat();
                    }
                    if (component.value.HasMember("rotation"))
                    {
                        rotation = component.value["rotation"].GetFloat();
                    }
                    if (component.value.HasMember("has_collider"))
                    {
                        has_collider = component.value["has_collider"].GetBool();
                    }
                    if (component.value.HasMember("has_trigger"))
                    {
                        has_trigger = component.value["has_trigger"].GetBool();
                    }
                    // collider overrides:
                    if (component.value.HasMember("collider_type"))
                    {
                        collider_type = component.value["collider_type"].GetString();
                    }
                    if (component.value.HasMember("width"))
                    {
                        collider_width = component.value["width"].GetFloat();
                    }
                    if (component.value.HasMember("height"))
                    {
                        collider_height = component.value["height"].GetFloat();
                    }
                    if (component.value.HasMember("radius"))
                    {
                        collider_radius = component.value["radius"].GetFloat();
                    }
                    if (component.value.HasMember("friction"))
                    {
                        collider_friction = component.value["friction"].GetFloat();
                    }
                    if (component.value.HasMember("bounciness"))
                    {
                        collider_bounciness = component.value["bounciness"].GetFloat();
                    }

                    // trigger overrides:
                    if (component.value.HasMember("trigger_type"))
                    {
                        trigger_type = component.value["trigger_type"].GetString();
                    }
                    if (component.value.HasMember("trigger_width"))
                    {
                        trigger_width = component.value["trigger_width"].GetFloat();
                    }
                    if (component.value.HasMember("trigger_height"))
                    {
                        trigger_height = component.value["trigger_height"].GetFloat();
                    }
                    if (component.value.HasMember("trigger_radius"))
                    {
                        trigger_radius = component.value["trigger_radius"].GetFloat();
                    }


                    // create rigid body (TODO: might need to make a vector of shared pointers?)
                    new_component.type = "Rigidbody";
                    new_component.enabled = true;
                    // create the rigid body
                    RigidBody* rigid_body_component = new RigidBody();
                    // apply properties of the template to this new rigid body...?
                    rigid_body_component->_x = x;
                    rigid_body_component->_y = y;
                    rigid_body_component->_body_type = body_type;
                    rigid_body_component->_precise = precise;
                    rigid_body_component->_gravity_scale = gravity_scale;
                    rigid_body_component->_density = density;
                    rigid_body_component->_angular_friction = angular_friction;
                    rigid_body_component->_rotation = rotation;
                    rigid_body_component->_has_collider = has_collider;
                    rigid_body_component->_has_trigger = has_trigger;
                    rigid_body_component->_collider_type = collider_type;
                    rigid_body_component->_collider_width = collider_width;
                    rigid_body_component->_collider_height = collider_height;
                    rigid_body_component->_collider_radius = collider_radius;
                    rigid_body_component->_collider_friction = collider_friction;
                    rigid_body_component->_collider_bounciness = collider_bounciness;
                    rigid_body_component->_trigger_type = trigger_type;
                    rigid_body_component->_trigger_width = trigger_width;
                    rigid_body_component->_trigger_height = trigger_height;
                    rigid_body_component->_trigger_radius = trigger_radius;
                    rigid_body_component->_actor = actor.get();
                
                    rigid_body_component->key = component_name;
                    // have lua reference to the rigid body.
                    luabridge::LuaRef luaRigidBodyRef = luabridge::LuaRef(ComponentManager::_lua_state, rigid_body_component);

                    new_component.lua_table = std::make_shared<luabridge::LuaRef>(luaRigidBodyRef);
                    // imported_actors.back()->components.insert({component_name, new_component});
                    actor->components.insert({component_name, new_component});
                }
                else
                {
                // let's see if this component exists.
                check_if_lua_file_exists(new_component.type);
                // Load and get the base table from the Lua state
                // std::cout << "before inheritance\n";
                luabridge::LuaRef baseTable = *ComponentManager::_component_tables[new_component.type];
                // printLuaTable(baseTable);
                // Create a new table for the component instance
                // std::cout << "before new table\n";
                new_component.lua_table = std::make_shared<luabridge::LuaRef>(luabridge::newTable(ComponentManager::_lua_state));
                // printLuaTable(*new_component.lua_table);
                // establish inheritance
                ComponentManager::establish_inheritance(*new_component.lua_table, baseTable);
                // std::cout << "after inheritance\n";
                // printLuaTable(*new_component.lua_table);
                // inject reference
                

                // we will write properties to the lua table.
                for (auto& property : component.value.GetObject())
                {
                    if (property.name != "type")
                    {
                        std::string property_name = property.name.GetString();
                        if (property.value.IsString())
                        {
                            (*new_component.lua_table)[property_name] = property.value.GetString();
                        }
                        else if (property.value.IsFloat())
                        {
                            (*new_component.lua_table)[property_name] = property.value.GetDouble();
                        }
                        else if (property.value.IsInt())
                        {
                            (*new_component.lua_table)[property_name] = property.value.GetInt();
                        }
                        else if (property.value.IsBool())
                        {
                            (*new_component.lua_table)[property_name] = property.value.GetBool();
                        }
                    }
                }
                // test.
                (*new_component.lua_table)["enabled"] = new_component.enabled;
                (*new_component.lua_table)["type"] = new_component.type;
                (*new_component.lua_table)["key"] = component_name;
                // inject reference
                actor->InjectReference(new_component.lua_table);
                
                actor->components.insert({component_name, new_component});

                // Establish inheritance
                // ComponentManager::establish_inheritance(*new_component.lua_table, baseTable);
                // push this to the vector of components.
                // template_components.push_back({component_name, new_component});
            }
            }
        }
    }
    return actor;
}   

