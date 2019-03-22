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
// Common declarations.

#ifndef LANDEX_COMMON_H
#define LANDEX_COMMON_H

// Disable annoying MSVC warnings at /W4
#ifdef _MSC_VER
#pragma warning(disable:4100) /* unreferenced formal parameter */
#pragma warning(disable:4127) /* conditional expression is constant */
#endif // _MSC_VER

// Include abseil headers
#include "absl/strings/str_cat.h"

namespace xplmpp {

}  // namespace xplmpp

#endif  // #ifndef LANDEX_COMMON_H
