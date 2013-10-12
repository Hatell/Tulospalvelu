
source ./constants.sh

mkdir prebuild/qtserialport-build/include/QtSerialPort -p && \
cd prebuild/qtserialport-build && \
$HOST-qmake-qt4 PREFIX=$PREFIX ../../git/qtserialport/qtserialport.pro && \
make -j5 && \
mkdir -p $PREFIX/include && \
mkdir -p $PREFIX/lib && \
mkdir -p $PREFIX/bin && \
cp -r include/QtSerialPort $PREFIX/include && \
cp src/serialport/release/QtSerialPort4.dll $PREFIX/bin && \
cp src/serialport/release/libQtSerialPort4.a $PREFIX/lib

