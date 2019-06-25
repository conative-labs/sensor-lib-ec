#include "lib_ec.h"
#include "lib_ec_config.h"

#include <math.h>

#define MAGIC_NUMBER_DEFAULT 0x64

#define CAL_DRY_DEFAULT 0
#define CAL_LOW_IN_DEFAULT 0
#define CAL_LOW_OUT_DEFAULT 0
#define CAL_HIGH_IN_DEFAULT 0
#define CAL_HIGH_OUT_DEFAULT 0

#define K_DEFAULT 1.0
#define ALPHA 0.02

static lib_ec_temperature_t temperature = 25;
static lib_ec_params_t *params;

static void calibration_set_default(void) {
	params->cal_dry = CAL_DRY_DEFAULT;
	params->cal_low_in = CAL_LOW_IN_DEFAULT;
	params->cal_low_out = CAL_LOW_OUT_DEFAULT;
	params->cal_high_in = CAL_HIGH_IN_DEFAULT;
	params->cal_high_out = CAL_HIGH_OUT_DEFAULT;
}

static void params_set_default(void) {
	params->magic_number = MAGIC_NUMBER_DEFAULT;
	params->k = K_DEFAULT;
	calibration_set_default();
}

void lib_ec_init(lib_ec_params_t *ext_params) {
	params = ext_params;
	if(params->magic_number != MAGIC_NUMBER_DEFAULT)
		params_set_default();

//	params->cal_dry = 50;
//	params->cal_low_in = 13365;
//	params->cal_low_out = 12880;
//	params->cal_high_in = 66250;
//	params->cal_high_out = 80000;
}

static lib_ec_eC_t eC_temperature(lib_ec_eC_t eC, lib_ec_temperature_t temperature) {
	//NaCl-based solutions typically have a temperature coefficient
	//(α) of 0.02-0.0214 (~ 2% change/degree C).
	//EC25=ECambient /[ 1 + α (tambient – 25) ], α= 0.02
	eC /= (float)(1.0 + ALPHA * (float)(temperature - 25.0));
	//reference conductivity value to 25C
//		if(25 != temperature) {
//			float temp_ec = eC;
//			temp_ec *= 265*exp(-2.43*temperature);
//			temp_ec *= 1000000.0;
//			temp_ec *= 1000000.0;
//			temp_ec *= 1000000.0;
//			temp_ec *= 1000000.0;
//			eC = temp_ec;
//		}
	return eC;
}


static lib_ec_salinity_t conductivity_to_salinity(lib_ec_eC_t conductivity, lib_ec_temperature_t temp) {
	lib_ec_salinity_t sal;

	if(conductivity > 0) {
		sal = sqrt((conductivity/42914.0) / (0.6766097+temp*(0.0200564+temp*(0.0001104259+temp*(-0.00000069698+temp*0.0000000010031)))));

		sal = (0.008+sal*(-0.1692+sal*(25.3851+sal*(14.0941+sal*(-7.0261+sal*2.7081))))) + (((temp-15.0)/(1.0+0.0162*(temp-15.0))) * (0.0005+sal*(-0.0056+sal*(-0.0066+sal*(-0.0375+sal*(0.0636+sal*-0.0144))))));

		if(sal > 42.0)sal = 42.0;
	} else sal = 0;

	return sal;
}


static lib_ec_eC_t get_eC() {
	return (1000000.0 * params->k / Rprobe());
}

void lib_ec_cmd(lib_ec_cmd_t cmd, void *buffer) {
	switch(cmd) {
		case read: {
			lib_ec_reading_t *vals = buffer;

			vals->eC = get_eC();

			if(vals->eC > params->cal_dry)
				vals->eC -= params->cal_dry;
			else
				vals->eC = 0;

			//apply calibration value
			//if low point is set then we have 2 point calibration
			if((CAL_LOW_IN_DEFAULT != params->cal_low_in) && (CAL_HIGH_IN_DEFAULT != params->cal_high_in)) {
			//			eC -= params->cal_low_in;
			//			eC *= ((float)(params->cal_high_out - params->cal_low_out)/(float)(params->cal_high_in - params->cal_low_in));
			//			eC += params->cal_low_out;
			//			eC = map(eC, params->cal_low_in, params->cal_high_in, params->cal_low_out, params->cal_high_out);
				if(vals->eC < params->cal_low_in) {
					vals->eC *= (float)params->cal_low_out/(float)params->cal_low_in;
				} else if (vals->eC > params->cal_high_in) {
					vals->eC *= (float)params->cal_high_out/(float)params->cal_high_in;
				} else {
					lib_ec_eC_t temp_ec;

					temp_ec = (vals->eC > params->cal_low_in ? vals->eC - params->cal_low_in : params->cal_low_in - vals->eC);
					temp_ec *= ((float)(params->cal_high_out - params->cal_low_out)/(float)(params->cal_high_in - params->cal_low_in));
					temp_ec = params->cal_low_out + (vals->eC > params->cal_low_in ? temp_ec : -temp_ec);
					vals->eC = temp_ec;
				}
			} else if(CAL_HIGH_IN_DEFAULT != params->cal_high_in) {
				vals->eC *= (float)params->cal_high_out/(float)params->cal_high_in;
			}

			//calculate salinity value
			vals->salinity = conductivity_to_salinity(vals->eC, temperature);
			//add temperature effect
			vals->eC = eC_temperature(vals->eC, temperature);
			vals->TDS = vals->eC*0.54;
		} break;
		case get_temperature:
			*(lib_ec_temperature_t*)buffer = temperature;
		break;
		case set_temperature:
			temperature = *(lib_ec_temperature_t*)buffer;
		break;
		case get_k:
			*(lib_ec_k_t*)buffer = params->k;
		break;
		case set_k:
			params->k = *(lib_ec_k_t*)buffer;
		break;
		case cal_dry:
			params->cal_dry = eC_temperature(get_eC(), temperature);
		break;
		case cal_low:
			params->cal_low_in = eC_temperature(get_eC(), temperature);
			params->cal_low_out = *(lib_ec_cal_low_t*)buffer;
		break;
		case cal_high:
		case cal_single:
			params->cal_high_in = eC_temperature(get_eC(), temperature);
			params->cal_high_out = *(lib_ec_cal_high_t*)buffer;
		break;
		case cal_get:
			((uint8_t *)buffer)[0] = 0;
			if(CAL_HIGH_IN_DEFAULT != params->cal_high_in)
				((uint8_t *)buffer)[0]++;
			if(CAL_LOW_IN_DEFAULT != params->cal_low_in)
				((uint8_t *)buffer)[0]++;
		break;
		case cal_clear:
			calibration_set_default();
		break;
		case reset:
			params_set_default();
		break;
	}
}
