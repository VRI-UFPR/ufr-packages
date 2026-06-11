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

#include <ufr.h>
#include <unistd.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


const char* args_frame = "@new video @cols 640 @rows 480 @@new mqtt @@coder msgpack @@topic /camera_rgb @@host 177.153.62.174";

// ============================================================================
//  Test
// ============================================================================

int main() {
    link_t frame = ufr_subscriber(args_frame);

    while( ufr_loop() ) {
        if ( ufr_recv(&frame) == false ) {
            break;
        }

        int type;
        int size[2];
        void* data;
        ufr_get(&frame, "%d %d %d %p", &type, &size[0], &size[1], &data);
        printf("%d %d %p\n", size[0], size[1], data);
if ( data == NULL ) {
continue;
}

        // Show the image
        Mat image(2, size, type, data, 0);
        imshow("janela", image);
        waitKey(1);
    }

    // Fim
    printf("Fim\n");
    ufr_close(&frame);
    return 0;
}
