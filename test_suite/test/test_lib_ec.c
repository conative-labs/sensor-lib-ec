#include "unity.h"
#include "lib_ec.h"

#include "mock_lib_ec_config_test.h"

#define MAGIC_NUMBER_DEFAULT 0x64



void setUP(void)
{
}

void tearDown(void)
{
}

void test_lib_ec_init_withMagicNumberAlreadySet(void)
{
  lib_ec_params_t params;
  params.k = 13221; // some random number
  params.cal_dry = 13221;
  params.cal_low_in = 13221;
  params.cal_low_out = 13221;
  params.cal_high_in = 13221;
  params.cal_high_out = 13221;
  params.magic_number = MAGIC_NUMBER_DEFAULT; // mock the magic number 
  lib_ec_init(&params);
  TEST_ASSERT_EQUAL(params.k, 13221); // k didn't change then nothing changed
  TEST_ASSERT_EQUAL(params.cal_dry, 13221);
  TEST_ASSERT_EQUAL(params.cal_low_in, 13221);
  TEST_ASSERT_EQUAL(params.cal_low_out, 13221);
  TEST_ASSERT_EQUAL(params.cal_high_in, 13221);
  TEST_ASSERT_EQUAL(params.cal_high_out, 13221);
}

void test_lib_ec_init_withoutMagicNumberAlreadySet(void)
{
  lib_ec_params_t params;
  params.k = 13221; // some random number
  params.cal_dry = 13221;
  params.cal_low_in = 13221;
  params.cal_low_out = 13221;
  params.cal_high_in = 13221;
  params.cal_high_out = 13221;
  lib_ec_init(&params);
  TEST_ASSERT_EQUAL(params.k, 1.0); // k didn't change then nothing changed
  TEST_ASSERT_EQUAL(params.cal_dry, 0);
  TEST_ASSERT_EQUAL(params.cal_low_in, 0);
  TEST_ASSERT_EQUAL(params.cal_low_out, 0);
  TEST_ASSERT_EQUAL(params.cal_high_in, 0);
  TEST_ASSERT_EQUAL(params.cal_high_out, 0);
}

void test_lib_ec_cmd_ReadEcValLessThanCalDry(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10000000); // get number closer to zero
    
    lib_ec_init(&params);

    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(0, reading.eC);
    TEST_ASSERT_EQUAL(0, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0, reading.salinity);
}

void test_lib_ec_cmd_ReadValidEcValueAndNotCaliberated(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10000); // get number closer to zero
    
    lib_ec_init(&params);

    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(100, reading.eC);
    TEST_ASSERT_EQUAL(54, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0.0517007, reading.salinity);
}

void test_lib_ec_cmd_ReadValidEcValueAndNotCaliberatedSalValMoreThan42(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10); // this will generate sal > 42 
    
    lib_ec_init(&params);

    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL_FLOAT(42, reading.salinity);
}


void test_lib_ec_cmd_ReadValidEcValueAndCalHighOnly(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10000); // get number closer to zero

    lib_ec_init(&params);

    params.cal_high_in = 10;
    params.cal_high_out = 8;
    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(80, reading.eC);
    TEST_ASSERT_EQUAL(43, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0.0428384, reading.salinity);
}


void test_lib_ec_cmd_ReadValidEcValueAndCalLowOnly(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10000); // get number closer to zero
    
    lib_ec_init(&params);

    params.cal_low_in = 10;
    params.cal_low_out = 8;
    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(100, reading.eC);
    TEST_ASSERT_EQUAL(54, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0.0517007, reading.salinity);
}

void test_lib_ec_cmd_ReadEcValueMoreThanHighAndCalBoth(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10000); // get number closer to zero

    lib_ec_init(&params);

    params.cal_high_in = 10;
    params.cal_high_out = 8;
    params.cal_low_in = 2;
    params.cal_low_out = 1;
    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(80, reading.eC);
    TEST_ASSERT_EQUAL(43, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0.0428384, reading.salinity);
}

void test_lib_ec_cmd_ReadEcValueLessThanLowAndCalBoth(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(100000); // get number closer to zero

    lib_ec_init(&params);

    params.cal_high_in = 100;
    params.cal_high_out = 80;
    params.cal_low_in = 20;
    params.cal_low_out = 16;
    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(8, reading.eC);
    TEST_ASSERT_EQUAL(4, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0.0134775, reading.salinity);
}

void test_lib_ec_cmd_ReadEcValueBetweenLowAndHighAndCalBoth(void)
{
    lib_ec_params_t params;
    lib_ec_reading_t reading;
    Rprobe_ExpectAndReturn(10000); // get number closer to zero

    lib_ec_init(&params);

    params.cal_high_in = 200;
    params.cal_high_out = 160;
    params.cal_low_in = 20;
    params.cal_low_out = 16;
    lib_ec_cmd(ec_read, &reading);
    TEST_ASSERT_EQUAL(80, reading.eC);
    TEST_ASSERT_EQUAL(43, reading.TDS);
    TEST_ASSERT_EQUAL_FLOAT(0.0428384, reading.salinity);
}

void test_lib_ec_cmd_TemperatureSetAndGet(void)
{
  lib_ec_params_t params;
  lib_ec_temperature_t temp = 23.5, temp_val; 
  lib_ec_init(&params);

  lib_ec_cmd(ec_set_temperature, &temp);
  lib_ec_cmd(ec_get_temperature, &temp_val);

  TEST_ASSERT_EQUAL_FLOAT(23.5, temp_val);
}

void test_lib_ec_cmd_KfactorSetAndGet(void)
{
  lib_ec_params_t params;
  lib_ec_k_t k = 23.5, k_val; 
  lib_ec_init(&params);

  lib_ec_cmd(ec_set_k, &k);
  lib_ec_cmd(ec_get_k, &k_val);

  TEST_ASSERT_EQUAL_FLOAT(23.5, k_val);
}

void test_lib_ec_cmd_CalDry(void)
{
  lib_ec_params_t params;

  Rprobe_ExpectAndReturn(100000);
    
  lib_ec_init(&params);

  lib_ec_cmd(ec_cal_dry, NULL);

  TEST_ASSERT_EQUAL_FLOAT(10, params.cal_dry);
}

void test_lib_ec_cmd_CalLow(void)
{
  lib_ec_params_t params;
  lib_ec_cal_low_t low = 15;
  Rprobe_ExpectAndReturn(100000);
    
  lib_ec_init(&params);

  lib_ec_cmd(ec_cal_low, &low);

  TEST_ASSERT_EQUAL_FLOAT(10, params.cal_low_in);
  TEST_ASSERT_EQUAL_FLOAT(15, params.cal_low_out);
}

void test_lib_ec_cmd_CalHigh(void)
{
  lib_ec_params_t params;
  lib_ec_cal_high_t high = 15;
  Rprobe_ExpectAndReturn(100000);
    
  lib_ec_init(&params);

  lib_ec_cmd(ec_cal_high, &high);

  TEST_ASSERT_EQUAL(10, params.cal_high_in);
  TEST_ASSERT_EQUAL(15, params.cal_high_out);
}

void test_lib_ec_cmd_CalSingle(void)
{
  lib_ec_params_t params;
  lib_ec_cal_high_t single = 15;
  Rprobe_ExpectAndReturn(100000);
    
  lib_ec_init(&params);

  lib_ec_cmd(ec_cal_single, &single);

  TEST_ASSERT_EQUAL(10, params.cal_high_in);
  TEST_ASSERT_EQUAL(15, params.cal_high_out);
}

void test_lib_ec_cmd_CalGetHighOnly(void)
{
  lib_ec_params_t params;
  uint8_t get;
  
  lib_ec_init(&params);
  params.cal_high_in = 51; // any value..
  lib_ec_cmd(ec_cal_get, &get);
  
  TEST_ASSERT_EQUAL(1, get);
}

void test_lib_ec_cmd_CalGetLowOnly(void)
{
  lib_ec_params_t params;
  uint8_t get;

  
  lib_ec_init(&params);

  params.cal_low_in = 51; // any value..
    
  lib_ec_cmd(ec_cal_get, &get);
  
  TEST_ASSERT_EQUAL(1, get);
}

void test_lib_ec_cmd_CalGetAll(void)
{
  lib_ec_params_t params;
  uint8_t get;


  lib_ec_init(&params);

  params.cal_low_in = 51; // any value..
  params.cal_high_in = 51; // any value..
  lib_ec_cmd(ec_cal_get, &get);
  
  TEST_ASSERT_EQUAL(2, get);
}

void test_lib_ec_cmd_CalGetNone(void)
{
  lib_ec_params_t params;
  uint8_t get;

  lib_ec_init(&params);

  lib_ec_cmd(ec_cal_get, &get);
  
  TEST_ASSERT_EQUAL(0, get);
}


void test_lib_ec_cmd_clear(void)
{
  lib_ec_params_t params;
  params.k = 13221; // some random number
  params.cal_dry = 13221;
  params.cal_low_in = 13221;
  params.cal_low_out = 13221;
  params.cal_high_in = 13221;
  params.cal_high_out = 13221;
  params.magic_number = MAGIC_NUMBER_DEFAULT; // mock the magic number 
  lib_ec_init(&params);

  lib_ec_cmd(ec_cal_clear, NULL);
  TEST_ASSERT_EQUAL(params.k, 13221); // k don't change in clear
  TEST_ASSERT_EQUAL(params.cal_dry, 0);
  TEST_ASSERT_EQUAL(params.cal_low_in, 0);
  TEST_ASSERT_EQUAL(params.cal_low_out, 0);
  TEST_ASSERT_EQUAL(params.cal_high_in, 0);
  TEST_ASSERT_EQUAL(params.cal_high_out, 0);
}


void test_lib_ec_cmd_reset(void)
{
  lib_ec_params_t params;
  params.k = 13221; // some random number
  params.cal_dry = 13221;
  params.cal_low_in = 13221;
  params.cal_low_out = 13221;
  params.cal_high_in = 13221;
  params.cal_high_out = 13221;
  params.magic_number = MAGIC_NUMBER_DEFAULT; // mock the magic number 
  lib_ec_init(&params);

  lib_ec_cmd(ec_reset, NULL);
  TEST_ASSERT_EQUAL(params.k, 1.0); 
  TEST_ASSERT_EQUAL(params.cal_dry, 0);
  TEST_ASSERT_EQUAL(params.cal_low_in, 0);
  TEST_ASSERT_EQUAL(params.cal_low_out, 0);
  TEST_ASSERT_EQUAL(params.cal_high_in, 0);
  TEST_ASSERT_EQUAL(params.cal_high_out, 0);
}

