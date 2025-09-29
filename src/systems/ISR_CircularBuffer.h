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
#define EVENT_BIT_DATA_AVAILABLE (1 << 0)
#define EVENT_BIT_BUFFER_FULL (1 << 1)

/* Information:
    - T: generic data type (int, float, struct, etc).
    - N: compile-time buffer capacity.
    - This makes the class type-safe and size-safe at compile time (no malloc).
*/
template <typename T, size_t N> struct ISR_CircularBuffer {
  std::array<T, N> arr;
  std::size_t write_idx = 0; // Index of the next element to write (push)
  std::size_t read_idx = 0;  // Index of the next element to read (pop)
  std::size_t count = 0;     // Number of elements in the buffer

  portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
  EventGroupHandle_t event_group;

  /* constructor */
  ISR_CircularBuffer() { event_group = xEventGroupCreate(); }

  void push_isr(const T &t) {
    portENTER_CRITICAL_ISR(&mux);
    arr.at(write_idx) = t;
    write_idx = (write_idx + 1) % N;
    if (count < N) {
      count++;
    } else {
      /* buffer is full, move forward read_idx */
      read_idx = (read_idx + 1)
    }
    portEXIT_CRITICAL_ISR(&mux);

    xEventGroupSetBitsFromISR(event_group, EVENT_BIT_DATA_AVAILABLE, nullptr);

    if (count == N) {
      xEventGroupSetBitsFromISR(event_group, EVENT_BIT_BUFFER_FULL, nullptr);
    }
  }

  T pop() {
    if (count == 0) {
      // Buffer is empty, return a default-constructed T.
      return T{};
    }
    portENTER_CRITICAL_ISR(&mux);
    T value = arr.at(read_idx);
    read_idx = (read_idx + 1) % N;
    --count;
    portEXIT_CRITICAL_ISR(&mux);
    return value;
  }

  bool is_empty() {
    portENTER_CRITICAL(&mux);
    bool empty = (count == 0);
    portEXIT_CRITICAL(&mux);
    return empty;
  }
};