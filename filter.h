#ifndef FILTER_H
#define FILTER_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>


typedef jack_default_audio_sample_t jaudio_t;

// Initializes buffers to zero
void init_filter();

// Function that does the filtering
int filter(jaudio_t* input, jaudio_t* output, jack_nframes_t num_frames);


#endif
