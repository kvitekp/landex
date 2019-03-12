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
// Main plugin module implementation.

#include "LandExPlugin.h"

#include <sstream>

#include "xplmpp/XPLMLog.h"

namespace xplmpp {

#define ACTIVATE_PLUGIN_ERROR_CALLBACK 1

namespace {

float RoundOff(float value, float factor = 10.0f) {
  return rintf(value * factor) / factor;
}

float MetersPerSecondToFeetPerMinute(float meters_per_second) {
  return meters_per_second * 3.28084f * 60.0f;
}

float MetersPerSecondToKnots(float meters_per_second) {
  return meters_per_second * 1.94384f;
}

float MetersToFeet(float meters) {
  return meters * 3.28084f;
}

std::string LandingQuality(float vy) {
  if (vy < 0.25f) return "EXCELLENT LANDING"; // ~50 fpm
  if (vy < 0.50f) return "GREAT LANDING";
  if (vy < 1.00f) return "GOOD LANDING";
  if (vy < 1.50f) return "ACCEPTABLE LANDING";
  if (vy < 2.00f) return "HARD LANDING";
  if (vy < 2.50f) return "BAD LANDING";
  if (vy < 3.00f) return "ANY SURVIVORS?";

  return "R.I.P.";
}

}  // namespace

/*
 * LandEx plugin implementation.
 */
LandExPlugin::LandExPlugin()
: name_("LandEx")
, signature_("com.kvitek.x-plane.landex")
, description_("A plugin that displays landing statistics.")
, menu_(this) {
  g_log.set_prefix("LandEx: ");
}

LandExPlugin::~LandExPlugin() {
}

bool LandExPlugin::OnStart(char* name, char* signature, char* description) {
  LOG(INFO, "LandExPlugin::OnStart: ");

#if ACTIVATE_PLUGIN_ERROR_CALLBACK
  error_callback_ = std::make_unique<XPLMErrorCallback>(this);
#endif

  strcpy_s(name, kMaxOnStartStringLength, name_.c_str());
  strcpy_s(signature, kMaxOnStartStringLength, signature_.c_str());
  strcpy_s(description, kMaxOnStartStringLength, description_.c_str());

  vr_enabled_.Find("sim/graphics/VR/enabled");

  return true;
}

bool LandExPlugin::OnEnable() {
  LOG(INFO, "LandExPlugin::OnEnable: ");

  return Init();
}

void LandExPlugin::OnDisable() {
  LOG(INFO, "LandExPlugin::OnDisable: ");

  Quit();
}

void LandExPlugin::OnStop() {
  LOG(INFO, "LandExPlugin::OnStop: ");

  Quit();
}

void LandExPlugin::OnReceiveMessage(XPLMPluginID from, int msg, void* param) {
  LOG(INFO, "LandExPlugin::OnReceiveMessage: ");

  if (from == XPLM_PLUGIN_XPLANE) {
    switch (msg) {
      case XPLM_MSG_ENTERED_VR:
        window_.UpdateOnVRChange(true);
        break;
      case XPLM_MSG_EXITING_VR:
        window_.UpdateOnVRChange(false);
        break;
    }
  }
}

void LandExPlugin::OnCommand(Cmd cmd) {
  LOG(INFO, "LandExPlugin::OnCommand: ");

  switch (cmd) {
    case Cmd::showWindow:
      window_.Show();
      break;
    case Cmd::clearWindow:
      window_.Clear();
      break;
  }
}

void LandExPlugin::OnAirplaneFlying(const FlyingInfo& info) {
  std::stringstream sst;
  sst << "Flying:   "
      << "  Vy=" << RoundOff(MetersPerSecondToFeetPerMinute(info.vertical_speed)) << " fpm"
      << "  Vg=" << RoundOff(MetersPerSecondToKnots(info.ground_speed)) << " kts"
      << "  Agl=" << RoundOff(MetersToFeet(info.agl)) << " ft"
    ;
  window_.AddLine(sst.str());
}

void LandExPlugin::OnAirplaneLanded(const LandingInfo& info) {
  std::stringstream sst;
  sst << "Landed: "
      << "  Vy=" << RoundOff(MetersPerSecondToFeetPerMinute(info.vertical_speed)) << " fpm"
      << "  Vg=" << RoundOff(MetersPerSecondToKnots(info.ground_speed)) << " kts"
      << "  G=" << RoundOff(info.gforce) << " m/sec^2"
      << "    " << LandingQuality(fabs(info.vertical_speed))
      ;
  window_.AddLine(sst.str());
}

void LandExPlugin::OnPluginError(const char* error) {
  LOG(ERROR, error);
}

bool LandExPlugin::Init() {
  if (!window_.Create(IsVREnabled())) {
    LOG(FATAL, "Could not create the window.");
    return false;
  }

  if (!menu_.Create()) {
    LOG(FATAL, "Could not create the menu.");
    return false;
  }

  flight_loop_ = FlightLoop::Create(this);

  return true;
}

void LandExPlugin::Quit() {
  flight_loop_.reset(nullptr);
  window_.Destroy();
  menu_.Destroy();
}

bool LandExPlugin::IsVREnabled() {
  return !!vr_enabled_.GetDatai();
}

/*
 * LandEx plugin factory implementation.
 */
std::unique_ptr<XPLMPlugin> LandExPluginFactory::CreatePlugin() {
  return std::make_unique<LandExPlugin>();
}

std::unique_ptr<XPLMPluginFactory> g_plugin_factory = std::make_unique<LandExPluginFactory>();

}  // namespace xplmpp
