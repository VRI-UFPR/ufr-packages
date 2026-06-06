/**
* This file is part of ORB-SLAM3
*
* Copyright (C) 2017-2021 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
* Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
*
* ORB-SLAM3 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM3 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
* the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with ORB-SLAM3.
* If not, see <http://www.gnu.org/licenses/>.
*/

// ============================================================================
//  Header
// ============================================================================

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>

#include<opencv2/core/core.hpp>
#include<ufr.h>
// #include<ufr_cv.hpp>
#include<System.h>
#include<time.h>

using namespace std;

// ============================================================================
//  Functions
// ============================================================================

cv::Mat ufr_cv_get_mat(link_t* link) {
    // get the needed data
    int type, rows, cols;
    ufr_get(link, "iii", &type, &rows, &cols);
    void* data = (void*) ufr_get_rawptr(link);
    const int size[2] = {rows, cols};

    // return cv::Mat
    cv::Mat image(2, size, type, data, 0);
    return image;
}

// ============================================================================
//  Main
// ============================================================================

int main(int argc, char **argv) {
    if(argc != 2) {
        cerr << endl << "Usage: ./rgbd_tum path_to_vocabulary" << endl;
        return 1;
    }

    // Open the subscriber links
    link_t cam_rgb = ufr_subscriber("@new ros_noetic @coder ros_noetic:image @topic /camera_rgb");
    ufr_exit_if_error(&cam_rgb);
    link_t cam_depth = ufr_subscriber("@new ros_noetic @coder ros_noetic:image @topic /camera_depth");
    ufr_exit_if_error(&cam_depth);

    cv::Mat imRGB, imD;
    while ( ufr_loop_ok() ) {
        // Read image and depthmap from the subscriber
        if ( ufr_recv_2s(&cam_rgb, &cam_depth, 100) != UFR_OK ) {
            continue;
        }

        imRGB = ufr_cv_get_mat(&cam_rgb);
        // imD = ufr_cv_get_mat(&cam_rgb);
        cv::imshow("aaa", imRGB);
        cv::waitKey(1);
    }
    return 0;


    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    // ORB_SLAM3::System SLAM(argv[1],argv[2],ORB_SLAM3::System::RGBD,true);

    ORB_SLAM3::System SLAM("../Vocabulary/ORBvoc.txt",
        "../Examples/RGB-D/TUM1.yaml",ORB_SLAM3::System::RGBD,true);
    printf("aaa\n");
    const float imageScale = SLAM.GetImageScale();

    // Main loop
    cv::Mat imRGB, imD;
    while ( ufr_loop_ok() ) {
        // Read image and depthmap from the subscriber
        if ( ufr_recv_2s(&cam_rgb, &cam_depth, 100) != UFR_OK ) {
            continue;
        }

        imRGB = ufr_cv_get_mat(&cam_rgb);
        imD = ufr_cv_get_mat(&cam_rgb);
        cv::imshow("aaa", imRGB);
        cv::waitKey(1);

        continue;

        // double tframe = vTimestamps[ni];
        
        const double tframe = time(0);

        if(imRGB.empty()) {
            break;
        }

        if(imageScale != 1.f) {
            int width = imRGB.cols * imageScale;
            int height = imRGB.rows * imageScale;
            cv::resize(imRGB, imRGB, cv::Size(width, height));
            cv::resize(imD, imD, cv::Size(width, height));
        }

        // Pass the image to the SLAM system
        SLAM.TrackRGBD(imRGB,imD,tframe);
    }

    // Stop all threads
    SLAM.Shutdown();
    return 0;
}


