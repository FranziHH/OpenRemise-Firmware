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

/// Initialize peripherals when resuming ZUSI task
///
/// \file   drv/out/susi/zimo/zusi/resume.cpp
/// \author Vincent Hamp
/// \date   27/03/2023

#include "resume.hpp"
#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <algorithm>

namespace drv::out::susi::zimo::zusi {

namespace {

/// \todo document
esp_err_t init_alarm() {
  ESP_ERROR_CHECK(gptimer_enable(gptimer));
  return gptimer_start(gptimer);
}

/// \todo document
esp_err_t init_gpio() { return gpio_set_level(enable_gpio_num, 1u); }

} // namespace

/// \todo document
esp_err_t resume() {
  ESP_ERROR_CHECK(init_alarm());
  return init_gpio();
}

} // namespace drv::out::susi::zimo::zusi
