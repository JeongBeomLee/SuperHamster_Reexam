#include "pch.h"
#include "UIButton.h"
#include "Input.h"
#include "Engine.h"

UIButton::UIButton()
{
}

UIButton::~UIButton()
{
}

void UIButton::Update()
{
    UpdateMouseInput();
}

void UIButton::UpdateMouseInput()
{
    _isHovered = IsMouseOver();

    if (_isHovered) {
        if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON)) {
            _isPressed = true;
        }
        else if (INPUT->GetButtonUp(KEY_TYPE::LBUTTON) && _isPressed) {
            _isPressed = false;
            if (_onClick)
                _onClick();
        }
    }
    else {
        _isPressed = false;
    }
}

bool UIButton::IsMouseOver()
{
    POINT mousePos = INPUT->GetMousePos();

    const WindowInfo& window = GEngine->GetWindow();
    Vec2 windowSize = Vec2(static_cast<float>(window.width),
        static_cast<float>(window.height));

    // 화면 중심이 (0,0)이 되도록 마우스 좌표 변환
    Vec2 mouseUIPos(
        static_cast<float>(mousePos.x - windowSize.x / 2.0f),
        -static_cast<float>(mousePos.y - windowSize.y / 2.0f)
    ); 

    Vec3 position = GetTransform()->GetLocalPosition();
    Vec3 scale = GetTransform()->GetLocalScale();

    // 버튼 위치(중심)와 절반 크기 계산
    Vec2 uiPosition(position.x, position.y);
    Vec2 halfSize(scale.x * 0.5f, scale.y * 0.5f);

    Vec2 buttonMin = uiPosition - halfSize;
    Vec2 buttonMax = uiPosition + halfSize;

    return (mouseUIPos.x >= buttonMin.x && mouseUIPos.x <= buttonMax.x &&
        mouseUIPos.y >= buttonMin.y && mouseUIPos.y <= buttonMax.y);
}

