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
// Command handler interface.

#ifndef LANDEX_CMDHANDLER_H
#define LANDEX_CMDHANDLER_H

namespace xplmpp {

// Plugin commands
enum class Cmd {
  showWindow = 1,
  clearWindow,
};

// Plugin command handler interface.
struct CmdHandler {
  virtual void OnCommand(Cmd cmd) = 0;
};


}  // namespace xplmpp

#endif  // #ifndef LANDEX_CMDHANDLER_H
