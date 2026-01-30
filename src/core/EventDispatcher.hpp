#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <deque>
#include <memory>

#include "core/EventTypes.hpp"
#include "core/logging/Logger.hpp"

using ListenerFn = std::function<bool(const EventPayload&)>;

class EventDispatcher {
public:
    EventDispatcher();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    void TriggerEvent(Event e);
    void Subscribe(EventType type, ListenerFn fn);
    void ProcessQueue();

private:
    bool initialized = false;
    std::unordered_map<EventType, std::vector<ListenerFn>> listeners;
    std::deque<Event> queue;
    Logger* loggerPtr = nullptr;
};