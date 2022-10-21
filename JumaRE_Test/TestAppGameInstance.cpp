// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestAppGameInstance.h"

#include <JumaEngine/Engine.h>
#include <JumaEngine/ShadersSubsystem.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>
#include <JumaRE/vertex/VertexBuffer.h>
#include <JumaRE/vertex/VertexBufferData.h>
#include <JumaRE/vertex/Vertex2D.h>
#include <JumaRE/vertex/Vertex2D_TexCoord.h>
#include <jutils/json/json_parser.h>

bool TestAppGameInstance::initInternal()
{
    if (!Super::initInternal())
    {
        return false;
    }

    JE::Engine* engine = getEngine();
    JumaRE::RenderEngine* renderEngine = engine->getRenderEngine();

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

    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D> cursorVertexBufferData;
    cursorVertexBufferData.setVertices({
        { screenCoordsModifier * jutils::math::vector2{ 0.0f, 0.0f } },
        { screenCoordsModifier * jutils::math::vector2{ 2.0f, 0.0f } },
        { screenCoordsModifier * jutils::math::vector2{ 0.0f, 2.0f } },
        { screenCoordsModifier * jutils::math::vector2{ 0.0f, 2.0f } },
        { screenCoordsModifier * jutils::math::vector2{ 2.0f, 0.0f } },
        { screenCoordsModifier * jutils::math::vector2{ 2.0f, 2.0f } }
    });

    JE::ShadersSubsystem* shadersSubsystem = engine->getSubsystem<JE::ShadersSubsystem>();
    JumaRE::VertexBuffer* vertexBuffer = renderEngine->createVertexBuffer(&vertexBufferData);
    JumaRE::VertexBuffer* cursorVertexBuffer = renderEngine->createVertexBuffer(&cursorVertexBufferData);
    JumaRE::Texture* texture = renderEngine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = shadersSubsystem->getShader(JSTR("textureUnmodified"));
    JumaRE::Shader* cursorShader = shadersSubsystem->getShader(JSTR("cursor2D"));
    JumaRE::Material* material = renderEngine->createMaterial(shader);
    m_CursorMaterial = renderEngine->createMaterial(cursorShader);
    if ((vertexBuffer == nullptr) || (cursorVertexBuffer == nullptr) || (shader == nullptr) || (material == nullptr) || (m_CursorMaterial == nullptr))
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
        return false;
    }
    material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uLocation"), { 0.0f, 0.0f });
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uSize"), { 1.0f, 1.0f });

    getGameRenderTarget()->setDepthEnabled(false);
    getGameRenderTarget()->addRenderPrimitive({ vertexBuffer, material });
    getGameRenderTarget()->addRenderPrimitive({ cursorVertexBuffer, m_CursorMaterial });
    
    JumaRE::WindowController* windowController = renderEngine->getWindowController();
    windowController->OnInputButton.bind(this, &TestAppGameInstance::onInputButton);
    windowController->OnInputAxis.bind(this, &TestAppGameInstance::onInputAxis);
    windowController->OnInputAxis2D.bind(this, &TestAppGameInstance::onInputAxis2D);
    return true;
}

void TestAppGameInstance::clearInternal()
{
    JumaRE::RenderTarget* renderTarget = getGameRenderTarget();
    if (renderTarget != nullptr)
    {
        JumaRE::RenderEngine* renderEngine = renderTarget->getRenderEngine();
    
        JumaRE::WindowController* windowController = renderEngine->getWindowController();
        windowController->OnInputButton.unbind(this, &TestAppGameInstance::onInputButton);
        windowController->OnInputAxis.unbind(this, &TestAppGameInstance::onInputAxis);
        windowController->OnInputAxis2D.unbind(this, &TestAppGameInstance::onInputAxis2D);

        renderTarget->clearRenderPrimitives();
        renderEngine->destroyMaterial(m_CursorMaterial);

        m_CursorMaterial = nullptr;
    }    

    Super::clearInternal();
}

void TestAppGameInstance::onInputButton(JumaRE::WindowController* windowController, 
    const JumaRE::WindowData* windowData, const JumaRE::InputDevice device, 
    const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
{
    static JumaRE::window_id secondWindow = JumaRE::window_id_INVALID;
    if (action == JumaRE::InputButtonAction::Press)
    {
        switch (button)
        {
        case JumaRE::InputButton::L: 
            windowController->setCursorLocked(!windowController->isCursorLocked());
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
    const JumaRE::WindowData* windowData, const JumaRE::InputDevice device, 
    const JumaRE::InputAxis axis, const float value)
{
}

void TestAppGameInstance::onInputAxis2D(JumaRE::WindowController* windowController, 
    const JumaRE::WindowData* windowData, const JumaRE::InputDevice device, 
    const JumaRE::InputAxis axis, const jutils::math::vector2& value)
{
}

void TestAppGameInstance::update()
{
    Super::update();

    JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
    const jutils::math::vector2 screenCoordsModifier = renderEngine->getScreenCoordinateModifier();

    // TODO: Problem with DirectX11
    const jutils::math::uvector2 renderTargetSize = getGameRenderTarget()->getSize();
    const jutils::math::vector2 cursorPosition = getCursorPosition();
    const jutils::math::vector2 cursorLocation = 2.0f * (cursorPosition / renderTargetSize) - 1.0f;
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uLocation"), screenCoordsModifier * cursorLocation);

    const jutils::math::vector2 cursorSize = { 24.0f, 24.0f };
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uSize"), cursorSize / renderTargetSize);
}
