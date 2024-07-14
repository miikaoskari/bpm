#include <pipewire/pipewire.h>
#include <time.h>
#include <math.h>

#include "pipewire/keys.h"
#include "pipewire/main-loop.h"
#include "pipewire/properties.h"
#include "pipewire/stream.h"
#include "spa/param/audio/format-utils.h"
  
#define M_PI_M2 ( M_PI + M_PI )
 
#define DEFAULT_RATE            44100
#define DEFAULT_CHANNELS        2
#define DEFAULT_VOLUME          0.7

void analyze_audio_for_bpm(void* audio_data, size_t size);

struct data {
        struct pw_main_loop *loop;
        struct pw_stream *stream;
        double accumulator;
};

/* [on_process] */
static void on_process(void *userdata)
{
        struct data *data = userdata;
        struct pw_buffer *buffer;
        struct spa_buffer *spa_buffer;
        void *ptr;
        uint32_t size;

        if ((buffer = pw_stream_dequeue_buffer(data->stream)) == NULL) {
                fprintf(stderr, "out of buffers\n");
                return;
        }

        spa_buffer = buffer->buffer;
        ptr = spa_buffer->datas[0].data;
        size = spa_buffer->datas[0].chunk->size;

        fprintf(stdout, "processing %d bytes at %p\n", size, ptr);
        analyze_audio_for_bpm(&ptr, size);

        pw_stream_queue_buffer(data->stream, buffer);
}

void analyze_audio_for_bpm(void* audio_data, size_t size)
{
    // Analyze audio data for BPM
}

/* [on_process] */

static const struct pw_stream_events stream_events = {
        PW_VERSION_STREAM_EVENTS,
        .process = on_process,
};


int main(int argc, char *argv[])
{
        struct data data = { 0, };
        const struct spa_pod *params[1];
        uint8_t buffer[1024];
        struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

        pw_init(&argc, &argv);
        data.loop = pw_main_loop_new(NULL);
        data.stream = pw_stream_new_simple(
                pw_main_loop_get_loop(data.loop),
                "audio-src",
                pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY, "Playback",PW_KEY_MEDIA_ROLE, "Music", NULL), 
                &stream_events,
                &data);

                params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat,
                        &SPA_AUDIO_INFO_RAW_INIT(
                                .format = SPA_AUDIO_FORMAT_S16,
                                .channels = DEFAULT_CHANNELS,
                                .rate = DEFAULT_RATE ));
 
        pw_stream_connect(data.stream,
                          PW_DIRECTION_INPUT,
                          PW_ID_ANY,
                          PW_STREAM_FLAG_AUTOCONNECT |
                          PW_STREAM_FLAG_MAP_BUFFERS |
                          PW_STREAM_FLAG_RT_PROCESS,
                          params, 1);
 
        pw_main_loop_run(data.loop);
 
        pw_stream_destroy(data.stream);
        pw_main_loop_destroy(data.loop);

        return 0;
}
