# Private bootstrap ABI inputs — byte candidates, UNRUN

The build-plane tuple is Node22.23.2, upstream commit
`aa4c77582be995286fc6e00aaf530dc7ade102a9`, tree
`cee5f56eb6b8b6b8e300419196dc90e8c7abd64b`. This is separate from the two
source-built product runtimes. The original headers declare the ordinary default
Node-API8 and `BUILDING_NODE_EXTENSION` selects Windows dllimport. No ABI or loaded
source authority is inferred from these declarations or public checksums.
The original version header declares supported API range1..10 and default8.
The fixed surface includes `napi_get_version`; actual observation remains a
post-loaded-source call, never a reason to call an unqualified module.

Full original raw source is retained read-only under the legacy checkout's
`native/node-runtime/source-build-inputs/current-node-originals33`; its complete
raw-corpus manifest records43494 blobs/627103490 logical bytes. It is not yet a
materialized, qualified build tree. Do not duplicate its full source or archives.

| Original header | Raw body | Bytes | Fresh SHA256 |
| --- | --- | ---: | --- |
| src/node_api.h | raw/033558.body | 10370 | 2d4560831e525b47b060ec8a0864ab73993df9e20215ce9f0fe7b24cd31af32a |
| src/js_native_api.h | raw/033538.body | 31770 | 582e64ead4d5fdf663e2bd934f2fc97e8e155a7740d28995830393d19d089705 |
| src/js_native_api_types.h | raw/033539.body | 7603 | 05e3d86e24c814f94a8b52b6a70cec11d7e91c810008253553528637e3a15b2f |
| src/node_api_types.h | raw/033560.body | 1825 | 088766b3cf1f0cd9b292c361ad30f7a04e7cc6db35a20ca52c572c1adf9635e7 |
| src/node_version.h | raw/033674.body | 4243 | 21f88237adb8b6823e2fb64fdc868c50fa785c8c2c5a19f6e7498401b1a963ff |

The existing physical image candidate is
`C:/data/apps/#dev/nvm/node/v22.23.2/node.exe`,86997320bytes, fresh SHA256
`0d0f5e39f9f3d9587bc19f73eab3c2c9c4903fd02d6dbf9c853dd81b3d95fad4`.
The PATH alias `C:/nvm4w/nodejs` is a symbolic link and is not the selected
no-reparse acquisition path. The complete physical ancestor/loaded/source graph
still needs native acquisition and independent admission.

The import library candidate was copied from the parent's official acquisition
into this owner's bounded `.tmp/cli39-oct16/bootstrap-inputs/node-v22.23.2-win-x64/node.lib`:
2790574bytes, fresh SHA256
`ac15f1e9d7c8279353723a77f6319967f1a41c06026521094a8234c2e6fbe052`.
Its adjacent CUSTODY.json and session MANIFEST.json retain ownership and cleanup
trigger. It is an active uncommitted input, not disposable current scratch.
Owner is CLI39/PR43; retain until authentic bootstrap input/acceptance transfer or
explicit candidate replacement/abandonment, then perform verified owned cleanup.
Parent removed its duplicate candidate copy after independently verifying this
custody. No binary is committed or loaded.

The native seed's closed tuple acquisition must hold the actual image/library and
all five original headers before native compile/use. Complete COFF short-import
and PE export correspondence checks constrain the fixed ordinary Node-API C
surface; symbol-name equality alone never authorizes a loaded function call.
The byte parser traverses every archive member and short import, retaining opaque
complete linker/descriptor bytes separately; it does not certify a linker index,
signature or a native function ABI from that traversal. Labelled pure fixtures
exercise wrong machine/DLL/reserved bits, duplicate imports and truncated bytes
without a fabricated native owner. They are locally UNRUN.
Actual compiler/header/import/source tuple, loaded image/module/RVA/native
principal/backing and original ROOT/profile joins remain mandatory. The current
producer guard stays false. No parser/compiler/link/load/build/test execution
occurred locally. The existing public listing signature/commit relation remains
unverified; candidate hash equality is byte correspondence only.

Format mechanism: [Microsoft PE/COFF format](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format).
