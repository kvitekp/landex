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
// Glide slope drawing.

#ifndef LANDEX_GLIDESLOPE_H
#define LANDEX_GLIDESLOPE_H

#include "Common.h"

#include "xplmpp/XPLMScreen.h"
#include "xplmpp/Rect.h"

namespace xplmpp {

// Represents the glide slope
class GlideSlope {
public:
  GlideSlope(const RectF& rc);
  ~GlideSlope();

  void Draw();

private:
  void DrawFrame();
  void DrawInfo();
  void DrawGrid();
  void DrawSlope();
  void DrawFlightPath();

  float WorldToWindowX(float x);
  float WorldToWindowY(float y);
  PointF WorldToWindow(const PointF& pt);

  RectF rc_;       // Caller's rectangle (frame)
  RectF rc_view_;  // View rectangle (caller's rectangle sans view margins)
  RectF rc_slope_; // Slope rectangle (BL=landing point, TR = slope top center)

  float slope_height_; // Slope triangle height on the right (window)
  PointF slope_right_; // Slope center right in world coordinates
};

}  // namespace xplmpp

#endif  // #ifndef LANDEX_GLIDESLOPE_H
