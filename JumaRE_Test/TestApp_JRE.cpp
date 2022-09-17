﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestApp_JRE.h"

#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>
#include <JumaRE/RenderTarget.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>
#include <JumaRE/vertex/Vertex2D_TexCoord.h>
#include <JumaRE/vertex/VertexBufferData.h>

void TestApp_JRE::run()
{
    if (initData())
    {
        startLoop();
    }
    destroy();
}

bool TestApp_JRE::initData()
{
    constexpr JumaRE::RenderAPI renderAPI = JumaRE::RenderAPI::Vulkan;
    m_Engine = JumaRE::CreateRenderEngine<renderAPI>();
    if (m_Engine == nullptr)
    {
        JUTILS_LOG(error, JSTR("Failed to create {} render engine"), renderAPI);
        return false;
    }
    if (!m_Engine->init({ fmt::format(JSTR("RenderEngine ({})"), renderAPI).c_str(), { 800, 600 }, JumaRE::TextureSamples::X1 }))
    {
        JUTILS_LOG(error, JSTR("Failed to initialize {} render engine"), renderAPI);
        return false;
    }
    JUTILS_LOG(correct, JSTR("Render engine ({}) initialized"), renderAPI);

    const jutils::math::vector2 screenCoordsModifier = m_Engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!m_Engine->shouldFlipLoadedTextures())
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

    JumaRE::WindowController* windowController = m_Engine->getWindowController();
    //const JumaRE::window_id windowID = windowController->createWindow({ JSTR("Second window"), { 300, 200 }, JumaRE::TextureSamples::X1 });

    const JumaRE::WindowData* mainWindowData = windowController->findWindowData(windowController->getMainWindowID());
    JumaRE::RenderTarget* renderTargetWindow = mainWindowData->windowRenderTarget;
    //JumaRE::RenderTarget* renderTargetWindow2 = windowController->findWindowData(windowID)->windowRenderTarget;
    JumaRE::VertexBuffer* vertexBuffer = m_Engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = m_Engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = m_Engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
    });
    JumaRE::Material* material = m_Engine->createMaterial(shader);
    if ((renderTargetWindow == nullptr) && (vertexBuffer == nullptr) && (shader == nullptr) && (material == nullptr))
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
        return false;
    }

    bool dataValid = material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
    dataValid &= renderTargetWindow->addRenderPrimitive({ vertexBuffer, material });
    if (!dataValid)
    {
        JUTILS_LOG(error, JSTR("Failed to build pipeline"));
        return false;
    }

    windowController->OnInputButton.bind(this, &TestApp_JRE::onInputButton);
    windowController->OnInputAxis.bind(this, &TestApp_JRE::onInputAxis);
    windowController->OnInputAxis2D.bind(this, &TestApp_JRE::onInputAxis2D);
    return true;
}

void TestApp_JRE::startLoop()
{
    JUTILS_LOG(info, JSTR("Render engine loop started..."));

    JumaRE::WindowController* windowController = m_Engine->getWindowController();
    JumaRE::RenderPipeline* renderPipeline = m_Engine->getRenderPipeline();
    while (!windowController->isMainWindowClosed())
    {
        if (!renderPipeline->render())
        {
            break;
        }
        windowController->updateWindows();
        update();
    }
    renderPipeline->waitForRenderFinished();

    JUTILS_LOG(info, JSTR("Render engine loop finished"));
}
void TestApp_JRE::update()
{
}

void TestApp_JRE::destroy()
{
    JumaRE::WindowController* windowController = m_Engine->getWindowController();
    windowController->OnInputButton.unbind(this, &TestApp_JRE::onInputButton);
    windowController->OnInputAxis.unbind(this, &TestApp_JRE::onInputAxis);
    windowController->OnInputAxis2D.unbind(this, &TestApp_JRE::onInputAxis2D);

    delete m_Engine;
    jutils::jstring_hash_table::ClearInstance();

    JUTILS_LOG(info, JSTR("Render engine destroyed"));
}

void TestApp_JRE::onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
    const JumaRE::InputDevice device, const JumaRE::InputButton button, 
    const JumaRE::InputButtonAction action)
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

void TestApp_JRE::onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
    const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const float value)
{
}

void TestApp_JRE::onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
    const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const jutils::math::vector2& value)
{
    if (axis == JumaRE::InputAxis::Mouse2D)
    {
        JUTILS_LOG(info, JSTR("Cursor position {}"), windowData->cursorPosition.toString());
    }
}
