#include "core/EventDispatcher.hpp"
#include <utility>

void EventDispatcher::TriggerEvent(Event e) {
    EventDispatcher::queue.push_back(std::move(e));
};

void EventDispatcher::Subscribe(EventType type, ListenerFn fn) {
    EventDispatcher::listeners[type].push_back(std::move(fn));
};

void EventDispatcher::ProcessQueue() {
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