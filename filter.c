#include "filter.h"

#define PI 3.14159265
#define TAPS 401

static double FIRF[TAPS] = {-0.000029, -0.000050, -0.000009, 0.000028, -0.000006, -0.000056, -0.000025, 0.000044, 0.000034, -0.000041, -0.000043, 0.000051, 0.000087, 0.000002, -0.000051, 0.000044, 0.000140, 0.000075, -0.000035, 0.000023, 0.000178, 0.000171, 0.000016, 0.000001, 0.000185, 0.000270, 0.000109, -0.000008, 0.000156, 0.000345, 0.000235, 0.000016, 0.000095, 0.000369, 0.000370, 0.000087, 0.000022, 0.000322, 0.000479, 0.000203, -0.000037, 0.000200, 0.000519, 0.000344, -0.000052, 0.000021, 0.000452, 0.000471, -0.000008, -0.000178, 0.000263, 0.000529, 0.000092, -0.000346, -0.000037, 0.000465, 0.000216, -0.000439, -0.000404, 0.000242, 0.000310, -0.000428, -0.000764, -0.000141, 0.000304, -0.000319, -0.001034, -0.000641, 0.000138, -0.000157, -0.001143, -0.001171, -0.000215, -0.000016, -0.001053, -0.001612, -0.000733, 0.000017, -0.000780, -0.001842, -0.001333, -0.000129, -0.000395, -0.001771, -0.001879, -0.000477, -0.000009, -0.001367, -0.002208, -0.000980, 0.000253, -0.000679, -0.002171, -0.001519, 0.000294, 0.000165, -0.001677, -0.001914, 0.000082, 0.000985, -0.000731, -0.001969, -0.000320, 0.001590, 0.000549, -0.001526, -0.000762, 0.001836, 0.001942, -0.000517, -0.001024, 0.001676, 0.003168, 0.000986, -0.000882, 0.001184, 0.003949, 0.002769, -0.000172, 0.000559, 0.004089, 0.004493, 0.001140, 0.000075, 0.003534, 0.005761, 0.002910, 0.000011, 0.002407, 0.006213, 0.004816, 0.000560, 0.000991, 0.005619, 0.006398, 0.001742, -0.000323, 0.003961, 0.007151, 0.003356, -0.001147, 0.001465, 0.006646, 0.004982, -0.001206, -0.001419, 0.004653, 0.006042, -0.000448, -0.004108, 0.001237, 0.005919, 0.000893, -0.006016, -0.003205, 0.004111, 0.002299, -0.006709, -0.007978, 0.000382, 0.003042, -0.006048, -0.012204, -0.005115, 0.002326, -0.004276, -0.014986, -0.011793, -0.000531, -0.002036, -0.015603, -0.018658, -0.005915, -0.000301, -0.013682, -0.024421, -0.013784, -0.000255, -0.009310, -0.027645, -0.023601, -0.003158, -0.003055, -0.026846, -0.034386, -0.010345, 0.004118, -0.020348, -0.044846, -0.023702, 0.011012, -0.005132, -0.053600, -0.048461, 0.016429, 0.030621, -0.059418, -0.119052, 0.019406, 0.294884, 0.438542, 0.294884, 0.019406, -0.119052, -0.059418, 0.030621, 0.016429, -0.048461, -0.053600, -0.005132, 0.011012, -0.023702, -0.044846, -0.020348, 0.004118, -0.010345, -0.034386, -0.026846, -0.003055, -0.003158, -0.023601, -0.027645, -0.009310, -0.000255, -0.013784, -0.024421, -0.013682, -0.000301, -0.005915, -0.018658, -0.015603, -0.002036, -0.000531, -0.011793, -0.014986, -0.004276, 0.002326, -0.005115, -0.012204, -0.006048, 0.003042, 0.000382, -0.007978, -0.006709, 0.002299, 0.004111, -0.003205, -0.006016, 0.000893, 0.005919, 0.001237, -0.004108, -0.000448, 0.006042, 0.004653, -0.001419, -0.001206, 0.004982, 0.006646, 0.001465, -0.001147, 0.003356, 0.007151, 0.003961, -0.000323, 0.001742, 0.006398, 0.005619, 0.000991, 0.000560, 0.004816, 0.006213, 0.002407, 0.000011, 0.002910, 0.005761, 0.003534, 0.000075, 0.001140, 0.004493, 0.004089, 0.000559, -0.000172, 0.002769, 0.003949, 0.001184, -0.000882, 0.000986, 0.003168, 0.001676, -0.001024, -0.000517, 0.001942, 0.001836, -0.000762, -0.001526, 0.000549, 0.001590, -0.000320, -0.001969, -0.000731, 0.000985, 0.000082, -0.001914, -0.001677, 0.000165, 0.000294, -0.001519, -0.002171, -0.000679, 0.000253, -0.000980, -0.002208, -0.001367, -0.000009, -0.000477, -0.001879, -0.001771, -0.000395, -0.000129, -0.001333, -0.001842, -0.000780, 0.000017, -0.000733, -0.001612, -0.001053, -0.000016, -0.000215, -0.001171, -0.001143, -0.000157, 0.000138, -0.000641, -0.001034, -0.000319, 0.000304, -0.000141, -0.000764, -0.000428, 0.000310, 0.000242, -0.000404, -0.000439, 0.000216, 0.000465, -0.000037, -0.000346, 0.000092, 0.000529, 0.000263, -0.000178, -0.000008, 0.000471, 0.000452, 0.000021, -0.000052, 0.000344, 0.000519, 0.000200, -0.000037, 0.000203, 0.000479, 0.000322, 0.000022, 0.000087, 0.000370, 0.000369, 0.000095, 0.000016, 0.000235, 0.000345, 0.000156, -0.000008, 0.000109, 0.000270, 0.000185, 0.000001, 0.000016, 0.000171, 0.000178, 0.000023, -0.000035, 0.000075, 0.000140, 0.000044, -0.000051, 0.000002, 0.000087, 0.000051, -0.000043, -0.000041, 0.000034, 0.000044, -0.000025, -0.000056, -0.000006, 0.000028, -0.000009, -0.000050, -0.000029};


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
	// Make combined buffer
	jaudio_t * comb = (jaudio_t*)malloc(sizeof(jaudio_t)*((TAPS-1)+num_frames));

	// Copy overbuff into combined buffer
	for(int frame=0; frame<(TAPS-1); frame++){
		comb[frame] = overbuff[frame];
	}
	// Copy input into combined buffer
	for(int frame=0; frame<num_frames; frame++){
		comb[frame+(TAPS-1)] = input[frame];
	}

	// Perform the FIR filter
	for(int outFrame=0; outFrame<num_frames; outFrame++){
		double sum = 0; 
		for(int filt=0; filt<(TAPS-1); filt++){
			sum += comb[outFrame+filt]*FIRF[filt];
		}
		output[outFrame] = sum; 
	}

	// Copy leftover frames to overbuff
	for(int frame=0; frame<(TAPS-1); frame++){
		overbuff[frame] = input[num_frames-(TAPS-1)+frame];
	}

	//for(int frame=0; frame<num_frames; frame++)
	//	output[frame] = cos((counter+frame)/PI);	

	counter += num_frames; 

	// Free the combined buffer
	free(comb); 
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
