#ifndef RIGID_BODY_H
#define RIGID_BODY_H
#include "Common.h"
#include "PhysicsManager.h"

class RigidBody
{
public:
    b2Body* body;
    b2BodyDef body_def;
    b2PolygonShape my_shape;
    b2FixtureDef fixture_def;
    b2CircleShape my_circle;
    float _x;
    float _y;
    std::string _body_type;
    bool _precise;
    float _gravity_scale;
    float _density;
    float _angular_friction;
    float _rotation;
    bool _has_collider;
    bool _has_trigger;
    std::string _collider_type;
    float _collider_width;
    float _collider_height;
    float _collider_radius;
    float _collider_friction;
    float _collider_bounciness;
    Actor* _actor;
    std::string _trigger_type;
    float _trigger_width;
    float _trigger_height;
    float _trigger_radius;
    std::vector<b2Vec2> _forces_to_be_applied;
    std::vector<b2Vec2> _velocities_to_be_applied;
    std::string key;

    // default constructor.
    RigidBody()
    {
        body = nullptr;
        _x = 0.0f;
        _y = 0.0f;
        _body_type = "dynamic";
        _precise = true;
        _gravity_scale = 1.0f;
        _density = 1.0f;
        _angular_friction = 0.0f;
        _rotation = 0.0f;
        _has_collider = true;
        _has_trigger = true;
        _collider_type = "box";
        _collider_width = 1.0f;
        _collider_height = 1.0f;
        _collider_radius = 0.5f;
        _collider_friction = 0.3f;
        _collider_bounciness = 0.3f;
        _actor = nullptr;
        _trigger_type = "box";
        _trigger_width = 1.0f;
        _trigger_height = 1.0f;
        _trigger_radius = 0.5f;
    }
    
    RigidBody( 
              float in_x,
              float in_y,
              std::string in_body_type,
              bool in_precise,
              float in_gravity_scale,
              float in_density,
              float in_angular_friction,
              float in_rotation,
              bool in_has_collider,
              bool in_has_trigger,
              std::string in_collider_type,
              float in_collider_width,
              float in_collider_height,
              float in_collider_radius,
              float in_collider_friction,
              float in_collider_bounciness,
              Actor* actor,
              std::string trigger_type,
              float trigger_width,
              float trigger_height,
              float trigger_radius)
    {
        bool _is_default = true;
        // set body def position.
        body_def.position.Set(in_x, in_y);
        // set body type.
        if (in_body_type == "static")
        {
            body_def.type = b2_staticBody;
        }
        else if (in_body_type == "dynamic")
        {
            body_def.type = b2_dynamicBody;
        }
        else if (in_body_type == "kinematic")
        {
            body_def.type = b2_kinematicBody;
        }
        else
        {
            std::cout << "error: invalid body type" << std::endl;
            exit(0);
        }
        // set body def bullet.
        if (in_precise)
        {
            body_def.bullet = true;
        }
        else
        {
            body_def.bullet = false;
        }
        // set gravity scale.
        body_def.gravityScale = in_gravity_scale;
        // set angular friction.
        body_def.angularDamping = in_angular_friction;
        // set rotation.
        body_def.angle = convert_deg_to_rad(in_rotation);
        // create body into the world.
        body = PhysicsManager::physics_world->CreateBody(&body_def);

        // now collider properties. // TODO: DO I NEED TO ADD DENSITIES TO COLLIDER FIXTURES???!???!???!
        if (in_has_collider)
        {
            _is_default = false;
            b2FixtureDef collider_fixture;
            collider_fixture.isSensor = false;
            if (in_collider_type == "box")
            {
                //b2PolygonShape collider_shape;
                b2PolygonShape* collider_shape = new b2PolygonShape();
                collider_shape->SetAsBox(in_collider_width * 0.5f, in_collider_height * 0.5f);
                collider_fixture.shape = collider_shape;
            }
            else if (in_collider_type == "circle")
            {
                //b2CircleShape collider_circle;
                b2CircleShape* collider_circle = new b2CircleShape();
                collider_circle->m_radius = in_collider_radius;
                collider_fixture.shape = collider_circle;
            }
            else
            {
                std::cout << "error: invalid collider type" << std::endl;
                exit(0);
            }
            // set collider friction and bounciness.
            collider_fixture.friction = in_collider_friction;
            collider_fixture.restitution = in_collider_bounciness;
            collider_fixture.density = in_density;
            // set userdata
            collider_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
            // now add the fixture to the body.
            body->CreateFixture(&collider_fixture);
        }

        // now trigger properties.
        if (in_has_trigger)
        {
            _is_default = false;
            b2FixtureDef trigger_fixture;
            trigger_fixture.isSensor = true;
            if (trigger_type == "box")
            {
                //b2PolygonShape trigger_shape;
                b2PolygonShape* trigger_shape = new b2PolygonShape();
                trigger_shape->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
                trigger_fixture.shape = trigger_shape;
            }
            else if (trigger_type == "circle")
            {
                //b2CircleShape trigger_circle;
                b2CircleShape* trigger_circle = new b2CircleShape();
                trigger_circle->m_radius = trigger_radius;
                trigger_fixture.shape = trigger_circle;
            }
            else
            {
                std::cout << "error: invalid trigger type" << std::endl;
                exit(0);
            }
            // set userdata
            trigger_fixture.density = in_density;
            trigger_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
            // now add the fixture to the body.
            body->CreateFixture(&trigger_fixture);
        }
        // phantom sensor to make bodies move if neither collider nor trigger is set.
        if (!in_has_collider && !in_has_trigger)
        {
            _is_default = false;
            b2PolygonShape phantom_shape;
            phantom_shape.SetAsBox(in_collider_width * 0.5f, in_collider_height * 0.5f);

            b2FixtureDef phantom_fixture_def;
            phantom_fixture_def.shape = &phantom_shape;
            phantom_fixture_def.density = in_density;

            // because it is a sensor (with no callback even), no collision will ever occur.
            phantom_fixture_def.isSensor = true;
            body->CreateFixture(&phantom_fixture_def);
        }
        // create basic shape and fixtures for the body (default case)
        if (_is_default)
        {
            // create basic fixture for the body.
            my_shape.SetAsBox(0.5f, 0.5f);
            // set fixture shape.
            fixture_def.shape = &my_shape;
            // set default density. (Other fixtures will change this...?)
            fixture_def.density = 1.0f;

            

            // set userdata
            fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);
            body->CreateFixture(&fixture_def);
        }
    }
    /*************** INTERNAL HELPERS ***************/
    float convert_rad_to_deg(float rad);
    float convert_deg_to_rad(float deg);
    void set_properties(float in_x,
                        float in_y,
                        std::string in_body_type,
                        bool in_precise,
                        float in_gravity_scale,
                        float in_density,
                        float in_angular_friction,
                        float in_rotation,
                        bool in_has_collider,
                        bool in_has_trigger,
                        std::string in_collider_type,
                        float in_collider_width,
                        float in_collider_height,
                        float in_collider_radius,
                        float in_collider_friction,
                        float in_collider_bounciness,
                        Actor* actor,
                        std::string trigger_type,
                        float trigger_width,
                        float trigger_height,
                        float trigger_radius);

    /*************** FROM C++ TO LUA FUNCS ***************/
    b2Vec2 GetPosition();
    float GetRotation();
    void AddForce(b2Vec2& force);

    void SetVelocity(b2Vec2& velocity);
    void SetPosition(b2Vec2& position);
    void SetRotation(float degrees_clockwise);
    void SetAngularVelocity(float degrees_clockwise);
    void SetGravityScale(float scale);
    void SetUpDirection(b2Vec2& direction);
    void SetRightDirection(b2Vec2& direction);

    b2Vec2 GetVelocity();
    float GetAngularVelocity();
    float GetGravityScale();
    b2Vec2 GetUpDirection();
    b2Vec2 GetRightDirection();
    
};

#endif // RIGID_BODY_H
