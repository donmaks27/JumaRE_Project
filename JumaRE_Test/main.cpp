// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestApp_JRE.h"
#include "TestApp_JE.h"

int main()
{
#ifdef JDEBUG
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    
    _CrtMemState memoryState;
    _CrtMemCheckpoint(&memoryState);
#endif

    {
        TestApp* app = new TestApp_JE();
        app->run();
        delete app;
    }

#ifdef JDEBUG
    _CrtMemDumpAllObjectsSince(&memoryState);
#endif
    return 0;
}