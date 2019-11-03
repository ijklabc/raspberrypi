#!/bin/bash
workdir=$(pwd)

git clone https://github.com/raspberrypi/firmware
git clone https://github.com/raspberrypi/linux.git -b rpi-4.19.y
wget https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/4.19/patch-4.19.72-rt25.patch.xz

cd linux
xacat ../patch-4.19.72-rt25.patch.xz | patch -p1
# check conflicts and then go on

ncpu=$(grep processor /proc/cpuinfo | wc -l)
make ARCH=arm64 CROSS_COMPILE=aarch64-unknown-linux-gnu- menuconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-unknown-linux-gnu- -j$ncpu Image modules dtbs

install_dir=/mnt/gentoo
mkdir -p $install_dir/boot
cp -r $workdir/firmware/boot/* $install_dir/boot
rm -f $install_dir/boot/kernel*

cp -v arch/arm64/boot/Image $install_dir/boot/kernel8.img
cp -v System.map $install_dir/boot/
cp -r arch/arm64/boot/dts/broadcom/bcm2711-rpi-4-b.dtb $install_dir/boot/
cp -r arch/arm64/boot/dts/overlays/ $install_dir/boot/
make ARCH=arm64 CROSS_COMPILE=aarch64-unknown-linux-gnu- modules_install INSTALL_MOD_PATH=$install_dir

version=$(ls --sort=time $install_dir/lib/modules | head -n 1)
echo $version
cp -v .config $install_dir/boot/config-$version
rm $install_dir/lib/modules/${version}/source
rm $install_dir/lib/modules/${version}/build
ln -vs /usr/src/linux $install_dir/lib/modules/${version}/source
ln -vs /usr/src/linux $install_dir/lib/modules/${version}/build

mkdir -p $workdir/releases
pkg=$workdir/releases/linux-rpi64-${version}-$(date  +%y%m%d%H%M%S).tar.gz
pushd $install_dir
	echo tar zcpf $pkg ...
	tar zcpf $pkg boot lib
popd
