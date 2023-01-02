﻿// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "TestAppGameInstance.h"

#include <JumaEngine/Engine.h>
#include <JumaEngine/subsystems/shaders/ShadersSubsystem.h>
#include <JumaEngine/subsystems/textures/TexturesSubsystem.h>
#include <JumaEngine/subsystems/meshes/MeshesSubsystem.h>
#include <JumaEngine/subsystems/ui/UIElement.h>
#include <JumaEngine/subsystems/ui/UISubsystem.h>
#include <JumaRE/RenderPipeline.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>

using namespace jutils;

bool TestAppGameInstance::initLogic()
{
    if (!Super::initLogic())
    {
        return false;
    }

    JE::Engine* engine = getEngine();
    const JumaRE::RenderEngine* renderEngine = engine->getRenderEngine();
    const JumaRE::RenderAPI renderAPI = renderEngine->getRenderAPI();
    const JumaRE::render_target_id mainWindowRenderTargetID = getGameRenderTarget()->getID();

    JE::ShadersSubsystem* shadersSubsystem = engine->getSubsystem<JE::ShadersSubsystem>();
    JE::TexturesSubsystem* texturesSubsystem = engine->getSubsystem<JE::TexturesSubsystem>();
    JE::MeshesSubsystem* meshesSubsystem = engine->getSubsystem<JE::MeshesSubsystem>();

    JE::Texture* texture = texturesSubsystem->getTexture(JSTR("testTexture"));

    JE::Shader* cubeShader = shadersSubsystem->getShader(JSTR("position3D"));
    JE::Material* cubeMaterial = shadersSubsystem->createMaterial(cubeShader);
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture->getTexture());
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Mat4>(JSTR("uModelMatrix"), jutils::math::matrix4(1.0f));
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Mat4>(
        JSTR("uViewMatrix"), 
        math::viewMatrix_lookAt({ 3, 3, -2 }, { 0, 0, 0 }, { 0, 0, 1 })
    );
    const bool shouldInvertProjMatrix = (renderAPI == JumaRE::RenderAPI::DirectX11) || (renderAPI == JumaRE::RenderAPI::DirectX12);
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Mat4>(
        JSTR("uProjectionMatrix"), 
        math::projectionMatrix_perspective(math::degreesToRads(90.0f), 4.0f / 3.0f, 0.1f, 1000.f, shouldInvertProjMatrix)
        //math::projectionMatrix_orthogonal({ 8, 6 }, 0.1f, 1000.0f, shouldInvertProjMatrix)
    );
    
    JE::Mesh* cube = meshesSubsystem->generateCudeMesh(cubeMaterial);

    JumaRE::RenderTarget* renderTarget = engine->getRenderEngine()->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 800, 600 }, JumaRE::TextureSamples::X1);
    renderEngine->getRenderPipeline()->addRenderTargetDependecy(mainWindowRenderTargetID, renderTarget->getID());

    JE::Shader* shader = shadersSubsystem->getShader(JSTR("textureUnmodified"));
    JE::Shader* cursorShader = shadersSubsystem->getShader(JSTR("cursor2D"));
    JE::Material* plane2DMaterial = shadersSubsystem->createMaterial(shader);
    JE::Material* cursorMaterial = shadersSubsystem->createMaterial(cursorShader);

    m_UIObject = engine->getSubsystem<JE::UISubsystem>()->createUIObject();
    m_UIObject->setRenderTarget(getGameRenderTarget());

    m_UICursorElement = m_UIObject->addElement();
    m_UICursorElement->setMaterial(cursorMaterial);
    m_UICursorElement->setSize(math::vector2(24.0f, 24.0f) / getGameRenderTarget()->getSize());
    if (renderAPI == JumaRE::RenderAPI::Vulkan)
    {
        m_UICursorElement->setOffset({ 1.0f, 1.0f });
    }
    else
    {
        m_UICursorElement->setOffset({ 1.0f, -1.0f });
    }

    JE::UIElement* backgroundElement = m_UIObject->addElement();
    backgroundElement->setMaterial(plane2DMaterial);
    backgroundElement->setDepth(1.0f);
    plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
    //plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture->getTexture());
    
    getGameRenderTarget()->setDepthEnabled(true);
    m_Primitives.add({ renderTarget, cube, cubeMaterial });
    //m_Primitives.add({ getGameRenderTarget(), cube, cubeMaterial });
    return true;
}

bool TestAppGameInstance::update(float deltaTime)
{
    if (!Super::update(deltaTime))
    {
        return false;
    }

    JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
    const math::vector2 screenCoordsModifier = renderEngine->getRenderAPI() == JumaRE::RenderAPI::Vulkan ? 
        math::vector2(1.0f) : math::vector2(1.0f, -1.0f);

    const jutils::math::uvector2 renderTargetSize = getGameRenderTarget()->getSize();
    const jutils::math::vector2 cursorPosition = getCursorPosition();
    const jutils::math::vector2 cursorLocation = (2.0f * (cursorPosition / renderTargetSize) - 1.0f) * screenCoordsModifier;
    m_UICursorElement->setLocation(cursorLocation);

    for (const auto& primitive : m_Primitives)
    {
        renderEngine->addPrimitiveToRenderList(primitive.renderTarget, primitive.mesh->getVertexBuffer(0), primitive.material->getMaterial());
    }
    m_UIObject->update();

    return true;
}

void TestAppGameInstance::stopLogic()
{
    m_UICursorElement = nullptr;
    m_UIObject = nullptr;

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
