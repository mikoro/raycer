TARGET = raycer
TEMPLATE = app
DESTDIR = bin
CONFIG += c++11 warn_off debug
OBJECTS_DIR = build
QMAKE_LIBDIR += platform/linux/lib
LIBS += -lGL -lGLEW -lglfw -lfreetype -lfreetype-gl -lOpenCL -lboost_system
QMAKE_POST_LINK += platform/linux/post-build.sh

INCLUDEPATH += include \
			   src

HEADERS += src/App.h \
           src/CLRaytracing/CLManager.h \
           src/CLRaytracing/CLRaytracer.h \
           src/CLRaytracing/CLScene.h \
           src/CLRaytracing/CLStructs.h \
           src/InteractiveStates/CLRaytracingState.h \
           src/InteractiveStates/InteractiveState.h \
           src/InteractiveStates/RaytracingState.h \
           src/Math/AxisAngle.h \
           src/Math/Color.h \
           src/Math/EulerAngle.h \
           src/Math/MathUtils.h \
           src/Math/Matrix4x4.h \
           src/Math/MovingAverage.h \
           src/Math/Quaternion.h \
           src/Math/Triangle.h \
           src/Math/Vector2.h \
           src/Math/Vector3.h \
           src/Raytracing/Camera.h \
           src/Raytracing/CheckerTexture.h \
           src/Raytracing/ColorTexture.h \
           src/Raytracing/Fog.h \
           src/Raytracing/ImageTexture.h \
           src/Raytracing/Intersection.h \
           src/Raytracing/Light.h \
           src/Raytracing/MarbleTexture.h \
           src/Raytracing/Material.h \
           src/Raytracing/Mesh.h \
           src/Raytracing/Plane.h \
           src/Raytracing/Primitive.h \
           src/Raytracing/Ray.h \
           src/Raytracing/Raytracer.h \
           src/Raytracing/Scene.h \
           src/Raytracing/Sphere.h \
           src/Raytracing/Texture.h \
           src/Raytracing/ToneMapper.h \
           src/Raytracing/WoodTexture.h \
           src/Rendering/Framebuffer.h \
           src/Rendering/GLHelper.h \
           src/Rendering/RenderTarget.h \
           src/Rendering/Text.h \
           src/Runners/ConsoleRunner.h \
           src/Runners/InteractiveRunner.h \
           src/Runners/NetworkRunner.h \
           src/Utils/Errors.h \
           src/Utils/FpsCounter.h \
           src/Utils/Image.h \
           src/Utils/IniReader.h \
           src/Utils/Log.h \
           src/Utils/PerlinNoise.h \
           src/Utils/Settings.h
           
SOURCES += src/App.cpp \
           src/CLRaytracing/CLManager.cpp \
           src/CLRaytracing/CLRaytracer.cpp \
           src/CLRaytracing/CLScene.cpp \
           src/InteractiveStates/CLRaytracingState.cpp \
           src/InteractiveStates/RaytracingState.cpp \
           src/Math/AxisAngle.cpp \
           src/Math/Color.cpp \
           src/Math/EulerAngle.cpp \
           src/Math/MathUtils.cpp \
           src/Math/Matrix4x4.cpp \
           src/Math/MovingAverage.cpp \
           src/Math/Quaternion.cpp \
           src/Math/Vector2.cpp \
           src/Math/Vector3.cpp \
           src/Raytracing/Camera.cpp \
           src/Raytracing/CheckerTexture.cpp \
           src/Raytracing/ColorTexture.cpp \
           src/Raytracing/Fog.cpp \
           src/Raytracing/ImageTexture.cpp \
           src/Raytracing/MarbleTexture.cpp \
           src/Raytracing/Mesh.cpp \
           src/Raytracing/Plane.cpp \
           src/Raytracing/Ray.cpp \
           src/Raytracing/Raytracer.cpp \
           src/Raytracing/Scene.cpp \
           src/Raytracing/Sphere.cpp \
           src/Raytracing/ToneMapper.cpp \
           src/Raytracing/WoodTexture.cpp \
           src/Rendering/Framebuffer.cpp \
           src/Rendering/GLHelper.cpp \
           src/Rendering/Text.cpp \
           src/Runners/ConsoleRunner.cpp \
           src/Runners/InteractiveRunner.cpp \
           src/Runners/NetworkRunner.cpp \
           src/Utils/Errors.cpp \
           src/Utils/FpsCounter.cpp \
           src/Utils/Image.cpp \
           src/Utils/IniReader.cpp \
           src/Utils/Log.cpp \
           src/Utils/PerlinNoise.cpp \
           src/Utils/Settings.cpp