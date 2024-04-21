#include "ComponentManager.h"
#include "SceneManager.h"
#include "RigidBody.h"
#include "PhysicsManager.h"
#include "EventBus.h"



void ComponentManager::init_functions()
{
    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Debug")
        .addFunction("Log", &DebugPrint)
        .addFunction("LogError", &DebugPrintError)
        .endNamespace(); // this creates a table and adds the functions to it.

    luabridge::getGlobalNamespace(_lua_state)
        .beginClass<Actor>("Actor")
        .addStaticFunction("Find", &Actor::Find)
        .addStaticFunction("FindAll", &Actor::FindAll)
        .addStaticFunction("Instantiate", &Actor::Instantiate)
        .addStaticFunction("Destroy", &Actor::Destroy)
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponent)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass();


    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Application")
        .addFunction("Quit", &ApplicationQuit)
        .addFunction("Sleep", &ApplicationSleep)
        .addFunction("GetFrame", &ApplicationGetFrame)
        .addFunction("OpenUrl", &ApplicationOpenUrl)
        .endNamespace(); 

    luabridge::getGlobalNamespace(_lua_state)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();
    
    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Input")
        .addFunction("GetKey", &InputManager::string_get_key)
        .addFunction("GetKeyDown", &InputManager::string_get_key_down)
        .addFunction("GetKeyUp", &InputManager::string_get_key_up)
        .addFunction("GetMouseButton", &InputManager::GetMouseButton)
        .addFunction("GetMouseButtonDown", &InputManager::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", &InputManager::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &InputManager::GetMouseScrollDelta)
        .addFunction("GetMousePosition", &InputManager::GetMousePosition)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Text")
        .addFunction("Draw", &TextManager::Lua_DrawText)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Audio")
        .addFunction("Play", &AudioManager::Play)
        .addFunction("Halt", &AudioManager::Halt)
        .addFunction("SetVolume", &AudioManager::SetVolume)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Image")
        .addFunction("DrawUI", &ImageManager::DrawUI)
        .addFunction("DrawUIEx", &ImageManager::DrawUIEx)
        .addFunction("Draw", &ImageManager::Draw)
        .addFunction("DrawEx", &ImageManager::DrawEx)
        .addFunction("DrawPixel", &ImageManager::DrawPixel)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Camera")
        .addFunction("SetPosition", &Camera::SetPosition)
        .addFunction("GetPositionX", &Camera::GetPositionX)
        .addFunction("GetPositionY", &Camera::GetPositionY)
        .addFunction("SetZoom", &Camera::SetZoom)
        .addFunction("GetZoom", &Camera::GetZoom)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Scene")
        .addFunction("Load", &SceneManager::Load)
        .addFunction("GetCurrent", &SceneManager::GetCurrent)
        .addFunction("DontDestroy", &SceneManager::DontDestroy)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void(*)(float, float)>()
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addProperty("x", &b2Vec2::x)
        .addProperty("y", &b2Vec2::y)
        .addFunction("__add", &b2Vec2::operator_add) // went inside of Box2d and edit the code to allow for this.
        .addFunction("__sub", &b2Vec2::operator_sub) // this too
        .addFunction("__mul", &b2Vec2::operator_mul) // also this :|
        .addStaticFunction("Distance", &b2Distance)
        .addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endClass();

    luabridge::getGlobalNamespace(_lua_state)
        .beginClass<RigidBody>("Rigidbody")
        .addFunction("GetPosition", &RigidBody::GetPosition)
        .addFunction("GetRotation", &RigidBody::GetRotation)
        .addFunction("AddForce", &RigidBody::AddForce)
        .addFunction("SetVelocity", &RigidBody::SetVelocity)
        .addFunction("SetPosition", &RigidBody::SetPosition)
        .addFunction("SetRotation", &RigidBody::SetRotation)
        .addFunction("SetAngularVelocity", &RigidBody::SetAngularVelocity)
        .addFunction("SetGravityScale", &RigidBody::SetGravityScale)
        .addFunction("SetUpDirection", &RigidBody::SetUpDirection)
        .addFunction("SetRightDirection", &RigidBody::SetRightDirection)
        .addFunction("GetVelocity", &RigidBody::GetVelocity)
        .addFunction("GetAngularVelocity", &RigidBody::GetAngularVelocity)
        .addFunction("GetGravityScale", &RigidBody::GetGravityScale)
        .addFunction("GetUpDirection", &RigidBody::GetUpDirection)
        .addFunction("GetRightDirection", &RigidBody::GetRightDirection)
        .addData("gravity_scale", &RigidBody::_gravity_scale)
        .addData("x", &RigidBody::_x)
        .addData("y", &RigidBody::_y)
        .addData("rotation", &RigidBody::_rotation)
        .endClass();

    luabridge::getGlobalNamespace(_lua_state)
        .beginClass<Collision>("Collision")
        .addData("other", &Collision::other)
        .addData("point", &Collision::point)
        .addData("relative_velocity", &Collision::relative_velocity)
        .addData("normal", &Collision::normal)
        .endClass();

    luabridge::getGlobalNamespace(_lua_state)
        .beginClass<HitResult>("HitResult")
        .addData("actor", &HitResult::actor)
        .addData("point", &HitResult::point)
        .addData("normal", &HitResult::normal)
        .addData("is_trigger", &HitResult::is_trigger)
        .endClass();

    // note: lua doesn't like &
    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Physics")
        .addFunction("Raycast", &PhysicsManager::Raycast)
        .addFunction("RaycastAll", &PhysicsManager::RaycastAll)
        .endNamespace();

    luabridge::getGlobalNamespace(_lua_state)
        .beginNamespace("Event")
        .addFunction("Publish", &EventBus::Publish)
        .addFunction("Subscribe", &EventBus::Subscribe)
        .addFunction("Unsubscribe", &EventBus::Unsubscribe) 
        .endNamespace();
}
