/* SOURCE-UNRUN. Structural custody only; never native/source/tool admission.
 * Original format reference: https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#archive-library-file-format
 * No installed archive has been parsed or extracted by this source work. */
function requireShape(ok, message) { if (!ok) throw new Error(message); }
function field(raw, start, size) {
  const bytes = raw.subarray(start, start + size);
  requireShape(bytes.length === size && bytes.every(byte => byte >= 32 && byte <= 126), 'Non-ASCII archive field');
  return bytes.toString('ascii').replace(/ +$/, '');
}
function integer(text, radix = 10, empty = false) {
  if (empty && text === '') return 0;
  requireShape((radix === 8 ? /^[0-7]+$/ : /^[0-9]+$/).test(text), 'Invalid archive integer');
  const value = Number.parseInt(text, radix);
  requireShape(Number.isSafeInteger(value) && value >= 0, 'Archive integer overflow');
  return value;
}
function strings(raw, start, count) {
  requireShape(Number.isSafeInteger(count) && count >= 0 && count <= raw.length - start, 'Invalid symbol count');
  const result = [];
  let cursor = start;
  for (let index = 0; index < count; index++) {
    const end = raw.indexOf(0, cursor);
    requireShape(end > cursor, 'Missing/empty archive symbol');
    result.push(raw.subarray(cursor, end)); cursor = end + 1;
  }
  requireShape(cursor === raw.length, 'Trailing archive symbol bytes');
  return result;
}
function firstDirectory(member, originals) {
  const raw = member.body;
  requireShape(raw.length >= 4, 'Short first linker directory');
  const count = raw.readUInt32BE(0);
  requireShape(count <= Math.floor((raw.length - 4) / 5), 'First directory length overflow');
  const names = strings(raw, 4 + count * 4, count);
  const rows = [];
  let previous = 0;
  for (let index = 0; index < count; index++) {
    const offset = raw.readUInt32BE(4 + index * 4);
    requireShape(originals.has(offset) && offset >= previous, 'First directory wrong original/order');
    previous = offset; rows.push({ symbol: names[index], memberOffset: offset });
  }
  return rows;
}
function secondDirectory(member, originals) {
  const raw = member.body;
  requireShape(raw.length >= 8, 'Short second linker directory');
  const count = raw.readUInt32LE(0);
  requireShape(count <= Math.floor((raw.length - 8) / 4), 'Second directory member length overflow');
  const offsets = [];
  let previous = 0;
  for (let index = 0; index < count; index++) {
    const offset = raw.readUInt32LE(4 + index * 4);
    requireShape(originals.has(offset) && offset > previous, 'Second directory wrong original/order');
    previous = offset; offsets.push(offset);
  }
  const symbolAt = 4 + count * 4;
  const symbols = raw.readUInt32LE(symbolAt);
  requireShape(symbols <= Math.floor((raw.length - symbolAt - 4) / 3), 'Second directory symbol length overflow');
  const names = strings(raw, symbolAt + 4 + symbols * 2, symbols);
  const rows = [];
  for (let index = 0; index < symbols; index++) {
    const ordinal = raw.readUInt16LE(symbolAt + 4 + index * 2);
    requireShape(ordinal >= 1 && ordinal <= offsets.length, 'Second directory index outside original members');
    if (index) requireShape(Buffer.compare(names[index - 1], names[index]) <= 0, 'Unordered archive symbols');
    rows.push({ symbol: names[index], memberOffset: offsets[ordinal - 1] });
  }
  return rows;
}

/* Returns borrowed ranges of the SAME input Buffer. Its original owning
 * caller must retain that whole body and immutable custody through verification.
 * Names may repeat: exact header offset/body/source association selects a member,
 * not a guessed basename or matching symbol alone. */
export function readCoffArchiveMembers(raw) {
  requireShape(Buffer.isBuffer(raw) && raw.length >= 8 && raw.subarray(0, 8).equals(Buffer.from('!<arch>\n')), 'Wrong archive signature');
  const members = [];
  let cursor = 8;
  while (cursor < raw.length) {
    requireShape((cursor & 1) === 0 && raw.length - cursor >= 60, 'Short/misaligned archive header');
    const headerOffset = cursor;
    const nameField = field(raw, cursor, 16);
    integer(field(raw, cursor + 16, 12), 10, true);
    integer(field(raw, cursor + 28, 6), 10, true);
    integer(field(raw, cursor + 34, 6), 10, true);
    integer(field(raw, cursor + 40, 8), 8, true);
    const size = integer(field(raw, cursor + 48, 10));
    requireShape(raw[cursor + 58] === 96 && raw[cursor + 59] === 10 && size <= raw.length - cursor - 60, 'Invalid archive body extent');
    cursor += 60;
    const body = raw.subarray(cursor, cursor + size);
    cursor += size;
    if (cursor & 1) { requireShape(cursor < raw.length && raw[cursor] === 10, 'Missing exact archive padding'); cursor++; }
    members.push({ headerOffset, nameField, body });
  }
  requireShape(cursor === raw.length && members.length >= 2 && members[0].nameField === '/' && members[1].nameField === '/', 'Missing original linker directories');
  const longnames = members[2]?.nameField === '//' ? members[2].body : null;
  const starts = new Map();
  if (longnames) {
    let offset = 0;
    while (offset < longnames.length) {
      const end = longnames.indexOf(0, offset);
      requireShape(end > offset && longnames.subarray(offset, end).every(byte => byte >= 32 && byte <= 126), 'Invalid long member name');
      starts.set(offset, longnames.subarray(offset, end).toString('ascii')); offset = end + 1;
    }
  }
  const objectMembers = members.slice(longnames ? 3 : 2);
  const originals = new Map();
  for (const member of objectMembers) {
    const name = member.nameField;
    if (/^\/[0-9]+$/.test(name)) {
      const offset = integer(name.slice(1));
      requireShape(starts.has(offset), 'Long name is not an original name boundary'); member.name = starts.get(offset);
    } else {
      requireShape(name.length > 1 && !name.startsWith('/') && name.endsWith('/'), 'Unsupported archive member variant');
      member.name = name.slice(0, -1);
    }
    originals.set(member.headerOffset, member);
  }
  const first = firstDirectory(members[0], originals);
  const second = secondDirectory(members[1], originals);
  /* Both genuine directories must bind the same symbol/member multiset,
   * independently of their distinct physical ordering. */
  const key = row => `${row.memberOffset}:${row.symbol.toString('hex')}`;
  const left = first.map(key).sort(), right = second.map(key).sort();
  requireShape(left.length === right.length && left.every((value, index) => value === right[index]), 'Linker directories disagree');
  return { raw, members: objectMembers, firstDirectory: first, secondDirectory: second };
}
