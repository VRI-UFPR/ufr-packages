#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libfreenect.h>
#include <opencv2/opencv.hpp>
#include <ufr.h>

freenect_context *f_ctx;
freenect_device *f_dev;
unsigned char *rgb_buffer;
unsigned short *depth_buffer;

uint32_t g_count_rgb = 0;
uint32_t g_count_depth = 0;

link_t g_cam_rgb;


void video_cb(freenect_device *dev, void *rgb, uint32_t timestamp) {
    memcpy(rgb_buffer, rgb, 640 * 480 * 3);

    ufr_put(&g_cam_rgb, "s", "rgb8");
    // ufr_put(&g_cam_rgb, "iii", 480, 640, 640 * 480 *3);
    // ufr_put_raw(&g_cam_rgb, rgb_buffer, 640 * 480 *3);
    ufr_put(&g_cam_rgb, "\n");

    g_count_rgb += 1;
}

void depth_cb(freenect_device *dev, void *depth, uint32_t timestamp) {
    memcpy(depth_buffer, depth, 640 * 480 * 2);
    g_count_depth += 1;
}

int main() {
    // Inicializando a biblioteca Freenect
    if (freenect_init(&f_ctx, NULL) < 0) {
        fprintf(stderr, "Erro ao inicializar o Freenect.\n");
        return -1;
    }

    // Abrindo o dispositivo Kinect
    if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
        fprintf(stderr, "Erro ao abrir o dispositivo Kinect.\n");
        return -1;
    }

    g_cam_rgb = ufr_publisher("@new ros_humble:topic @msg image @topic /kinect/rgb @debug 0");
    // g_cam2 = ufr_publisher("@new ros_humble:topic @msg image @topic camera1 @debug 0");

    // Alocando buffers para RGB e profundidade
    rgb_buffer = (unsigned char *)malloc(640 * 480 * 4);  // Imagem RGB
    depth_buffer = (unsigned short *)malloc(640 * 480 * 2);  // Imagem de profundidade

    // Registrando os callbacks
    freenect_set_video_callback(f_dev, video_cb);
    freenect_set_depth_callback(f_dev, depth_cb);

    // Iniciando a captura de vídeo e profundidade
    freenect_start_video(f_dev);
    freenect_start_depth(f_dev);

    // Criando as imagens OpenCV
    cv::Mat rgb_image(480, 640, CV_8UC3); // Imagem RGB (4 canais)
    cv::Mat depth_image(480, 640, CV_16UC1); // Imagem de profundidade (16 bits)

    while (1) {
        // Atualizando os dados de vídeo e profundidade
        freenect_process_events(f_ctx);

        // Copiando os buffers para as imagens OpenCV
        memcpy(rgb_image.data, rgb_buffer, 640 * 480 * 3);
        memcpy(depth_image.data, depth_buffer, 640 * 480 * 2);

        // Exibindo as imagens com OpenCV
        cv::imshow("RGB Image", rgb_image);
        cv::imshow("Depth Image", depth_image);

        // Aguardando a tecla 'ESC' para sair
        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    // Parando a captura
    printf("Fechando\n");
    freenect_stop_video(f_dev);
    freenect_stop_depth(f_dev);

    // Liberando os buffers
    free(rgb_buffer);
    free(depth_buffer);

    // Fechando o dispositivo e a biblioteca Freenect
    freenect_close_device(f_dev);
    freenect_shutdown(f_ctx);
    return 0;
}
