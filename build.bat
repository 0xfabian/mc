@ECHO OFF

SET SDL_INCLUDE_PATH=
SET GLAD_INCLUDE_PATH=
SET GLM_INCLUDE_PATH=
SET MY_INCLUDE_PATH=

SET SDL_LIB_PATH=
SET GLAD_SRC=

SET FLAGS=-lmingw32 -lSDL2main -lSDL2 -O2

SET FILES=src/*.cpp

SET APP_NAME=mc

g++ %FILES% %GLAD_SRC% -o %APP_NAME% -I%SDL_INCLUDE_PATH% -I%GLAD_INCLUDE_PATH% -I%GLM_INCLUDE_PATH% -I%MY_INCLUDE_PATH% -L%SDL_LIB_PATH% %FLAGS%
