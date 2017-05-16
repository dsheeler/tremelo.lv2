/*
  Copyright 2017 Daniel Sheeler <dsheeler@pobox.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <math.h>
#include <stdlib.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#ifndef M_PI
#    define M_PI 3.14159265
#endif

/** Define a macro for converting a gain in dB to a coefficient. */
#define DB_CO(g) ((g) > -120.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)

#define TREMELO_URI "http://dsheeler.org/plugins/tremelo"

typedef enum {
	TREMELO_GAIN   = 0,
  TREMELO_FREQ   = 1,
	TREMELO_MODE   = 2,
  TREMELO_INPUT  = 3,
	TREMELO_OUTPUT = 4
} PortIndex;

static const uint32_t wave_len_s = 2048;

typedef struct {
	// Port buffers
	const float* gain;
  const float* freq;
	const float* mode;
  const float* input;
	float*       output;

  float gain_db;
  float gain_coeff_target;
  float gain_coeff;

  float freq_target;
  float freq_last;
  float freq_actual;
  double sr;
  float phase;
  float phase_inc;
  uint32_t wave_offset;
  float* wave_sine;
  float* wave_square;
  uint32_t wave_len;
} Tremelo;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features) {
	Tremelo* trem = (Tremelo*)calloc(1, sizeof(Tremelo));
  trem->sr = rate;
  trem->wave_len = wave_len_s;
  trem->wave_sine = (float*)malloc(trem->wave_len * sizeof(float));
  trem->wave_square = (float*)malloc(trem->wave_len * sizeof(float));
  for (uint32_t i = 0; i < trem->wave_len; i++) {
    trem->wave_sine[i] = sinf(i * 2 * M_PI / trem->wave_len);
    trem->wave_square[i] = (float)trem->wave_sine[i] > 0 ? 1.0 : -1.0;
  }
  return (LV2_Handle)trem;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	Tremelo* trem = (Tremelo*)instance;

	switch ((PortIndex)port) {
	case TREMELO_GAIN:
		trem->gain = data;
		break;
	case TREMELO_FREQ:
		trem->freq = data;
		break;
  case TREMELO_MODE:
    trem->mode = data;
    break;
	case TREMELO_INPUT:
		trem->input = data;
		break;
	case TREMELO_OUTPUT:
		trem->output = data;
		break;
	}
}

static void
activate(LV2_Handle instance)
{
  Tremelo* trem = (Tremelo*) instance;
  trem->phase = 0;
  trem->gain_db = *trem->gain;
  trem->gain_coeff = DB_CO(*trem->gain);
  trem->gain_coeff_target = DB_CO(*trem->gain);
  trem->freq_actual = *trem->freq;
  trem->freq_target = *trem->freq;
  trem->freq_last = *trem->freq;
}


static void
run(LV2_Handle instance, uint32_t n_samples)
{
	Tremelo* trem = (Tremelo*)instance;
  int mode = rint(*trem->mode);
	const float gain = *trem->gain;
	const float freq = *trem->freq;
	const float *input  = trem->input;
	float *const output = trem->output;
  uint32_t idx;
  if (trem->gain_db != gain) {
    trem->gain_db = gain;
    float db = trem->gain_db;
    if (db > 0) db = 0;
    if (db < -120) db = -120;
    trem->gain_coeff_target = DB_CO(db);
  }
  trem->gain_coeff += 0.1 * (trem->gain_coeff_target - trem->gain_coeff) + 1e-12;
  if (trem->freq_last != freq) {
    trem->freq_target = freq;
  }
  trem->freq_actual += 0.1 * (trem->freq_target - trem->freq_actual) + 1e-12;
  trem->phase_inc =  trem->freq_actual / trem->sr;

	const float coef_c = trem->gain_coeff;
	float *wave = mode <= 0 ? trem->wave_sine : trem->wave_square;
  for (uint32_t pos = 0; pos < n_samples; pos++) {
		idx = (uint32_t) (trem->phase * trem->wave_len);
    output[pos] = input[pos] * (0.5 * (1 + coef_c)
     + 0.5 * (1 - coef_c) * wave[idx]);
    trem->phase = fmodf(trem->phase + trem->phase_inc, 1.0);
	}
  trem->freq_last = freq;
}

static void
deactivate(LV2_Handle instance)
{
}

static void
cleanup(LV2_Handle instance)
{
  Tremelo *trem = (Tremelo*)instance;
  free(trem->wave_sine);
  free(trem->wave_square);
	free(trem);
}

static const void*
extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	TREMELO_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:  return &descriptor;
	default: return NULL;
	}
}
