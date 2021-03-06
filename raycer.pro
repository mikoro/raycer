TARGET = raycer
TEMPLATE = app
DESTDIR = bin
OBJECTS_DIR = build
CONFIG += c++11
QMAKE_LIBDIR += platform/linux/lib
QMAKE_CXXFLAGS += -fopenmp -Wno-unknown-pragmas -Wno-deprecated-declarations
LIBS += -lstdc++ -ldl -lm -lpthread -lglfw -lGL -lOpenCL -lfreetype-gl -lfreetype -lboost_system -lboost_filesystem -lXrandr -lXi -lXcursor -lXinerama -fopenmp
QMAKE_POST_LINK += platform/linux/post-build.sh

INCLUDEPATH += include \
               src

HEADERS += src/App.h \
           src/Common.h \
           src/Settings.h \
           src/stdafx.h \
           src/Math/AxisAngle.h \
           src/Math/Color.h \
           src/Math/EulerAngle.h \
           src/Math/MathUtils.h \
           src/Math/Matrix4x4.h \
           src/Math/MovingAverage.h \
           src/Math/ONB.h \
           src/Math/Polynomial.h \
           src/Math/Quaternion.h \
           src/Math/Solver.h \
           src/Math/Vector2.h \
           src/Math/Vector3.h \
           src/Math/Vector4.h \
           src/OpenCL/CLManager.h \
           src/OpenCL/CLScene.h \
           src/OpenCL/CLStructs.h \
           src/OpenCL/CLTracer.h \
           src/Raytracing/AABB.h \
           src/Raytracing/Camera.h \
           src/Raytracing/Intersection.h \
           src/Raytracing/Lights.h \
           src/Raytracing/Material.h \
           src/Raytracing/Ray.h \
           src/Raytracing/Scene.h \
           src/Rendering/Film.h \
           src/Rendering/FilmRenderer.h \
           src/Rendering/Image.h \
           src/Rendering/ImagePool.h \
           src/Rendering/Text.h \
           src/Runners/ConsoleRunner.h \
           src/Runners/NetworkRunner.h \
           src/Runners/WindowRunner.h \
           src/Utils/CellNoise.h \
           src/Utils/ColorGradient.h \
           src/Utils/FpsCounter.h \
           src/Utils/GLHelper.h \
           src/Utils/IniReader.h \
           src/Utils/Log.h \
           src/Utils/ModelLoader.h \
           src/Utils/PerlinNoise.h \
           src/Utils/PoissonDisc.h \
           src/Utils/StringUtils.h \
           src/Utils/SysUtils.h \
           src/Utils/Timer.h \
           src/Utils/ValueNoise.h \
           src/Raytracing/Primitives/BlinnBlob.h \
           src/Raytracing/Primitives/Box.h \
           src/Raytracing/Primitives/CSG.h \
           src/Raytracing/Primitives/Cylinder.h \
           src/Raytracing/Primitives/FlatBVH.h \
           src/Raytracing/Primitives/Instance.h \
           src/Raytracing/Primitives/Plane.h \
           src/Raytracing/Primitives/Primitive.h \
           src/Raytracing/Primitives/PrimitiveGroup.h \
           src/Raytracing/Primitives/Sphere.h \
           src/Raytracing/Primitives/Torus.h \
           src/Raytracing/Primitives/Triangle.h \
           src/Raytracing/Textures/AtmosphereTexture.h \
           src/Raytracing/Textures/CellNoiseTexture.h \
           src/Raytracing/Textures/CheckerTexture.h \
           src/Raytracing/Textures/ColorGradientTexture.h \
           src/Raytracing/Textures/ColorTexture.h \
           src/Raytracing/Textures/FireTexture.h \
           src/Raytracing/Textures/ImageTexture.h \
           src/Raytracing/Textures/MarbleTexture.h \
           src/Raytracing/Textures/PerlinNoiseTexture.h \
           src/Raytracing/Textures/Texture.h \
           src/Raytracing/Textures/ValueNoiseTexture.h \
           src/Raytracing/Textures/VoronoiTexture.h \
           src/Raytracing/Textures/WoodTexture.h \
           src/Raytracing/Tracers/PathTracer.h \
           src/Raytracing/Tracers/Raytracer.h \
           src/Raytracing/Tracers/Tracer.h \
           src/Raytracing/Tracers/TracerState.h \
           src/Rendering/Filters/BellFilter.h \
           src/Rendering/Filters/BoxFilter.h \
           src/Rendering/Filters/Filter.h \
           src/Rendering/Filters/GaussianFilter.h \
           src/Rendering/Filters/LanczosSincFilter.h \
           src/Rendering/Filters/MitchellFilter.h \
           src/Rendering/Filters/TentFilter.h \
           src/Rendering/Samplers/CenterSampler.h \
           src/Rendering/Samplers/CMJSampler.h \
           src/Rendering/Samplers/JitteredSampler.h \
           src/Rendering/Samplers/PoissonDiscSampler.h \
           src/Rendering/Samplers/RandomSampler.h \
           src/Rendering/Samplers/RegularSampler.h \
           src/Rendering/Samplers/Sampler.h \
           src/Rendering/ToneMappers/LinearToneMapper.h \
           src/Rendering/ToneMappers/PassthroughToneMapper.h \
           src/Rendering/ToneMappers/ReinhardToneMapper.h \
           src/Rendering/ToneMappers/SimpleToneMapper.h \
           src/Rendering/ToneMappers/ToneMapper.h \
           src/Runners/WindowRunnerStates/DefaultState.h \
           src/Runners/WindowRunnerStates/WindowRunnerState.h

SOURCES += src/App.cpp \
           src/gl3w.cpp \
           src/Settings.cpp \
           src/stdafx.cpp \
           src/Math/AxisAngle.cpp \
           src/Math/Color.cpp \
           src/Math/EulerAngle.cpp \
           src/Math/MathUtils.cpp \
           src/Math/Matrix4x4.cpp \
           src/Math/MovingAverage.cpp \
           src/Math/ONB.cpp \
           src/Math/Quaternion.cpp \
           src/Math/Solver.cpp \
           src/Math/Vector2.cpp \
           src/Math/Vector3.cpp \
           src/Math/Vector4.cpp \
           src/OpenCL/CLManager.cpp \
           src/OpenCL/CLScene.cpp \
           src/OpenCL/CLTracer.cpp \
           src/Raytracing/AABB.cpp \
           src/Raytracing/Camera.cpp \
           src/Raytracing/Ray.cpp \
           src/Raytracing/Scene.cpp \
           src/Rendering/Film.cpp \
           src/Rendering/FilmRenderer.cpp \
           src/Rendering/Image.cpp \
           src/Rendering/ImagePool.cpp \
           src/Rendering/Text.cpp \
           src/Runners/ConsoleRunner.cpp \
           src/Runners/NetworkRunner.cpp \
           src/Runners/WindowRunner.cpp \
           src/Tests/ColorGradientTest.cpp \
           src/Tests/EulerAngleTest.cpp \
           src/Tests/FilterTest.cpp \
           src/Tests/ImageTest.cpp \
           src/Tests/MathUtilsTest.cpp \
           src/Tests/Matrix4x4Test.cpp \
           src/Tests/ModelLoaderTest.cpp \
           src/Tests/PolynomialTest.cpp \
           src/Tests/SamplerTest.cpp \
           src/Tests/SolverTest.cpp \
           src/Tests/TestScenesTest.cpp \
           src/Tests/Vector3Test.cpp \
           src/TestScenes/TestScene1.cpp \
           src/TestScenes/TestScene10.cpp \
           src/TestScenes/TestScene11.cpp \
           src/TestScenes/TestScene12.cpp \
           src/TestScenes/TestScene13.cpp \
           src/TestScenes/TestScene14.cpp \
           src/TestScenes/TestScene15.cpp \
           src/TestScenes/TestScene16.cpp \
           src/TestScenes/TestScene17.cpp \
           src/TestScenes/TestScene18.cpp \
           src/TestScenes/TestScene19.cpp \
           src/TestScenes/TestScene2.cpp \
           src/TestScenes/TestScene3.cpp \
           src/TestScenes/TestScene4.cpp \
           src/TestScenes/TestScene5.cpp \
           src/TestScenes/TestScene6.cpp \
           src/TestScenes/TestScene7.cpp \
           src/TestScenes/TestScene8.cpp \
           src/TestScenes/TestScene9.cpp \
           src/Utils/CellNoise.cpp \
           src/Utils/ColorGradient.cpp \
           src/Utils/FpsCounter.cpp \
           src/Utils/GLHelper.cpp \
           src/Utils/IniReader.cpp \
           src/Utils/Log.cpp \
           src/Utils/ObjModelLoader.cpp \
           src/Utils/PerlinNoise.cpp \
           src/Utils/PlyModelLoader.cpp \
           src/Utils/PoissonDisc.cpp \
           src/Utils/StringUtils.cpp \
           src/Utils/SysUtils.cpp \
           src/Utils/Timer.cpp \
           src/Utils/ValueNoise.cpp \
           src/Raytracing/Primitives/BlinnBlob.cpp \
           src/Raytracing/Primitives/Box.cpp \
           src/Raytracing/Primitives/CSG.cpp \
           src/Raytracing/Primitives/Cylinder.cpp \
           src/Raytracing/Primitives/FlatBVH.cpp \
           src/Raytracing/Primitives/Instance.cpp \
           src/Raytracing/Primitives/Plane.cpp \
           src/Raytracing/Primitives/PrimitiveGroup.cpp \
           src/Raytracing/Primitives/Sphere.cpp \
           src/Raytracing/Primitives/Torus.cpp \
           src/Raytracing/Primitives/Triangle.cpp \
           src/Raytracing/Textures/AtmosphereTexture.cpp \
           src/Raytracing/Textures/CellNoiseTexture.cpp \
           src/Raytracing/Textures/CheckerTexture.cpp \
           src/Raytracing/Textures/ColorGradientTexture.cpp \
           src/Raytracing/Textures/ColorTexture.cpp \
           src/Raytracing/Textures/FireTexture.cpp \
           src/Raytracing/Textures/ImageTexture.cpp \
           src/Raytracing/Textures/MarbleTexture.cpp \
           src/Raytracing/Textures/PerlinNoiseTexture.cpp \
           src/Raytracing/Textures/ValueNoiseTexture.cpp \
           src/Raytracing/Textures/VoronoiTexture.cpp \
           src/Raytracing/Textures/WoodTexture.cpp \
           src/Raytracing/Tracers/PathTracer.cpp \
           src/Raytracing/Tracers/Raytracer.cpp \
           src/Raytracing/Tracers/Tracer.cpp \
           src/Rendering/Filters/BellFilter.cpp \
           src/Rendering/Filters/BoxFilter.cpp \
           src/Rendering/Filters/Filter.cpp \
           src/Rendering/Filters/GaussianFilter.cpp \
           src/Rendering/Filters/LanczosSincFilter.cpp \
           src/Rendering/Filters/MitchellFilter.cpp \
           src/Rendering/Filters/TentFilter.cpp \
           src/Rendering/Samplers/CenterSampler.cpp \
           src/Rendering/Samplers/CMJSampler.cpp \
           src/Rendering/Samplers/JitteredSampler.cpp \
           src/Rendering/Samplers/PoissonDiscSampler.cpp \
           src/Rendering/Samplers/RandomSampler.cpp \
           src/Rendering/Samplers/RegularSampler.cpp \
           src/Rendering/Samplers/Sampler.cpp \
           src/Rendering/ToneMappers/LinearToneMapper.cpp \
           src/Rendering/ToneMappers/PassthroughToneMapper.cpp \
           src/Rendering/ToneMappers/ReinhardToneMapper.cpp \
           src/Rendering/ToneMappers/SimpleToneMapper.cpp \
           src/Rendering/ToneMappers/ToneMapper.cpp \
           src/Runners/WindowRunnerStates/DefaultState.cpp
