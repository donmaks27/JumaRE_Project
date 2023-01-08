// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "TestAppGameInstance.h"

#include <JumaEngine/Engine.h>
#include <JumaEngine/subsystems/shaders/ShadersSubsystem.h>
#include <JumaEngine/subsystems/textures/TexturesSubsystem.h>
#include <JumaEngine/subsystems/meshes/MeshesSubsystem.h>
#include <JumaEngine/subsystems/ui/UISubsystem.h>
#include <JumaEngine/subsystems/ui/TestWidget.h>
#include <JumaRE/RenderPipeline.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>

using namespace jutils;

void TestAppGameInstance::onLogicStarted()
{
    Super::onLogicStarted();

    const JE::Engine* engine = getEngine();
    const JumaRE::RenderEngine* renderEngine = engine->getRenderEngine();
    const JumaRE::RenderAPI renderAPI = renderEngine->getRenderAPI();
    JumaRE::WindowController* windowController = renderEngine->getWindowController();
    const JumaRE::render_target_id mainWindowRenderTargetID = getGameRenderTarget()->getID();

    windowController->setCursorMode(windowController->getMainWindowID(), JumaRE::WindowCursorMode::Hidden);

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

    JE::WidgetContainer* widgetContainer = createWidget(getGameRenderTarget());
    widgetContainer->setRootWidget<JE::TestWidget>()->setMaterial(cursorMaterial);

    //JE::UIElement* backgroundElement = m_UIObject->addElement();
    //backgroundElement->setMaterial(plane2DMaterial);
    //backgroundElement->setDepth(1.0f);
    //plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
    //plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture->getTexture());
    
    getGameRenderTarget()->setDepthEnabled(true);
    m_Primitives.add({ renderTarget, cube, cubeMaterial });
    //m_Primitives.add({ getGameRenderTarget(), cube, cubeMaterial });
}

void TestAppGameInstance::onUpdate(float deltaTime)
{
    Super::onUpdate(deltaTime);
}

void TestAppGameInstance::onPostUpdate()
{
    Super::onPostUpdate();

    JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
    for (const auto& primitive : m_Primitives)
    {
        renderEngine->addPrimitiveToRenderList(primitive.renderTarget, primitive.mesh->getVertexBuffer(0), primitive.material->getMaterial());
    }
}

void TestAppGameInstance::onLogicStopping()
{
    Super::onLogicStopping();
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
            {
                const JumaRE::WindowCursorMode cursorMode = windowController->getCursorMode(windowController->getMainWindowID());
                windowController->setCursorMode(windowController->getMainWindowID(), 
                    cursorMode == JumaRE::WindowCursorMode::Locked ? JumaRE::WindowCursorMode::Hidden : JumaRE::WindowCursorMode::Locked
                );
            }
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
