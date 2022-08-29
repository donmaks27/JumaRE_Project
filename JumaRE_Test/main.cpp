// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>
#include <JumaRE/RenderTarget.h>
#include <JumaRE/material/Material.h>
#include <JumaRE/material/Shader.h>
#include <JumaRE/texture/Texture.h>
#include <JumaRE/vertex/VertexBuffer.h>
#include <JumaRE/vertex/VertexBufferData.h>
#include <JumaRE/vertex/Vertex2D.h>
#include <JumaRE/vertex/Vertex2D_TexCoord.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void runTests(void (*testFunc)(JumaRE::RenderEngine*, const jutils::jstring&));

// create window
void testCase0(JumaRE::RenderEngine*, const jutils::jstring&);
// position2D to Color
void testCase1(JumaRE::RenderEngine*, const jutils::jstring&);
// textureCoords to color
void testCase2(JumaRE::RenderEngine*, const jutils::jstring&);
// texture
void testCase3(JumaRE::RenderEngine*, const jutils::jstring&);
// render target
void testCase4(JumaRE::RenderEngine*, const jutils::jstring&);
// multisampling
void testCase5(JumaRE::RenderEngine*, const jutils::jstring&);
// uniforms
void testCase6(JumaRE::RenderEngine*, const jutils::jstring&);
// render target's mip maps
void testCase7(JumaRE::RenderEngine*, const jutils::jstring&);
// window mods
void testCase8(JumaRE::RenderEngine*, const jutils::jstring&);
// handling input
void testCase9(JumaRE::RenderEngine*, const jutils::jstring&);

int main()
{
#ifdef JDEBUG
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    
    _CrtMemState memoryState;
    _CrtMemCheckpoint(&memoryState);
#endif

    //runTests(testCase0);
    //runTests(testCase1);
    //runTests(testCase2);
    //runTests(testCase3);
    //runTests(testCase4);
    //runTests(testCase5);
    //runTests(testCase6);
    //runTests(testCase7);
    //runTests(testCase8);
    runTests(testCase9);

#ifdef JDEBUG
    _CrtMemDumpAllObjectsSince(&memoryState);
#endif
    return 0;
}

JumaRE::RenderEngine* createEngine(const JumaRE::RenderAPI renderAPI)
{
    switch (renderAPI)
    {
    case JumaRE::RenderAPI::Vulkan: return JumaRE::CreateRenderEngine<JumaRE::RenderAPI::Vulkan>();
    case JumaRE::RenderAPI::OpenGL: return JumaRE::CreateRenderEngine<JumaRE::RenderAPI::OpenGL>();
    case JumaRE::RenderAPI::DirectX11: return JumaRE::CreateRenderEngine<JumaRE::RenderAPI::DirectX11>();
    case JumaRE::RenderAPI::DirectX12: return JumaRE::CreateRenderEngine<JumaRE::RenderAPI::DirectX12>();
    default: ;
    }
    return nullptr;
}
template<JumaRE::RenderAPI API>
void runTestsTemplate(void (*testFunc)(JumaRE::RenderEngine*, const jutils::jstring&), const jutils::jstring& apiName)
{
    JumaRE::RenderEngine* engine = createEngine(API);
    if (engine == nullptr)
    {
        JUTILS_LOG(error, JSTR("Failed to create {} render engine"), apiName);
        return;
    }
    testFunc(engine, apiName);
    delete engine;
    jutils::jstring_hash_table::ClearInstance();
}
void runTests(void (*testFunc)(JumaRE::RenderEngine*, const jutils::jstring&))
{
    //runTestsTemplate<JumaRE::RenderAPI::OpenGL>(testFunc, JSTR("OpenGL"));
    runTestsTemplate<JumaRE::RenderAPI::Vulkan>(testFunc, JSTR("Vulkan"));
    //runTestsTemplate<JumaRE::RenderAPI::DirectX11>(testFunc, JSTR("DirectX11"));
    //runTestsTemplate<JumaRE::RenderAPI::DirectX12>(testFunc, JSTR("DirectX12"));
}

// create window
void testCase0(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Window: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTarget = windowController->findWindowData(1)->windowRenderTarget;
    if (renderTarget != nullptr)
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool pipelineValid = renderPipeline != nullptr;
        pipelineValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTarget);
        pipelineValid &= renderPipeline->buildPipelineQueue();
        if (pipelineValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init render pipeline"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * position2D to Color
 * OpenGL, DirectX11: flipped
 */
void testCase1(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Position2D to Color: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    JumaRE::WindowController* windowController = engine->getWindowController();
    const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D> vertexBufferData;
    vertexBufferData.setVertices({
        { screenCoordsModifier * jutils::math::vector2{ -1.0f, -1.0f } },
        { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f } },
        { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f } },
        { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f } },
        { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f } },
        { screenCoordsModifier * jutils::math::vector2{  1.0f,  1.0f } }
    });

    JumaRE::RenderTarget* renderTarget = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("position2DtoColor") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("position2DtoColor") }
    }, {
        JSTR("position")
    }, {});
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTarget != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (material != nullptr))
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool pipelineValid = renderPipeline != nullptr;
        pipelineValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTarget);
        pipelineValid &= renderPipeline->buildPipelineQueue();
        pipelineValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, material });
        if (pipelineValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init render pipeline"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * textureCoords to color
 * OpenGL: flipped
 */
void testCase2(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("TextureCoords to Color: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    if (!engine->shouldFlipLoadedTextures())
    {
        vertexBufferData.setVertices({
            { screenCoordsModifier * jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 1.0f } }
        });
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
    }

    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTarget = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureCoordsToColor") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureCoordsToColor") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {});
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTarget != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (material != nullptr))
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool pipelineValid = renderPipeline != nullptr;
        pipelineValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTarget);
        pipelineValid &= renderPipeline->buildPipelineQueue();
        pipelineValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, material });
        if (pipelineValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init render pipeline"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * texture
 */
void testCase3(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Texture: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData1;
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData2;
    jutils::jarray<jutils::uint8> textureData;
    if (!engine->shouldFlipLoadedTextures())
    {
        vertexBufferData1.setVertices({
            { screenCoordsModifier * jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f, -1.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  0.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  0.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f, -1.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  0.0f }, { 1.0f, 1.0f } }
        });
        vertexBufferData2.setVertices({
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  0.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  0.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  0.0f }, { 1.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 1.0f } }
        });
        textureData = {
            255, 0, 0, 255,   0, 255, 0, 255,
            0, 0, 255, 255,   0, 0, 0, 255
        };
    }
    else
    {
        vertexBufferData1.setVertices({
            { screenCoordsModifier * jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f, -1.0f }, { 1.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  0.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{ -1.0f,  0.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f, -1.0f }, { 1.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  0.0f }, { 1.0f, 0.0f } }
        });
        vertexBufferData2.setVertices({
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  0.0f }, { 0.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  0.0f }, { 1.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  0.0f,  1.0f }, { 0.0f, 0.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  0.0f }, { 1.0f, 1.0f } },
            { screenCoordsModifier * jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 0.0f } }
        });
        textureData = {
            0, 0, 255, 255,   0, 0, 0, 255,
            255, 0, 0, 255,   0, 255, 0, 255
        };
    }
    
    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTarget = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::VertexBuffer* vertexBuffer1 = engine->createVertexBuffer(&vertexBufferData1);
    JumaRE::VertexBuffer* vertexBuffer2 = engine->createVertexBuffer(&vertexBufferData2);
    JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
    });
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTarget != nullptr) && (vertexBuffer1 != nullptr) && (shader != nullptr) && (material != nullptr))
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool dataValid = renderPipeline != nullptr;
        //dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
        dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTarget);
        dataValid &= renderPipeline->buildPipelineQueue();
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer1, material });
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer2, material });
        if (dataValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init data"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * render target
 */
void testCase4(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Render Target: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!engine->shouldFlipLoadedTextures())
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
    
    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTargetWindow = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::RenderTarget* renderTarget = engine->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 800, 600 }, JumaRE::TextureSamples::X1);
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
    });
    JumaRE::Material* materialWindow = engine->createMaterial(shader);
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTargetWindow != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (materialWindow != nullptr))
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool dataValid = renderPipeline != nullptr;
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("renderTarget"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTargetWindow);
        dataValid &= renderPipeline->addPipelineStageDependency(JSTR("window1"), JSTR("renderTarget"));
        dataValid &= renderPipeline->buildPipelineQueue();
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("renderTarget"), { vertexBuffer, material });
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, materialWindow });
        if (dataValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init data"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * multisampling
 */
void testCase5(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Multisampling: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!engine->shouldFlipLoadedTextures())
    {
        vertexBufferData.setVertices({
            { screenCoordsModifier * jutils::math::vector2{ -0.5f, -0.5f }, { 0.0f, 0.0f } },
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
            { screenCoordsModifier * jutils::math::vector2{ -0.5f, -0.5f }, { 0.0f, 1.0f } },
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

    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTargetWindow = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::RenderTarget* renderTarget = engine->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 400, 300 }, JumaRE::TextureSamples::X8);
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
    });
    JumaRE::Material* materialWindow = engine->createMaterial(shader);
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTargetWindow != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (materialWindow != nullptr))
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool dataValid = renderPipeline != nullptr;
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("renderTarget"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTargetWindow);
        dataValid &= renderPipeline->addPipelineStageDependency(JSTR("window1"), JSTR("renderTarget"));
        dataValid &= renderPipeline->buildPipelineQueue();
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("renderTarget"), { vertexBuffer, material });
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, materialWindow });
        if (dataValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init data"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * uniforms
 */
void testCase6(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Uniforms: ") + apiName, { 800, 600 }, JumaRE::TextureSamples::X2 }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!engine->shouldFlipLoadedTextures())
    {
        vertexBufferData.setVertices({
            { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { {  1.0f,  1.0f }, { 1.0f, 1.0f } }
        });
        textureData = {
            255, 0, 0, 255,   0, 255, 0, 255,
            0, 0, 255, 255,   0, 0, 0, 255
        };
    }
    else
    {
        vertexBufferData.setVertices({
            { { -0.5f, -0.5f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f }, { 1.0f, 0.0f } }
        });
        textureData = {
            0, 0, 255, 255,   0, 0, 0, 255,
            255, 0, 0, 255,   0, 255, 0, 255
        };
    }
    
    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTargetWindow = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::RenderTarget* renderTarget = engine->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 400, 300 }, JumaRE::TextureSamples::X1);
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("texture") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("texture") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uScreenCoordsModifier"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Vec2, JumaRE::SHADER_STAGE_VERTEX, 0, 0 } },
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 1, 0 } }
    });
    JumaRE::Material* materialWindow = engine->createMaterial(shader);
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTargetWindow != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (materialWindow != nullptr))
    {
        const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool dataValid = renderPipeline != nullptr;
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uScreenCoordsModifier"), screenCoordsModifier);
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uScreenCoordsModifier"), screenCoordsModifier);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("renderTarget"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTargetWindow);
        dataValid &= renderPipeline->addPipelineStageDependency(JSTR("window1"), JSTR("renderTarget"));
        dataValid &= renderPipeline->buildPipelineQueue();
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("renderTarget"), { vertexBuffer, material });
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, materialWindow });
        if (dataValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init data"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * render target's mip maps
 */
void testCase7(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Mip maps: ") + apiName, { 800, 600 }, JumaRE::TextureSamples::X2 }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!engine->shouldFlipLoadedTextures())
    {
        vertexBufferData.setVertices({
            { { -1.0, -1.0 }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { {  1.0f,  1.0f }, { 1.0f, 1.0f } }
        });
        textureData = {
            255, 0, 0, 255,   0, 255, 0, 255,
            0, 0, 255, 255,   0, 0, 0, 255
        };
    }
    else
    {
        vertexBufferData.setVertices({
            { { -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f }, { 1.0f, 0.0f } }
        });
        textureData = {
            0, 0, 255, 255,   0, 0, 0, 255,
            255, 0, 0, 255,   0, 255, 0, 255
        };
    }
    
    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTargetWindow = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::RenderTarget* renderTarget = engine->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 2048, 1024 }, JumaRE::TextureSamples::X1);
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("texture") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("texture") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uScreenCoordsModifier"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Vec2, JumaRE::SHADER_STAGE_VERTEX, 0, 0 } },
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 1, 0 } }
    });
    JumaRE::Material* materialWindow = engine->createMaterial(shader);
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTargetWindow != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (materialWindow != nullptr))
    {
        const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool dataValid = renderPipeline != nullptr;
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uScreenCoordsModifier"), screenCoordsModifier);
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Vec2>(JSTR("uScreenCoordsModifier"), screenCoordsModifier);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("renderTarget"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTargetWindow);
        dataValid &= renderPipeline->addPipelineStageDependency(JSTR("window1"), JSTR("renderTarget"));
        dataValid &= renderPipeline->buildPipelineQueue();
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("renderTarget"), { vertexBuffer, material });
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, materialWindow });
        if (dataValid)
        {
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init data"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

/**
 * window mods
 */
void testCase8(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Window mods: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
    JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
    jutils::jarray<jutils::uint8> textureData;
    if (!engine->shouldFlipLoadedTextures())
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
    
    JumaRE::WindowController* windowController = engine->getWindowController();
    JumaRE::RenderTarget* renderTargetWindow = windowController->findWindowData(1)->windowRenderTarget;
    JumaRE::RenderTarget* renderTarget = engine->createRenderTarget(JumaRE::TextureFormat::RGBA8, { 800, 600 }, JumaRE::TextureSamples::X1);
    JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
    JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
    JumaRE::Shader* shader = engine->createShader({
        { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
    }, {
        JSTR("position"), JSTR("textureCoords")
    }, {
        { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
    });
    JumaRE::Material* materialWindow = engine->createMaterial(shader);
    JumaRE::Material* material = engine->createMaterial(shader);
    if ((renderTargetWindow != nullptr) && (vertexBuffer != nullptr) && (shader != nullptr) && (materialWindow != nullptr))
    {
        JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
        bool dataValid = renderPipeline != nullptr;
        dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
        dataValid &= materialWindow->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("renderTarget"), renderTarget);
        dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTargetWindow);
        dataValid &= renderPipeline->addPipelineStageDependency(JSTR("window1"), JSTR("renderTarget"));
        dataValid &= renderPipeline->buildPipelineQueue();
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("renderTarget"), { vertexBuffer, material });
        dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, materialWindow });
        if (dataValid)
        {
            bool pressedKeyQ = false;
            bool pressedKeyW = false;
            bool pressedKeyE = false;

            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }

                const bool qWasPressed = GetKeyState('Q') < 0;
                const bool wWasPressed = GetKeyState('W') < 0;
                const bool eWasPressed = GetKeyState('E') < 0;
                if (qWasPressed && !pressedKeyQ)
                {
                    windowController->setMainWindowMode(JumaRE::WindowMode::Normal);
                }
                else if (wWasPressed && !pressedKeyW)
                {
                    windowController->setMainWindowMode(JumaRE::WindowMode::WindowedFullscreen);
                }
                else if (eWasPressed && !pressedKeyE)
                {
                    windowController->setMainWindowMode(JumaRE::WindowMode::Fullscreen);
                }
                pressedKeyQ = qWasPressed;
                pressedKeyW = wWasPressed;
                pressedKeyE = eWasPressed;

                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        else
        {
            JUTILS_LOG(error, JSTR("Failed to init data"));
        }
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to create assets"));
    }
}

// handling input
void testCase9(JumaRE::RenderEngine* engine, const jutils::jstring& apiName)
{
    if (!engine->init({ JSTR("Input: ") + apiName, { 800, 600 } }))
    {
        JUTILS_LOG(error, JSTR("Failed to init render engine"));
        return;
    }

    struct TestApplication
    {
        JumaRE::RenderEngine* engine = nullptr;

        bool init()
        {
            const jutils::math::vector2 screenCoordsModifier = engine->getScreenCoordinateModifier();
            JumaRE::VertexBufferDataImpl<JumaRE::Vertex2D_TexCoord> vertexBufferData;
            jutils::jarray<jutils::uint8> textureData;
            if (!engine->shouldFlipLoadedTextures())
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

            JumaRE::WindowController* windowController = engine->getWindowController();
            JumaRE::RenderTarget* renderTargetWindow = windowController->findWindowData(windowController->getMainWindowID())->windowRenderTarget;
            JumaRE::VertexBuffer* vertexBuffer = engine->createVertexBuffer(&vertexBufferData);
            JumaRE::Texture* texture = engine->createTexture({ 2, 2 }, JumaRE::TextureFormat::RGBA8, textureData.getData());
            JumaRE::Shader* shader = engine->createShader({
                { JumaRE::SHADER_STAGE_VERTEX, JSTR("textureUnmodified") }, { JumaRE::SHADER_STAGE_FRAGMENT, JSTR("textureUnmodified") }
            }, {
                JSTR("position"), JSTR("textureCoords")
            }, {
                { JSTR("uTexture"), JumaRE::ShaderUniform{ JumaRE::ShaderUniformType::Texture, JumaRE::SHADER_STAGE_FRAGMENT, 0, 0 } }
            });
            JumaRE::Material* material = engine->createMaterial(shader);
            if ((renderTargetWindow == nullptr) && (vertexBuffer == nullptr) && (shader == nullptr) && (material == nullptr))
            {
                JUTILS_LOG(error, JSTR("Failed to create assets"));
                return false;
            }

            JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
            bool dataValid = renderPipeline != nullptr;
            dataValid &= material->setParamValue<JumaRE::ShaderUniformType::Texture>(JSTR("uTexture"), texture);
            dataValid &= renderPipeline->addPipelineStage(JSTR("window1"), renderTargetWindow);
            dataValid &= renderPipeline->buildPipelineQueue();
            dataValid &= renderPipeline->addRenderPrimitive(JSTR("window1"), { vertexBuffer, material });
            if (!dataValid)
            {
                JUTILS_LOG(error, JSTR("Failed to build pipeline"));
                return false;
            }

            windowController->OnInputButton.bind(this, &TestApplication::onInputButton);
            windowController->OnInputAxis.bind(this, &TestApplication::onInputAxis);
            windowController->OnInputAxis2D.bind(this, &TestApplication::onInputAxis2D);
            return true;
        }
        void start()
        {
            JumaRE::WindowController* windowController = engine->getWindowController();
            JumaRE::RenderPipeline* renderPipeline = engine->getRenderPipeline();
            while (!windowController->shouldCloseWindow(1))
            {
                if (!renderPipeline->render())
                {
                    break;
                }
                windowController->updateWindows();
            }
            renderPipeline->waitForRenderFinished();
        }
        void destroy()
        {
            JumaRE::WindowController* windowController = engine->getWindowController();
            windowController->OnInputButton.unbind(this, &TestApplication::onInputButton);
            windowController->OnInputAxis.unbind(this, &TestApplication::onInputAxis);
            windowController->OnInputAxis2D.unbind(this, &TestApplication::onInputAxis2D);
        }

        void onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, const JumaRE::InputDeviceType device, 
            const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
        {
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
                default: ;
                }
            }
        }
        void onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, const JumaRE::InputDeviceType device, 
            const JumaRE::InputAxis axis, const float value)
        {
        }
        void onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, const JumaRE::InputDeviceType device, 
            const JumaRE::InputAxis axis, const jutils::math::vector2& value)
        {
            if (axis == JumaRE::InputAxis::Mouse2D)
            {
                JUTILS_LOG(info, JSTR("Cursor position {}"), windowData->cursorPosition.toString());
            }
        }
    } app = { engine };
    if (!app.init())
    {
        JUTILS_LOG(error, JSTR("Failed to init application"));
        return;
    }
    app.start();
    app.destroy();
}