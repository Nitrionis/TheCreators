#pragma once

#include "BaseCode.h"

class Noncopyable {
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(Noncopyable const & ) = delete;
    Noncopyable& operator=(Noncopyable const &) = delete;
};

class StandardBehavior : public BaseCode {
public:
    StandardBehavior() {}
    ~StandardBehavior() {}
    // "Start" is called once when the script is initialized.
    virtual void Start() {}
    // "Update" is called once per frame.
    virtual void Update() {}
    // "LateUpdate" is called once per frame, after the Update.
    virtual void LateUpdate() {}
    // "OnApplicationPause" is called once when application on paused.
    virtual void OnApplicationPause() {}
    // "OnApplicationClosed" is called once when application closed.
    virtual void OnApplicationClosed() {}
};