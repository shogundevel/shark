
mkdir build

cd build

psp-cmake -DBUILD_PRX=1 -DENC_PRX=1 ..

make

cd ..

cp build/EBOOT.PBP EBOOT.PBP

