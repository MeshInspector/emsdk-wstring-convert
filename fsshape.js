// fsshape.js — the shape libfs.js uses
var FS = {
  initialized: false,
  chmod(path, mode, dontFollow) { return [path, mode, dontFollow]; },
  mkdev(path, mode, dev) { return [path, mode, dev]; },
  init(input, output, error) { FS.initialized = true; return [input, output, error]; },
};
if (!FS.initialized) FS.init();
globalThis.out = FS;
