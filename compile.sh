g++ -g -Wl,--no-as-needed `root-config --cflags` `root-config --libs` -o SNTRA SNTRA_v2.cpp
cp SNTRA ../
