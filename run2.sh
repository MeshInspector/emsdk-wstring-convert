set -u; cd /src
emcc --version | head -1
NODE=$(ls -d /emsdk/node/*/bin/node | head -1)
CC=/emsdk/upstream/emscripten/node_modules/google-closure-compiler
pkgver() { grep -o '"version": *"[^"]*"' "$1/package.json" 2>/dev/null | cut -d'"' -f4; }
ver() { echo "launcher=$(pkgver $CC) java=$(pkgver $CC-java) linux=$(pkgver $CC-linux) selfreport=$($NODE $CC/cli.js --version 2>&1 | tr '\n' ' ' | cut -c1-100)"; }
echo "RESULT version=$VERSION before-swap: $(ver)"
em++ hello2.cpp -O2 -sEXPORT_ES6=1 --closure=1 -Wall -o r.mjs > /tmp/r.log 2>&1
echo "RESULT version=$VERSION native wrong_count=$(grep -c JSC_WRONG_ARGUMENT_COUNT /tmp/r.log)"
OTHER=20240317.0.0; [ "$VERSION" = "5.0.7" ] && OTHER=20260726.0.0
(cd /emsdk/upstream/emscripten && npm install --no-save --no-audit --no-fund google-closure-compiler@$OTHER > /tmp/npm.log 2>&1) || cat /tmp/npm.log
echo "RESULT version=$VERSION after-swap: $(ver)"
em++ hello2.cpp -O2 -sEXPORT_ES6=1 --closure=1 -Wall -o x.mjs > /tmp/x.log 2>&1
echo "RESULT version=$VERSION swapped wrong_count=$(grep -c JSC_WRONG_ARGUMENT_COUNT /tmp/x.log) $(grep -o '[0-9.]*% typed' /tmp/x.log | tail -1)"
grep -m2 'WARNING - \[JSC' /tmp/x.log | cut -c1-200
exit 0
