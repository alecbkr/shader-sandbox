#include "core/EventDispatcher.hpp"
#include "core/logging/Logger.hpp"

EventDispatcher::EventDispatcher() {
    initialized = false;
    listeners.clear();
    queue.clear();
    loggerPtr = nullptr;
}

bool EventDispatcher::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Event Dispatcher Initialization", "Event Dispatcher was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    listeners.clear();
    queue.clear();

    initialized = true;
    return true;
}

void EventDispatcher::shutdown() {
    if (!initialized) return;
    queue.clear();
    listeners.clear();
    initialized = false;
    loggerPtr = nullptr;
}

void EventDispatcher::TriggerEvent(Event e) {
    if (!initialized) return;
    EventDispatcher::queue.push_back(std::move(e));
};

void EventDispatcher::Subscribe(EventType type, ListenerFn fn) {
    if (!initialized) return;
    EventDispatcher::listeners[type].push_back(std::move(fn));
};

void EventDispatcher::ProcessQueue() {
    if (!initialized) return;
    while (!queue.empty()) {
        auto e = std::move(queue.front());
        queue.pop_front();

        auto it = listeners.find(e.type);
        if (it == listeners.end()) continue;

        for (auto& fn : it->second) {
            if (e.handled) break;
            if (fn(e.payload)) e.handled = true;
        }
    }
}