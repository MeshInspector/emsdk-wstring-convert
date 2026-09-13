// Stands in for MeshLib's MRPch.h: the real build pulls <locale> in through the PCH
// (MRPch.h -> MREigen.h -> Eigen/Core -> <complex> -> <sstream> -> <istream> -> <locale>)
// rather than from the translation unit itself.
#pragma once

#include <codecvt>
#include <locale>
#include <sstream>
#include <string>
