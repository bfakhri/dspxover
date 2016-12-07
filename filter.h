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
void init_filters();

// Function that does the high pass filtering
int filter_hp(jaudio_t* input, jaudio_t* output, jack_nframes_t num_frames);

// Function that does the low pass filtering
int filter_lp(jaudio_t* input, jaudio_t* output, jack_nframes_t num_frames);


#endif
