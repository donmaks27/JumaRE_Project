// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include <JumaEngine/GameInstance.h>

#include <JumaEngine/subsystems/meshes/Mesh.h>
#include <JumaEngine/subsystems/shaders/Material.h>
#include <JumaRE/input/InputButtons.h>

class TestAppGameInstance : public JE::GameInstance
{
    JUMAENGINE_CLASS(TestAppGameInstance, JE::GameInstance)

public:
    TestAppGameInstance() = default;
    virtual ~TestAppGameInstance() override = default;

protected:

    virtual bool initLogic() override;
    virtual bool update(float deltaTime) override;
    virtual void stopLogic() override;

    virtual void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action) override;

private:

    struct primitive
    {
        JumaRE::RenderTarget* renderTarget = nullptr;
        JE::Mesh* mesh = nullptr;
        JE::Material* material = nullptr;
    };

    jutils::jarray<primitive> m_Primitives;
    JE::Material* m_CursorMaterial = nullptr;
};
