cmake_minimum_required(VERSION 3.12)

project(JumaRE_Test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(jutils/jutils)
add_subdirectory(JumaRE/JumaRE)
add_subdirectory(JumaEngine/JumaEngine)

add_executable(JumaRE_Test 
    JumaRE_Test/main.cpp
    JumaRE_Test/TestApp.cpp
    JumaRE_Test/TestApp_JE.cpp
    JumaRE_Test/TestAppGameInstance.cpp
)
target_include_directories(JumaRE_Test INTERFACE $<TARGET_PROPERTY:JumaEngine,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(JumaRE_Test JumaEngine)

add_dependencies(JumaRE jutils)
add_dependencies(JumaEngine JumaRE)
add_dependencies(JumaRE_Test JumaEngine)

install(DIRECTORY JumaRE_Test/content/ DESTINATION bin/content)
install(TARGETS JumaRE_Test DESTINATION bin)
