param([string]$compilerSetup = '')
$ErrorActionPreference = 'Stop'
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
if (-not $compilerSetup) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (Test-Path -LiteralPath $vswhere) {
        $installPath = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($installPath) { $compilerSetup = Join-Path $installPath 'VC/Auxiliary/Build/vcvars64.bat' }
    }
}
if (-not $compilerSetup -or -not (Test-Path -LiteralPath $compilerSetup)) {
    throw 'MSVC not found. Supply -compilerSetup with the installed vcvars64.bat path.'
}
$buildRoot = Join-Path $projectRoot '.pio/host-tests'
New-Item -ItemType Directory -Force -Path $buildRoot | Out-Null
$batchPath = Join-Path $buildRoot 'run.cmd'
$batchLines = @(
    '@echo off',
    'chcp 65001 >nul',
    ('call "{0}" >nul' -f $compilerSetup),
    'if errorlevel 1 exit /b 1',
    ('cd /d "{0}"' -f $buildRoot),
    ('cl /nologo /std:c++17 /EHsc /W4 /WX /DNDEBUG /I"{0}\include" "{0}\src\systemConfig.cpp" "{0}\test\host\systemConfigTest.cpp" /Fe:systemConfigTest.exe' -f $projectRoot),
    'if errorlevel 1 exit /b 1',
    'systemConfigTest.exe',
    'exit /b %errorlevel%'
)
[System.IO.File]::WriteAllLines($batchPath, $batchLines, [System.Text.UTF8Encoding]::new($false))
& cmd.exe /d /c "`"$batchPath`""
exit $LASTEXITCODE
