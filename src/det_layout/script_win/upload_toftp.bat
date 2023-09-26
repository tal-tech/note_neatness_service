@echo off

::1-获取属性信息-manifest.ini
CALL readconfig ..\manifest.ini version vnumber
set "vnumber=%vnumber: =%"
CALL readconfig ..\manifest.ini id curdir
set "curdir=%curdir: =%"

::2-压缩文件夹
set rfloder=%curdir%_%vnumber%
cd ..
cd release
..\script_win\WinRAR.exe a %rfloder%.zip %rfloder% -r

rd /q/s temp
mkdir temp
cd temp
::3-通过下载文件判断 ftp上文件是否存在
echo open 10.1.2.202 21>> temp.txt
echo webmaster>> temp.txt
echo facethink2016>> temp.txt
echo cd AILab>> temp.txt
echo cd AI_Engine>> temp.txt
echo mkdir %curdir%>> temp.txt
echo cd %curdir%>> temp.txt
echo mkdir release>> temp.txt
echo cd release>> temp.txt
echo mkdir Win64>> temp.txt
echo cd Win64>> temp.txt
echo bin >> temp.txt
echo get %rfloder%.zip >> temp.txt
echo bye >> temp.txt
ftp -i -s:temp.txt
del /q temp.txt

if exist %rfloder%.zip (
  cd ..
  echo %rfloder%.zip is already exit
  echo upload %rfloder%.zip failed!!!
  goto end
)
cd ..

::4-上传文件夹
echo open 10.1.2.202 21>> temp.txt
echo webmaster>> temp.txt
echo facethink2016>> temp.txt
echo cd AILab>> temp.txt
echo cd AI_Engine>> temp.txt
echo mkdir %curdir%>> temp.txt
echo cd %curdir%>> temp.txt
echo mkdir release>> temp.txt
echo cd release>> temp.txt
echo mkdir Win64>> temp.txt
echo cd Win64>> temp.txt
echo bin >> temp.txt
echo put %rfloder%.zip >> temp.txt
echo bye >> temp.txt
ftp -i -s:temp.txt
del /q temp.txt

:end

cd ..
rd /q/s release

pause