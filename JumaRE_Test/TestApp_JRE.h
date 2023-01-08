// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "TestApp.h"

#include <JumaRE/RenderEngine.h>

class TestApp_JRE : public TestApp
{
public:
    TestApp_JRE() = default;
    virtual ~TestApp_JRE() override = default;

    virtual void run() override;

private:

    JumaRE::RenderEngine* m_Engine = nullptr;

    JumaRE::RenderTarget* m_RenderTarget = nullptr;
    JumaRE::VertexBuffer* m_VertexBuffer = nullptr;
    JumaRE::Material* m_Material = nullptr;


    bool initData();

    void startLoop();
    void update();

    void destroy();

    void onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, const JumaRE::InputActionData& input);
    void onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
    void onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
    void onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        JumaRE::InputDevice device, JumaRE::InputAxis axis, const jutils::math::vector2& value);
};
