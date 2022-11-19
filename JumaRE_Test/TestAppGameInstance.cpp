// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestAppGameInstance.h"

#include <JumaEngine/Engine.h>
#include <JumaEngine/subsystems/shaders/ShadersSubsystem.h>
#include <JumaEngine/subsystems/textures/TexturesSubsystem.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>
#include <JumaRE/vertex/VertexBuffer.h>
#include <JumaRE/vertex/VertexBufferData.h>
#include <jutils/json/json_parser.h>

bool TestAppGameInstance::initLogic()
{
    if (!Super::initLogic())
    {
        return false;
    }

    JE::Engine* engine = getEngine();
    JumaRE::RenderEngine* renderEngine = engine->getRenderEngine();

    struct Vertex2D_TexCoord
    {
        jutils::math::vector2 position2D;
        jutils::math::vector2 texCoords;
    };
    jutils::jarray<Vertex2D_TexCoord> vertices;
    if (!renderEngine->shouldFlipLoadedTextures())
    {
        vertices = {
            { jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 1.0f } }
        };
    }
    else
    {
        vertices = {
            { jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 0.0f } }
        };
    }

    struct Vertex2D
    {
        jutils::math::vector2 position2D;
    };
    const jutils::jarray<Vertex2D> cursorVertices = {
        { jutils::math::vector2{ 0.0f, 0.0f } },
        { jutils::math::vector2{ 2.0f, 0.0f } },
        { jutils::math::vector2{ 0.0f, 2.0f } },
        { jutils::math::vector2{ 0.0f, 2.0f } },
        { jutils::math::vector2{ 2.0f, 0.0f } },
        { jutils::math::vector2{ 2.0f, 2.0f } }
    };

    JE::ShadersSubsystem* shadersSubsystem = engine->getSubsystem<JE::ShadersSubsystem>();
    JE::TexturesSubsystem* texturesSubsystem = engine->getSubsystem<JE::TexturesSubsystem>();
    JumaRE::VertexBuffer* vertexBuffer = renderEngine->createVertexBuffer(
        JumaRE::MakeVertexBufferData({{ JSTR("position2D"), JSTR("textureCoords") }}, vertices)
    );
    JumaRE::VertexBuffer* cursorVertexBuffer = renderEngine->createVertexBuffer(
        JumaRE::MakeVertexBufferData({{ JSTR("position2D") }}, cursorVertices)
    );
    JE::Texture* texture = texturesSubsystem->getTexture(JSTR("testTexture"));
    JE::Shader* shader = shadersSubsystem->getShader(JSTR("textureUnmodified"));
    JE::Shader* cursorShader = shadersSubsystem->getShader(JSTR("cursor2D"));
    JE::Material* material = shadersSubsystem->createMaterial(shader);
    m_CursorMaterial = shadersSubsystem->createMaterial(cursorShader);
    if ((vertexBuffer == nullptr) || (cursorVertexBuffer == nullptr) || (texture == nullptr) || (shader == nullptr) || (material == nullptr) || (m_CursorMaterial == nullptr))
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
        return false;
    }
    material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture->getTexture());
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uLocation"), { 0.0f, 0.0f });
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uSize"), { 1.0f, 1.0f });

    getGameRenderTarget()->setDepthEnabled(false);
    getGameRenderTarget()->addRenderPrimitive({ vertexBuffer, material->getMaterial() });
    getGameRenderTarget()->addRenderPrimitive({ cursorVertexBuffer, m_CursorMaterial->getMaterial() });
    return true;
}

bool TestAppGameInstance::update(float deltaTime)
{
    if (!Super::update(deltaTime))
    {
        return false;
    }

    const jutils::math::uvector2 renderTargetSize = getGameRenderTarget()->getSize();
    const jutils::math::vector2 cursorPosition = getCursorPosition();
    const jutils::math::vector2 cursorLocation = 2.0f * (cursorPosition / renderTargetSize) - 1.0f;
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uLocation"), cursorLocation);

    const jutils::math::vector2 cursorSize = { 24.0f, 24.0f };
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uSize"), cursorSize / renderTargetSize);

    return true;
}

void TestAppGameInstance::stopLogic()
{
    JumaRE::RenderTarget* renderTarget = getGameRenderTarget();
    if (renderTarget != nullptr)
    {
        renderTarget->clearRenderPrimitives();

        getEngine()->getSubsystem<JE::ShadersSubsystem>()->destroyMaterial(m_CursorMaterial);
        m_CursorMaterial = nullptr;
    }

    Super::stopLogic();
}

void TestAppGameInstance::onInputButton(const JumaRE::InputDevice device, const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
{
    Super::onInputButton(device, button, action);

    static JumaRE::window_id secondWindow = JumaRE::window_id_INVALID;
    JumaRE::WindowController* windowController = getEngine()->getRenderEngine()->getWindowController();
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
