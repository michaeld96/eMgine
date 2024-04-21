#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H
#include "Common.h"
#include "MapHelper.h"

// forward declaration
class Actor;

// class gets called when two objects collide (trigger or collider)
class ContactListener : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    
};

class PhysicsManager
{
public:
    static b2World* physics_world;

    // PhysicsManager()
    // {
    //     b2Vec2 gravity = b2Vec2(0.0f, 9.8f);
    //     physics_world = new b2World(gravity);
    // }
    static void Init()
    {
        b2Vec2 gravity = b2Vec2(0.0f, 9.8f);
        physics_world = new b2World(gravity);
        physics_world->SetContactListener(new ContactListener());
    }

    ~PhysicsManager()
    {
        delete physics_world;
    }

    static void WorldStep()
    {
        const float TIME_STEP = 1.0f / 60.0f;
        const int VELOCITY_ITERATIONS = 8;
        const int POSITION_ITERATIONS = 3;
        physics_world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    }

    static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist);
    static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);


};



// This is the type for OnCollisionEnter, OnCollisionExit, OnTriggerEnter, OnTriggerExit
class Collision
{
public:
    Actor* other;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

class HitResult
{
public:
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger;
    float fraction;
};

class RayCastCallback : public b2RayCastCallback
{
public:
    std::vector<HitResult> hits;
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
};

#endif // PHYSICS_MANAGER_H