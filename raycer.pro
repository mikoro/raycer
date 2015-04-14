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
		   src/CpuRaytracing/Camera.h \
		   src/CpuRaytracing/CpuRaytracer.h \
		   src/CpuRaytracing/Intersection.h \
		   src/CpuRaytracing/Light.h \
		   src/CpuRaytracing/Material.h \
		   src/CpuRaytracing/Mesh.h \
		   src/CpuRaytracing/Plane.h \
		   src/CpuRaytracing/Primitive.h \
		   src/CpuRaytracing/Ray.h \
		   src/CpuRaytracing/RaytraceInfo.h \
		   src/CpuRaytracing/Scene.h \
		   src/CpuRaytracing/Sphere.h \
		   src/CpuRaytracing/Triangle.h \
		   src/GpuRaytracing/GpuRaytracer.h \
		   src/GpuRaytracing/OpenCL.h \
		   src/Math/Color.h \
		   src/Math/EulerAngle.h \
		   src/Math/MathUtils.h \
		   src/Math/Matrix4x4.h \
		   src/Math/Vector2.h \
		   src/Math/Vector3.h \
		   src/Rendering/Framebuffer.h \
		   src/Rendering/Image.h \
		   src/Rendering/RenderTarget.h \
		   src/Rendering/Text.h \
		   src/Runners/ConsoleRunner.h \
		   src/Runners/InteractiveRunner.h \
		   src/Runners/NetworkRunner.h \
		   src/States/CpuTracingState.h \
		   src/States/GpuTracingState.h \
		   src/States/InteractiveState.h \
		   src/Utils/Errors.h \
		   src/Utils/FpsCounter.h \
		   src/Utils/IniReader.h \
		   src/Utils/Log.h \
		   src/Utils/OpenGL.h \
		   src/Utils/Settings.h

SOURCES += src/App.cpp \
		   src/CpuRaytracing/Camera.cpp \
		   src/CpuRaytracing/CpuRaytracer.cpp \
		   src/CpuRaytracing/Mesh.cpp \
		   src/CpuRaytracing/Plane.cpp \
		   src/CpuRaytracing/Ray.cpp \
		   src/CpuRaytracing/Scene.cpp \
		   src/CpuRaytracing/Sphere.cpp \
		   src/CpuRaytracing/Triangle.cpp \
		   src/GpuRaytracing/GpuRaytracer.cpp \
		   src/GpuRaytracing/OpenCL.cpp \
		   src/Math/Color.cpp \
		   src/Math/EulerAngle.cpp \
		   src/Math/MathUtils.cpp \
		   src/Math/Matrix4x4.cpp \
		   src/Math/Vector2.cpp \
		   src/Math/Vector3.cpp \
		   src/Rendering/Framebuffer.cpp \
		   src/Rendering/Image.cpp \
		   src/Rendering/Text.cpp \
		   src/Runners/ConsoleRunner.cpp \
		   src/Runners/InteractiveRunner.cpp \
		   src/Runners/NetworkRunner.cpp \
		   src/States/CpuTracingState.cpp \
		   src/States/GpuTracingState.cpp \
		   src/Utils/Errors.cpp \
		   src/Utils/FpsCounter.cpp \
		   src/Utils/IniReader.cpp \
		   src/Utils/Log.cpp \
		   src/Utils/OpenGL.cpp \
		   src/Utils/Settings.cpp
