#pragma once
#include "MonoBehaviour.h"
class UIButton : public MonoBehaviour
{
public:
    UIButton();
    virtual ~UIButton();
    virtual void Update() override;

    void SetClickCallback(std::function<void()> callback) { _onClick = callback; }

    bool IsHovered() const { return _isHovered; }
    bool IsPressed() const { return _isPressed; }

private:
    void UpdateMouseInput();
    bool IsMouseOver();

private:
    bool _isHovered = false;
    bool _isPressed = false;
    std::function<void()> _onClick;
};

