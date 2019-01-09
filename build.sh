echo Copying updated board definition
cp -Ruv boards/saikostrobe ../RIOT/boards
echo Building...
make BOARD=saikostrobe BUILD_IN_DOCKER=1
