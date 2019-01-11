echo Copying updated board definition
cp -Ruv boards/saikostrobe ../RIOT/boards
cp -Ruv drivers/include ../RIOT/drivers
cp -Ruv drivers/tps92661 ../RIOT/drivers
cp -Ruv drivers/Makefile.* ../RIOT/drivers

echo Building...
make BOARD=saikostrobe BUILD_IN_DOCKER=1
