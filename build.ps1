param([switch]$Test)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $compiler = (Get-Command g++.exe -ErrorAction SilentlyContinue).Source
    if (-not $compiler -and (Test-Path 'C:\msys64\ucrt64\bin\g++.exe')) { $compiler = 'C:\msys64\ucrt64\bin\g++.exe' }
    if (-not $compiler) { throw 'Install a MinGW-w64 C++17 compiler and place g++ on PATH.' }
    $cCompiler = Join-Path (Split-Path $compiler) 'gcc.exe'
    $ar = Join-Path (Split-Path $compiler) 'ar.exe'
    $ray = 'vendor/raylib-5.5_win64_mingw-w64'
    $ph = 'vendor/phevaluator/cpp/src'
    New-Item -ItemType Directory -Force -Path 'build' | Out-Null
    $objects = @()
    foreach ($unit in @('evaluator5','hashtable5','hash','hashtable','dptables','tables_bitwise')) {
        $object = "build/$unit.o"
        & $cCompiler '-O2' '-std=c99' '-c' "$ph/$unit.c" '-o' $object
        if ($LASTEXITCODE -ne 0) { throw "Could not compile $unit." }
        $objects += $object
    }
    & $ar 'rcs' 'build/libpheval.a' @objects
    if ($LASTEXITCODE -ne 0) { throw 'Could not archive the poker evaluator.' }

    $compilerFlags = @(
        '-std=c++17', '-O2', '-Wall', '-Wextra', '-Wpedantic',
        '-static', '-static-libgcc', '-static-libstdc++', '-Isrc'
    )
    $gameSources = @(Get-ChildItem -Path 'src' -Filter '*.cpp' -Recurse |
        Sort-Object FullName | Select-Object -ExpandProperty FullName)
    $engineSources = @('src/game.cpp') + @(Get-ChildItem -Path 'src/engine' -Filter '*.cpp' |
        Sort-Object FullName | Select-Object -ExpandProperty FullName)
    $graphicsLibraries = @(
        'build/libpheval.a', "$ray/lib/libraylib.a", '-lopengl32', '-lgdi32', '-lwinmm'
    )

    & $compiler @compilerFlags '-mwindows' @gameSources 'tests/ui_smoke.cpp' `
        "-I$ray/include" @graphicsLibraries '-o' 'build/NeonAntePoker.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Game build failed.' }
    if ($Test) {
        & $compiler @compilerFlags 'tests/game_tests.cpp' @engineSources `
            'build/libpheval.a' '-o' 'build/game_tests.exe'
        if ($LASTEXITCODE -ne 0) { throw 'Test build failed.' }
        & '.\build\game_tests.exe'
        if ($LASTEXITCODE -ne 0) { throw 'Game tests failed.' }
        $smoke = Start-Process -FilePath (Join-Path $PWD 'build\NeonAntePoker.exe') -ArgumentList '--smoke-test' -WorkingDirectory (Join-Path $PWD 'build') -WindowStyle Hidden -Wait -PassThru
        Get-Content -LiteralPath 'build\ui-smoke.txt'
        if ($smoke.ExitCode -ne 0) { throw 'UI smoke test failed.' }
    }
    Write-Output 'Built build/NeonAntePoker.exe'
} finally { Pop-Location }
