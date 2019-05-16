// Copyright (c) 2019 Peter Kvitek. All rights reserved.
//
// Author: Peter Kvitek (pete@kvitek.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Persistent settings interface.

#ifndef LANDEX_SETTINGS_H
#define LANDEX_SETTINGS_H

#include <vector>
#include <functional>

#include "Common.h"

namespace xplmpp {

class Settings {
public:
  Settings() = default;
  ~Settings() = default;

  std::string GetSettingsFilename();

  bool Load(const char* filename);
  bool Load(const std::string& filename) {
    return Load(filename.c_str());
  }
  bool Load() {
    return Load(GetSettingsFilename());
  }

#define SETTING_I(name, def) \
   private: \
    int name##_ = (def); \
   public: \
    int name() const { return name##_; } \
    void set_##name(int name) { name##_ = name; }

#define SETTING_F(name, def) \
   private: \
    float name##_ = (def); \
   public: \
    float name() const { return name##_; } \
    void set_##name(float name) { name##_ = name; }

  SETTING_I(log_level, xplmpp::kDefaultLogLevel);
  SETTING_F(runway_distance,   0.1f * kNmToMeters);
  SETTING_F(approach_distance, 1.0f * kNmToMeters);
  SETTING_F(vertical_grid,     0.1f * kNmToMeters);
  SETTING_F(horizontal_grid, 100.0f * kFtToMeters);

  #undef SETTING_I
  #undef SETTING_F

private:
  bool Load(const std::vector<std::string>& vstr);
  bool SetDistance(const std::vector<std::string>& vstr,
                   std::function<void(Settings&, float)> setter);
};

extern Settings g_settings;

}  // namespace xplmpp

#endif  // #ifndef LANDEX_SETTINGS_H
