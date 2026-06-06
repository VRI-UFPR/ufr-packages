
# https://docs.ultralytics.com/tasks/pose/

# =============================================================================
#  Header
# =============================================================================

import ufr
import numpy as np
from ultralytics import YOLO

# Pontos
# 0. Nose
# 1. Left Eye
# 2. Right Eye
# 3. Left Ear
# 4. Right Ear
# 5. Left Shoulder
# 6. Right Shoulder
# 7. Left Elbow
# 8. Right Elbow
# 9. Left Wrist
# 10. Right Wrist
# 11. Left Hip
# 12. Right Hip
# 13. Left Knee
# 14. Right Knee
# 15. Left Ankle
# 16. Right Ankle

# =============================================================================
#  Main
# =============================================================================

# Load a model
model = YOLO("yolo11n-pose.pt")  # load an official model

# Create a Image Subscriber
sub = ufr.Subscriber("@new video @id 0")
pub = ufr.Publisher("@new mqtt @coder msgpack @host 127.0.0.1 @topic yolo_pose")

# Arquivo de Video
# sub = ufr.Subscriber("@new video @file video.mp4")

# Topico MQTT
# sub = ufr.Subscriber("@new video @@new mqtt @@host 127.0.0.1 @@topic camera_rgb)

# Loop principal
while True:
    frame = sub.recv_cv_image()

    # Predict with the model
    results = model(frame)  # predict on an image

    # Access the results
    pub.put("i", 1) # len(results)
    for result in results:
        xy = result.keypoints.xy  # x and y coordinates
        # xyn = result.keypoints.xyn  # normalized
        # kpts = result.keypoints.data  # x, y, visibility (if available)

        # send the pose
        xy_np = xy.cpu().numpy() if hasattr(xy, 'cpu') else np.array(xy)
        for lin in xy_np[0]:
            pub.put("ff", lin[0], lin[1])
            print("aaaa", lin[0], lin[1])
    pub.put("\n")

sub.close()