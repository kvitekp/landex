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
// Main plugin module.

#ifndef LANDEX_PLUGIN_H
#define LANDEX_PLUGIN_H

#include "Common.h"

#include "xplmpp/XPLMPlugin.h"
#include "xplmpp/XPLMPluginFactory.h"
#include "xplmpp/XPLMErrorCallback.h"
#include "xplmpp/XPLMCommand.h"
#include "xplmpp/XPLMData.h"

#include "LandExWindow.h"
#include "LandExMenu.h"
#include "LandExCmdHandler.h"
#include "FlightLoop.h"

namespace xplmpp {

// Represents the plugin
class LandExPlugin : public XPLMPlugin,
                     public CmdHandler,
                     public FlightLoopClient,
                     public XPLMErrorCallback::Handler {
public:
  LandExPlugin();
  ~LandExPlugin() override;

private:
  // XPLMPlugin interface.
  bool OnStart(char* name, char* signature, char* description) override;
  bool OnEnable() override;
  void OnDisable() override;
  void OnStop() override;
  void OnReceiveMessage(XPLMPluginID from, int msg, void* param) override;

  // CmdHandler interface
  void OnCommand(Cmd cmd) override;

  // FlightLoopClient interface
  void OnAirplaneFlying(const FlyingInfo& info) override;
  void OnAirplaneLanded(const LandingInfo& info) override;

  // XPLMErrorCallback::Handler
  void OnPluginError(const char* error) override;

  bool Init();
  void Quit();

  bool IsVREnabled();

  std::string name_;
  std::string signature_;
  std::string description_;

  LandExWindow window_;
  LandExMenu menu_;

  std::unique_ptr<FlightLoop> flight_loop_;
  std::unique_ptr<XPLMErrorCallback> error_callback_;

  XPLMData vr_enabled_;
};

// Implements the plugin factory
class LandExPluginFactory : public XPLMPluginFactory {
 public:
  std::unique_ptr<XPLMPlugin> CreatePlugin() override;
};

}  // namespace xplmpp

#endif  // #ifndef LANDEX_PLUGIN_H
