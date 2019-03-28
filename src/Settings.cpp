// Copyright 2019 Peter Kvitek.
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
// Persistent settings implementation.

#include "Settings.h"

#include "absl/strings/ascii.h"
#include "absl/strings/str_split.h"
#include "absl/strings/strip.h"
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"

#include "xplmpp/File.h"
#include "xplmpp/Log.h"

namespace xplmpp {

Settings g_settings;

bool Settings::Load(const char* filename) {
  File file;
  if (!file.Open(filename, "rt")) {
    LOG(WARNING) << "Could not open settings file '"
                 << filename << "', error: " << errno;
    return false;
  }

  std::string line;
  while (std::getline(file.ifstream(), line)) {
    absl::string_view input = absl::StripAsciiWhitespace(line);
    if (input.empty() || absl::StartsWith(input, "#"))
      continue;

    if (!Load(absl::StrSplit(input, absl::ByAnyChar(" ="), absl::SkipWhitespace()))) {
      LOG(WARNING) << "Invalid setting: '" << line << "', ignored.";
    }
  }

  return true;
}

bool Settings::Load(const std::vector<std::string>& vstr) {
  if (vstr.size() < 2)
    return false;

  if (vstr[0] == "runway_distance") {
    SetDistance(vstr, &Settings::set_runway_distance);
  } else
  if (vstr[0] == "approach_distance") {
    SetDistance(vstr, &Settings::set_approach_distance);
  } else
    return false;

  return true;
}

bool Settings::SetDistance(const std::vector<std::string>& vstr, 
                           std::function<void(Settings&, float)> setter) {
  // Convert value
  float value = 0;
  if (!absl::SimpleAtof(vstr[1], &value)) {
    LOG(WARNING) << "Invalid '" << vstr[0] << "' value, ignored.";
    return false;
  }

  // Convert units, if any
  if (vstr.size() > 2) {
    if (vstr[2] == "nm") {
      value *= Settings::kNmToMeters;
    } else
    if (vstr[2] == "ft") {
      value *= Settings::kFtToMeters;
    } else
    if (vstr[2] == "mi") {
      value *= Settings::kMiToMeters;
    } else
    if (vstr[2] == "km") {
      value *= Settings::kKmToMeters;
    } else {
      LOG(WARNING) << "Invalid '" << vstr[0] << "' units ignored.";
      return false;
    }
  }

  setter(*this, value);
  return true;
}

}  // namespace xplmpp
