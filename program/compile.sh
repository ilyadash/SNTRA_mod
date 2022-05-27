g++ -g -Wl,--no-as-needed `root-config --cflags` `root-config --libs` -o SNTRA SNTRA.cpp
cp SNTRA ../
