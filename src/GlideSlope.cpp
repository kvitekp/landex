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

#include "xplmpp/XPLMLog.h"

#include "FlightData.h"

namespace xplmpp {

static const float kFrameClr[] = { 1.0f, 1.0f, 1.0f, 0.5f };
static const float kSlopeClrOuter[] = { 1.0f, 1.0f, 1.0f, 0.2f };
static const float kSlopeClrInner[] = { 1.0f, 1.0f, 1.0f, 0.25f };
static const float kSlopeClrCenter[] = { 1.0f, 1.0f, 1.0f, 0.3f };
static const float kSlopeClrPath[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
static const float kSlopeClrPath2[] = { 0.0f, 1.0f, 0.0f, 1.0f };

static const float kSlopeMargin = 0.005f;
static const float kSlopeLeftOffset = 0.05f;
static const float kSlopeTopOffset = 0.05f;
static const float kSlopeRightHeight = 0.25f;

static const float kPtDifferenceThreshold = 0.5f;

// Standard slopes 3nm 1000ft AGL point in meters. Corresponds to our center
// right point of the slope.
static const PointF kStandardSlopeTop(3 * 1852.0f, 1000 * 0.3048f);

namespace {

template<typename T>
T Scale(T v0, T v0min, T v0max, T v1min, T v1max) {
  if (v0max == v0min)
    return v1max;

  return v1min + ((v0 - v0min) * (v1max - v1min)) / (v0max - v0min);
}

PointF PosToWindow(const PointF& pos, const RectF& rc) {
  PointF pt;
  pt.x = Scale(pos.x, 0.0f, kStandardSlopeTop.x, rc.left, rc.right);
  pt.y = Scale(pos.y, 0.0f, kStandardSlopeTop.y, rc.bottom, rc.top);
  return pt;
}

bool PtDifference(const PointF& pt, const PointF& pt2) {
  return (fabs(pt2.x - pt.x) > kPtDifferenceThreshold ||
          fabs(pt2.y - pt.y) > kPtDifferenceThreshold);
}

}  // namespace

GlideSlope::GlideSlope(const RectF& rc)
: rc_(rc) {
}

GlideSlope::~GlideSlope() {
}

void GlideSlope::Draw() {
  DrawFrame();

  // Figure out slope geometry
  RectF rc = rc_;
  rc.Deflate(rc_.Width() * kSlopeMargin, rc_.Height() * kSlopeMargin);

  PointF slope_left(rc.left + rc_.Width() * kSlopeLeftOffset, rc.bottom);

  float slope_top = rc.top - rc_.Height() * kSlopeTopOffset;
  float slope_right_height = rc_.Height() * kSlopeRightHeight;
  float slope_bottom = slope_top - slope_right_height;
  float slope_inner_offset = slope_right_height / 3;
  float slope_center_right = slope_top - slope_right_height / 2;

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
  glColor4fv(kSlopeClrCenter);
  glBegin(GL_LINE_STRIP);
  glVertex2(slope_left);
  glVertex2f(rc.right, slope_center_right);
  glEnd();

  // Draw slope if we have flight data
  if (g_flight_data.HasLanding()) {
    RectF rc_slope;
    rc_slope.left = slope_left.x;
    rc_slope.bottom = slope_left.y;
    rc_slope.right = rc.right;
    rc_slope.top = slope_center_right;
    DrawSlope(rc_slope);
  }
}

void GlideSlope::DrawFrame() {
  glLineWidth(1.0);
  glColor4fv(kFrameClr);
  glBegin(GL_LINE_LOOP);
  glVertex2(rc_);
  glEnd();
}

void GlideSlope::DrawSlope(const RectF& rc) {
  FlightData::const_iterator it_landing;
  if (!g_flight_data.GetLanding(it_landing))
    return;

  //LOG(INFO, absl::StrCat("GlideSlope::DrawSlope: rc=", rc.ToString(), " flight_data.size=", g_flight_data.size()));

  // The landing point corresponds to the bottom left point of the standard
  // slope rectangle, so walk the flight data back in time to draw the flight
  // path before langing.
  { glColor4fv(kSlopeClrPath);
    glBegin(GL_LINE_STRIP);

    PointF pos(0);
    float prev_time = it_landing->time;
    float prev_ground_speed = it_landing->ground_speed;
    PointF prev_pt(rc.BottomLeft());
    glVertex2(prev_pt);

    for (FlightData::const_iterator it = --it_landing; it != g_flight_data.cbegin(); --it) {
      float ground_speed = (prev_ground_speed + it->ground_speed) / 2;
      float distance = (prev_time - it->time) * ground_speed;
      pos.x += distance;
      pos.y = it->agl;

      PointF pt = PosToWindow(pos, rc);
      if (!rc_.PtInRect(pt))
        break;

      //LOG(INFO, absl::StrCat("GlideSlope::DrawSlope:<pos=", pos.ToString(), " pt=", pt.ToString()));

      if (PtDifference(prev_pt, pt)) {
        glVertex2(pt);
        prev_pt = pt;
      }

      prev_ground_speed = it->ground_speed;
      prev_time = it->time;
    }

    glEnd();
  }

  // Now walk the flight data forward from the landing moment to draw
  // the flight path after landing.
  { glColor4fv(kSlopeClrPath2);
    glBegin(GL_LINE_STRIP);

    PointF pos(0);
    float prev_time = it_landing->time;
    float prev_ground_speed = it_landing->ground_speed;
    PointF prev_pt(rc.BottomLeft());
    glVertex2(prev_pt);

    for (FlightData::const_iterator it = it_landing; it != g_flight_data.cend(); ++it) {
      float ground_speed = (prev_ground_speed + it->ground_speed) / 2;
      float distance = (it->time - prev_time) * ground_speed;
      pos.x -= distance;
      pos.y = it->agl;

      PointF pt = PosToWindow(pos, rc);
      if (!rc_.PtInRect(pt))
        break;

      //LOG(INFO, absl::StrCat("GlideSlope::DrawSlope:>pos=", pos.ToString(), " pt=", pt.ToString()));

      if (PtDifference(prev_pt, pt)) {
        glVertex2(pt);
        prev_pt = pt;
      }

      prev_ground_speed = it->ground_speed;
      prev_time = it->time;
    }

    glEnd();
  }


}

}  // namespace xplmpp
