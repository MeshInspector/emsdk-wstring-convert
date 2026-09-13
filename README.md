# emsdk wstring_convert deprecation probe

Bumping MeshLib/MeshInspector from emsdk 4.0.19 to 5.0.7 made this fail:

```
__locale_dir/wstring_convert.h:179:1: error: 'wstring_convert<std::codecvt_utf8<wchar_t>>'
    is deprecated [-Werror,-Wdeprecated-declarations]
  note: in instantiation of member function 'to_bytes' requested here
  MRStringConvert.cpp:28: note: in instantiation of member function 'to_bytes' requested here
```

even though the call sites are wrapped in `#pragma clang diagnostic ignored
"-Wdeprecated-declarations"`. The pragma cannot reach it: libc++ reports the diagnostic against
its own header, at the declarator of the out-of-line `to_bytes` definition, not against our call.

We fixed it with a per-file `-Wno-deprecated-declarations`, then went looking for the upstream
change that made it start happening. Source inspection did not find one:

- emscripten's vendored libc++ has the **same** construct in both versions - the out-of-line
  `to_bytes`, the same `_LIBCPP_DEPRECATED_IN_CXX17` macro chain, the same `#pragma GCC
  system_header`. Only the file split (`<locale>` -> `__locale_dir/wstring_convert.h`) and a
  dropped `_LIBCPP_TEMPLATE_VIS` differ.
- upstream clang **21.1 and 22.1 both warn** on the same code through libc++, and today's trunk
  does not. So the diagnostic is not new in the LLVM 23 that emsdk 5.0.7 ships; if anything it
  was removed afterwards.

So the premise needed testing rather than more reading. This repo compiles that one translation
unit under every emsdk version across the range, with MeshLib's own warning flags, and reports
whether the warning appears and whether `-Werror` rejects it.

Read the results by grepping the run log for `RESULT` and `RESULT_WERROR`.

## Result

Not a toolchain change at all. Every emsdk from 4.0.19 to 6.0.9 behaves identically:

| how the TU is compiled | 4.0.19 ... 6.0.9 |
| --- | --- |
| plain | warns, `-Werror` rejects |
| through a PCH | warns, `-Werror` rejects |
| through a PCH that included `<locale>` inside a `-Wdeprecated-declarations`-ignored region | **clean** |

So whether the warning appears depends on where the header is *first lexed*, not on the
compiler version: clang records diagnostic state per source location, and a header first pulled
in inside an ignored region stays ignored for the rest of the build.

That is what MeshLib was relying on without knowing it. Its PCH reaches `<locale>` through
`MRHashMap.h` / `MRSpdlog.h`, which ignore `-Wdeprecated-declarations` around their includes on
emscripten. Two libc++ commits decide the outcome:

- **llvm/llvm-project#146650** ([ca697096a](https://github.com/llvm/llvm-project/commit/ca697096a),
  2025-07-05) granularized `<locale>`, moving `wstring_convert` into
  `__locale_dir/wstring_convert.h`. emsdk 4.0.19 vendors a libc++ from before that split, so the
  definition was still inside the file the PCH had stamped as ignored. From emsdk 5.0.0 the split
  is in, the header is first reached elsewhere (Eigen -> `<complex>` -> `<sstream>` -> `<istream>`),
  and the warning fires at the out-of-line `to_bytes` declarator - where a call-site pragma cannot
  reach it.
- **llvm/llvm-project#201633** (merged 2026-06-04) fixes it properly, wrapping that definition in
  `_LIBCPP_SUPPRESS_DEPRECATED_PUSH/POP`. Its description is our exact complaint. emsdk 6.0.9 has
  the macro around the destructor and `from_bytes` but **not** around `to_bytes`, which is why
  6.0.9 still warns here.

MeshLib therefore needs `-Wno-deprecated-declarations` on `MRStringConvert.cpp` for emsdk 5.0.0
through at least 6.0.9, and can drop it once a vendored libc++ carries #201633.
