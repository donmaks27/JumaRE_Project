// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include <JumaEngine/GameInstance.h>
#include <JumaRE/input/InputButtons.h>

class TestAppGameInstance : public JE::GameInstance
{
    using Super = JE::GameInstance;

public:
    TestAppGameInstance() = default;
    virtual ~TestAppGameInstance() override = default;

    virtual void update() override;

protected:

    virtual bool initInternal() override;
    virtual void clearInternal() override;

private:

    JumaRE::Material* m_CursorMaterial = nullptr;


    void onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
    void onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
    void onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        JumaRE::InputDevice device, JumaRE::InputAxis axis, const jutils::math::vector2& value);
};
