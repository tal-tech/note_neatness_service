 - sudo apt-get install golang-go
 - sudo apt-get install libglib2.0-dev
 - sudo apt-get install libopencv-dev
 - sudo apt-get install libboost-dev libboost-filesystem-dev libboost-system-dev libboost-log-dev
 - sudo apt-get install libopenblas-dev python3-tk

```
升级系统
    sudo apt-get update
    sudo apt-get upgrade
    sudo updatemanager -c -d
```

 - 安装nvidia驱动
```
    手动安装
    https://www.nvidia.cn/Download/index.aspx?lang=cn
    选定相应显卡，下载驱动程序
    禁用nouveau第三方驱动
    打开编辑配置文件： sudo gedit /etc/modprobe.d/blacklist.conf
    在最后一行添加：blacklist nouveau
    改好后执行命令：sudo update-initramfs -u
    重启使之生效：reboot
    进入recover模式，使用root用户安装驱动程序
    sudo chmod a+x NVIDIA-Linux-x86_64-415.18.run     (下载的驱动文件名）
    ./NVIDIA-Linux-x86_64-415.18.run -no-opengl-files     注意安装时提示
    
    命令安装
    sudo add-apt-repository ppa:graphics-drivers/ppa
    sudo apt-get install nvidia-driver-415
    
    检测驱动安装状态：
    nvidia-smi
```
 - 安装cuda10
```
    https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&target_distro=Ubuntu&target_version=1804&target_type=runfilelocal
    wget https://developer.nvidia.com/compute/cuda/10.0/Prod/local_installers/cuda_10.0.130_410.48_linux
    下载完成后，输入sudo sh cuda_10.0.130_410.48_linux.run安装，然后根据提示一步一步安装，注意不要安装它自带的显卡驱动。
    sudo vim /etc/profile
    在文件末尾添加以下两行。
    export PATH=/usr/local/cuda-10.0/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/cuda-10.0/lib64:$LD_LIBRARY_PATH
    
    source /etc/profile
    
    验证cuda toolkit：
    nvcc -V
```

 - 安装cudnn
```
    sudo dpkg -i libcudnn7_7.4.1.5-1+cuda10.0_amd64.deb
    sudo dpkg -i libcudnn7-dev_7.4.1.5-1+cuda10.0_amd64.deb
```

 - 源码使用说明
进入src目录下：
   - 编译环境的设置  sudo scripts/requirments.sh
   - 编译        sh scripts/build.sh release 1.0.0.0(会自动上传至ftp)
   - 运行        在bin目录下执行./GodeyeCrowVideo --setting=config/config.ini port


- 安装算法依赖

```
    tqdm==4.11.2
    opencv-python==3.4.0.12
    Shapely==1.6.4.post1
    matplotlib==1.5.3
    tensorflow-gpu==1.13.1
    Cython==0.24.1
    ipython==5.1.0

    以上写入文件re.txt
    执行pip3 install -r re.txt
```
    -更新pandas模块：
        pip3 install -U pandas