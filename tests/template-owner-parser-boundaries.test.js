import test from "node:test";
import assert from "node:assert/strict";
import { strictJson, sortedJson, portablePath, portablePathsCompatible } from "../dist/template-json.js";

// Unexecuted decoder regressions. These do not impersonate an admitted original
// tuple or the complete ordinary native TC01..12/CA01..08 qualification paths.
const decode = source => strictJson(Buffer.from(source, "utf8"), 32768);
const rejected = action => assert.throws(action, error => error?.code === "invalid_template_bundle");

test("original JSON refuses replacement UTF8 and escaped lone surrogates", () => {
  for (const bytes of [Buffer.from([0x22,0xc0,0xaf,0x22]), Buffer.from([0x22,0xed,0xa0,0x80,0x22]), Buffer.from([0x22,0xf4,0x90,0x80,0x80,0x22])]) rejected(() => strictJson(bytes, 32768));
  for (const source of ['"\\ud800"', '"\\udc00"', '"\\ud800x"', '"\\ud800\\u0041"']) rejected(() => decode(source));
  assert.equal(decode('"\\ud800\\udc00"'), String.fromCodePoint(0x10000));
});

test("duplicate JSON names reject after actual escape decoding", () => {
  for (const source of ['{"a":1,"\\u0061":2}', '{"x":{"a":1,"a":2}}', '{"__proto__":1,"\\u005f_proto__":2}']) rejected(() => decode(source));
  const value = decode('{"__proto__":{"polluted":true},"constructor":0}');
  assert.equal(Object.getPrototypeOf(value), null);
  assert.equal(Object.prototype.hasOwnProperty.call(value, "__proto__"), true);
  assert.equal({}.polluted, undefined);
});

test("owner sorted JSON uses UTF16 property order and preserves array order", () => {
  const astral = String.fromCodePoint(0x10000), bmp = String.fromCharCode(0xe000);
  const value = decode(`{"${bmp}":1,"${astral}":2,"a":[3,1,2]}`);
  assert.equal(sortedJson(value), `{"a":[3,1,2],"${astral}":2,"${bmp}":1}`);
});

test("only original JSON whitespace, complete tokens and bounded nesting pass", () => {
  for (const source of ['\u00a0{}', '{}\u00a0', '01', 'true false', '[1,]', '{"a":1,}', '1e999', '{}x']) rejected(() => decode(source));
  rejected(() => decode('['.repeat(18) + '0' + ']'.repeat(18)));
  assert.deepEqual(Array.from(decode(' \t\r\n[true,false,null,0]\n')), [true,false,null,0]);
});

test("portable namespace blocks devices, directory alias and file-parent overlap", () => {
  for (const path of ['../x','a//b','a.','CON.txt','com9','a\\b','/a','a/..']) rejected(() => portablePath(path));
  for (const pair of [['A/x','a/y'], ['a','a/b'], ['a/b','a'], ['x','x']]) assert.equal(portablePathsCompatible(...pair), false);
  assert.equal(portablePathsCompatible('A/x','A/y'), true);
  assert.equal(portablePathsCompatible('A/x','b/y'), true);
});