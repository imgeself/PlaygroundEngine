#include "PGSystemEventDispatcher.h"

PGSystemEventDispatcher::PGSystemEventDispatcher() {

}

void PGSystemEventDispatcher::RegisterListener(ISystemEventListener* listener) {
    m_listeners.insert(listener);
}

void PGSystemEventDispatcher::RemoveListener(ISystemEventListener* listener) {
    m_listeners.erase(listener);
}

void PGSystemEventDispatcher::DispatchSystemEvent(SystemEvent event) {
    for (ISystemEventListener* listener : m_listeners) {
        if (listener) {
            listener->OnSystemEvent(event);
        }
    }
}

