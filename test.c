/** @file simple_client.c
 *
 * @brief This simple client demonstrates the most basic features of JACK
 * as they would be used by many applications.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>

jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 2000 Hz

* 0 Hz - 400 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 4.1393894966071585 dB

* 500 Hz - 1000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -40.07355419274887 dB

*/

#define FILTER_TAP_NUM 21

static double filter_taps[FILTER_TAP_NUM] = {
  -0.02010411882885732,
  -0.05842798004352509,
  -0.061178403647821976,
  -0.010939393385338943,
  0.05125096443534972,
  0.033220867678947885,
  -0.05655276971833928,
  -0.08565500737264514,
  0.0633795996605449,
  0.310854403656636,
  0.4344309124179415,
  0.310854403656636,
  0.0633795996605449,
  -0.08565500737264514,
  -0.05655276971833928,
  0.033220867678947885,
  0.05125096443534972,
  -0.010939393385338943,
  -0.061178403647821976,
  -0.05842798004352509,
  -0.02010411882885732
};


jack_default_audio_sample_t left_buffer[FILTER_TAP_NUM-1]; 

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
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	fprintf (stderr, "Proper Shutdown\n");
	exit (1);
}

int
main (int argc, char *argv[])
{
	// Clears the left buffer
	for(int i=0; i<FILTER_TAP_NUM-1; i++)
		left_buffer[i] = 0;

	const char **ports;
	const char *client_name = "simple";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	/* open a client connection to the JACK server */

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	jack_set_process_callback (client, process, 0);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, 0);

	/* display the current sample rate. 
	 */

	printf ("engine sample rate: %" PRIu32 "\n",
		jack_get_sample_rate (client));

	/* create two ports */

	input_port = jack_port_register (client, "input",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsInput, 0);
	output_port = jack_port_register (client, "output",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	if ((input_port == NULL) || (output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}

	if (jack_connect (client, ports[0], jack_port_name (input_port))) {
		fprintf (stderr, "cannot connect input ports\n");
	}

	free (ports);
	
	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	free (ports);

	/* keep running until stopped by the user */

	sleep (-1);

	/* this is never reached but if the program
	   had some other way to exit besides being killed,
	   they would be important to call.
	*/

	jack_client_close (client);
	exit (0);
}
