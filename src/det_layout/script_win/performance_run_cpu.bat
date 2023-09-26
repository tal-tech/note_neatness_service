@echo off

::det_face_id model
SET CAFFE_MODEL=model/det_face_id_v1.0.2.pb

::Set test data
SET IMAGES_PATH=images/testing/

SET CONFIG_PATH=model/config.ini

start libs/x64/cpu/performance_testing_CPU.exe ^
%CAFFE_MODEL% ^
%IMAGES_PATH% ^
%CONFIG_PATH%
