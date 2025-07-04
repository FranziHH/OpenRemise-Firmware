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

/// HTTP server for station
///
/// \file   intf/http/sta/server.hpp
/// \author Vincent Hamp
/// \date   01/03/2023

#pragma once

#include <memory>
#include "../endpoints.hpp"
#include "utility.hpp"

namespace intf::http::sta {

///
class Server : public Endpoints {
public:
  Server();
  ~Server();

  Response settingsGetRequest(Request const& req);
  Response settingsPostRequest(Request const& req);
  Response sysGetRequest(Request const& req);

private:
  esp_err_t deleteHandler(httpd_req_t* req);
  esp_err_t getHandler(httpd_req_t* req);
  esp_err_t putPostHandler(httpd_req_t* req);

  esp_err_t otaWsHandler(httpd_req_t* req);

  esp_err_t rocoZ21WsHandler(httpd_req_t* req);

  esp_err_t zimoDecupZppWsHandler(httpd_req_t* req);
  esp_err_t zimoDecupZsuWsHandler(httpd_req_t* req);
  esp_err_t zimoMduZppWsHandler(httpd_req_t* req);
  esp_err_t zimoMduZsuWsHandler(httpd_req_t* req);
  esp_err_t zimoZusiWsHandler(httpd_req_t* req);

  esp_err_t wildcardGetHandler(httpd_req_t* req);
};

} // namespace intf::http::sta
