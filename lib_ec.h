#ifndef lib_ec_h
#define lib_ec_h

#include <stdint.h>

typedef uint16_t lib_ec_cal_dry_t;
typedef uint16_t lib_ec_cal_low_t;
typedef uint32_t lib_ec_cal_high_t;
typedef uint32_t lib_ec_eC_t;
typedef uint32_t lib_ec_TDS_t;
typedef float lib_ec_salinity_t;
typedef float lib_ec_temperature_t;
typedef float lib_ec_k_t;

typedef struct lib_ec_params_t {
	uint8_t magic_number;
	lib_ec_cal_dry_t cal_dry;
	lib_ec_cal_low_t cal_low_in;
	lib_ec_cal_low_t cal_low_out;
	lib_ec_cal_high_t cal_high_in;
	lib_ec_cal_high_t cal_high_out;
	lib_ec_k_t k;
} lib_ec_params_t;

typedef struct lib_ec_reading_t {
	lib_ec_eC_t eC;
	lib_ec_TDS_t TDS;
	lib_ec_salinity_t salinity;
} lib_ec_reading_t;

typedef enum lib_ec_cmd_t {
	read,
	get_temperature,
	set_temperature,
	get_k,
	set_k,
	cal_dry,
	cal_single,
	cal_low,
	cal_high,
	cal_get,
	cal_clear,
	reset,
} lib_ec_cmd_t;

void lib_ec_init(lib_ec_params_t *params_struct);
void lib_ec_cmd(lib_ec_cmd_t command, void *output_buffer);

#endif
