
imgFolder=bad_test

imglist=`ls $imgFolder`

for img in $imglist
do
 	echo ${imgFolder}/$img '0'>>list.txt

done
