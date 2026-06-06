#!/usr/bin/env python3
#
# Projeto Pioneer
# Copyright (C) 2025  Visao Robotica e Imagem (VRI)
#  - Luize Duarte
#  - Felipe Gustavo Bombardelli
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#  
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# ============================================================================
#  Header
# ============================================================================

import cv2
import ufr
from ultralytics import YOLO

# ============================================================================
#  Main
# ============================================================================

video = ufr.Subscriber("@new video @@new mqtt @@coder msgpack @@topic camera_rgb")
objects = ufr.Subscriber("@new mqtt @coder msgpack @topic objects")

# Main Loop
while ufr.loop_ok():
    if video.recv_with(objects, 10):
        img = video.get_cv_image()
        img_object_len = objects.get("i")
        for i in range(img_object_len):
            img_object = objects.get("siiii")
            print(img_object)
            class_name = img_object[0]
            top = img_object[1]
            left = img_object[2]
            bottom = img_object[3]
            right = img_object[4]
            cv2.rectangle(img, (top, left), (bottom, right), (255, 0, 255), 3)
            cv2.putText(img, class_name, [top, left] , cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
        print()
        cv2.imshow('inference_result.jpg', img)
        cv2.waitKey(1)

# End