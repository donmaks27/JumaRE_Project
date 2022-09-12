// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

class TestApp
{
public:
    TestApp() = default;
    virtual ~TestApp() = default;

    virtual void run() = 0;
};
