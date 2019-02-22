cmake_minimum_required(VERSION 3.0)

set(MY_ADDITIONAL_SOURCES
    # Please add your source files here
    rt/solids/sky.cpp
    rt/materials/phong.cpp
    rt/materials/mirror.cpp
    rt/materials/lambertian.cpp
    rt/materials/flatmaterial.cpp
    rt/materials/combine.cpp
    rt/materials/cooktorrance.cpp
    rt/textures/constant.cpp
    rt/textures/checkerboard.cpp
    rt/textures/imagetex.cpp
    rt/integrators/recraytrace.cpp
    rt/coordmappers/cylindrical.cpp
    rt/coordmappers/plane.cpp
    rt/coordmappers/spherical.cpp
    rt/coordmappers/tmapper.cpp
    rt/coordmappers/world.cpp
    rt/coordmappers/environment.cpp
)

set(ASSIGNMENT_SOURCES
    rt/materials/lambertian.h
    rt/materials/phong.h
    rt/materials/mirror.h
    rt/materials/combine.h
    rt/integrators/recraytrace.h
    rt/textures/texture.h
    rt/textures/constant.h
    core/interpolate.cpp
    core/interpolate-impl.h
    core/interpolate.h
    rt/coordmappers/coordmapper.h
    rt/coordmappers/cylindrical.h
    rt/coordmappers/plane.h
    rt/coordmappers/spherical.h
    rt/coordmappers/tmapper.h
    rt/coordmappers/world.h
    rt/materials/flatmaterial.h
    rt/textures/checkerboard.h
    rt/textures/imagetex.h
    rt/textures/perlin.cpp
    rt/textures/perlin.h
    ${MY_ADDITIONAL_SOURCES}
    main/a_materials.cpp
    main/a_textures.cpp
    main/a_local.cpp
    main/a_mappers.cpp
)

file(COPY
    models/stones_diffuse.png
    models/sky.png
    models/sky_s.png
    DESTINATION ${CMAKE_BINARY_DIR}/models
)
