# text-detection-ctpn

## download model
Model saved in FTP server.

Model Directory: /home/disk3/ftp/AILab/AI_Models_SDK/det_ge_mma_ctpn

## setup
nms and bbox utils are written in cython, hence you have to build the library first.
```shell
cd ctpn_utils/bbox
chmod +x make.sh
./make.sh
```
It will generate a nms.so and a bbox.so in current folder.

## interface
```python
det_line_v1.0.py
def detect_line(raw_image):
    """
    xxx
    """
    return boxes,img,(rh,rw)
# boxes - 文本检测的矩形框
# img - resize之后的图片
# (rh,rw) - resize之后的高/宽
```
