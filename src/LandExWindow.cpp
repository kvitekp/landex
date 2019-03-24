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
// Main plugin window implementation.

#include "LandExWindow.h"

#include "xplmpp/XPLMScreen.h"
#include "xplmpp/XPLMLog.h"
#include "xplmpp/Point.h"
#include "xplmpp/Rect.h"
#include "xplmpp/Size.h"

#include "XPLMGraphics.h"

#include "GlideSlope.h"

namespace xplmpp {

static const int kLineCountLimit = 100;

static const Point kDefWindowPos = Point(50, 150);
static const Size kDefWindowSize = Size(465, 300);
static const Size kMinWindowSize = Size(465, 150);
static const Size kMaxWindowSize = Size(800, 1000);

/*
 * LandEx plugin window implementation.
 */
LandExWindow::LandExWindow() {
}

LandExWindow::~LandExWindow() {
}

bool LandExWindow::Create(bool vr_enabled) {
  Rect rc;
  GetDefaultWindowPos(rc);
  if (!XPLMWindow::Create(rc))
    return false;

  SetWindowPositioningMode(vr_enabled ? xplm_WindowVR : xplm_WindowPositionFree);
  SetWindowResizingLimits(kMinWindowSize, kMaxWindowSize);
  SetWindowTitle("Landing log");

  return true;
}

void LandExWindow::Destroy() {
  DestroyWindow();
}

void LandExWindow::Show() {
  SetWindowIsVisible();
  BringToFront();
}

void LandExWindow::Hide() {
  SetWindowIsVisible(false);
}

void LandExWindow::Clear() {
  lines_.clear();
}

void LandExWindow::UpdateOnVRChange(bool vr) {
  SetWindowPositioningMode(vr ? xplm_WindowVR : xplm_WindowPositionFree);

  // Window is not moved to the main monitor when exiting VR.
  if (!vr && WindowIsInVR()) {
    Rect rc;
    GetDefaultWindowPos(rc);
    SetWindowGeometry(rc);
  }
}

void LandExWindow::AddLine(const std::string& line) {
  lines_.emplace_back(line);

  while (lines_.size() > kLineCountLimit) {
    lines_.erase(lines_.begin());
  }
}

void LandExWindow::GetDefaultWindowPos(Rect& rc) {
  Rect rcScreenBounds;
  XPLMScreen::GetBoundsGlobal(rcScreenBounds);
  rc.left = rcScreenBounds.left + kDefWindowPos.x;
  rc.bottom = rcScreenBounds.bottom + kDefWindowPos.y;
  rc.right = rc.left + kDefWindowSize.width;
  rc.top = rc.bottom + kDefWindowSize.height;

  LOG(INFO, absl::StrCat("LandExWindow::GetDefaultWindowPos: lbrt=", rc.ToString()));
}

void LandExWindow::OnDrawWindow() {
  ::XPLMSetGraphicsState(
      0 /* no fog */,
      0 /* 0 texture units */,
      0 /* no lighting */,
      0 /* no alpha testing */,
      1 /* do alpha blend */,
      1 /* do depth testing */,
      0 /* no depth writing */
  );

  Rect rc;
  GetWindowGeometry(rc);

  int glide_slope_bottom = rc.Center().y;

  RectF rc_glide_slope(
    static_cast<float>(rc.left),
    static_cast<float>(rc.top),
    static_cast<float>(rc.right),
    static_cast<float>(glide_slope_bottom));

  GlideSlope glide_slope(rc_glide_slope);
  glide_slope.Draw();

  int char_width, char_height;
  ::XPLMGetFontDimensions(xplmFont_Proportional, &char_width, &char_height, nullptr);

  int line_height = char_height + char_height / 4;
  int text_height = line_height * static_cast<int>(lines_.size());

  int x = rc.left;
  int y = rc.bottom + text_height - line_height;
  for (const std::string& line : lines_) {
    if (y + line_height < glide_slope_bottom) {
      static float clr_white[] = { 1.0, 1.0, 1.0 };
      ::XPLMDrawString(clr_white, x, y, const_cast <char*>(line.c_str()), nullptr, xplmFont_Proportional);
    }

    y -= line_height;
  }
}

}  // namespace xplmpp
