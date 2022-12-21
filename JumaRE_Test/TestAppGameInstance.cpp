// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestAppGameInstance.h"

#include <JumaEngine/Engine.h>
#include <JumaEngine/subsystems/shaders/ShadersSubsystem.h>
#include <JumaEngine/subsystems/textures/TexturesSubsystem.h>
#include <JumaEngine/subsystems/meshes/MeshesSubsystem.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/texture/Texture.h>

#include "JumaRE/RenderPipeline.h"

using namespace jutils;

math::matrix4 lookAt(const math::vector3& viewPosition, const math::vector3& targetPosition, const math::vector3& upDirection)
{
    const math::vector3 forward = math::normalize(targetPosition - viewPosition);
    const math::vector3 right = math::normalize(math::cross(upDirection, forward));
    const math::vector3 up = math::cross(forward, right);
    math::matrix4 matrix(1.0f);
    matrix[0][0] = right.x; matrix[0][1] = up.x; matrix[0][2] = forward.x;
    matrix[1][0] = right.y; matrix[1][1] = up.y; matrix[1][2] = forward.y;
    matrix[2][0] = right.z; matrix[2][1] = up.z; matrix[2][2] = forward.z;
    matrix[3][0] = -math::dot(right, viewPosition);
    matrix[3][1] = -math::dot(up, viewPosition);
    matrix[3][2] = -math::dot(forward, viewPosition);
    return matrix;
}
math::matrix4 perspective(const float angleFOV, const math::vector2& viewSize, const float near, const float far)
{
    if (math::isEqual(viewSize.x, 0.0f) || math::isEqual(viewSize.y, 0.0f) || math::isEqual(far, near))
    {
        return math::matrix4(1.0f);
    }
    const float angleTan = std::tan(angleFOV / 2);
    const float aspect = viewSize.x / viewSize.y;
    math::matrix4 matrix(0.0f);
    matrix[0][0] = 1.0f / (aspect * angleTan);
    matrix[1][1] = -1.0f / angleTan;
    matrix[2][2] = far / (far - near);
    matrix[2][3] = 1.0f;
    matrix[3][2] = -far * near / (far - near);
    return matrix;
}
math::matrix4 orthogonal(const math::vector2& viewSize, const float near, const float far)
{
    if (math::isEqual(viewSize.x, 0.0f) || math::isEqual(viewSize.y, 0.0f) || math::isEqual(far, near))
    {
        return math::matrix4(1.0f);
    }
    math::matrix4 matrix(1.0f);
    matrix[0][0] = 2.0f / viewSize.x;
    matrix[1][1] = -2.0f / viewSize.y;
    matrix[2][2] = 1.0f / (far - near);
    matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = -near / (far - near);
    return matrix;
}

bool TestAppGameInstance::initLogic()
{
    if (!Super::initLogic())
    {
        return false;
    }

    const JE::Engine* engine = getEngine();

    JE::ShadersSubsystem* shadersSubsystem = engine->getSubsystem<JE::ShadersSubsystem>();
    JE::TexturesSubsystem* texturesSubsystem = engine->getSubsystem<JE::TexturesSubsystem>();
    JE::MeshesSubsystem* meshesSubsystem = engine->getSubsystem<JE::MeshesSubsystem>();

    JE::Texture* texture = texturesSubsystem->getTexture(JSTR("testTexture"));

    JE::Shader* cubeShader = shadersSubsystem->getShader(JSTR("position3D"));
    JE::Material* cubeMaterial = shadersSubsystem->createMaterial(cubeShader);
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture->getTexture());
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Mat4>(JSTR("uModelMatrix"), jutils::math::matrix4(1.0f));
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Mat4>(
        JSTR("uViewMatrix"), lookAt({ 3, 3, -2 }, { 0, 0, 0 }, { 0, 0, 1 })
    );
    cubeMaterial->setParamValue<JumaRE::ShaderUniformType::Mat4>(
        JSTR("uProjectionMatrix"), 
        perspective(1.5708f, { 800, 600 }, 0.1f, 1000.f)
        //orthogonal({ 8, 6 }, 0.1f, 1000.0f)
    );
    
    JE::Mesh* cube = meshesSubsystem->generateCudeMesh(cubeMaterial);

    JumaRE::RenderTarget* renderTarget = engine->getRenderEngine()->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 800, 600 }, JumaRE::TextureSamples::X1);
    engine->getRenderEngine()->getRenderPipeline()->addPipelineStage(JSTR("cubeRT"), renderTarget);
    engine->getRenderEngine()->getRenderPipeline()->addPipelineStageDependency(JSTR("window1"), JSTR("cubeRT"));
    engine->getRenderEngine()->getRenderPipeline()->buildPipelineQueue();

    JE::Shader* shader = shadersSubsystem->getShader(JSTR("textureUnmodified"));
    JE::Shader* cursorShader = shadersSubsystem->getShader(JSTR("cursor2D"));
    JE::Material* plane2DMaterial = shadersSubsystem->createMaterial(shader);
    m_CursorMaterial = shadersSubsystem->createMaterial(cursorShader);
    if ((texture == nullptr) || (shader == nullptr) || (plane2DMaterial == nullptr) || (m_CursorMaterial == nullptr))
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
        return false;
    }
    plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
    //plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture->getTexture());
    plane2DMaterial->setParamValue<JumaRE::ShaderUniformType::Float>(JSTR("uDepth"), 1.0f);
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uLocation"), { 0.0f, 0.0f });
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uSize"), { 1.0f, 1.0f });
    if (engine->getRenderEngine()->getRenderAPI() == JumaRE::RenderAPI::Vulkan)
    {
        m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uOffset"), { 1.0f, 1.0f });
    }
    else
    {
        m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uOffset"), { 1.0f, -1.0f });
    }
    m_CursorMaterial->setParamValue<JumaRE::ShaderUniformType::Float>(JSTR("uDepth"), 0.0f);
    
    JE::Mesh* plane = meshesSubsystem->generatePlane2DMesh(plane2DMaterial);
    JE::Mesh* cursor = meshesSubsystem->generatePlane2DMesh(m_CursorMaterial);

    getGameRenderTarget()->setDepthEnabled(true);
    renderTarget->addRenderPrimitive({ cube->getVertexBuffer(0), cubeMaterial->getMaterial() });
    getGameRenderTarget()->addRenderPrimitive({ plane->getVertexBuffer(0), plane2DMaterial->getMaterial() });
    getGameRenderTarget()->addRenderPrimitive({ cursor->getVertexBuffer(0), m_CursorMaterial->getMaterial() });
    //getGameRenderTarget()->addRenderPrimitive({ cube->getVertexBuffer(0), cubeMaterial->getMaterial() });
    return true;
}

bool TestAppGameInstance::update(float deltaTime)
{
    if (!Super::update(deltaTime))
    {
        return false;
    }

    const math::vector2 screenCoordsModifier = getEngine()->getRenderEngine()->getRenderAPI() == JumaRE::RenderAPI::Vulkan ? 
        math::vector2(1.0f) : math::vector2(1.0f, -1.0f);

    const jutils::math::uvector2 renderTargetSize = getGameRenderTarget()->getSize();
    const jutils::math::vector2 cursorPosition = getCursorPosition();
    const jutils::math::vector2 cursorLocation = (2.0f * (cursorPosition / renderTargetSize) - 1.0f) * screenCoordsModifier;
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
