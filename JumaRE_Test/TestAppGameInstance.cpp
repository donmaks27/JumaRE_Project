// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestAppGameInstance.h"

#include <JumaEngine/Engine.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>
#include <JumaRE/vertex/VertexBuffer.h>
#include <JumaRE/vertex/VertexBufferData.h>
#include <JumaRE/vertex/Vertex2D_TexCoord.h>

bool TestAppGameInstance::initInternal()
{
    if (!Super::initInternal())
    {
        return false;
    }

    JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();

    const jutils::math::vector2 screenCoordsModifier = renderEngine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!renderEngine->shouldFlipLoadedTextures())
    {
        vertexBufferData.setVertices({
            { screenCoordsModifier * jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 1.0f } }
        });
        textureData = {
            255, 0, 0, 255,   0, 255, 0, 255,
            0, 0, 255, 255,   0, 0, 0, 255
        };
    }
    else
    {
        vertexBufferData.setVertices({
            { screenCoordsModifier * jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 0.0f } }
        });
        textureData = {
            0, 0, 255, 255,   0, 0, 0, 255,
            255, 0, 0, 255,   0, 255, 0, 255
        };
    }

    JumaRE::VertexBuffer* vertexBuffer = renderEngine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = renderEngine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = renderEngine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
    });
    JumaRE::Material* material = renderEngine->createMaterial(shader);
    if ((vertexBuffer == nullptr) && (shader == nullptr) && (material == nullptr))
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
        return false;
    }
    material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);

    getGameRenderTarget()->addRenderPrimitive({ vertexBuffer, material });
    
    JumaRE::WindowController* windowController = renderEngine->getWindowController();
    windowController->OnInputButton.bind(this, &TestAppGameInstance::onInputButton);
    windowController->OnInputAxis.bind(this, &TestAppGameInstance::onInputAxis);
    windowController->OnInputAxis2D.bind(this, &TestAppGameInstance::onInputAxis2D);
    return true;
}

void TestAppGameInstance::clearInternal()
{
    JumaRE::WindowController* windowController = getEngine()->getRenderEngine()->getWindowController();
    windowController->OnInputButton.unbind(this, &TestAppGameInstance::onInputButton);
    windowController->OnInputAxis.unbind(this, &TestAppGameInstance::onInputAxis);
    windowController->OnInputAxis2D.unbind(this, &TestAppGameInstance::onInputAxis2D);

    Super::clearInternal();
}

void TestAppGameInstance::onInputButton(JumaRE::WindowController* windowController, 
    const JumaRE::WindowData* windowData, const JumaRE::InputDeviceType device, 
    const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
{
    static JumaRE::window_id secondWindow = JumaRE::window_id_INVALID;
    if (action == JumaRE::InputButtonAction::Press)
    {
        switch (button)
        {
        case JumaRE::InputButton::L: 
            windowController->setCursorLockedToMainWindow(!windowController->isCursorLockedToMainWindow());
            break;
        case JumaRE::InputButton::Q:
            windowController->setMainWindowMode(JumaRE::WindowMode::Normal);
            break;
        case JumaRE::InputButton::W:
            windowController->setMainWindowMode(JumaRE::WindowMode::WindowedFullscreen);
            break;
        case JumaRE::InputButton::E:
            windowController->setMainWindowMode(JumaRE::WindowMode::Fullscreen);
            break;
        case JumaRE::InputButton::Escape:
            windowController->destroyWindow(windowController->getMainWindowID());
            break;
        case JumaRE::InputButton::O:
            if (secondWindow == JumaRE::window_id_INVALID)
            {
                secondWindow = windowController->createWindow({ JSTR("Second window"), { 200, 400 }, JumaRE::TextureSamples::X1 });
            }
            break;
        case JumaRE::InputButton::C:
            if (secondWindow != JumaRE::window_id_INVALID)
            {
                windowController->destroyWindow(secondWindow);
                secondWindow = JumaRE::window_id_INVALID;
            }
            break;
        default: ;
        }
    }
}

void TestAppGameInstance::onInputAxis(JumaRE::WindowController* windowController, 
    const JumaRE::WindowData* windowData, const JumaRE::InputDeviceType device, 
    const JumaRE::InputAxis axis, const float value)
{
}

void TestAppGameInstance::onInputAxis2D(JumaRE::WindowController* windowController, 
    const JumaRE::WindowData* windowData, const JumaRE::InputDeviceType device, 
    const JumaRE::InputAxis axis, const jutils::math::vector2& value)
{
    if (axis == JumaRE::InputAxis::Mouse2D)
    {
        JUTILS_LOG(info, JSTR("Cursor position {}"), windowData->cursorPosition.toString());
    }
}
