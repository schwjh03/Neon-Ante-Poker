param(
    [string]$Emsdk = (Join-Path $PSScriptRoot 'build/emsdk'),
    [string]$Raylib = (Join-Path $PSScriptRoot 'build/raylib-src'),
    [switch]$TestEngine
)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $Emsdk = (Resolve-Path -LiteralPath $Emsdk).Path
    $Raylib = (Resolve-Path -LiteralPath $Raylib).Path
    $python = Get-ChildItem -LiteralPath (Join-Path $Emsdk 'python') -Filter python.exe -Recurse | Select-Object -First 1 -ExpandProperty FullName
    $compiler = Join-Path $Emsdk 'upstream/emscripten/emcc.py'
    $archiver = Join-Path $Emsdk 'upstream/emscripten/emar.py'
    if (-not $python -or -not (Test-Path -LiteralPath $compiler)) { throw 'Install and activate the Emscripten SDK first. See README.md.' }
    $env:EM_CONFIG = Join-Path $Emsdk '.emscripten'
    New-Item -ItemType Directory -Force build/web,docs | Out-Null
    $raySource = Join-Path $Raylib 'src'
    $rayObjects = @()
    foreach ($unit in @('rcore','rshapes','rtextures','rtext','utils')) {
        $object = "build/web/$unit.o"
        & $python $compiler '-O2' '-DPLATFORM_WEB' '-DGRAPHICS_API_OPENGL_ES2' "-I$raySource" '-c' (Join-Path $raySource "$unit.c") '-o' $object
        if ($LASTEXITCODE -ne 0) { throw "Browser graphics compilation failed: $unit" }
        $rayObjects += $object
    }
    & $python $archiver 'rcs' 'build/web/libraylib.a' @rayObjects
    if ($LASTEXITCODE -ne 0) { throw 'Could not archive browser graphics.' }
    $pokerObjects = @()
    foreach ($unit in @('evaluator5','hashtable5','hash','hashtable','dptables','tables_bitwise')) {
        $object = "build/web/$unit.o"
        & $python $compiler '-O2' '-std=c99' '-c' "vendor/phevaluator/cpp/src/$unit.c" '-o' $object
        if ($LASTEXITCODE -ne 0) { throw "Browser evaluator compilation failed: $unit" }
        $pokerObjects += $object
    }
    & $python $archiver 'rcs' 'build/web/libpheval.a' @pokerObjects
    $sources = @(Get-ChildItem src -Filter '*.cpp' -Recurse | Sort-Object FullName | Select-Object -ExpandProperty FullName)
    & $python $compiler '-std=c++17' '-O2' '-Wall' '-Wextra' '-Isrc' "-I$raySource" @sources `
        'build/web/libpheval.a' 'build/web/libraylib.a' '-sUSE_GLFW=3' '-sALLOW_MEMORY_GROWTH=1' `
        '-sINITIAL_MEMORY=67108864' '-sSTACK_SIZE=2097152' '-sENVIRONMENT=web' '-sFORCE_FILESYSTEM=1' `
        '-sEXPORTED_FUNCTIONS=_main,_neon_import_build,_neon_export_build' '-sEXPORTED_RUNTIME_METHODS=ccall' `
        '-sASSERTIONS=1' '--preload-file' 'web/assets@/assets' '-o' 'docs/neon.js'
    if ($LASTEXITCODE -ne 0) { throw 'Browser game build failed.' }
    Copy-Item web/index.html,web/styles.css,web/browser.js -Destination docs
    Copy-Item docs/screenshots/table.png docs/poster.png
    Copy-Item THIRD_PARTY.md docs/THIRD_PARTY.md
    New-Item -ItemType Directory -Force docs/licenses | Out-Null
    Copy-Item web/assets/fonts/*-OFL.txt docs/licenses
    Copy-Item (Join-Path $Raylib 'LICENSE') docs/licenses/raylib-LICENSE.txt
    Copy-Item vendor/phevaluator/LICENSE docs/licenses/PokerHandEvaluator-LICENSE.txt
    Copy-Item (Join-Path $Emsdk 'upstream/emscripten/LICENSE') docs/licenses/Emscripten-LICENSE.txt
    if ($TestEngine) {
        $engine = @('src/game.cpp') + @(Get-ChildItem src/engine -Filter '*.cpp' | Sort-Object FullName | Select-Object -ExpandProperty FullName)
        & $python $compiler '-std=c++17' '-O2' '-Isrc' 'tests/game_tests.cpp' @engine 'build/web/libpheval.a' `
            '-sENVIRONMENT=node' '-sALLOW_MEMORY_GROWTH=1' '-sSTACK_SIZE=2097152' '-o' 'build/web/engine-tests.js'
        if ($LASTEXITCODE -ne 0) { throw 'Browser engine test build failed.' }
        $node = Get-ChildItem -LiteralPath (Join-Path $Emsdk 'node') -Filter node.exe -Recurse | Select-Object -First 1 -ExpandProperty FullName
        & $node build/web/engine-tests.js
        if ($LASTEXITCODE -ne 0) { throw 'WebAssembly engine tests failed.' }
    }
    Write-Output 'Built browser game in docs/ (index.html, neon.js, neon.wasm, neon.data).'
} finally { Pop-Location }
