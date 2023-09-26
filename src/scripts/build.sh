
#!/bin/bash


TYPE=$(echo $1 | tr  '[A-Z]' '[a-z]')
if [ "$TYPE"x != "debug"x -a "$TYPE"x != "release"x ]
then
    	echo "build type either debug or release"
    exit
fi

if [ $# -ne 2 ]; then
    echo "wrong param num:"
    exit
fi

CON_DIR="cmake-build-"$TYPE

if [ -e $CON_DIR ] ;then
echo "the build Dir is not empty"

#rm -rf $CON_DIR/*

else
mkdir ./$CON_DIR

if [ $? -ne 0 ] ;then
echo "create build dir failed"
fi

fi


cd ./$CON_DIR
cmake  -DCMAKE_BUILD_TYPE=$TYPE -DPACKVERSION="$2" ../
make -j 4

if [ $? -ne 0 ]; then
exit

fi

if [ "$TYPE"x == "release"x ] ;then
#generate symbol file for binary
echo "generate symbol file for binary then strip"

for  program in `ls ../bin/release`

do
./../bin/release/dump_syms ../bin/release/$program > $program.sym
uuid=`head -n1 $program.sym | awk '{print $4}'`
prodir=`head -n1 $program.sym | awk '{print $5}'`
mkdir -p ../bin/release/symbols/$prodir/$uuid
mv $program.sym ../bin/release/symbols/$prodir/$uuid/$prodir.sym
strip ../bin/release/$program
done

for  program in `ls ../lib/release`
do
./../bin/release/dump_syms ../lib/release/$program > $program.sym
uuid=`head -n1 $program.sym | awk '{print $4}'`
prodir=`head -n1 $program.sym | awk '{print $5}'`
mkdir -p ../bin/release/symbols/$prodir/$uuid
mv $program.sym ../bin/release/symbols/$prodir/$uuid/$prodir.sym

strip ../lib/release/$program
done

mv ../bin/release/symbols ../symbols

make install

else
make install
fi

make clean

if [ $? -ne 0 ]; then
exit

fi


back_date=`date  "+%Y%m%d%H%M%S"`
pakname=GodeyeNoteImgCrow-$2-$back_date.tar.gz
symbolpakname=GodeyeNoteImgCrow-symbol-$2-$back_date.tar.gz
echo "package name is $pakname"
echo "symbolpakname name is $symbolpakname"

chmod -f a+x ../$pakname/*.sh

tar czvf ../$pakname -C ../package/ .

tar czvf ../$symbolpakname -C ../symbols/ .


#if [ -e ../$pakname ]; then
#    echo "upload to ftp server"
#    Date=`date "+%Y%m%d"`
#    host='ftp.ailab.100tal.com'
#    user='godeye'
#    passwd='o.7qKYtjJ0z.I'
#
#ftp -i -n $host <<EOF
#    user ${user} ${passwd}
#    binary
#    cd GodEye/GodeyeNoteImgCrow/C++
#    quote pasv
#    passive
#    put  ../$pakname  $pakname
#    mkdir  $Date
#    cd $Date
#    put ../$pakname  $pakname
#    put ../$symbolpakname $symbolpakname
#    quit
#EOF
#fi
