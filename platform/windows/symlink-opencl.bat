@echo off

if exist ..\..\bin\Debug\data\opencl (
  rmdir /Q /S ..\..\bin\Debug\data\opencl
)

if exist ..\..\bin\Release\data\opencl (
  rmdir /Q /S ..\..\bin\Release\data\opencl
)

if exist ..\..\bin\Debug\data (
  mklink /D ..\..\bin\Debug\data\opencl ..\..\..\data\opencl
)

if exist ..\..\bin\Release\data (
  mklink /D ..\..\bin\Release\data\opencl ..\..\..\data\opencl
)
