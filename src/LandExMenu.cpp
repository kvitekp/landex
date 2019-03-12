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
// Plugin menu implementation.

#include "LandExMenu.h"

#include "xplmpp/XPLMScreen.h"
#include "xplmpp/XPLMLog.h"
#include "xplmpp/Rect.h"

namespace xplmpp {

/*
 * LandEx plugin window implementation.
 */
LandExMenu::LandExMenu(CmdHandler* cmd_handler)
: cmd_handler_(cmd_handler)
, cmd_show_window_(this)
, cmd_clear_window_(this) {
}

LandExMenu::~LandExMenu() {
}

bool LandExMenu::Create() {
  if (!XPLMMenu::Create("LandEx"))
    return false;

  AppendMenuItemWithCommand("Show Window",
      cmd_show_window_.Create("LandEx/show_window", "Show Window"));

  AppendMenuItemWithCommand("Clear Window",
      cmd_clear_window_.Create("LandEx/clear_window", "Clear Window"));

  return true;
}

void LandExMenu::Destroy() {
  DestroyMenu();
}

bool LandExMenu::OnCommand(XPLMCommandRef cmd_ref, XPLMCommandPhase phase) {
  assert(cmd_handler_);

  if (phase != xplm_CommandBegin)
    return false;

  if (cmd_ref == cmd_show_window_.ref()) {
    cmd_handler_->OnCommand(Cmd::showWindow);
  } else
  if (cmd_ref == cmd_clear_window_.ref()) {
    cmd_handler_->OnCommand(Cmd::clearWindow);
  }

  return false;
}

}  // namespace xplmpp
