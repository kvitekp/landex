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
// Main plugin window.

#ifndef LANDEX_WINDOW_H
#define LANDEX_WINDOW_H

#include <vector>

#include "xplmpp/XPLMWindow.h"

namespace xplmpp {

// Represents the plugin window
class LandExWindow : public XPLMWindow {
public:
  LandExWindow();
  ~LandExWindow() override;

  bool Create(bool vr_enabled);
  void Destroy();

  void Show();
  void Hide();
  void Clear();

  void UpdateOnVRChange(bool vr);

  void AddLine(const std::string& line);

private:
  // XPLMWindow interface.
  void OnDraw() override;

  void GetDefaultWindowPos(Rect& rc);

  std::vector<std::string> lines_;

};

}  // namespace xplmpp

#endif  // #ifndef LANDEX_WINDOW_H
