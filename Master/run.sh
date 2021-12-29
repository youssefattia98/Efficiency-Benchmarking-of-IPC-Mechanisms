gcc prod.c -o prod -lrt -pthread
gcc cons.c -o cons -lrt -pthread
gcc Master.c -o master -lm
./master
