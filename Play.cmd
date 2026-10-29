@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\dev.ps1" -Action Play
if errorlevel 1 pause
