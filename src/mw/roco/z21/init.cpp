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

///
///
/// \file   mw/roco/z21/init.cpp
/// \author Vincent Hamp
/// \date   15/04/2024

#include "init.hpp"
#include "intf/http/sta/server.hpp"
#include "mw/dcc/service.hpp"
#include "service.hpp"

namespace mw::roco::z21 {

/// \todo document
esp_err_t init() {
  if (intf::http::sta::server) {
    service = std::make_shared<Service>();
    service->dcc(dcc::service);
    dcc::service->z21(service, service);
    intf::http::sta::server->subscribe(
      {.uri = "/roco/z21/"}, service, &Service::socket);
  }
  return ESP_OK;
}

} // namespace mw::roco::z21
