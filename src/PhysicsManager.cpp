#include "PhysicsManager.h"
#include "ComponentManager.h"

b2World* PhysicsManager::physics_world = nullptr;

void ContactListener::BeginContact(b2Contact* contact)
{
    b2WorldManifold world_manifold;
    contact->GetWorldManifold(&world_manifold);
    // get the fixtures that collided
    b2Fixture* fixture_a = contact->GetFixtureA();
    b2Fixture* fixture_b = contact->GetFixtureB();

    // retrieve the actors from the fixtures
    Actor* actor_a = reinterpret_cast<Actor*>(fixture_a->GetUserData().pointer);
    Actor* actor_b = reinterpret_cast<Actor*>(fixture_b->GetUserData().pointer);

    if (actor_a == nullptr || actor_b == nullptr)
    {
        return;
    }
    
    // create a new collision object
    Collision collision;
    collision.other = actor_b;
    collision.point = world_manifold.points[0];
    collision.relative_velocity = fixture_a->GetBody()->GetLinearVelocity() - fixture_b->GetBody()->GetLinearVelocity();
    collision.normal = world_manifold.normal;

    if (fixture_a->IsSensor() && fixture_b->IsSensor())
    {
        // set these for OnTriggerEnter
        collision.point = b2Vec2(-999.0f, -999.0f);
        collision.normal = b2Vec2(-999.0f, -999.0f);
        // call life cycle on with fixture A, then B.
        for (auto& [key, value] : actor_a->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnTriggerEnter"].isFunction())
                {
                    try
                    {
                        component["OnTriggerEnter"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cerr << "LuaException: " << e.what() << std::endl;
                    }
                }
            }
        }
        
        // now call OnTriggerEnter on fixture B
        collision.other = actor_a;
        for (auto& [key, value] : actor_b->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnTriggerEnter"].isFunction())
                {
                    try
                    {
                        component["OnTriggerEnter"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cerr << "LuaException: " << e.what() << std::endl;
                    }
                }
            }
        }
    }
    else if (!fixture_a->IsSensor() && !fixture_b->IsSensor())
    {

        // call life cycle on with fixture A, then B.
        for (auto& [key, value] : actor_a->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnCollisionEnter"].isFunction())
                {
                    try
                    {
                        component["OnCollisionEnter"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cerr << "LuaException: " << e.what() << std::endl;
                    }
                }
            }
        }
        
        // now call OnCollisionEnter on fixture B
        collision.other = actor_a;
        collision.point = world_manifold.points[0];
        collision.relative_velocity = fixture_a->GetBody()->GetLinearVelocity() - fixture_b->GetBody()->GetLinearVelocity();
        collision.normal = world_manifold.normal;
        for (auto& [key, value] : actor_b->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnCollisionEnter"].isFunction())
                {
                    try
                    {
                        component["OnCollisionEnter"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cerr << "LuaException: " << e.what() << std::endl;
                    }
                }
            }
        }
    }
}

void ContactListener::EndContact(b2Contact* contact)
{
    // get the fixtures that collided
    b2Fixture* fixture_a = contact->GetFixtureA();
    b2Fixture* fixture_b = contact->GetFixtureB();

    // retrieve the actors from the fixtures
    Actor* actor_a = reinterpret_cast<Actor*>(fixture_a->GetUserData().pointer);
    Actor* actor_b = reinterpret_cast<Actor*>(fixture_b->GetUserData().pointer);

    if (actor_a == nullptr || actor_b == nullptr)
    {
        return;
    }

    // on EndContact two of the four values in collision object are invalid: set 
    // point and normal to -999.0f
    Collision collision;
    collision.other = actor_b;
    collision.point = b2Vec2(-999.0f, -999.0f);
    collision.relative_velocity = fixture_a->GetBody()->GetLinearVelocity() - fixture_b->GetBody()->GetLinearVelocity();
    collision.normal = b2Vec2(-999.0f, -999.0f);
    if (fixture_a->IsSensor() && fixture_b->IsSensor())
    {
        // call life cycle on with fixture A, then B.
        for (auto& [key, value] : actor_a->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnTriggerExit"].isFunction())
                {
                    try
                    {
                        component["OnTriggerExit"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cout << "\033[31m" << actor_a->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                    }
                }
            }
        }
        
        // now call OnTriggerEnter on fixture B
        collision.other = actor_a;
        for (auto& [key, value] : actor_b->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnTriggerExit"].isFunction())
                {
                    try
                    {
                        component["OnTriggerExit"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cout << "\033[31m" << actor_b->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                    }
                }
            }
        }
    }
    else if (!fixture_a->IsSensor() && !fixture_b->IsSensor())
    {
        // loop thru all components of actor A and B and call OnCollisionExit
        for (auto& [key, value] : actor_a->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnCollisionExit"].isFunction())
                {
                    try
                    {
                        component["OnCollisionExit"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cout << "\033[31m" << actor_a->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                    }
                }
            }
        }

        collision.other = actor_a;
        collision.relative_velocity = fixture_a->GetBody()->GetLinearVelocity() - fixture_b->GetBody()->GetLinearVelocity();

        for (auto& [key, value] : actor_b->components)
        {
            if (value.enabled)
            {
                luabridge::LuaRef component = *value.lua_table;
                if (component["OnCollisionExit"].isFunction())
                {
                    try
                    {
                        component["OnCollisionExit"](component, collision);
                    }
                    catch (luabridge::LuaException const& e)
                    {
                        std::cout << "\033[31m" << actor_b->actor_name << " : " << e.what() << "\033[0m" << std::endl;
                    }
                }
            }
        }
    }
}

float RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
    // If the raycast starts inside a fixture, we ignore it (return -1)
    if (fraction == 0.0f)
    {
        return -1;
    }

    auto actor_check = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);

    if (actor_check == nullptr)
    {
        return -1;
    }

    HitResult hit;
    hit.actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
    hit.point = point;
    hit.normal = normal;
    hit.is_trigger = fixture->IsSensor();
    hit.fraction = fraction;
    hits.push_back(hit);
    // return fraction;  // this returns to tell box2d to stop at this point.
    return 1.0f; // this tells box2d to keep going.
    // return 0; // this tells box2d to stop at this point.
}

luabridge::LuaRef PhysicsManager::Raycast(b2Vec2 pos, b2Vec2 dir, float dist)
{
    if (dist <= 0)
    {
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
    b2Vec2 p1 = pos;
    dir.Normalize();
    b2Vec2 p2 = pos + (dist * dir);
    RayCastCallback callback;
    PhysicsManager::physics_world->RayCast(&callback, p1, p2);

    // if there are no hits or the distance is 0 or negative return nil
    if (callback.hits.size() == 0)
    {
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
    // sort the hits by distance
    // sort hits by fraction 
    std::sort(callback.hits.begin(), callback.hits.end(),
                [](const HitResult& a, const HitResult& b) {
                  return a.fraction < b.fraction;
              });

    // if there is a hit, return the first actor hit on the path of the ray.
    return luabridge::LuaRef(ComponentManager::_lua_state, callback.hits[0]);
}

luabridge::LuaRef PhysicsManager::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist)
{
    if (dist <= 0)
    {
        return luabridge::LuaRef(ComponentManager::_lua_state);
    }
    b2Vec2 p1 = pos;
    dir.Normalize();
    b2Vec2 p2 = pos + dist * dir;

    RayCastCallback callback;
    PhysicsManager::physics_world->RayCast(&callback, p1, p2);

    // sort distance along the raycast (nearest to farthest)
    std::sort(callback.hits.begin(), callback.hits.end(),
                [](const HitResult& a, const HitResult& b) {
                  return a.fraction < b.fraction;
              });
    
    // create a new table to store all the actors hit by the raycast.
    luabridge::LuaRef table = luabridge::newTable(ComponentManager::_lua_state);
    int index = 1;
    for (auto& hit : callback.hits)
    {
        if (Helper::GetFrameNumber() == 70)
        {
            int debug = -1;
        }
        // table["actor"] = hit.actor;
        table[index] = hit;
        index += 1;
    }
    return table;
}