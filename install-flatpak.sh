mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="$(flatpak info --show-location org.kde.kstars)/files/" ..
make
make install
cd ..

cd ; git clone https://github.com/tino/pyFirmata.git
flatpak run --command=sh org.kde.kstars -c "python3 -m ensurepip --user"
flatpak run --command=sh org.kde.kstars -c "cd ~/pyFirmata && python3 -m pip install --user ."
cd -

