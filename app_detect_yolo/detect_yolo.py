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

import ufr
from ultralytics import YOLO

# ============================================================================
#  Main
# ============================================================================

# Start
video = ufr.Subscriber("@new video @@new mqtt @@coder msgpack @@topic camera_rgb")
out = ufr.Publisher("@new mqtt @coder msgpack @topic objects")
model = YOLO('yolo_model/yolov8n.pt')

# Main Loop
while True:
    # get image and detect objects
    img = video.recv_cv_image()
    results = model.track(img, persist=True)

    # send the result
    for r in results:
        boxes = r.boxes
        out.put("i", len(boxes))
        for box in boxes:
            b = box.xyxy[0].to('cpu').detach().numpy().copy() # get box coordinates in (top, left, bottom, right) format
            c = box.cls
            class_name = model.names[int(c)]
            top = int(b[0])
            left = int(b[1])
            bottom = int(b[2])
            right = int(b[3])
            out.put("siiii", class_name, top, left, bottom, right)
        out.put("\n")

# End
out.close()