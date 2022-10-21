// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestApp_JE.h"

#include <JumaEngine/GameEngine.h>

#include "TestAppGameInstance.h"

void TestApp_JE::run()
{
    JE::GameEngine* engine = new JE::GameEngine();
    if (engine->init() && (engine->createGameInstance<TestAppGameInstance>() != nullptr))
    {
        while (engine->update()) {}
    }
    else
    {
        JUTILS_LOG(error, JSTR("Failed to init game engine"));
    }

    engine->clear();
    delete engine;

    jutils::jstring_hash_table::ClearInstance();
}
