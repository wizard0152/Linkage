@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\dev.ps1" -Action Build
if errorlevel 1 pause
