#include "RigidBody.h"

float RigidBody::convert_rad_to_deg(float rad)
{
    return rad * (180.0f / b2_pi);
}

float RigidBody::convert_deg_to_rad(float deg)
{
    return deg * (b2_pi / 180.0f);
}

b2Vec2 RigidBody::GetPosition()
{
    if (body == nullptr)
    {
        return b2Vec2(this->_x, this->_y);
    }
    return body->GetPosition();
}

float RigidBody::GetRotation()
{
    // our user uses degrees, but box2d uses radians.
    // do conversion and then return the value.
    if (body == nullptr)
    {
        return this->_rotation;
    }
    return convert_rad_to_deg(body->GetAngle());
}

void RigidBody::AddForce(b2Vec2& force)
{
    if (body == nullptr)
    {
        this->_forces_to_be_applied.push_back(force);
    }
    body->ApplyForceToCenter(force, true);
}

void RigidBody::SetVelocity(b2Vec2& velocity)
{
    if (body == nullptr)
    {
        this->_velocities_to_be_applied.push_back(velocity);
    }
    body->SetLinearVelocity(velocity);
}

void RigidBody::SetPosition(b2Vec2& position)
{
    if (body == nullptr)
    {
        this->_x = position.x;
        this->_y = position.y;
        return;
    }
    body->SetTransform(position, body->GetAngle());
}

void RigidBody::SetRotation(float degrees_clockwise)
{
    // TODO: need to implement if needed.s
    if (body == nullptr)
    {
        this->_rotation = degrees_clockwise;
        return;
    }
    body->SetTransform(body->GetPosition(), convert_deg_to_rad(degrees_clockwise));
}

void RigidBody::SetAngularVelocity(float degrees_clockwise)
{
    body->SetAngularVelocity(convert_deg_to_rad(degrees_clockwise));
}

void RigidBody::SetGravityScale(float scale)
{
    body->SetGravityScale(scale);
}

void RigidBody::SetUpDirection(b2Vec2& direction)
{
    // normalize the direction
    b2Vec2 normalized_direction = direction;
    normalized_direction.Normalize();
    body->SetTransform(body->GetPosition(), glm::atan(normalized_direction.x, -normalized_direction.y));
    // body->SetTransform(body->GetPosition(), glm::atan(direction.x, -direction.y));
}

void RigidBody::SetRightDirection(b2Vec2& direction)
{
    // body->SetTransform(body->GetPosition(), glm::atan(direction.y, direction.x) );
    b2Vec2 normalized_direction = direction;
    normalized_direction.Normalize();

    body->SetTransform(body->GetPosition(), (glm::atan(normalized_direction.x, -normalized_direction.y) - (b2_pi / 2.0f)));
}

b2Vec2 RigidBody::GetVelocity()
{
    return body->GetLinearVelocity();
}

float RigidBody::GetAngularVelocity()
{
    // need to convert from rads to degrees.
    return convert_rad_to_deg(body->GetAngularVelocity());
    // return body->GetAngularVelocity();
}

float RigidBody::GetGravityScale()
{
    return body->GetGravityScale();
}

// b2Vec2 RigidBody::GetUpDirection()
// {
//     // up for us is -1 for box2d.
//     b2Vec2 upDirection(glm::cos(body->GetAngle() - (b2_pi / 2.0f)), glm::sin(body->GetAngle() - (b2_pi / 2.0f)));
//     upDirection.Normalize();
//     return upDirection;
// }

// Vector2( glm::sin(angle), -glm::cos(angle)).Normalized();

b2Vec2 RigidBody::GetUpDirection() 
{
    b2Vec2 upDirection(glm::sin(body->GetAngle()), -glm::cos(body->GetAngle())); // rotate (0,1) by the body's angle
    upDirection.Normalize();
    return upDirection; 
}


b2Vec2 RigidBody::GetRightDirection()
{
    b2Vec2 rightDirection(glm::cos(body->GetAngle()), glm::sin(body->GetAngle()));
    rightDirection.Normalize();
    return rightDirection;
}

void RigidBody::set_properties(
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
    // else
    // {
    //     std::cout << "error: invalid body type" << std::endl;
    //     exit(0);
    // }
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

    // now collider properties. // TODO: DO I NEED TO ADD DENSITIES TO COLLIDER FIXTURES???!???!???! (yes?)
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
