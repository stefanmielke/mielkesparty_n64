docker build -t libdragon-build-local .
docker run -it --rm -v$(pwd):/libdragon/homebrew libdragon-build-local make clean -C /libdragon/homebrew/libs/libdragon
docker run -it --rm -v$(pwd):/libdragon/homebrew libdragon-build-local make all -C /libdragon/homebrew/libs/libdragon -j