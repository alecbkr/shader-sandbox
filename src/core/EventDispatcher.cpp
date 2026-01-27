#include "core/EventDispatcher.hpp"
#include <utility>
#include "core/logging/Logger.hpp"

bool EventDispatcher::initialized = false;
std::unordered_map<EventType, std::vector<ListenerFn>> EventDispatcher::listeners{};
std::deque<Event> EventDispatcher::queue{};

bool EventDispatcher::initialize() {
    listeners.clear();
    queue.clear();
    initialized = true;
    return true;
}

void EventDispatcher::shutdown() {
    queue.clear();
    listeners.clear();
    initialized = false;
}

void EventDispatcher::TriggerEvent(Event e) {
    if (!initialized) return;
    Logger::addLog(LogLevel::INFO, "EventDispatcher::TriggerEvent", "An event was triggered.");
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