// Copyright (C) 2025 Vincent Hamp
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

/// DECUP task function
///
/// \file   drv/out/track/zimo/decup/task_function.cpp
/// \author Vincent Hamp
/// \date   14/08/2024

#include "task_function.hpp"
#include <driver/gpio.h>
#include <decup/decup.hpp>
#include <ulf/decup_ein.hpp>
#include "../../current_limit.hpp"
#include "log.h"
#include "mem/nvs/settings.hpp"
#include "resume.hpp"
#include "suspend.hpp"
#include "utility.hpp"

namespace drv::out::track::zimo::decup {

using namespace ::decup;
using ::ulf::decup_ein::ack, ::ulf::decup_ein::nak;

namespace {

uint8_t volatile DRAM_ATTR ack_count{};

/// \todo document
bool IRAM_ATTR rmt_callback(rmt_channel_handle_t,
                            rmt_tx_done_event_data_t const*,
                            void*) {
  // Clear any glitches
  ack_count = 0u;

  return pdFALSE;
}

/// \todo document
void IRAM_ATTR ack_isr_handler(void*) { ++ack_count; }

/// \todo document
std::optional<Packet> receive_packet() {
  Packet packet;
  auto const then{xTaskGetTickCount() + pdMS_TO_TICKS(task.timeout)};
  while (xTaskGetTickCount() < then)
    //
    if (auto const bytes_received{
          xMessageBufferReceive(tx_message_buffer.front_handle,
                                data(packet),
                                packet.max_size(),
                                pdMS_TO_TICKS(100u))}) {
      packet.resize(bytes_received);
      return packet;
    }
    //
    else if (std::to_underlying(state.load() &
                                (State::Suspend | State::ShortCircuit)))
      break;
  return std::nullopt;
}

/// \todo document
esp_err_t transmit_packet_blocking(Packet const& packet) {
  static constexpr rmt_transmit_config_t config{.flags = {.eot_level = 1u}};
  ESP_ERROR_CHECK(
    rmt_transmit(channel, encoder, data(packet), size(packet), &config));
  return rmt_tx_wait_all_done(channel, -1);
}

/// \todo document
uint8_t receive_acks(uint32_t us) {
  // Wait for up to 2 acks
  auto then{esp_timer_get_time() + us};
  while (esp_timer_get_time() < then)
    if (ack_count == 2u) break;

  // Mandatory delay
  then = esp_timer_get_time() + 100u;
  while (esp_timer_get_time() < then);

  return ack_count;
}

/// \todo document
esp_err_t transmit_acks(uint8_t acks) {
  return xMessageBufferSend(
           rx_message_buffer.handle, &acks, sizeof(acks), 0u) == sizeof(acks)
           ? ESP_OK
           : ESP_FAIL;
}

/// \todo document
esp_err_t loop() {
  ESP_ERROR_CHECK(set_current_limit(CurrentLimit::_4100mA));

  for (;;) {
    // Return on empty packet, suspend or short circuit
    if (auto const packet{receive_packet()};
        !packet || std::to_underlying(state.load() &
                                      (State::Suspend | State::ShortCircuit)))
      return rmt_tx_wait_all_done(channel, -1);
    // Transmit packet
    else {
      if (get_current_limit() == CurrentLimit::_4100mA &&
          packet->front() != std::to_underlying(decup::Command::Preamble0) &&
          packet->front() != std::to_underlying(decup::Command::Preamble1))
        ESP_ERROR_CHECK(set_current_limit(CurrentLimit::_1300mA));
      ESP_ERROR_CHECK(transmit_packet_blocking(*packet));
      auto const us{decup::packet2pulse_timeout(*packet)};
      auto const acks{receive_acks(us)};
      ESP_ERROR_CHECK(transmit_acks(acks));
    }
  }
}

/// \todo document that this pings a decoder (default MX645)
esp_err_t test_loop(uint8_t decoder_id = 221u) {
  ESP_ERROR_CHECK(set_current_limit(CurrentLimit::_1300mA));

  for (auto i{0uz}; i < 200uz; ++i) {
    Packet packet{0xEFu};
    ESP_ERROR_CHECK(transmit_packet_blocking(packet));
    receive_acks(300u);
    packet[0uz] = 0xBFu;
    ESP_ERROR_CHECK(transmit_packet_blocking(packet));
    receive_acks(300u);
  }

  Packet packet{decoder_id};
  ESP_ERROR_CHECK(transmit_packet_blocking(packet));
  auto const acks{receive_acks(size(packet) > 1uz ? 100'000u : 5000u)};
  if (acks == 2uz) {
    LOGI("DECUP test success");
    return ESP_OK;
  } else {
    LOGE("DECUP test failure");
    return ESP_FAIL;
  }
}

} // namespace

/// \todo document
[[noreturn]] void task_function(void*) {
  switch (decup_encoder_config_t encoder_config{}; state.load()) {
    case State::DECUPZpp: [[fallthrough]];
    case State::DECUPZsu: [[fallthrough]];
    case State::ULF_DECUP_EIN:
      ESP_ERROR_CHECK(resume(encoder_config, rmt_callback, ack_isr_handler));
      ESP_ERROR_CHECK(loop());
      ESP_ERROR_CHECK(suspend());
      break;
    default: assert(false); break;
  }
  LOGI_TASK_DESTROY();
}

} // namespace drv::out::track::zimo::decup
