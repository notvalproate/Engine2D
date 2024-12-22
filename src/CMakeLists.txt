set(SOURCES
    components/BoxCollider.cpp
    components/Camera.cpp
    components/CapsuleCollider.cpp
    components/CircleCollider.cpp
    components/Collider.cpp
    components/EdgeCollider.cpp
    components/PolygonCollider.cpp
    components/RigidBody.cpp
    components/SpriteRenderer.cpp
    Behaviour.cpp
    Color.cpp
    Component.cpp
    CursorHandler.cpp
    Engine2D.cpp
    GameObject.cpp
    InputHandler.cpp
    MathModule.cpp
    Object.cpp
    PhysicsHandler.cpp
    RenderingHandler.cpp
    Scene.cpp
    SceneHandler.cpp
    ScreenHandler.cpp
    TextureHandler.cpp
    TimeHandler.cpp
    Transform.cpp
    Vector2D.cpp
)

add_library(Engine2D SHARED ${SOURCES})
add_library(notval::Engine2D ALIAS Engine2D)

target_link_libraries(Engine2D
    PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        box2d
)

target_include_directories(Engine2D PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_options(Engine2D
    PRIVATE
        ${CXX_COMPILER_FLAGS}
)

target_compile_definitions(Engine2D 
    PRIVATE 
        ENGINE2D_EXPORTS
)

install(TARGETS Engine2D 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
)

include(cppcheck_analysis)
add_to_cppcheck(Engine2D SOURCES)

# Post build command to copy over runtime libraries to binary output directory

set(RUNTIME_LIBRARIES ${SDL2_RUNTIME} ${SDL2_IMAGE_RUNTIME} ${Box2D_RUNTIME})
set(RUNTIME_LIBRARY_DIRECTORY $<TARGET_FILE_DIR:Engine2D>)

if(RUNTIME_LIBRARIES)
    add_custom_command(TARGET Engine2D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${RUNTIME_LIBRARIES}
        ${RUNTIME_LIBRARY_DIRECTORY}
    )
endif()