
source ./constants.sh


TARGET=$1

if [ -z "$TARGET" ]
then
  echo "Anna kohdehakemisto"
  exit
fi

mkdir -p $TARGET

FILES="bin/Tulospalvelu.exe bin/QtSerialPort4.dll"
DLLS="libgcc_s_sjlj-1.dll libstdc++-6.dll QtCore4.dll QtGui4.dll QtSql4.dll QtXml4.dll zlib1.dll libpng15-15.dll libsqlite3-0.dll"

for FILE in $FILES
do
  cp $PREFIX/$FILE $TARGET
done

for DLL in $DLLS
do
  cp $DEP_PREFIX/bin/$DLL $TARGET
done

mkdir -p $TARGET/sqldrivers
cp $DEP_PREFIX/lib/qt4/plugins/sqldrivers/qsqlite4.dll $TARGET/sqldrivers
