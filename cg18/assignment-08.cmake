cmake_minimum_required(VERSION 3.0)

set(MY_ADDITIONAL_SOURCES
    # Please add your source files here
    rt/volumes/volume.h
    rt/volumes/uniformfogambientlight.h
    rt/volumes/uniformfogambientlight.cpp
    rt/volumes/uniformfog.h
    rt/volumes/uniformfog.cpp
    rt/volumes/heterogenousfog.h
    rt/volumes/heterogenousfog.cpp
    rt/integrators/volumetracer.h
    rt/integrators/volumetracer.cpp
        )

set(ASSIGNMENT_SOURCES
    rt/primmod/bmap.cpp
    rt/primmod/bmap.h
    ${MY_ADDITIONAL_SOURCES}
    main/a_bump.cpp
)

file(COPY
    models/stones_bump.png
    models/stones_spec.png
    DESTINATION ${CMAKE_BINARY_DIR}/models
)
