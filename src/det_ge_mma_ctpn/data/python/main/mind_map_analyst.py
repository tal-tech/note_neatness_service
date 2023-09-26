import os
import shutil
import sys
import time

import cv2
import numpy as np
import tensorflow as tf

sys.path.append(os.getcwd())
from nets import model_train as model
from ctpn_utils.rpn_msr.proposal_layer import proposal_layer
from ctpn_utils.text_connector.detectors import TextDetector


### CONFIG
CHECKPOINT_PATH = ""

class Mind_Map_Analyst:

    def __init__(self):
        pass

    
    def process(self,image):
        pass

    
    def tag_analyst(self,image,lines):
        w,h = image.shape[0],image.shape[1]

        

        pass


    @staticmethod
    def resize_image(img):
        img_size = img.shape
        im_size_min = np.min(img_size[0:2])
        im_size_max = np.max(img_size[0:2])

        im_scale = float(600) / float(im_size_min)
        if np.round(im_scale * im_size_max) > 1200:
            im_scale = float(1200) / float(im_size_max)
        new_h = int(img_size[0] * im_scale)
        new_w = int(img_size[1] * im_scale)

        new_h = new_h if new_h // 16 == 0 else (new_h // 16 + 1) * 16
        new_w = new_w if new_w // 16 == 0 else (new_w // 16 + 1) * 16

        re_im = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_LINEAR)
        return re_im, (new_h / img_size[0], new_w / img_size[1])

    
    def detect_line(self,raw_image):

        with tf.get_default_graph().as_default():
            input_image = tf.placeholder(tf.float32, shape=[None, None, None, 3], name='input_image')
            input_im_info = tf.placeholder(tf.float32, shape=[None, 3], name='input_im_info')

            global_step = tf.get_variable('global_step', [], initializer=tf.constant_initializer(0), trainable=False)

            bbox_pred, cls_pred, cls_prob = model.model(input_image)

            variable_averages = tf.train.ExponentialMovingAverage(0.997, global_step)
            saver = tf.train.Saver(variable_averages.variables_to_restore())

            with tf.Session(config=tf.ConfigProto(allow_soft_placement=True)) as sess:
                ckpt_state = tf.train.get_checkpoint_state(CHECKPOINT_PATH)
                model_path = os.path.join(CHECKPOINT_PATH, os.path.basename(ckpt_state.model_checkpoint_path))
                print('Restore from {}'.format(model_path))
                saver.restore(sess, model_path)


                img, (rh, rw) = Mind_Map_Analyst.resize_image(raw_image)
                h, w, c = img.shape
                im_info = np.array([h, w, c]).reshape([1, 3])
                bbox_pred_val, cls_prob_val = sess.run([bbox_pred, cls_prob],
                                                    feed_dict={input_image: [img],
                                                                input_im_info: im_info})

                textsegs, _ = proposal_layer(cls_prob_val, bbox_pred_val, im_info)
                scores = textsegs[:, 0]
                textsegs = textsegs[:, 1:5]

                textdetector = TextDetector(DETECT_MODE='O')
                boxes = textdetector.detect(textsegs, scores[:, np.newaxis], img.shape[:2])
                boxes = np.array(boxes, dtype=np.int)

                ### Viz part
                # cost_time = (time.time() - start)
                # print("cost time: {:.2f}s".format(cost_time))

                # for i, box in enumerate(boxes):
                #     cv2.polylines(img, [box[:8].astype(np.int32).reshape((-1, 1, 2))], True, color=(0, 255, 0),
                #                 thickness=2)
                # img = cv2.resize(img, None, None, fx=1.0 / rh, fy=1.0 / rw, interpolation=cv2.INTER_LINEAR)
                # cv2.imwrite(os.path.join(FLAGS.output_path, os.path.basename(im_fn)), img[:, :, ::-1])

                # with open(os.path.join(FLAGS.output_path, os.path.splitext(os.path.basename(im_fn))[0]) + ".txt",
                #         "w") as f:
                #     for i, box in enumerate(boxes):
                #         line = ",".join(str(box[k]) for k in range(8))
                #         line += "," + str(scores[i]) + "\r\n"
                #         f.writelines(line)
                ###

        return boxes
                
    @staticmethod
    def get_layout_result(input_image):
        # 思维导图分析中主要提取留白面积
        gray_img = cv2.cvtColor(input_image,cv2.COLOR_BGR2GRAY)
        # 图像二值化 cv::threshold
        ret,bin_img = cv2.threshold(gray_img,127,255,cv2.THRESH_BINARY_INV)
        # 此处不需要做腐蚀膨胀
        # tmp_result = dilated_process(bin_img)
        _, labels, bboxes, centroids = cv2.connectedComponentsWithStats(bin_img)
        bboxes = bboxes[1::]
        # 由于是二值图 计算全局占比的时候直接加和即可
        text_part = np.sum(input_image/255.0)
        background_part = input_image.shape[0]*input_image.shape[1]
        op = text_part/background_part
        # relative propotion
        # 找到所有文本块的边界
        if bboxes.shape[0] > 0:
            top_left = np.min(bboxes[:,:2],axis=0)
            bottom_right = np.max(bboxes[:,:2]+bboxes[:,2:4],axis=0)
            relative_bg_part = (bottom_right[0] - top_left[0])*(bottom_right[1]-top_left[1])
            rp = text_part/relative_bg_part
        else:
            top_left = [0.0,0.0]
            bottom_right = [0.0,0.0]
            rp = 0.0
        # 相对占比＋绝对占比＋裁剪框
        return op,rp,(top_left,bottom_right)

    
# self test
if __name__ == "__main__":
    pass