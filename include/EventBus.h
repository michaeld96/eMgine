#ifndef EVENTBUS_H
#define EVENTBUS_H
#include "Common.h"
class EventBus
{
public:
    using EventHandler = std::pair<luabridge::LuaRef, luabridge::LuaRef>;
    using EventHandlers = std::vector<EventHandler>;

    static void Publish(const std::string& event_name, const luabridge::LuaRef& event_object)
    {
        if (subscriptions.find(event_name) != subscriptions.end())
        {
            for (auto& [component, function] : subscriptions[event_name])
            {
                if (function.isFunction())
                {
                    function(component, event_object);
                }
            }
        }
    }

    static void Subscribe(const std::string& event_name, const luabridge::LuaRef& component, const luabridge::LuaRef& function)
    {
        _pending_subscriptions.push_back({ event_name, { component, function } });
        // subscriptions[event_name].push_back({ component, function });
    }

    static void Unsubscribe(const std::string& event_name, const luabridge::LuaRef& component, const luabridge::LuaRef& function)
    {
        _pending_unsubscriptions.push_back({ event_name, { component, function } });
    }
    // call this to update the subscriptions.
    static void UpdateSubscriptions()
    {
        for (auto& [event_name, handler] : _pending_subscriptions)
        {
            subscriptions[event_name].push_back(handler);
        }
        _pending_subscriptions.clear();
        // now remove the unsubscriptions.
        // for (auto& [event_name, component] : _pending_unsubscriptions)
        // {
        //     if (subscriptions.find(event_name) != subscriptions.end())
        //     {
        //         auto& handlers = subscriptions[event_name];
        //         handlers.erase(std::remove_if(handlers.begin(), handlers.end(), [&](const EventHandler& handler) {
        //             return handler.first == component.first && handler.second == component.second;
        //         }), handlers.end());
        //     }
        // }
        // _pending_unsubscriptions.clear();
        for (auto& item : _pending_unsubscriptions)
        {
            const std::string& event_name = item.first;
            const EventHandler& unsub_handler = item.second;

            if (subscriptions.find(event_name) != subscriptions.end())
            {
                auto& handlers = subscriptions[event_name];
                handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
                    [&](const EventHandler& handler) {
                        return handler.first == unsub_handler.first && handler.second == unsub_handler.second;
                    }),
                    handlers.end());
            }
        }
        _pending_unsubscriptions.clear();

    }

    static void clean_up()
    {
        subscriptions.clear();
        _pending_subscriptions.clear();
        _pending_unsubscriptions.clear();
    }

private:
    static inline std::unordered_map<std::string, EventHandlers> subscriptions;
    static inline std::vector<std::pair<std::string, EventHandler>> _pending_subscriptions;
    static inline std::vector<std::pair<std::string, EventHandler>> _pending_unsubscriptions;
};
#endif // EVENTBUS_H