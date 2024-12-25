@echo off

set flags=-m64 -Wall -Wextra -Werror -pedantic -O3
set headers=-I"src/headers"
set crt_flags=-MD

clang "src/main.c" "src/console.c" "src/user_input.c" "src/file.c" %headers% %flags% %crt_flags% -o app.exe
