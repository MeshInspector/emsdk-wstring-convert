// Does std::wstring_convert warn through the in-source pragma, and since which emsdk?
//
// This is MeshLib's source/MRMesh/MRStringConvert.cpp reduced to the part that matters:
// the two conversion functions, wrapped in exactly the suppression MeshLib wraps them in
// (MR_SUPPRESS_WARNING expands to _Pragma("clang diagnostic ignored ...") on clang).
#include <codecvt>
#include <locale>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

std::wstring utf8ToWide( const char* utf8 )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes( utf8 );
}

std::string wideToUtf8( const wchar_t* wide )
{
    if ( !wide )
        return {};
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
    return conv.to_bytes( wide );
}

#pragma clang diagnostic pop

int main()
{
    return int( utf8ToWide( "x" ).size() + wideToUtf8( L"x" ).size() );
}
