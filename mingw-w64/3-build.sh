
source ./constants.sh

cd git/Tulospalvelu/gui && \
$HOST-qmake-qt4 PREFIX=$PREFIX INCLUDEPATH+=$PREFIX/include LIBS+=$PREFIX/lib/libQtSerialPort4.a && \
make -j5 && \
mkdir $PREFIX/bin -p && \
cp release/Tulospalvelu.exe $PREFIX/bin

