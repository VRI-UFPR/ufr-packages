#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <ufr.h>

int main() {
    int err;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    int dir;
    char *buffer;
    int size;

    link_t topic = ufr_publisher("@new mqtt @coder msgpack @host 177.153.62.174 @topic /pioneer/microfone");
    
    // 1. Abrir o dispositivo de captura PCM (microfone) no modo de bloqueio
    err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "Erro ao abrir o dispositivo de áudio: %s\n", snd_strerror(err));
        exit(1);
    }

    // 2. Alocar o container de parâmetros de hardware
    snd_pcm_hw_params_alloca(&params);

    // 3. Preencher os parâmetros com os valores padrão
    snd_pcm_hw_params_any(handle, params);

    // 4. Definir os parâmetros para Intercalado (Interleaved), 16-bits, Mono, 44.1kHz
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_U32);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    unsigned int val = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

    // 5. Aplicar os parâmetros ao driver do dispositivo
    err = snd_pcm_hw_params(handle, params);
    if (err < 0) {
        fprintf(stderr, "Erro ao configurar parâmetros de hardware: %s\n", snd_strerror(err));
        exit(1);
    }

    // 6. Usar um buffer de 32 frames por vez
    const snd_pcm_uframes_t frames = 1024;
    size = frames * 4; // 2 bytes por frame (16-bit)
    buffer = (char *) malloc(size);

    printf("Iniciando captura do microfone...\n");

    // Loop de captura por 5 segundos (aproximadamente)
    for (int i = 0; i < (5 * 44100 / frames); ++i) {
        err = snd_pcm_readi(handle, buffer, frames);
        if (err == -EPIPE) {
            // O buffer estourou (overrun)
            fprintf(stderr, "Overrun ocorrido\n");
            snd_pcm_prepare(handle);
        } else if (err < 0) {
            fprintf(stderr, "Erro ao ler dados da placa de som: %s\n", snd_strerror(err));
        } else if (err != (int)frames) {
            fprintf(stderr, "Leitura incompleta, lidos %d frames de %d\n", err, (int)frames);
        } else {
            // Aqui você processa os dados de áudio brutos (buffer)
            // Exemplo: printf("Frame 1: %d\n", ((short*)buffer)[0]);
            printf("frame %d\n", i);
            ufr_put_af32(&topic, buffer, frames);
            ufr_send(&topic);
        }
    }

    // 7. Limpeza e fechamento
    free(buffer);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    
    printf("Captura finalizada.\n");
    return 0;
}

