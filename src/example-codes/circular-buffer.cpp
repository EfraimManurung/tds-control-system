/**
 *
 * Circular buffer concept
 *
 * Author: Efraim Manurung
 * Email: efraim.manurung@gmail.com
 *
 * Changelog:
 *
 * [01-09-2025]
 *  - First initialization.
 *  - Implement simple circular buffer
 */

/*
Key Points:
    - onTimer() is a real ISR (IRAM_ATTR ensures it lives in IRAM, not flash).
    - ISR only calls rb.push_isr() → fast, safe, non-blocking.
    - Heavy work (Serial.printf) happens in the consumer task, not ISR.
    - This is exactly the recommended ISR → task workflow for ESP32.
*/

// file: src/main.cpp
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <Arduino.h>

#define EVENT_BIT_DATA_AVAILABLE (1 << 0)
#define EVENT_BIT_BUFFER_FULL (1 << 1)

template <class T, size_t N> struct ISR_CircularBuffer {
  T arr[N];
  size_t write_idx = 0;
  size_t read_idx = 0;
  size_t count = 0;

  portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
  EventGroupHandle_t event_group;

  ISR_CircularBuffer() { event_group = xEventGroupCreate(); }

  void push_isr(const T &t) {
    portENTER_CRITICAL_ISR(&mux);
    arr[write_idx] = t;
    write_idx = (write_idx + 1) % N;
    if (count < N) {
      count++;
    } else {
      read_idx = (read_idx + 1) % N; // overwrite oldest
    }
    portEXIT_CRITICAL_ISR(&mux);

    xEventGroupSetBitsFromISR(event_group, EVENT_BIT_DATA_AVAILABLE, nullptr);
    if (count == N) {
      xEventGroupSetBitsFromISR(event_group, EVENT_BIT_BUFFER_FULL, nullptr);
    }
  }

  T pop() {
    T value{};
    portENTER_CRITICAL(&mux);
    if (count > 0) {
      value = arr[read_idx];
      read_idx = (read_idx + 1) % N;
      count--;
    }
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

// Global buffer
ISR_CircularBuffer<int, 8> rb;

// Timer handle
hw_timer_t *timer = nullptr;
volatile int isr_counter = 0;

// Actual ISR
void IRAM_ATTR onTimer() { rb.push_isr(isr_counter++); }

// Consumer task
void consumer_task(void *pvParams) {
  while (true) {
    EventBits_t bits =
        xEventGroupWaitBits(rb.event_group, EVENT_BIT_DATA_AVAILABLE, pdTRUE,
                            pdFALSE, portMAX_DELAY);

    if (bits & EVENT_BIT_DATA_AVAILABLE) {
      while (!rb.is_empty()) {
        int val = rb.pop();
        Serial.printf("Popped from ISR: %d\n", val);
      }
    }

    if (bits & EVENT_BIT_BUFFER_FULL) {
      Serial.println("⚠️ Buffer full (oldest overwritten)");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting ISR CircularBuffer with hardware timer...");

  // Setup hardware timer (0, prescaler 80 → 1 tick = 1µs)
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 500000, true); // 500ms
  timerAlarmEnable(timer);

  // Consumer task
  xTaskCreatePinnedToCore(consumer_task, "ConsumerTask", 4096, nullptr, 1,
                          nullptr, APP_CPU_NUM);
}

void loop() {
  // nothing
}