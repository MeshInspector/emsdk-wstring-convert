// Like pch.h, but <locale> is first pulled in INSIDE a suppressed region, the way MeshLib's
// MRPch reaches it (MRHashMap.h / MRSpdlog.h ignore -Wdeprecated-declarations around their
// includes on emscripten). Clang records diagnostic state per source location, at the point a
// line is first lexed - so this asks whether that recorded state is what used to suppress the
// wstring_convert deprecation, and whether splitting the header out of <locale> changed it.
#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <codecvt>
#include <locale>
#include <sstream>
#pragma clang diagnostic pop

#include <string>
