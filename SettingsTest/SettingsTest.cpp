#include "pch.h"

#include <iostream>

#include "xplmpp/Log.h"

#include "Settings.h"

// Force abseil libraries
#pragma comment(lib, "absl_base")
#pragma comment(lib, "absl_strings")
#pragma comment(lib, "absl_internal_throw_delegate")

using namespace xplmpp;

int main() {
  Settings settings;
  if (!settings.Load("settings.prf"))
    return 1;

  assert(settings.runway_distance() == 0.5 * Settings::kNmToMeters);
  assert(settings.approach_distance() == 3 * Settings::kNmToMeters);

  LOG(VERBOSE) << "DONE!";

  return 0;
}
