#pragma once

#include "PGEvent.h"
#include <set>

class PG_API PGSystemEventDispatcher {

public:
    PGSystemEventDispatcher();
    virtual ~PGSystemEventDispatcher() = default;
    
    void RegisterListener(ISystemEventListener* listener);
    void RemoveListener(ISystemEventListener* listener);
    void DispatchSystemEvent(SystemEvent event, uint64_t param1 = 0, uint64_t param2 = 0);

private:
    std::set<ISystemEventListener*> m_listeners;
};

