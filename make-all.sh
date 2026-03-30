#! /bin/bash

set -e

rm -rf dist
mkdir dist

if [ -d /opt/codex/rm1/5.2.96-dirty ]; then
  echo "Building for reMarkable 1..."
  rm -rf build-rm1
  mkdir build-rm1
  cd build-rm1
  echo " | Configuring CMake..."
  . /opt/codex/rm1/5.2.96-dirty/environment-setup-cortexa9hf-neon-remarkable-linux-gnueabi
  export CMAKE_FILE=/opt/codex/rm1/5.2.96-dirty/sysroots/x86_64-codexsdk-linux/usr/share/cmake/cortexa9hf-neon-remarkable-linux-gnueabi-toolchain.cmake
  cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE="$CMAKE_FILE" -DCMAKE_BUILD_TYPE=Release > /dev/null
  echo " | Building..."
  cmake --build . > /dev/null
  cd dist/vnsee
  chmod +x vnsee
  cd ..
  echo " | Creating distribution..."
  zip -rq vnsee-rm1.zip .
  mv vnsee-rm1.zip ../../dist/
  cd ../..
  echo " | Done!"
else
  echo "rM1 5.2.96 toolchain not found, skipping..."
fi

echo

if [ -d /opt/codex/rm2/5.2.96-dirty ]; then
  echo "Building for reMarkable 2..."
  rm -rf build-rm2
  mkdir build-rm2
  cd build-rm2
  echo " | Configuring CMake..."
  . /opt/codex/rm2/5.2.96-dirty/environment-setup-cortexa7hf-neon-remarkable-linux-gnueabi
  export CMAKE_FILE=/opt/codex/rm2/5.2.96-dirty/sysroots/x86_64-codexsdk-linux/usr/share/cmake/cortexa7hf-neon-remarkable-linux-gnueabi-toolchain.cmake
  cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE="$CMAKE_FILE" -DCMAKE_BUILD_TYPE=Release > /dev/null
  echo " | Building..."
  cmake --build . > /dev/null
  cd dist/vnsee
  chmod +x vnsee
  cd ..
  echo " | Creating distribution..."
  zip -rq vnsee-rm2.zip .
  mv vnsee-rm2.zip ../../dist/
  cd ../..
  echo " | Done!"
else
  echo "rM2 5.2.96 toolchain not found, skipping..."
fi

echo

if [ -d /opt/codex/ferrari/5.2.96-dirty ]; then
  echo "Building for reMarkable Paper Pro..."
  rm -rf build-ferrari
  mkdir build-ferrari
  cd build-ferrari
  echo " | Configuring CMake..."
  . /opt/codex/ferrari/5.2.96-dirty/environment-setup-cortexa53-crypto-remarkable-linux
  export CMAKE_FILE=/opt/codex/ferrari/5.2.96-dirty/sysroots/x86_64-codexsdk-linux/usr/share/cmake/cortexa53-crypto-remarkable-linux-toolchain.cmake
  cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE="$CMAKE_FILE" -DCMAKE_BUILD_TYPE=Release > /dev/null
  echo " | Building..."
  cmake --build . > /dev/null
  cd dist/vnsee
  chmod +x vnsee
  cd ..
  echo " | Creating distribution..."
  zip -rq vnsee-ferrari.zip .
  mv vnsee-ferrari.zip ../../dist/
  cd ../..
  echo " | Done!"
else
  echo "Ferrari 5.2.96 toolchain not found, skipping..."
fi

echo

if [ -d /opt/codex/chiappa/5.2.96-dirty ]; then
  echo "Building for reMarkable Paper Pro Move..."
  rm -rf build-chiappa
  mkdir build-chiappa
  cd build-chiappa
  echo " | Configuring CMake..."
  . /opt/codex/chiappa/5.2.96-dirty/environment-setup-cortexa55-remarkable-linux
  export CMAKE_FILE=/opt/codex/chiappa/5.2.96-dirty/sysroots/x86_64-codexsdk-linux/usr/share/cmake/cortexa55-remarkable-linux-toolchain.cmake
  cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE="$CMAKE_FILE" -DCMAKE_BUILD_TYPE=Release > /dev/null
  echo " | Building..."
  cmake --build . > /dev/null
  cd dist/vnsee
  chmod +x vnsee
  cd ..
  echo " | Creating distribution..."
  zip -rq vnsee-chiappa.zip .
  mv vnsee-chiappa.zip ../../dist/
  cd ../..
  echo " | Done!"
else
  echo "Chiappa 5.2.96 toolchain not found, skipping..."
fi
