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
// Plugin menu.

#ifndef LANDEX_MENU_H
#define LANDEX_MENU_H

#include "Common.h"

#include "xplmpp/XPLMMenu.h"
#include "xplmpp/XPLMCommand.h"

#include "LandExCmdHandler.h"

namespace xplmpp {

// Represents the plugin menu
class LandExMenu : public XPLMMenu,
                   public XPLMCommand::Handler {
public:
  LandExMenu(CmdHandler* cmd_handler);
  ~LandExMenu();

  bool Create();
  void Destroy();

private:

  bool OnCommand(XPLMCommandRef cmd_ref, XPLMCommandPhase phase) override;

  XPLMCommand cmd_show_window_;
  XPLMCommand cmd_clear_window_;

  CmdHandler* cmd_handler_;
};

}  // namespace xplmpp

#endif  // #ifndef LANDEX_MENU_H
