@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\dev.ps1" -Action Editor
if errorlevel 1 pause
