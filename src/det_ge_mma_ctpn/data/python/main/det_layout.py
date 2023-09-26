import sys
import os
import cv2
import numpy as np
# from matplotlib import pyplot as plt
# import seaborn as sns
# import pandas as pd


# 腐蚀膨胀 行串联
def dilated_process(image):
#     element1 = cv2.getStructuringElement(cv2.MORPH_CROSS,(2,2))
#     eroded = cv2.erode(image,element1)  
    element1 = cv2.getStructuringElement(cv2.MORPH_CROSS,(4,2))
    dilated = cv2.dilate(image,element1)    
    return dilated


# 文本块检测框水平链接
def connector(image,bboxes):
    # bboxes = [x,y,w,h]
    # 获取左右边界中点
    # print(bboxes.shape)
    top_left_y = bboxes[:,1]
    tmp_y_sort = top_left_y.argsort()
    tmp_bboxes = np.zeros(bboxes.shape)
    for ti in range(tmp_y_sort.shape[0]):
        tmp_bboxes[ti] = bboxes[tmp_y_sort[ti]]
    bboxes = tmp_bboxes
    box_center_x = bboxes[:,0]+bboxes[:,2]/2.0
    box_center_y = bboxes[:,1]+bboxes[:,3]/2.0
    left_center_x = bboxes[:,0]
    right_center_x = bboxes[:,0]+bboxes[:,2]
    
    # sort_seq = np.array([box_center_y,top_left_y],dtype=[('x','float'), ('y', 'float')])
    # print(sort_seq.shape)
    # 计算宽度均值 以及垂直偏移量
    width_mean = np.mean(bboxes[:,3])
    target_height = 0.5 * width_mean
    # print(width_mean,target_height)
    

    # 按照y轴排序 进行链接
    # y_sort = box_center_y.argsort()
    y_sort = np.lexsort((bboxes[:,1],bboxes[:,1]+bboxes[:,3]/2.0))
    # y_sort = np.argsort(sort_seq,order=('x','y')) # 排序角标
    # print(y_sort)
    # for ti in range(y_sort.shape[0]):
    #     print(bboxes[y_sort[ti]],box_center_y[y_sort[ti]])
    line_group = []
    i = 0
    while i < y_sort.shape[0]:
        tmp_group = set()
        tmp_left_center_x = left_center_x[y_sort[i]]
        tmp_right_center_x = right_center_x[y_sort[i]]
        for j in range(y_sort.shape[0]):
            # 竖直方向距离小于width_mean
            if np.abs(box_center_y[y_sort[j]] - box_center_y[y_sort[i]]) < width_mean:
                # 左&右 or 右&左 水平方向偏移小于target_height
                # print(bboxes[y_sort[i]],bboxes[y_sort[j]],
                # np.abs(tmp_left_center_x - right_center_x[y_sort[j]]),
                # np.abs(left_center_x[y_sort[j]] - tmp_right_center_x))
                if (np.abs(tmp_left_center_x - right_center_x[y_sort[j]]) < target_height 
                    or np.abs(left_center_x[y_sort[j]] - tmp_right_center_x) < target_height):
                    # print(bboxes[y_sort[i]],bboxes[y_sort[j]])
                    # 组成新的文本块
                    tmp_group.add(y_sort[i])
                    tmp_group.add(y_sort[j])
                    # 计算新文本块的左右中心点
                    tmp_left_center_x = np.minimum(tmp_left_center_x,left_center_x[y_sort[j]])
                    tmp_right_center_x = np.maximum(tmp_right_center_x,right_center_x[y_sort[j]])
        # print()
        if len(tmp_group) > 0:
            line_group.append(tmp_group.copy())
        i += 1
    
    # 融合新的文本块
    new_line_boxes = [] #
    new_reg_line_boxes = []
    for m in range(len(line_group)):
        new_boxes = []
        # 把group中的文本块弹出
        # for n in range(len(line_group[m])):
        #     new_boxes.append(bboxes[line_group[m].pop()])
        while len(line_group[m]) > 0:
            new_boxes.append(bboxes[line_group[m].pop()])
        # for t_box in new_boxes:
        #     print(t_box,)
        # print()
        line_box = np.array(new_boxes)
        top_left = np.min(line_box[:,:2],axis=0)
        # 寻找文本块左上角和右下角点坐标
        bottom_right = np.max(line_box[:,:2]+line_box[:,2:4],axis=0)
        new_line_boxes.append((top_left,bottom_right))
        # 把坐标按照[x,y,w,h]的方式保存
        new_reg_line_boxes.append([top_left[0],top_left[1],bottom_right[0]-top_left[0],bottom_right[1]-top_left[1]])
        
    # 返回结果
    return new_line_boxes,np.array(new_reg_line_boxes)


# ! 此处暂时不需要实现
def calculate_angle(image,bbox):
    c_img = image[bbox[0]:bbox[0]+bbox[2],bbox[1]:bbox[1]+bbox[3]] / 255.
    
    angles = range(0,180,5)
    center = [bbox[0]+bbox[2]/2,bbox[1]+bbox[3]/2]
    width = bbox[2]
    height = bbox[3]
    
    project = []
    if c_img.shape[0] == 0 or c_img.shape[1] == 0:
        return 0
    for theta in angles:
    
        theta *= np.pi / 180 # convert to rad

        v_x = (np.cos(theta), np.sin(theta))
        v_y = (-np.sin(theta), np.cos(theta))
        s_x = center[0] - v_x[0] * (width / 2) - v_y[0] * (height / 2)
        s_y = center[1] - v_x[1] * (width / 2) - v_y[1] * (height / 2)

        mapping = np.array([[v_x[0],v_y[0], s_x],
                            [v_x[1],v_y[1], s_y]])

        a_img = cv2.warpAffine(c_img.copy(),mapping,(width, height),flags=cv2.WARP_INVERSE_MAP,borderMode=cv2.BORDER_REPLICATE)
        tmp_p = np.sum(a_img,axis=1)
        p = tmp_p[tmp_p>0].shape[0]
        project.append(p)
    min_angle = np.argmin(project)
        
    return angles[min_angle]

# 计算文本占比
def calculate_propotion(image,bboxes):
    # 由于是二值图 计算全局占比的时候直接加和即可
    text_part = np.sum(image/255.0)
    background_part = image.shape[0]*image.shape[1]
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

# NMS
def nms(bboxes,max_area,threshold):
    x1 = bboxes[:,0]
    y1 = bboxes[:,1]
    x2 = bboxes[:,2] + bboxes[:,0]
    y2 = bboxes[:,3] + bboxes[:,1]
    area = (x2-x1)*(y2-y1)
    # 按照面积排序
    I = area.argsort()
    
    pick = []
    result_boxes = []
    while len(I) > 0:
        # 如果文本块的面积大于整个面积的1/5，则忽略掉这个文本块
        if area[I[-1]] > 0.2*max_area:
            I = I[0:-1]
            continue
        # 计算IOU
        xx1 = np.maximum(x1[I[-1]], x1[I[0:-1]])
        yy1 = np.maximum(y1[I[-1]], y1[I[0:-1]])
        xx2 = np.minimum(x2[I[-1]], x2[I[0:-1]])
        yy2 = np.minimum(y2[I[-1]], y2[I[0:-1]]) 
        w = np.maximum(0.0, xx2 - xx1 + 1)
        h = np.maximum(0.0, yy2 - yy1 + 1)
        inter = w * h
        o = inter / area[I[0:-1]]
        # 将这个文本框添加入选取列表
        pick.append(I[-1])
        # 去掉IOU大于一定阈值的文本块
        tmp_I = I[np.where(o>threshold)]
        I = I[np.where(o<=threshold)]
        # 将IOU满足一定需求的文本块进行融合
        tmp_bboxes = bboxes[tmp_I].copy()
        if tmp_bboxes.shape[0] > 0:        
            tmp_top_left = np.min(tmp_bboxes[:,:2],axis=0)
            tmp_bottom_right = np.max(tmp_bboxes[:,:2]+tmp_bboxes[:,2:4],axis=0)
            result_boxes.append((tmp_top_left,tmp_bottom_right))
        else:
            result_boxes.append(((bboxes[pick[-1]][0],bboxes[pick[-1]][1]),
                                 (bboxes[pick[-1]][0]+bboxes[pick[-1]][2],
                                  bboxes[pick[-1]][1]+bboxes[pick[-1]][3])))
        # print(len(I))
    return pick,result_boxes


AREA_P = 0.3333
RELATIVE_P = 0.02
BOXES_COUNT = 30
BOX_W_STD = 40 # 
WIDTH_MEAN = 40 # <=
HEIGHT_MEAN = 100 # >=
# main code
def det_layout(image):
    # 图像转化灰度图
    # void cv::cvtColor
    img_w = image.shape[0]
    img_h = image.shape[1]

    gray_img = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    # 图像二值化 cv::threshold
    ret,bin_img = cv2.threshold(gray_img,127,255,cv2.THRESH_BINARY_INV)
    tmp_result = dilated_process(bin_img)

    # 获取联通区域
    _, labels, stats, centroids = cv2.connectedComponentsWithStats(tmp_result)
    # 去除最大连通域
    stats = stats[1::]
    # 计算占比
    overall_p,relative_p,overall_box = calculate_propotion(bin_img,stats)
    crop_area = (overall_box[1][0] - overall_box[0][0]) * (overall_box[1][1] - overall_box[0][1])
    area_p = crop_area / (image.shape[0]*image.shape[1]) 

    crop_x1 = overall_box[0][0]
    crop_y1 = overall_box[0][1]
    crop_x2 = overall_box[1][0]
    crop_y2 = overall_box[1][1]
    ow = (overall_box[1][0]-overall_box[0][0])
    oh = (overall_box[1][1]-overall_box[0][1])
    # 做文本块NMS
    pick_bboxes,new_boxes = nms(stats,bin_img.shape[0]*bin_img.shape[1],0.5)
    if len(pick_bboxes) == 0:
        return False,[area_p,relative_p,0.0,0.0,0.0],overall_box
    tmp_stats = stats[pick_bboxes]
    # 行文本块连接
    tmp,tmp_reg = connector(bin_img,tmp_stats)
    # 再做一遍nms
    if tmp_reg.shape[0] == 0:
        # fix bug of returning 0.0 while they are not 0.0.
        return False,[area_p,relative_p,0.0,0.0,0.0],overall_box
    
    new_picks,tmp_new_boxes = nms(tmp_reg,bin_img.shape[0]*bin_img.shape[1],0.5)
    new_tmp = tmp_reg[new_picks]
    # for tmp_box in new_tmp:
    #     print(tmp_box)
    #     cv2.rectangle(image, 
    #           (int(tmp_box[0]), int(tmp_box[1])), 
    #           (int(tmp_box[0])+int(tmp_box[2]), int(tmp_box[1])+int(tmp_box[3])), 
    #           (0,255,0), 1)
    # cv2.imshow("test",image)
    # cv2.waitKey()
    # cv2.imwrite("tmp_test.jpg",image)
    
    width_mean = np.mean(new_tmp[:,3])
    height_mean = np.mean(new_tmp[:,2])
    width_std = np.std(new_tmp[:,3])
    height_std = np.std(new_tmp[:,2])
    box_count = len(new_tmp)
    
    # 判断输出
    result = False
    if area_p > AREA_P and relative_p >= RELATIVE_P and box_count >= BOXES_COUNT and width_std <= BOX_W_STD:
        result = True
    # score_result = {'img_w':img_w,'img_h':img_h,'overall_propotion':overall_p,'relative_propotion':relative_p,'crop_x1':crop_x1,'crop_y1':crop_y1,
    #                 'crop_x2':crop_x2,'crop_y2':crop_y2,'width':ow,'height':oh,'boxes_count':box_count,'box_h_mean':height_mean,
    #                 'box_w_mean':width_mean,'box_h_std':height_std,'box_w_std':width_std}
    score_result = [area_p,relative_p,box_count,width_std,height_mean]
    return result,score_result,overall_box


# test
if __name__ == "__main__":
    tmp_img = cv2.imread("/home/tal-c/Src/layout_detection/data/simple_test/622.jpg")
    tmp_result,hm,tmp_score = det_layout(tmp_img)
    exit(0)
    test_dir = "/home/tal-c/Src/layout_detection/data/simple_test"
    test_list = os.listdir("/home/tal-c/Src/layout_detection/data/simple_test")

    data_dir = []
    for g in test_list:
        tmp_path = os.path.join(good_dir,g)
        data_dir.append((tmp_path,1))

    count = 0
    all_count = 0
    data = []
    tmp_open = open("tmp_result.txt","w")
    for d in data_dir:
        tmp_img = cv2.imread(d[0])
        result,score = det_layout(tmp_img)
        tmp_result = "{} {} {} {:.4f} \n".format(d[0],d[1],result,score)
        count += 1
        print(count,tmp_result)
        tmp_open.write(tmp_result)
    tmp_open.close()
    # base_dir = "../data/image_data"
    # file_list = os.listdir(base_dir)
    # image_path_list = []
    # for file in file_list:
    #     tmp_file_list = os.listdir(os.path.join(base_dir,file))
    #     for f in tmp_file_list:
    #         if f.endswith(".jpg"):
    #             image_path_list.append(os.path.join(base_dir,file,f))
    # wrong_img_list = [
    # "../data/image_data/202245_210/note_202245_210_27268_01_2.jpg",
    # "../data/image_data/169455_210/note_169455_210_63721_01_2.jpg",
    # "../data/image_data/203169_210/note_203169_210_42503_01_1.jpg",
    # "../data/image_data/203169_210/note_203169_210_42503_01_2.jpg",
    # "../data/image_data/203206_210/note_203206_210_42503_01_1.jpg",
    # "../data/image_data/199500_210/note_199500_210_27268_01_1.jpg",
    # "../data/image_data/199500_210/note_199500_210_27268_01_3.jpg",
    # "../data/image_data/187902_210/note_187902_210_37302_01_1.jpg",
    # "../data/image_data/206638_210/note_206638_210_27268_01_2.jpg",
    # "../data/image_data/206638_210/note_206638_210_27268_01_1.jpg",
    # "../data/image_data/201809_210/note_201809_210_36096_01_1.jpg"
    # ]
    # for wrong_img in wrong_img_list:
    #     image_path_list.remove(wrong_img)

    # goodcase = "../data/goodcase3/"
    # badcase = "../data/badcase3/"
    # good_count = 0
    # bad_count = 0
    # for test_img in image_path_list:
    #     tmp_img = cv2.imread(test_img)
    #     det_result,_ = det_layout(tmp_img)
    #     print(test_img,det_result)
    #     if det_result:
    #         if good_count < 1000:
    #             tmp_dir = os.path.join(goodcase,str(good_count)+".jpg")
    #             cv2.imwrite(tmp_dir,tmp_img)
    #             good_count += 1
    #     else:
    #         if bad_count < 1000:
    #             tmp_dir = os.path.join(badcase,str(bad_count)+".jpg")
    #             cv2.imwrite(tmp_dir,tmp_img)
    #             bad_count += 1
            
    #     if bad_count >= 10000 and good_count >= 1000:
    #         break
        
