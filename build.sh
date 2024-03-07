mkdir -p build && cd build
cmake ..
make
make test
sudo make install
