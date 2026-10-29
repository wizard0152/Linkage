param(
    [ValidateSet('Build','BuildGame','ProjectFiles','CreateMap','Smoke','Editor','Play')][string]$Action='Build',
    [string]$EngineRoot=$env:LINKAGE_UE_ROOT
)
$ErrorActionPreference='Stop'
if (!$EngineRoot) { $EngineRoot='D:/UE_5.7' }
$projectRoot=Split-Path -Parent $PSScriptRoot
$project=Join-Path $projectRoot 'Linkage.uproject'
if (!(Test-Path "$EngineRoot/Engine/Binaries/Win64/UnrealEditor.exe")) { throw "UE not found: $EngineRoot. Set LINKAGE_UE_ROOT." }
New-Item -ItemType Directory -Force "$projectRoot/Saved/Logs" | Out-Null
$env:UE_LocalDataCachePath="$projectRoot/DerivedDataCache"
# Use the documented hyphenated DDC environment variable.
[Environment]::SetEnvironmentVariable('UE-LocalDataCachePath', "$projectRoot/DerivedDataCache", 'Process')
$sdk='C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64'
if (Test-Path "$sdk/mt.exe") { $env:Path="$sdk;$env:Path" }
if ($Action -in @('Build','BuildGame')) {
    $target=if($Action -eq 'Build'){'LinkageEditor'}else{'Linkage'}
    & "$EngineRoot/Engine/Build/BatchFiles/Build.bat" $target Win64 Development $project -WaitMutex -NoHotReloadFromIDE -NoUBA "-Log=$projectRoot/Saved/Logs/Build-$target.log"
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $LASTEXITCODE" }
    exit 0
}
if ($Action -eq 'ProjectFiles') {
    & "$EngineRoot/Engine/Build/BatchFiles/Build.bat" -projectfiles "-project=$project" -game -engine
    if ($LASTEXITCODE -ne 0) { throw 'Project generation failed' }
    exit 0
}
$arguments=@($project,'-NoSplash','-DDC=NoZenLocalFallback',"-LocalDataCachePath=$projectRoot/DerivedDataCache","-ShaderWorkingDir=$projectRoot/Saved/ShaderWork")
if ($Action -eq 'CreateMap') {
    & "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" @arguments -Unattended -NullRHI -run=pythonscript "-script=$PSScriptRoot/build_development_map.py" "-abslog=$projectRoot/Saved/Logs/CreateMap.log"
    if ($LASTEXITCODE -ne 0 -or !(Test-Path "$projectRoot/Content/Maps/L_Development.umap")) { throw 'Map generation failed' }
} elseif ($Action -eq 'Smoke') {
    $smokeLog="$projectRoot/Saved/Logs/Smoke.log"
    & "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" @arguments /Game/Maps/L_Development -game -Unattended -NullRHI -NoSound -LinkageSmoke "-abslog=$smokeLog"
    if ($LASTEXITCODE -ne 0 -or !(Select-String -LiteralPath $smokeLog -SimpleMatch 'LINKAGE_SMOKE PASS' -Quiet)) { throw 'Runtime smoke failed' }
} else {
    if ($Action -eq 'Play') { $arguments+=@('/Game/Maps/L_Development','-game','-windowed','-ResX=1600','-ResY=900') }
    Start-Process -FilePath "$EngineRoot/Engine/Binaries/Win64/UnrealEditor.exe" -ArgumentList ($arguments | ForEach-Object { '"'+$_+'"' })
}
