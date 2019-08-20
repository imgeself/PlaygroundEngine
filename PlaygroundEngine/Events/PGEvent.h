#pragma once

enum SystemEvent {
    INITIALIZE,
    CLOSE,
};

struct ISystemEventListener {

public:
    virtual ~ISystemEventListener() = default;
    virtual void OnSystemEvent(SystemEvent event) = 0;
};

struct ISystemEventDispatcher {
    
public:
    virtual ~ISystemEventDispatcher() = default;

    virtual void RegisterListener(ISystemEventListener* listener) = 0;
    virtual void RemoveListener(ISystemEventListener* listener) = 0;
    virtual void DispatchSystemEvent(SystemEvent event) = 0;
};