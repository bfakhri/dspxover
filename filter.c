#include "filter.h"

#define PI 3.14159265
#define TAPS 61

static double filter_taps[TAPS] = {-0.000000, 0.000210, 0.000513, 0.000732, 0.000620, -0.000000, -0.001071, -0.002209, -0.002771, -0.002120, -0.000000, 0.003144, 0.006115, 0.007296, 0.005349, 0.000000, -0.007422, -0.014079, -0.016480, -0.011920, -0.000000, 0.016427, 0.031443, 0.037550, 0.028119, 0.000000, -0.044683, -0.098351, -0.149649, -0.186566, 0.800000, -0.186566, -0.149649, -0.098351, -0.044683, 0.000000, 0.028119, 0.037550, 0.031443, 0.016427, -0.000000, -0.011920, -0.016480, -0.014079, -0.007422, 0.000000, 0.005349, 0.007296, 0.006115, 0.003144, -0.000000, -0.002120, -0.002771, -0.002209, -0.001071, -0.000000, 0.000620, 0.000732, 0.000513, 0.000210, -0.000000};


// Holds leftover samples for next batch
jaudio_t overbuff[TAPS-1]; 

// Holds counter for next batch
unsigned long counter = 0; 
	

void init_filter(){
	for(int i=0; i<TAPS-1; i++)
		overbuff[i] = 0;
}

int filter(jaudio_t* input, jaudio_t* output, jack_nframes_t num_frames)
{
	for(int frame=0; frame<num_frames; frame++)
		output[frame] = cos((counter+frame)/PI);	

	counter += num_frames; 
	return 0; 
} 

/*
int
process (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *in, *out;
	
	in = jack_port_get_buffer (input_port, nframes);

	jack_default_audio_sample_t * combined = (jack_default_audio_sample_t*)malloc(sizeof(jack_default_audio_sample_t)*nframes + sizeof(jack_default_audio_sample_t)*(FILTER_TAP_NUM-1)); 

	// Copy Left Buffer into the Combined Buffer
	memcpy(combined, left_buffer, sizeof(jack_default_audio_sample_t)*(FILTER_TAP_NUM-1));
	// Copy Input Buffer into the Combined Buffer
	memcpy(combined+(FILTER_TAP_NUM-1), in, sizeof(jack_default_audio_sample_t)*(nframes - (FILTER_TAP_NUM-1)));
	

	for(int f=0; f<nframes; f++){
		in[f] = 0; 
		for(int t=0; t<FILTER_TAP_NUM; t++){
			in[f] += filter_taps[t]*combined[f+t];
		}
	}

	out = jack_port_get_buffer (output_port, nframes);
	memcpy (out, in,
		sizeof (jack_default_audio_sample_t) * nframes);

	memcpy(left_buffer, combined+nframes, sizeof (jack_default_audio_sample_t)*(FILTER_TAP_NUM-1));

	fprintf (stderr, "Scooted %d frames\n", nframes);

	return 0;      
}*/
