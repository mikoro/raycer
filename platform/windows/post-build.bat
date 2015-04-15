@echo off

if not exist bin\%1\data (
  xcopy /E /I /Y /Q data bin\%1\data
)

if not exist bin\%1\data\dll (
  xcopy /E /I /Y /Q platform\windows\dll bin\%1\data\dll
)

if not exist bin\%1\raycer.exe.config (
  xcopy /Y /Q platform\windows\raycer.exe.config bin\%1
)

if not exist bin\%1\settings.ini (
  xcopy /Y /Q misc\settings.ini bin\%1
)
