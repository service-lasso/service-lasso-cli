//go:build linux

package main

import("crypto/sha256";"encoding/binary";"golang.org/x/sys/unix")

func writerStatStable(a,b unix.Stat_t)bool{return a.Dev==b.Dev&&a.Ino==b.Ino&&a.Nlink==b.Nlink&&a.Mode==b.Mode&&a.Uid==b.Uid&&a.Gid==b.Gid&&a.Size==b.Size&&a.Mtim==b.Mtim&&a.Ctim==b.Ctim}
func writerStatDigest(s unix.Stat_t)[32]byte{values:=[12]uint64{uint64(s.Dev),uint64(s.Ino),uint64(s.Nlink),uint64(s.Mode),uint64(s.Uid),uint64(s.Gid),uint64(s.Size),uint64(s.Mtim.Sec),uint64(s.Mtim.Nsec),uint64(s.Ctim.Sec),uint64(s.Ctim.Nsec),1};var raw [96]byte;for i,v:=range values{binary.BigEndian.PutUint64(raw[i*8:i*8+8],v)};return sha256.Sum256(raw[:])}
