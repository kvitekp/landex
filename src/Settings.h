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

  bool Load(const char* filename);
  bool Load(const std::string& filename) {
    return Load(filename.c_str());
  }

  static constexpr float kFtToMeters = 0.3048f;
  static constexpr float kNmToMeters = 1852.0f;
  static constexpr float kMiToMeters = 1609.34f;
  static constexpr float kKmToMeters = 1000.0f;

  #define SETTING_F(name, def) \
   private: \
    float name##_ = (def); \
   public: \
    float name() const { return name##_; } \
    void set_##name(float name) { name##_ = name; }

  SETTING_F(runway_distance, 0.5 * kNmToMeters);
  SETTING_F(approach_distance, 3 * kNmToMeters);

  #undef SETTING_F

private:
  bool Load(const std::vector<std::string>& vstr);
  bool SetDistance(const std::vector<std::string>& vstr,
                   std::function<void(Settings&, float)> setter);
};

extern Settings g_settings;

}  // namespace xplmpp

#endif  // #ifndef LANDEX_SETTINGS_H
