#!/usr/bin/sh

VERSION="0.7.0"
REVISION="1"
ARCHITECTURE=`uname -r | sed 's,[^-]*-[^-]*-,,'`
BIN_DIR="../../binaries/Release"
INC_DIR="../../include/"
RES_DIR="../../resource/"
DAT_DIR="../../data/"
C3DDEV="Castor3D-dev"

echo "Building file system files"
mkdir -p ./Debian
cd Debian
mkdir -p ./Castor3D/DEBIAN
mkdir -p ./Castor3D/usr/lib/
mkdir -p ./ASSIMPImporter/DEBIAN
mkdir -p ./ASSIMPImporter/usr/lib/Castor3D/
mkdir -p ./PlyImporter/DEBIAN
mkdir -p ./PlyImporter/usr/lib/Castor3D/
mkdir -p ./LoopDivider/DEBIAN
mkdir -p ./LoopDivider/usr/lib/Castor3D/
mkdir -p ./PnTrianglesDivider/DEBIAN
mkdir -p ./PnTrianglesDivider/usr/lib/Castor3D/
mkdir -p ./GlRenderSystem/DEBIAN
mkdir -p ./GlRenderSystem/usr/lib/Castor3D/
mkdir -p ./CastorViewer/DEBIAN
mkdir -p ./CastorViewer/usr/bin
mkdir -p ./CastorViewer/usr/share/applications/
mkdir -p ./CastorViewer/usr/share/CastorViewer/languages
mkdir -p ./CastorViewer/usr/share/CastorViewer/gfx
mkdir -p ./CastorUtils/DEBIAN
mkdir -p ./CastorUtils/usr/include/CastorUtils
mkdir -p ./CastorUtils/usr/lib
mkdir -p ./GuiCommon/DEBIAN
mkdir -p ./GuiCommon/usr/include/GuiCommon
mkdir -p ./GuiCommon/usr/lib
mkdir -p ./Castor3D-dev/DEBIAN
mkdir -p ./Castor3D-dev/usr/include/Castor3D
echo "Retrieving built binaries"
cp $BIN_DIR/lib/Castor3D/libASSIMPImporter.so				./libASSIMPImporter/usr/lib/Castor3D/
cp $BIN_DIR/lib/Castor3D/libPlyImporter.so					./PlyImporter/usr/lib/Castor3D/
cp $BIN_DIR/lib/Castor3D/libLoopDivider.so					./LoopDivider/usr/lib/Castor3D/
cp $BIN_DIR/lib/Castor3D/libPnTrianglesDivider.so			./PnTrianglesDivider/usr/lib/Castor3D/
cp $BIN_DIR/lib/Castor3D/libGlRenderSystem.so				./GlRenderSystem/usr/lib/Castor3D/
cp $BIN_DIR/bin/libCastor3D*.so								./Castor3D/usr/lib/
cp $BIN_DIR/bin/CastorViewer								./CastorViewer/usr/bin/
cp $BIN_DIR/bin/languages/*.*								./CastorViewer/usr/share/CastorViewer/languages/
cp $DAT_DIR/Images/castor.png								./CastorViewer/usr/share/CastorViewer/gfx/
cp $RES_DIR/CastorViewer/applications/CastorViewer.desktop	./CastorViewer/usr/share/applications/
cp $BIN_DIR/lib/libGuiCommon.a	 							./GuiCommon/usr/lib/
cp $INC_DIR/GuiCommon/* 									./GuiCommon/usr/include/GuiCommon/
cp $INC_DIR/Castor3D/* 										./Castor3D-dev/usr/include/Castor3D/
cp $BIN_DIR/lib/libCastorUtils.a 							./CastorUtils/usr/lib/
cp $INC_DIR/CastorUtils/* 									./CastorUtils/usr/include/CastorUtils/
echo "Building debian packages"
for i in *
do
	if [ -d $i ];
	then
		echo "Package: `echo $i | tr '[A-Z]' '[a-z]'`\nVersion: $VERSION\nSection: graphics\nPriority: optional\nArchitecture: $ARCHITECTURE\nEssential: no\nInstalled-Size: `du -s $i | cut -f 1`\nDepends: `cat $RESOURCE/$i/dependencies`\nMaintainer: Sylvain Doremus (dragonjoker59@hotmail.com)\nProvides: $i\nDescription: `cat $RESOURCE/$i/description`\n" > ./$i/DEBIAN/control
		conv=`echo "$i" | tr '[A-Z]' '[a-z]'`
		dpkg-deb --build $i "$conv"_"$VERSION"-"$REVISION"_"$ARCHITECTURE".deb > /dev/null
	fi
done
echo "Building archive"
mkdir Packages
mv *.deb Packages
cd Packages
tar -czf Packages.tar.gz ./*
mv Packages.tar.gz ../../
cd ../../
rm -r -f ./Debian
