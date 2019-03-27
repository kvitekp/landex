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
static const float kSlopeClrGrid[] = { 1.0f, 1.0f, 1.0f, 0.2f };
static const float kSlopeClrOuter[] = { 1.0f, 1.0f, 1.0f, 0.2f };
static const float kSlopeClrInner[] = { 1.0f, 1.0f, 1.0f, 0.25f };
static const float kSlopeClrCenter[] = { 1.0f, 1.0f, 1.0f, 0.3f };
static const float kSlopeClrPath[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
static const float kSlopeClrPath2[] = { 0.0f, 1.0f, 0.0f, 1.0f };

static const float kViewMargin = 0.005f;
static const float kSlopeTopOffset = 0.05f;
static const float kSlopeHeight = 0.25f;

static const float kPtDifferenceThreshold = 0.5f;

static const float kTan3 = 0.05240778f;
static const float kFtToMeters = 0.3048f;
static const float kNmToMeters = 1852.0f;

static const float kRunwayDistance = 0.5 * kNmToMeters;
static const float kApproachDistance = 1.5 * kNmToMeters;

static const float kVGrid = 1 * kNmToMeters;
static const float kHGrid = 500 * kFtToMeters;

namespace {

template<typename T>
T Scale(T v0, T v0min, T v0max, T v1min, T v1max) {
  if (v0max == v0min)
    return v1max;

  return v1min + ((v0 - v0min) * (v1max - v1min)) / (v0max - v0min);
}

float DistanceToHeight(float distance) {
  return distance * kTan3;
}

bool PtDifference(const PointF& pt, const PointF& pt2) {
  return (fabs(pt2.x - pt.x) > kPtDifferenceThreshold ||
          fabs(pt2.y - pt.y) > kPtDifferenceThreshold);
}

}  // namespace

GlideSlope::GlideSlope(const RectF& rc)
: rc_(rc) {
  // Calculate view rectangle: frame rectangle sans view margin.
  rc_view_ = rc_;
  rc_view_.Deflate(rc_.Width() * kViewMargin, rc_.Height() * kViewMargin);

  // Calculate slope rectangle with bottom left at landing point and
  // top right at slope center on the right.
  slope_height_ = rc_view_.Height() * kSlopeHeight;

  rc_slope_.left = rc_view_.left +
      kRunwayDistance * rc_view_.Width() / (kRunwayDistance + kApproachDistance);
  rc_slope_.bottom = rc_view_.bottom;
  rc_slope_.right = rc_view_.right;
  rc_slope_.top =
      rc_view_.top - rc_view_.Height() * kSlopeTopOffset - slope_height_ / 2;

  // Calculate slope right center point in world coordinates
  slope_right_.x = kApproachDistance;
  slope_right_.y = DistanceToHeight(kApproachDistance);

  //LOG(INFO) << "GlideSlope::ctor: slope_right=" << slope_right_;
}

GlideSlope::~GlideSlope() {
}

void GlideSlope::Draw() {
  glLineWidth(1.0);

  DrawFrame();
  DrawGrid();
  DrawSlope();
  DrawFlightPath();
}

void GlideSlope::DrawFrame() {
  glColor4fv(kFrameClr);
  glBegin(GL_LINE_LOOP);
  glVertex2(rc_);
  glEnd();
}

void GlideSlope::DrawGrid() {
  glColor4fv(kSlopeClrGrid);
  glBegin(GL_LINES);

  // Draw vertical grid lines every 1 nm
  float v_grid = WorldToWindowX(kVGrid) - WorldToWindowX(0);
  for (float x = rc_slope_.left; x <= rc_view_.right; x += v_grid) {
    glVertex2f(x, rc_view_.bottom);
    glVertex2f(x, rc_view_.top);
  }

  // Draw horizontal grid lines every 100 ft
  float h_grid = WorldToWindowY(kHGrid) - WorldToWindowY(0);
  for (float y = rc_slope_.bottom; y <= rc_view_.top; y += h_grid) {
    glVertex2f(rc_view_.left, y);
    glVertex2f(rc_view_.right, y);
  }

  glEnd();
}

void GlideSlope::DrawSlope() {
  // Draw outer slope area
  glColor4fv(kSlopeClrOuter);
  glBegin(GL_POLYGON);
  glVertex2(rc_slope_.BottomLeft());
  glVertex2f(rc_slope_.right, rc_slope_.top + slope_height_ / 2);
  glVertex2f(rc_slope_.right, rc_slope_.top - slope_height_ / 2);
  glEnd();

  // Draw inner slope area
  glColor4fv(kSlopeClrInner);
  glBegin(GL_POLYGON);
  glVertex2(rc_slope_.BottomLeft());
  glVertex2f(rc_slope_.right, rc_slope_.top + slope_height_ / 6);
  glVertex2f(rc_slope_.right, rc_slope_.top - slope_height_ / 6);
  glEnd();

  // Draw slope center line
  glColor4fv(kSlopeClrCenter);
  glBegin(GL_LINE_STRIP);
  glVertex2(rc_slope_.BottomLeft());
  glVertex2(rc_slope_.TopRight());
  glEnd();
}

void GlideSlope::DrawFlightPath() {
  FlightData::const_iterator it_landing;
  if (!g_flight_data.GetLanding(it_landing))
    return;

  LOG(INFO) << "GlideSlope::DrawFlightPath: flight_data.size=", g_flight_data.size();

  // The landing point corresponds to the bottom left point of the standard
  // slope rectangle, so walk the flight data back in time to draw the flight
  // path before landing.
  { glColor4fv(kSlopeClrPath);
    glBegin(GL_LINE_STRIP);

    PointF pos(0);
    float prev_time = it_landing->time;
    float prev_ground_speed = it_landing->ground_speed;
    PointF prev_pt(rc_slope_.BottomLeft());
    glVertex2(prev_pt);

    for (FlightData::const_iterator it = --it_landing; it != g_flight_data.cbegin(); --it) {
      float ground_speed = (prev_ground_speed + it->ground_speed) / 2;
      float distance = (prev_time - it->time) * ground_speed;
      pos.x += distance;
      pos.y = it->agl;

      PointF pt = WorldToWindow(pos);
      if (!rc_.PtInRect(pt))
        break;

      LOG(INFO) << "GlideSlope::DrawFlightPath: pos=" << pos << " pt=" << pt;

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
    PointF prev_pt(rc_slope_.BottomLeft());
    glVertex2(prev_pt);

    for (FlightData::const_iterator it = it_landing; it != g_flight_data.cend(); ++it) {
      float ground_speed = (prev_ground_speed + it->ground_speed) / 2;
      float distance = (it->time - prev_time) * ground_speed;
      pos.x -= distance;
      pos.y = it->agl;

      PointF pt = WorldToWindow(pos);
      if (!rc_.PtInRect(pt))
        break;

      LOG(INFO) << "GlideSlope::DrawFlightPath: pos=" << pos << " pt=" << pt;

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

float GlideSlope::WorldToWindowX(float x) {
  return Scale(x, 0.0f, slope_right_.x, rc_slope_.left, rc_slope_.right);
}

float GlideSlope::WorldToWindowY(float y) {
  return Scale(y, 0.0f, slope_right_.y, rc_slope_.bottom, rc_slope_.top);
}

PointF GlideSlope::WorldToWindow(const PointF& pt) {
  return PointF(WorldToWindowX(pt.x), WorldToWindowY(pt.y));
}

}  // namespace xplmpp
