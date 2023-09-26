# coding=utf-8

import os
import shutil
import sys
import time
from os import path
import cv2
import numpy as np

parent_path = os.path.dirname(__file__)
sys.path.append(parent_path)
sys.path.append(os.path.dirname(parent_path))

import tensorflow as tf
import det_layout
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

    SHORT_TAG = 20

    def __init__(self):
        checkpoint_path = os.path.join(os.path.dirname(__file__), "../models/")
        print("checkpoint path:", checkpoint_path)
        tf.reset_default_graph()
        config = tf.ConfigProto()
        config.gpu_options.per_process_gpu_memory_fraction = 0.5
        config.allow_soft_placement=True
        self.sess = tf.Session(config=config)
        self.textdetector = TextDetector(DETECT_MODE='O')
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
        raw_image = raw_image.astype('uint8')
        img, (rh, rw) = resize_image(raw_image)

        h, w, c = img.shape
        im_info = np.array([h, w, c]).reshape([1, 3])
        bbox_pred_val, cls_prob_val = self.sess.run([self.bbox_pred, self.cls_prob],
                                               feed_dict={self.input_image: [img],
                                                          self.input_im_info: im_info})

        textsegs, _ = proposal_layer(cls_prob_val, bbox_pred_val, im_info)
        scores = textsegs[:, 0]
        textsegs = textsegs[:, 1:5]


        boxes = self.textdetector.detect(textsegs, scores[:, np.newaxis], img.shape[:2])
        boxes = np.array(boxes, dtype=np.int)
        #print(boxes)
        return [boxes, img, rh, rw]

    # 转换文本检测框的表达形式
    def box_transfer(self,line_boxes):
        tmp_boxes = []
        for i in range(line_boxes.shape[0]):
            left_p_x = (line_boxes[i][0]+line_boxes[i][6])/2
            right_p_x = (line_boxes[i][2]+line_boxes[i][4])/2
            left_p_y = (line_boxes[i][1]+line_boxes[i][7])/2
            right_p_y = (line_boxes[i][3]+line_boxes[i][5])/2
            angle = 0.0
            height = ((line_boxes[i][7]-line_boxes[i][1])+(line_boxes[i][5]-line_boxes[i][3]))/2.0
            width = ((line_boxes[i][2]-line_boxes[i][0])+(line_boxes[i][4]-line_boxes[i][6]))/2.0
            tmp_boxes.append([left_p_x,left_p_y,right_p_x,right_p_y,width,height,angle])
        return tmp_boxes

    # 将垂直书写的一级标题进行融合
    def first_tag_merge(self,boxes):
        if len(boxes) == 0:
            return boxes
        
        boxes_np = np.array(boxes)
        # print(boxes_np.shape)
        center_x = (boxes_np[:,4] + boxes_np[:,0])/2.0
        center_y = (boxes_np[:,5] + boxes_np[:,1])/2.0
        
        mean_x = np.mean(center_x)
        mean_y = np.mean(center_y)
        
        height_mean = np.mean(boxes_np[:,5]-boxes_np[:,1])
        width_mean = np.mean(boxes_np[:,4]-boxes_np[:,0])
        
        sort_idx = np.argsort(boxes_np[:,1])
        
        result_boxes = [[boxes_np[sort_idx[0]]]]
        for i in range(boxes_np.shape[0]-1):
            tmp_result = result_boxes[-1]
            # print("center_y",center_y[sort_idx[i+1]], center_y[sort_idx[i]])
            if center_y[sort_idx[i+1]] - center_y[sort_idx[i]] < height_mean * 1.5:
                if len(tmp_result) == 0:
                    result_boxes[-1].append(boxes_np[sort_idx[i]])
                    result_boxes[-1].append(boxes_np[sort_idx[i+1]])
                elif (result_boxes[-1][-1] == boxes_np[sort_idx[i]]).all():
                    result_boxes[-1].append(boxes_np[sort_idx[i+1]])
                else:
                    tmp_result = [boxes_np[sort_idx[i]],boxes_np[sort_idx[i+1]]]
                    result_boxes.append(tmp_result)
        
        final_result = []
        for i in range(len(result_boxes)):
            tmp_box_np = np.array(result_boxes[i])
            tmp_x1 = np.min(tmp_box_np[:,0])
            tmp_y1 = np.min(tmp_box_np[:,1])
            
            tmp_x2 = np.max(tmp_box_np[:,2])
            tmp_y2 = np.min(tmp_box_np[:,3])
            
            tmp_x3 = np.max(tmp_box_np[:,4])
            tmp_y3 = np.max(tmp_box_np[:,5])
            
            tmp_x4 = np.min(tmp_box_np[:,6])
            tmp_y4 = np.max(tmp_box_np[:,7])
            
            tmp_box = np.array([tmp_x1,tmp_y1,tmp_x2,tmp_y2,tmp_x3,tmp_y3,tmp_x4,tmp_y4,0])
            final_result.append(tmp_box)
        return final_result

    # 一级/二级/三级标签分析
    def tag_analyst(self,image,line_boxes):
        
        # 将检测框转换成（左顶点，右顶点，宽，高，角度）信息
        line_det_result = self.box_transfer(line_boxes)
        h,w,_ = image.shape
        
        # 按照x轴计算每个坐标位置在垂直方向上包含多少个文本框
        tag_result = np.zeros((w))
        for i in range(w):
            tmp_count = 0
            for j in range(len(line_det_result)):
                tmp_left_x = line_det_result[j][0]
                tmp_right_x = line_det_result[j][2]
                if i>tmp_left_x and i<tmp_right_x:
                    tmp_count += 1
            tag_result[i] = tmp_count
        tmp_tag = np.array(tag_result)

        # 将具有相同数量的区间集合在一起
        tag_dict = {}
        count = 1
        while np.max(tmp_tag) > 0:
            tmp_tag -= 1.0
            z_idx = np.where(tmp_tag==0.0)
            if z_idx[0].shape[0] > 0:
                tag_dict[count] = z_idx[0]
            count += 1
        
        # 将连接并且长度大于SHORT_TAG的区域清洗出来作为最终tag分布的结果
        tag = {}
        count = 1
        for t,coord in tag_dict.items():
            tmp_left_idx = 0
            tmp_right_idx = 0
            tag_left_idx = 0
            tag_right_idx = 0
            tmp_len = 0
            max_len = 0
            # 如果区间单调递增，则进行选取
            for m in range(coord.shape[0]-1):
                if (coord[m+1] - coord[m]) == 1:
                    tmp_right_idx = m+1
                else:
                    # print(tmp_right_idx, tmp_left_idx)
                    tmp_len = tmp_right_idx - tmp_left_idx
                    if tmp_len > self.SHORT_TAG:
                        tag_left_idx = tmp_left_idx
                        tag_right_idx = tmp_right_idx
                        tag[count] = [coord[tag_left_idx],coord[tag_right_idx],tmp_len]
                        count += 1
                    tmp_left_idx = tmp_right_idx
            # 最后一组结果进行判断
            if tmp_left_idx != tmp_right_idx:
                tmp_len = tmp_right_idx - tmp_left_idx
                if tmp_len > self.SHORT_TAG:
                    tag_left_idx = tmp_left_idx
                    tag_right_idx = tmp_right_idx
                    # max_len = tmp_len
                    tag[count] = [coord[tag_left_idx],coord[tag_right_idx],tmp_len]
                    count += 1
        
        # 将获得到的区间按照x轴方向进行排序
        sorted_tag = sorted(tag.items(), key=lambda d: ((d[1][0]+d[1][1])/2.0,d[1][0]))
        # 构建返回结果
        final_boxes = {}
        final_boxes["1"] = []
        final_boxes["2"] = []
        final_boxes["others"] = []
        
        # 将页面按照w划分成三个区域，判断思维导图的书写方式是否从左往右书写。
        # 若三个区域文本框数量从左往右递增，则认为是从左往右书写。
        # 反之，如果三个区域分布符合大->小->大，或者其他情况，则认为不是从左往右书写。
        width_div = [0,w//3,w*2//3,w]
        lr_result = []
        for w_idx in range(len(width_div)-1):
            t_left = width_div[w_idx]
            t_right = width_div[w_idx+1]
            tmp_lr = 0
            for t_idx in range(len(line_det_result)):
                t_c = (line_det_result[t_idx][0]+line_det_result[t_idx][2])/2.0
                if t_c > t_left and t_c < t_right:
                    tmp_lr += 1
            lr_result.append(tmp_lr)
        ltor = False
        if lr_result[1] < lr_result[0] and lr_result[1] < lr_result[2]:
            ltor = False
        elif (lr_result[1]+lr_result[2] > lr_result[0]):# or (lr_result[0] + lr_result[1] < lr_result[2]):
            ltor = True
        
        # 如果只找到一级标题的区间，则直接返回结果，仅包含一级标题
        b = 0
        if len(sorted_tag) < 2:
            final_boxes["1"].append(line_boxes[b])
            return tag,final_boxes,ltor
        # 遍历文本框，将所有文本框划归其属于的区间，确定文本框属于哪一个标题区间
        while b < len(line_det_result):
            # 确定属于一级标题
            if line_det_result[b][0] >= sorted_tag[0][1][0]-21 and line_det_result[b][0] <= sorted_tag[0][1][1]:
                tmp_b = line_boxes[b].copy()
                final_boxes["1"].append(tmp_b)
            # 确定属于二级标题
            elif line_det_result[b][0] >= sorted_tag[1][1][0]-21 and line_det_result[b][0] <= sorted_tag[1][1][1]:
                tmp_b = line_boxes[b].copy()
                final_boxes["2"].append(tmp_b)
            # 剩余为其他标题
            else:
                final_boxes["others"].append(line_boxes[b])
            b+=1
        # 将属于一级标题的垂直文本框进行融合
        final_boxes["1"] = self.first_tag_merge(final_boxes["1"])
        
        return tag,final_boxes,ltor
    
    # 计算文本检测结果的行间距
    # 计算逻辑为每个文本框找到y轴上向下离它最近的文本框，计算行间距
    # 统计所有行间距的均值
    def get_interval_stats(self,bboxes):
        bboxes_np = np.array(bboxes)
        
        upper = (bboxes_np[:,1]+bboxes_np[:,3])/2.0
        bottom = (bboxes_np[:,5]+bboxes_np[:,7])/2.0
        
        center_y = (bboxes_np[:,5] + bboxes_np[:,1])/2.0
        
        y_idx = np.argsort(center_y)
        
        interval = []
        
        for i in range(y_idx.shape[0]-1):
            tmp_interval = np.abs(bottom[y_idx[i]] - upper[y_idx[i+1]])
            interval.append(tmp_interval)

        if len(interval) == 0:
            interval_mean = 0.0
            interval_std = 0.0
        else:
            interval_mean = np.mean(interval)
            interval_std = np.std(interval)
        # if only one box detected, the result is nan.
        # interval_mean = np.mean(interval)
        # interval_std = np.std(interval)
        
        return interval_mean,interval_std

    # Core code.
    # 统计所有结果，进行输出.
    def analyst(self, image, h, w, c):
        # raw return result
        test_final_boxes = {}
        test_final_boxes["1"] = []
        test_final_boxes["2"] = []
        test_final_boxes["others"] = []

        # CTPN检测文本框
        test_boxes,test_img,rh,rw = self.detect_line(image,h, w, c)

        # 传统算法返回检测结果
        result,score,overall_box = det_layout.det_layout(test_img)
        # 如果没有检测框，则直接返回结果
        if test_boxes.shape[0] == 0:
            return (score[0],score[1],0.0,0,False,0)

        # 计算行间距
        interval_mean,interval_std = self.get_interval_stats(test_boxes)
        # 计算所有文本框的下边界y轴均值
        tmp_test_box = (test_boxes[:,5]+test_boxes[:,7])/2.0
        # 找到最下方的文本框得到文本检测的底线值
        tmp_y_base = max(test_boxes[np.argmax(tmp_test_box),5],test_boxes[np.argmax(tmp_test_box),7])
        # 找到所有笔画的底线值
        canvas_base = overall_box[1][1]
        # 认为以上两个底线值的差为大括号剩余区域
        white_space_length = canvas_base - tmp_y_base

        # 分析思维导图分级标题
        tmp_tag, tmp_final_boxes,ltor = self.tag_analyst(test_img,test_boxes.copy())

        # 返回分析结果
        # area_p, relative_p, interval_mean, white_space_length, l2r, 3rdTag
        return (score[0],score[1],interval_mean,white_space_length,ltor,len(tmp_final_boxes["others"]))

if __name__ == '__main__':
    # path = "/home/liuxin/note/note_neatness_service/src/bin/debug/data/images/test.jpg"
    path = "../images/test.jpg"
    # path = "/home/tal-c/Src/text-detection-ctpn/data/MindMap/note_148691_210_57773_01_1.jpg"
    img = cv2.imread(path)
    shape = img.shape
    detector = PythonTextDetector()

    while True:
        a = time.time()
        print(type(a))
        tmp_result = detector.analyst(img.flatten(), shape[0], shape[1], shape[2])
        print(tmp_result)
        b = time.time() - a
        print ("耗时：%f" % b)
