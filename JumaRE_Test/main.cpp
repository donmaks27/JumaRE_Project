// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TestApp_JRE.h"

int main()
{
#ifdef JDEBUG
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    
    _CrtMemState memoryState;
    _CrtMemCheckpoint(&memoryState);
#endif

    {
        TestApp* app = new TestApp_JRE();
        app->run();
        delete app;
    }

#ifdef JDEBUG
    _CrtMemDumpAllObjectsSince(&memoryState);
#endif
    return 0;
}