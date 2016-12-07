#include "filter.h"

#define PI 3.14159265
#define TAPS_LP 37 
#define TAPS_HP 201 

// Filter taps generated here: http://www.arc.id.au/FilterDesign.html
// They are low precision and should be changed to high precision taps
// The filter assumes a 96kHz sample rate!!!!!!

static double FIR_LP[TAPS_LP] = {0.000307, 
0.000717, 
0.001360, 
0.002293, 
0.003566, 
0.005218, 
0.007266, 
0.009709, 
0.012512, 
0.015617, 
0.018934, 
0.022348, 
0.025725, 
0.028921, 
0.031788, 
0.034186, 
0.035994, 
0.037118, 
0.037500, 
0.037118, 
0.035994, 
0.034186, 
0.031788, 
0.028921, 
0.025725, 
0.022348, 
0.018934, 
0.015617, 
0.012512, 
0.009709, 
0.007266, 
0.005218, 
0.003566, 
0.002293, 
0.001360, 
0.000717, 
0.000307};

static double FIR_HP[TAPS_HP] = {-0.000060, -0.000066, -0.000070, -0.000071, -0.000069, -0.000063, -0.000052, -0.000036, -0.000015, 0.000012, 0.000045, 0.000083, 0.000127, 0.000176, 0.000228, 0.000283, 0.000340, 0.000396, 0.000451, 0.000502, 0.000547, 0.000584, 0.000610, 0.000624, 0.000622, 0.000604, 0.000568, 0.000511, 0.000433, 0.000333, 0.000211, 0.000068, -0.000096, -0.000278, -0.000477, -0.000688, -0.000908, -0.001133, -0.001358, -0.001576, -0.001782, -0.001969, -0.002131, -0.002261, -0.002353, -0.002400, -0.002397, -0.002338, -0.002219, -0.002037, -0.001790, -0.001477, -0.001098, -0.000655, -0.000154, 0.000402, 0.001004, 0.001644, 0.002311, 0.002993, 0.003676, 0.004345, 0.004984, 0.005577, 0.006105, 0.006553, 0.006901, 0.007132, 0.007231, 0.007182, 0.006971, 0.006586, 0.006017, 0.005256, 0.004299, 0.003142, 0.001787, 0.000237, -0.001500, -0.003414, -0.005492, -0.007717, -0.010070, -0.012528, -0.015067, -0.017661, -0.020282, -0.022899, -0.025483, -0.028003, -0.030428, -0.032728, -0.034875, -0.036840, -0.038599, -0.040129, -0.041409, -0.042423, -0.043157, -0.043601, 0.956250, -0.043601, -0.043157, -0.042423, -0.041409, -0.040129, -0.038599, -0.036840, -0.034875, -0.032728, -0.030428, -0.028003, -0.025483, -0.022899, -0.020282, -0.017661, -0.015067, -0.012528, -0.010070, -0.007717, -0.005492, -0.003414, -0.001500, 0.000237, 0.001787, 0.003142, 0.004299, 0.005256, 0.006017, 0.006586, 0.006971, 0.007182, 0.007231, 0.007132, 0.006901, 0.006553, 0.006105, 0.005577, 0.004984, 0.004345, 0.003676, 0.002993, 0.002311, 0.001644, 0.001004, 0.000402, -0.000154, -0.000655, -0.001098, -0.001477, -0.001790, -0.002037, -0.002219, -0.002338, -0.002397, -0.002400, -0.002353, -0.002261, -0.002131, -0.001969, -0.001782, -0.001576, -0.001358, -0.001133, -0.000908, -0.000688, -0.000477, -0.000278, -0.000096, 0.000068, 0.000211, 0.000333, 0.000433, 0.000511, 0.000568, 0.000604, 0.000622, 0.000624, 0.000610, 0.000584, 0.000547, 0.000502, 0.000451, 0.000396, 0.000340, 0.000283, 0.000228, 0.000176, 0.000127, 0.000083, 0.000045, 0.000012, -0.000015, -0.000036, -0.000052, -0.000063, -0.000069, -0.000071, -0.000070, -0.000066, -0.000060};

// Holds leftover samples for next batch
jaudio_t overbuff_lp[TAPS_LP-1]; 
jaudio_t overbuff_hp[TAPS_HP-1]; 

// Holds counter for next batch
uint64_t counter_lp = 0;
uint64_t counter_hp = 0;
//unsigned long counter_lp = 0; 
//unsigned long counter_hp = 0; 
	

void init_filters(){
	for(int i=0; i<TAPS_LP-1; i++)
		overbuff_lp[i] = 0;
	for(int i=0; i<TAPS_HP-1; i++)
		overbuff_hp[i] = 0;
}

int filter_lp(jaudio_t* input, jaudio_t* output, jack_nframes_t num_frames)
{
	// Perform safety check to ensure num_frames is big enough
	if(TAPS_LP > num_frames){
		for(int frame=0; frame<num_frames; frame++){
			output[frame] = 0;
		}
		fprintf (stderr, "ERROR!\tBuffer Size (num_frames=%d) is less than the overflow buffer (overbuff=%d frames\n", num_frames, TAPS_LP-1);
	}

	// Make combined buffer
	jaudio_t * comb = (jaudio_t*)malloc(sizeof(jaudio_t)*((TAPS_LP-1)+num_frames));

	// Copy overbuff into combined buffer
	for(int frame=0; frame<(TAPS_LP-1); frame++){
		comb[frame] = overbuff_lp[frame];
	}
	// Copy input into combined buffer
	for(int frame=0; frame<num_frames; frame++){
		comb[frame+(TAPS_LP-1)] = input[frame];
	}

	// Perform the FIR filter
	for(int outFrame=0; outFrame<num_frames; outFrame++){
		double sum = 0; 
		for(int filt=0; filt<(TAPS_LP-1); filt++){
			sum += comb[outFrame+filt]*FIR_LP[filt];
		}
		output[outFrame] = sum; 
	}

	// Copy leftover frames to overbuff
	for(int frame=0; frame<(TAPS_LP-1); frame++){
		overbuff_lp[frame] = input[num_frames-(TAPS_LP-1)+frame];
	}

	counter_lp += num_frames; 

	// Free the combined buffer
	free(comb); 
	return 0; 
} 

int filter_hp(jaudio_t* input, jaudio_t* output, jack_nframes_t num_frames)
{
	// Perform safety check to ensure num_frames is big enough
	if(TAPS_HP > num_frames){
		for(int frame=0; frame<num_frames; frame++){
			output[frame] = 0;
		}
		fprintf (stderr, "ERROR!\tBuffer Size (num_frames=%d) is less than the overflow buffer (overbuff=%d frames\n", num_frames, TAPS_HP-1);
	}

	// Make combined buffer
	jaudio_t * comb = (jaudio_t*)malloc(sizeof(jaudio_t)*((TAPS_HP-1)+num_frames));

	// Copy overbuff into combined buffer
	for(int frame=0; frame<(TAPS_HP-1); frame++){
		comb[frame] = overbuff_hp[frame];
	}
	// Copy input into combined buffer
	for(int frame=0; frame<num_frames; frame++){
		comb[frame+(TAPS_HP-1)] = input[frame];
	}

	// Perform the FIR filter
	for(int outFrame=0; outFrame<num_frames; outFrame++){
		double sum = 0; 
		for(int filt=0; filt<(TAPS_HP-1); filt++){
			sum += comb[outFrame+filt]*FIR_HP[filt];
		}
		output[outFrame] = sum; 
	}

	// Copy leftover frames to overbuff
	for(int frame=0; frame<(TAPS_HP-1); frame++){
		overbuff_hp[frame] = input[num_frames-(TAPS_HP-1)+frame];
	}

	fprintf (stderr, "HP Count %lu \n", counter_hp);
	counter_hp += num_frames; 

	// Free the combined buffer
	free(comb); 
	return 0; 
} 

