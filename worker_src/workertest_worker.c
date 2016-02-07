#include <pebble_worker.h>
#include "accsensorfilter.h"

#define PERSIST_KEY_STEPS 52

static int s_step_count;

static void save_data() {
  // Save the step count to Persistent Storage
  persist_write_int(PERSIST_KEY_STEPS, s_step_count);
}

static void accel_raw_handler(AccelData *data, uint32_t num_samples)
{
  double vec_length = accel_sensor_filter(data);

  s_step_count += vec_length;
  
  // static char buffer[] = "9999";
  // snprintf(buffer, sizeof("9999"), "%d", (int)vec_length);

  save_data();
}

static void init() {
  s_step_count = 0;
  
  accel_data_service_subscribe(1, accel_raw_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

static void deinit() {
  accel_data_service_unsubscribe();
}


int main(void) {
  init();
  worker_event_loop();
  deinit();
}
