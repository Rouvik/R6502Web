emcc asm6.c \
-s MODULARIZE=1 \
-s EXPORTED_FUNCTIONS=[_main] \
-s EXPORTED_RUNTIME_METHODS='["FS", "callMain"]' \
-o ../bin/asm6.mjs
