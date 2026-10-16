package main

import "os"

type heldReadback struct{mode os.FileMode;bytes uint32;sha256 [32]byte;nativeIdentity [32]byte}
