cd Producer
gcc prod.c -o prod -lrt -pthread
cd ..
cd Consumer
gcc cons.c -o cons -lrt -pthread
cd ..
cd Master
gcc Master.c -o master -lm
./master
