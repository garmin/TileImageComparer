# ComparerPlayground
This repo contains the code involved in the ComparerPlayground: a demo about OpenCV's capabilities and the techniques useful in image comparison.

## Build instructions
1. Clone the repo
```
git clone https://github.com/Navionics/TileImageComparer
```
2. Run the build script

```
cd TileImageComparer
./build.sh
```
4. Run the compiled binary
```
./ComparerPlayground -f1 dataset/7/1.png -f2 dataset/7/1_w.png --algorithm mse
```

To run the benchmark, run the script called `benchmark.sh` specifying the algorithm (e.g. `./benchmark.sh ssim`) 

## Example
![result](https://github.com/Navionics/TileImageComparer/assets/108278872/09d5caed-5e27-422c-bd08-eadd8a0d8c63)
