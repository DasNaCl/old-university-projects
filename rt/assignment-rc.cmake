cmake_minimum_required(VERSION 3.0)

set(MY_ADDITIONAL_SOURCES
    # Please add your source files here
    rt/integrators/recraymarching.h
    rt/integrators/recraymarching.cpp
    rt/lights/projectivecolorlight2.h
    rt/lights/projectivecolorlight2.cpp
    rt/lights/projectivecolorlight.h
    rt/lights/projectivecolorlight.cpp
    rt/lights/perlinspotlight.h
    rt/lights/perlinspotlight.cpp
    main/newScene.cpp
        )

set(ASSIGNMENT_SOURCES
    ${MY_ADDITIONAL_SOURCES}
    main/scenetests.cpp
)

file(COPY
    models/re.obj
    models/re.mtl
    models/dino.obj
    models/dino.mtl
    models/dino2.obj
    models/dino2.mtl
    models/viel.obj
    models/viel.mtl
    models/test.obj
    models/test.mtl
    models/lucy.obj
    models/lucy.mtl
    models/high_grass.png
    models/leather_bump_map.png
    models/dino_skin.png
    models/dino_skin_bump.png
    models/dirt.png
    models/water_bump.png
    DESTINATION ${CMAKE_BINARY_DIR}/models
)
