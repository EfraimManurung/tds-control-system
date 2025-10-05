/**
 * Circular buffer concept for ISR + FreeRTOS event group
 *
 * Author: Efraim Manurung
 * Email: efraim.manurung@gmail.com
 *
 * Changelog:
 * [01-09-2025]
 *  - First initialization.
 *  - Implement simple circular buffer
 */

#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <array>

// Event group bits
#define EVENT_BIT_SENSOR_AVAILABLE (1 << 0)
#define EVENT_BIT_DATA_AVAILABLE (1 << 1)
#define EVENT_BIT_BUFFER_FULL (1 << 2)

/* Information:
    - T: generic data type (int, float, struct, etc).
    - N: compile-time buffer capacity.
    - This makes the class type-safe and size-safe at compile time (no malloc).
*/
// --- File: ISR_CircularBuffer.h (replace the template methods with this safer
// version) ---
template <typename T, size_t N> struct ISR_CircularBuffer {
  std::array<T, N> arr;
  std::size_t write_idx = 0;
  std::size_t read_idx = 0;
  std::size_t count = 0;

  portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
  EventGroupHandle_t event_group;

  ISR_CircularBuffer() { event_group = xEventGroupCreate(); }

  // ISR-safe push
  void push(const T &t) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // perform buffer update inside the critical section
    portENTER_CRITICAL_ISR(&mux);
    arr[write_idx] = t; // use operator[] (no exceptions)
    write_idx = (write_idx + 1) % N;
    if (count < N) {
      ++count;
    } else {
      // buffer full -> overwrite oldest, advance read_idx
      read_idx = (read_idx + 1) % N;
    }
    portEXIT_CRITICAL_ISR(&mux);

    // notify consumer task; capture wake info so a context switch can occur
    xEventGroupSetBitsFromISR(event_group, EVENT_BIT_DATA_AVAILABLE,
                              &xHigherPriorityTaskWoken);

    if (count == N) {
      xEventGroupSetBitsFromISR(event_group, EVENT_BIT_BUFFER_FULL,
                                &xHigherPriorityTaskWoken);
    }

    // yield to higher priority task if needed
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  // Task-safe pop: perform check+pop inside critical to avoid races
  T pop() {
    portENTER_CRITICAL(&mux);
    if (count == 0) {
      portEXIT_CRITICAL(&mux);
      return T{}; // empty
    }
    T value = arr[read_idx];
    read_idx = (read_idx + 1) % N;
    --count;
    portEXIT_CRITICAL(&mux);
    return value;
  }

  bool is_empty() {
    portENTER_CRITICAL(&mux);
    bool empty = (count == 0);
    portEXIT_CRITICAL(&mux);
    return empty;
  }
};
