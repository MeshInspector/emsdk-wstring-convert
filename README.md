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
