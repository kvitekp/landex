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
// Glide slope drawing implementation.

#include "GlideSlope.h"

namespace xplmpp {

static const float kFrameClr[] = { 1.0f, 1.0f, 1.0f, 0.5f };
static const float kSlopeClrOuter[] = { 1.0f, 1.0f, 1.0f, 0.2f };
static const float kSlopeClrInner[] = { 1.0f, 1.0f, 1.0f, 0.25f };
static const float kSlopeClrCenter[] = { 1.0f, 1.0f, 1.0f, 0.3f };

static const float kSlopeMargin = 0.005f;
static const float kSlopeLeftOffset = 0.05f;
static const float kSlopeTopOffset = 0.05f;
static const float kSlopeRightHeight = 0.25f;

GlideSlope::GlideSlope(const RectF& rc)
: rc_(rc) {
}

GlideSlope::~GlideSlope() {
}

void GlideSlope::Draw() {
  DrawFrame();
  DrawSlope();
}

void GlideSlope::DrawSlope() {
  RectF rc = rc_;
  rc.Deflate(rc_.Width() * kSlopeMargin, rc_.Height() * kSlopeMargin);

  PointF slope_left(rc.left + rc_.Width() * kSlopeLeftOffset, rc.bottom);

  float slope_top = rc.top - rc_.Height() * kSlopeTopOffset;
  float slope_right_height = rc_.Height() * kSlopeRightHeight;
  float slope_bottom = slope_top - slope_right_height;
  float slope_inner_offset = slope_right_height / 3;
  float slope_center_offset = slope_right_height / 2;

  // Draw outer slope area
  glColor4fv(kSlopeClrOuter);
  glBegin(GL_POLYGON);
  glVertex2(slope_left);
  glVertex2f(rc.right, slope_top);
  glVertex2f(rc.right, slope_bottom);
  glEnd();

  // Draw inner slope area
  glColor4fv(kSlopeClrInner);
  glBegin(GL_POLYGON);
  glVertex2(slope_left);
  glVertex2f(rc.right, slope_top - slope_inner_offset);
  glVertex2f(rc.right, slope_bottom + slope_inner_offset);
  glEnd();

  // Draw center slope line
  glLineWidth(2.0);
  glColor4fv(kSlopeClrCenter);
  glBegin(GL_LINE_STRIP);
  glVertex2(slope_left);
  glVertex2f(rc.right, slope_top - slope_center_offset);
  glVertex2f(rc.right, slope_bottom + slope_center_offset);
  glEnd();
}

void GlideSlope::DrawFrame() {
  glLineWidth(1.0);
  glColor4fv(kFrameClr);
  glBegin(GL_LINE_LOOP);
  glVertex2(rc_);
  glEnd();
}

}  // namespace xplmpp
