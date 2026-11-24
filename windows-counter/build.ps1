[CmdletBinding()]
param(
    [string]$VisualStudioVersion = "2022",
    [string]$BuildType = "Release",
    [string]$BuildDirectory = "$PSScriptRoot\build",
    [switch]$SkipPackageInstall
)

$ErrorActionPreference = "Stop"

$CMakeVersion = "3.28.3"
$VsBuildToolsPackage = "Microsoft.VisualStudio.2022.BuildTools"
$CMakePackage = "Kitware.CMake"

function Install-Packages {
    if ($SkipPackageInstall) {
        return
    }

    if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
        throw "Winget is required. Install App Installer from Microsoft Store, then rerun this script."
    }

    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Host "Installing CMake $CMakeVersion..."
        winget install --id $CMakePackage --exact --version $CMakeVersion `
            --accept-package-agreements --accept-source-agreements
    }

    if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
        Write-Host "Installing Visual Studio $VisualStudioVersion Build Tools..."
        winget install --id $VsBuildToolsPackage --exact --override `
            "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended" `
            --accept-package-agreements --accept-source-agreements
    }
}

function Import-VsEnvironment {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        return
    }

    $installationPath = & $vswhere `
        -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath

    if (-not $installationPath) {
        return
    }

    $developerShell = Join-Path $installationPath "Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
    if (Test-Path $developerShell) {
        Import-Module $developerShell
        Enter-VsDevShell -VsInstallPath $installationPath -SkipAutomaticLocation
    }
}

function Refresh-CMakeCommand {
    if (Get-Command cmake -ErrorAction SilentlyContinue) {
        return
    }

    $candidates = @(
        "$env:ProgramFiles\CMake\bin\cmake.exe",
        "${env:ProgramFiles(x86)}\CMake\bin\cmake.exe",
        "$env:LocalAppData\Microsoft\WinGet\Links\cmake.exe"
    )

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path $candidate)) {
            $env:Path = "$(Split-Path $candidate);$env:Path"
            return
        }
    }
}

Install-Packages
Import-VsEnvironment
Refresh-CMakeCommand

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "CMake is unavailable. Restart the shell and rerun build.ps1."
}

# Triskelion needs OpenSSL (Ed25519 + SHA-256). Get it through vcpkg, cloned next to the build if missing.
$vcpkgRoot = if ($env:VCPKG_ROOT) { $env:VCPKG_ROOT } else { Join-Path $PSScriptRoot "vcpkg" }
if (-not (Test-Path (Join-Path $vcpkgRoot "vcpkg.exe"))) {
    if (-not (Test-Path $vcpkgRoot)) {
        git clone https://github.com/microsoft/vcpkg $vcpkgRoot
    }
    & (Join-Path $vcpkgRoot "bootstrap-vcpkg.bat") -disableMetrics
}
& (Join-Path $vcpkgRoot "vcpkg.exe") install openssl:x64-windows-static
$toolchain = Join-Path $vcpkgRoot "scripts\buildsystems\vcpkg.cmake"

$sourceDirectory = $PSScriptRoot

cmake -S $sourceDirectory -B $BuildDirectory -A x64 `
    "-DCMAKE_TOOLCHAIN_FILE=$toolchain" -DVCPKG_TARGET_TRIPLET=x64-windows-static `
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
cmake --build $BuildDirectory --config $BuildType --parallel

$executable = Join-Path $BuildDirectory "$BuildType\CounterApp.exe"
Write-Host "Executable: $executable"
