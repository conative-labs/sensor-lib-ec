# EC Sensor library
This library is made to manage the usage of a EC Probe.

# How it works!
The conductivity value is measured by measuring the conductivity of the water, and temperature compensated.

The `lib_ec_cmd` is used to recieve cmds from the *parser or any caller library* and return the required output, To read sensor data, use `ec_read` command, you can use calibrate cmds `ec_get_k`, `ec_set_k` to change and read the cell constant of the probe, `ec_cal_mid, ec_cal_high, ec_cal_single` to calibrate the sensor at runtime and use `ec_cal_clear` to restore the default calibration, also you can get the calibration status using `ec_cal_get` it should return `2` if all calibration points are set, lastly `ec_reset` resets all the device params back to default.

Notice: `MAGIC_NUMBER_DEFAULT` is used for data integrity check.

# How to use!
Copy lib_ec_config.template.h to lib_ec_config.h and implement the `Rprobe` function, which compute and return the resistance.

License
----

GNU General Public License v3.0

***Copyright (C) 2019 Conative Labs***
