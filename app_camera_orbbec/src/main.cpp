/* BSD 2-Clause License
 * 
 * Copyright (c) 2024, Visao Robotica e Imagem (VRI)
 *  - Felipe Bombardelli <felipebombardelli@gmail.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// ============================================================================
//  Header
// ============================================================================

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <OpenNI.h>
#include <ufr.h>
#include <vector>

using namespace openni;

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

#ifndef ROBOT_TOPIC_CAMERA_DEPTH
#define ROBOT_TOPIC_CAMERA_DEPTH "@new mqtt @coder msgpack @host 10.0.0.6 @topic camera1"
#endif

#ifndef ROBOT_TOPIC_CAMERA_RGB
#define ROBOT_TOPIC_CAMERA_RGB "@new mqtt @coder msgpack @host 10.0.0.6 @topic camera2"
#endif

// ============================================================================
//  Main
// ============================================================================

int main(int argc, char* argv[]) {
    link_t video1 = ufr_publisher(ROBOT_TOPIC_CAMERA_DEPTH);
    ufr_exit_if_error(&video1);

    link_t video2 = ufr_publisher(ROBOT_TOPIC_CAMERA_RGB);
    ufr_exit_if_error(&video2);

    Status rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return 1;
    }

    Device device;

    if (argc < 2)
        rc = device.open(ANY_DEVICE);
    else
        rc = device.open(argv[1]);

    if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}

    // Abre a camera de profundidade
    VideoStream depth;
    if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depth.create(device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
            return 3;
        }
    }

    rc = depth.start();
    if (rc != STATUS_OK)
    {
        printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
        return 4;
    }

    // Abre a camera RGB
    VideoStream rgb;
    if (device.getSensorInfo(SENSOR_COLOR) != NULL)
    {
        rc = rgb.create(device, SENSOR_COLOR);
        if (rc != STATUS_OK)
        {
            printf("Couldn't create rgb stream\n%s\n", OpenNI::getExtendedError());
            return 3;
        }
    }

    rc = rgb.start();
    if (rc != STATUS_OK)
    {
        printf("Couldn't start the rgb stream\n%s\n", OpenNI::getExtendedError());
        return 4;
    }

    device.setDepthColorSyncEnabled(true);
    device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);

    std::vector<uint8_t> buffer;
	VideoFrameRef frame, frame_rgb;

	while (1) {
		int changedStreamDummy;
		VideoStream* pStream = &depth;
		rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			continue;
		}

        // le a imagem de profundidade
        rc = depth.readFrame(&frame);
        if (rc != STATUS_OK) {
            printf("Read failed!\n%s\n", OpenNI::getExtendedError());
            continue;
        }
        if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
        {
            printf("Unexpected frame format\n");
            continue;
        }

        rc = rgb.readFrame(&frame_rgb);
        if (rc != STATUS_OK) {
            printf("Read failed!\n%s\n", OpenNI::getExtendedError());
            continue;
        }



        // int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;
        // printf("[%08llu] %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);

        // envia a imagem de profundidade
        DepthPixel* pDepth = (DepthPixel*)frame.getData();
        cv::Mat mat_depth(480,640,CV_16U,(void*)pDepth);
        imencode(".png", mat_depth, buffer);
        ufr_put(&video1, "sii", ".png", 480, 640);
        ufr_put_raw(&video1, &buffer[0], buffer.size());
        ufr_send(&video1);

        // envia a imagem de profundidade
        const void* pRgb = (void*)frame_rgb.getData();
        cv::Mat mat_rgb(480,640,CV_8UC3,(void*)pRgb);
        cv::Mat mat_bgr;
        cv::cvtColor(mat_rgb, mat_bgr, cv::COLOR_RGB2BGR);
        imencode(".png", mat_bgr, buffer);
        ufr_put(&video2, "sii", ".png", 480, 640);
        ufr_put_raw(&video2, &buffer[0], buffer.size());
        ufr_send(&video2);
    }

    rgb.stop();
    rgb.destroy();
    depth.stop();
    depth.destroy();
    device.close();
    OpenNI::shutdown();

    return 0;
}


