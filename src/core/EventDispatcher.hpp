#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <deque>
#include <memory>

#include "core/EventTypes.hpp"

using ListenerFn = std::function<bool(const EventPayload&)>;

class EventDispatcher {
public:
    static bool initialize();
    static void shutdown();
    static void TriggerEvent(Event e);
    static void Subscribe(EventType type, ListenerFn fn);
    static void ProcessQueue();

private:
    static bool initialized;
    static std::unordered_map<EventType, std::vector<ListenerFn>> listeners;
    static std::deque<Event> queue;
};