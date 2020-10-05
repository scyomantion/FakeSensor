cmake -A Win32 -S . -B "build32"
cmake -A x64 -S . -B "build64"

cmake --build build32 --config Release
cmake --build build32 --config Release --target create_zip

cmake --build build64 --config Release
cmake --build build64 --config Release --target create_zip

pause