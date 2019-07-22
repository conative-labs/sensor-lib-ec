/*
  Copyright (C) 2019 Conative Labs
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>
*/

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
                           ec_read,
                           ec_get_temperature,
                           ec_set_temperature,
                           ec_get_k,
                           ec_set_k,
                           ec_cal_dry,
                           ec_cal_single,
                           ec_cal_low,
                           ec_cal_high,
                           ec_cal_get,
                           ec_cal_clear,
                           ec_reset,
} lib_ec_cmd_t;

void lib_ec_init(lib_ec_params_t *params_struct);
void lib_ec_cmd(lib_ec_cmd_t command, void *output_buffer);

#endif
