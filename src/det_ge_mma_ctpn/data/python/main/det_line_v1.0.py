# coding=utf-8

import os
import shutil
import sys
import time

import cv2
import numpy as np


sys.path.append("/home/tal/work/det_ge_mma_ctpn/data/python/main/")
sys.path.append("/home/tal/work/det_ge_mma_ctpn/data/python/")

import tensorflow as tf

from nets import model_train as model
from ctpn_utils.rpn_msr.proposal_layer import proposal_layer
from ctpn_utils.text_connector.detectors import TextDetector


#CHECKPOINT_PATH = "/home/tal/work/det_ge_mma_ctpn/data/python/models/"


def resize_image(img):
    img_size = img.shape
    im_size_min = np.min(img_size[0:2])
    im_size_max = np.max(img_size[0:2])
    # 短边/长边 对齐
    im_scale = float(600) / float(im_size_min)
    if np.round(im_scale * im_size_max) > 1200:
        im_scale = float(1200) / float(im_size_max)
    new_h = int(img_size[0] * im_scale)
    new_w = int(img_size[1] * im_scale)

    # 确保宽高均可被16整除
    new_h = new_h if new_h // 16 == 0 else (new_h // 16 + 1) * 16
    new_w = new_w if new_w // 16 == 0 else (new_w // 16 + 1) * 16

    re_im = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_LINEAR)
    return re_im, (new_h / img_size[0], new_w / img_size[1])


class PythonTextDetector:

    sess, input_image, input_im_info, bbox_pred, cls_pred, cls_prob = None, None, None, None, None, None

    def __init__(self):
        checkpoint_path = os.path.dirname(__file__) + "/../models/"
        tf.reset_default_graph()
        self.sess = tf.Session(config=tf.ConfigProto(allow_soft_placement=True))

        with tf.get_default_graph().as_default():
            self.input_image = tf.placeholder(tf.float32, shape=[None, None, None, 3], name='input_image')
            self.input_im_info = tf.placeholder(tf.float32, shape=[None, 3], name='input_im_info')

            global_step = tf.get_variable('global_step', [], initializer=tf.constant_initializer(0), trainable=False)

            self.bbox_pred, self.cls_pred, self.cls_prob = model.model(self.input_image)

            variable_averages = tf.train.ExponentialMovingAverage(0.997, global_step)
            saver = tf.train.Saver(variable_averages.variables_to_restore())

            ckpt_state = tf.train.get_checkpoint_state(checkpoint_path)
            model_path = os.path.join(checkpoint_path, os.path.basename(ckpt_state.model_checkpoint_path))
            print('Restore from {}'.format(model_path))
            saver.restore(self.sess, model_path)

    def detect_line(self, raw_data, h, w, c):
        raw_data = np.array(raw_data)
        raw_image = raw_data.reshape(h, w, c)

        img, (rh, rw) = resize_image(raw_image)
        h, w, c = img.shape
        im_info = np.array([h, w, c]).reshape([1, 3])
        bbox_pred_val, cls_prob_val = self.sess.run([self.bbox_pred, self.cls_prob],
                                               feed_dict={self.input_image: [img],
                                                          self.input_im_info: im_info})

        textsegs, _ = proposal_layer(cls_prob_val, bbox_pred_val, im_info)
        scores = textsegs[:, 0]
        textsegs = textsegs[:, 1:5]

        textdetector = TextDetector(DETECT_MODE='O')
        boxes = textdetector.detect(textsegs, scores[:, np.newaxis], img.shape[:2])
        boxes = np.array(boxes, dtype=np.int)
        #print(boxes)
        return [boxes, img, rh, rw]


if __name__ == '__main__':
    path = "/home/tal/work/det_ge_mma_ctpn/data/images/test.jpg"
    img = cv2.imread(path)
    shape = img.shape
    detector = PythonTextDetector()

    while True:
        detector.detect_line(img.flatten(), shape[0], shape[1], shape[2])
