echo "BUILDING PHASE"
cmake -S . -B build -DCMAKE_BUILD_TYPE="Debug"
cd build
echo "COMPILING PHASE"
make
echo "COPYING EXECUTABLE"
cp bin/ComparerPlayground ../ComparerPlayground
