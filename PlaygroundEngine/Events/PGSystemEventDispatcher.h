#pragma once

#include "PGEvent.h"
#include <set>

class PGSystemEventDispatcher : public ISystemEventDispatcher {

public:
    PGSystemEventDispatcher();
    virtual ~PGSystemEventDispatcher() = default;
    
    virtual void RegisterListener(ISystemEventListener* listener) override;
    virtual void RemoveListener(ISystemEventListener* listener) override;
    virtual void DispatchSystemEvent(SystemEvent event) override;

private:
    std::set<ISystemEventListener*> m_listeners;
};

