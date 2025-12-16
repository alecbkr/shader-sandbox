#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <deque>
#include <memory>

#include "core/EventTypes.hpp"

struct Event {
    EventType type;
    bool handled = false;
    EventPayload payload;
};

using ListenerFn = std::function<bool(const EventPayload&)>;

class EventDispatcher {
  public:
    void TriggerEvent(Event e);
    void Subscribe(EventType type, ListenerFn fn);
    void ProcessQueue();

  private:
    std::unordered_map<EventType, std::vector<ListenerFn>> listeners;
    std::deque<Event> queue;
};