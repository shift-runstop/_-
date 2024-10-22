#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <portaudio.h>
#include <sndfile.h>

#define FRAMES_PER_BUFFER 512
#define MAX_GRAINS 100
#define MAX_GRAIN_SIZE 44100  // 1 second at 44.1kHz

typedef struct {
    float* data;
    int size;
    int position;
    float pan;
    float amplitude;
} Grain;

typedef struct {
    float* audioData;
    sf_count_t audioDataSize;
    Grain grains[MAX_GRAINS];
    int activeGrains;
    float density;
    float grainSize;
} GranularData;


/*
 * This is the PortAudio callback function. Used when PortAudio
 * needs more audio data to output. The parameters include input
 * and output buffers, the number of frames to process, timing
 * information, status flags, and user data.
 */
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
   /*
	* This cases the userData to GranularData and initialises
	* the output buffer to silence(0)
	*/
	GranularData *data = (GranularData*)userData;
    float *out = (float*)outputBuffer;

    for (unsigned int i = 0; i < framesPerBuffer * 2; i++) {
        out[i] = 0;
    }

    /*
	 * This loop processes all active grains, adding their output
	 * to the output buffer. It apples panning and amplitude to
	 * each grain.
	 */
    for (int i = 0; i < data->activeGrains; i++) {
        Grain *grain = &data->grains[i];
        for (unsigned int j = 0; j < framesPerBuffer; j++) {
            if (grain->position < grain->size) {
                float sample = grain->data[grain->position++] * grain->amplitude;
                out[j*2] += sample * (1 - grain->pan);  // Left channel
                out[j*2+1] += sample * grain->pan;      // Right channel
            }
        }
    }

    /*
	 * This generates new grains based on the density parameter. It randomly
	 * selects a start position in the input audio, and sets random pan and
	 * amplitiude values for each NEW grain.
	 */
    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        if ((float)rand() / RAND_MAX < data->density) {
            if (data->activeGrains < MAX_GRAINS) {
                Grain *newGrain = &data->grains[data->activeGrains++];
                int grainStart = rand() % (data->audioDataSize - (int)data->grainSize);
                newGrain->data = &data->audioData[grainStart];
                newGrain->size = (int)data->grainSize;
                newGrain->position = 0;
                newGrain->pan = (float)rand() / RAND_MAX;
                newGrain->amplitude = (float)rand() / RAND_MAX * 0.5f + 0.5f;
            }
        }
    }

    return paContinue;
}

int main() {
	// Variables
    PaStream *stream;
    GranularData data = {0}; //zeroes the struct
    SNDFILE *infile;
    SF_INFO sfinfo;

    // Load audio file
    memset(&sfinfo, 0, sizeof(sfinfo));
    infile = sf_open("input.wav", SFM_READ, &sfinfo);
    if (!infile) {
        printf("Error opening input file\n");
        return 1;
    }

    data.audioDataSize = sfinfo.frames;
    data.audioData = malloc(data.audioDataSize * sizeof(float));
    sf_read_float(infile, data.audioData, data.audioDataSize);
    sf_close(infile);

    // Initialize granular synthesis parameters
    data.density = 0.01f;
    data.grainSize = 0.1f * sfinfo.samplerate;  // 100ms grains

    // Initialize PortAudio
    Pa_Initialize();
    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, sfinfo.samplerate,
                         FRAMES_PER_BUFFER, paCallback, &data);
    Pa_StartStream(stream);

    printf("Playing. Press Enter to stop.\n");
    getchar();

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    free(data.audioData);

    return 0;
}
