param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$BuildType = 'Debug',
    [switch]$Reconfigure,
    [switch]$NoClean,
    [switch]$NoRun
)

$ErrorActionPreference = 'Stop'

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ProjectRoot 'build'
$ExePath = Join-Path $BuildDir 'MYC.exe'

$GccPath = 'C:/Strawberry/c/bin/gcc.exe'
$GxxPath = 'C:/Strawberry/c/bin/g++.exe'
$RuntimeBin = 'C:\Strawberry\c\bin'

if (-not (Test-Path $GccPath) -or -not (Test-Path $GxxPath)) {
    throw "未找到 Strawberry 编译器。请确认存在: $GccPath 和 $GxxPath"
}

Set-Location $ProjectRoot

Get-Process -Name gdb -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Get-Process -Name MYC -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue

if ($Reconfigure -or -not (Test-Path (Join-Path $BuildDir 'CMakeCache.txt'))) {
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }

    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_C_COMPILER=$GccPath -DCMAKE_CXX_COMPILER=$GxxPath
}

if (-not $NoClean) {
    cmake --build build --clean-first
} else {
    cmake --build build
}

if (-not $NoRun) {
    $env:Path = "$RuntimeBin;$env:Path"

    if (-not (Test-Path $ExePath)) {
        throw "构建完成，但未找到可执行文件: $ExePath"
    }

    & $ExePath
    exit $LASTEXITCODE
}