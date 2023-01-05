﻿// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include <JumaEngine/GameInstance.h>

#include <JumaEngine/subsystems/meshes/Mesh.h>
#include <JumaEngine/subsystems/shaders/Material.h>
#include <JumaEngine/subsystems/ui/WidgetContainer.h>
#include <JumaRE/input/InputButtons.h>

class TestAppGameInstance : public JE::GameInstance
{
    JUMAENGINE_CLASS(TestAppGameInstance, JE::GameInstance)

public:
    TestAppGameInstance() = default;
    virtual ~TestAppGameInstance() override = default;

protected:

    virtual bool initLogic() override;
    virtual void startLogic() override;
    virtual void update(float deltaTime) override;
    virtual void postUpdate() override;
    virtual void clearLogic() override;

    virtual void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action) override;

private:

    struct primitive
    {
        JumaRE::RenderTarget* renderTarget = nullptr;
        JE::Mesh* mesh = nullptr;
        JE::Material* material = nullptr;
    };

    jutils::jarray<primitive> m_Primitives;

    JE::WidgetContainer* m_WidgetContainer = nullptr;
};
