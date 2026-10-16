# UNRUN fixed x64 Wintrust structure initialization. The managed layout and
# native destination are both SAME preparation originals; no foreign buffer
# or caller-selected native write is accepted.
function New-Cli39PublisherInitializationWorker26 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $n=$c.publisherNativeOriginal
  if ($c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $null -ne $n.initializeWorkerOriginal -or $null -ne $n.storageReleaseOriginal -or
      $null -ne $n.verifyWorkerOriginal) { throw 'Same original once-only pre-verification initialization required.' }
  $a=$n.storageAcquireOriginal; $s=$n.subjectOriginal
  if ($null -eq $a -or -not $a.terminalJoined -or $a.thread.IsAlive -or
      $a.results[1] -ne 1 -or $a.pointerOriginal[0] -eq [IntPtr]::Zero -or
      $null -ne $a.exceptionOriginal[0] -or $null -eq $s -or
      -not [object]::ReferenceEquals($s,$c.subjectOriginals['publisher-subject']) -or
      $null -eq $s.stream -or $null -eq $s.handleOriginal -or $s.handleOriginal.IsClosed -or
      $s.handleOriginal.IsInvalid -or -not [IO.Path]::IsPathFullyQualified($s.path) -or
      $s.path.Contains([char]0)) {
    throw 'Actual held subject and same terminal acquired backing required.'
  }
  if ([IntPtr]::Size -ne 8 -or $n.storageBytes -le 168 -or $n.storageBytes -ge [int]::MaxValue -or
      $c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue) {
    throw 'Complete finite x64 native-layout and worker geometry required.'
  }
  $base=$a.pointerOriginal[0].ToInt64()
  if ($base -le 0 -or $base -gt ([long]::MaxValue-$n.storageBytes)) { throw 'Original native extent cannot represent full layout.' }
  $w=@{ phase='initialize'; preparation=$p; nativeOriginal=$n;
    bytesOriginal=$null; pathBytesOriginal=$null; pathEncodingOriginal=$null;
    terminatedPathOriginal=$null; subjectHandleOriginal=$s.handleOriginal;
    subjectPointerOriginal=[IntPtr]::Zero; results=[int[]]::new(2);
    exceptionOriginal=[object[]]::new(1); context=$null; method=$null; il=$null; body=$null;
    thread=$null; startAttempted=$false; startReturned=$false; startException=$null;
    terminalJoined=$false; joinPendingActive=$false; joinMultiplicityUnknown=$false;
    joinPending=@{ attempted=$false; returned=$false; value=$null; exception=$null };
    joinFalse=@{ returned=$false; value=$false; repeats=0L };
    joinTerminal=@{ returned=$false; value=$true };
    joinException=@{ present=$false; original=$null }; constructionFailure=$null }
  $n.initializeWorkerOriginal=$w
  try {
    # Private retained managed bytes begin zero-filled, including all padding.
    # Encoding and extent arithmetic are retained before any native write.
    $w.bytesOriginal=[byte[]]::new([int]$n.storageBytes)
    $w.pathEncodingOriginal=[Text.UnicodeEncoding]::new($false,$false,$true)
    $w.terminatedPathOriginal=$s.path+[char]0
    $w.pathBytesOriginal=$w.pathEncodingOriginal.GetBytes($w.terminatedPathOriginal)
    if ($w.pathBytesOriginal.Length -ne ($n.storageBytes-168)) { throw 'Native extent must equal complete fixed layout plus full terminated path.' }
    $w.subjectPointerOriginal=$s.handleOriginal.DangerousGetHandle()
    if ($w.subjectPointerOriginal -eq [IntPtr]::Zero -or $w.subjectPointerOriginal.ToInt64() -eq -1) { throw 'Actual live subject handle required.' }
    # WINTRUST_ACTION_GENERIC_VERIFY_V2, native GUID byte order.
    $guid=[byte[]]@(0x6b,0xc5,0xaa,0x00,0x44,0xcd,0xd0,0x11,0x8c,0xc2,0x00,0xc0,0x4f,0xc2,0x95,0xee)
    $guid.CopyTo($w.bytesOriginal,0)
    # FILE_INFO at16; WTD at48; signature settings at136; UTF16 path at168.
    # Complete layouts are fixed from actual SDK26100 x64 declarations.
    $fields=@(@(16,[uint32]32),@(24,[uint64]($base+168)),
      @(32,[uint64]$w.subjectPointerOriginal.ToInt64()),@(48,[uint32]88),
      @(72,[uint32]2),@(76,[uint32]1),@(80,[uint32]1),
      @(88,[uint64]($base+16)),@(120,[uint32]0x3040),
      @(128,[uint64]($base+136)),@(136,[uint32]32))
    # dwStateAction is written inside verify/close workers. Cache-only is a
    # selected policy flag, never a nonblocking or freshness assertion.
    $w.layoutFieldsOriginal=$fields; $w.guidBytesOriginal=$guid
    $w.fieldEncodingOriginals=[object[]]::new($fields.Count)
    for ($i=0;$i -lt $fields.Count;$i++) {
      $field=$fields[$i]
      $encoded=[BitConverter]::GetBytes($field[1]); $w.fieldEncodingOriginals[$i]=$encoded
      $encoded.CopyTo($w.bytesOriginal,[int]$field[0])
    }
    $w.pathBytesOriginal.CopyTo($w.bytesOriginal,168)
    $n.actionPointer=[IntPtr]::new($base); $n.wtdPointer=[IntPtr]::new($base+48)
    $w.context=[object[]]@($w.bytesOriginal,$a.pointerOriginal[0],$w.results,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedWintrustInitialize',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator(); $w.il=$il
    $failure=$il.DeclareLocal([Exception])
    function Emit-Cli39InitializeContext26([int]$Index,[Type]$Type,[bool]$ValueType=$false) {
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if ($ValueType) { $il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type) }
      else { $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type) }
    }
    $null=$il.BeginExceptionBlock()
    foreach ($index in @(0,1)) {
      Emit-Cli39InitializeContext26 2 ([int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      if ($index -eq 0) {
        Emit-Cli39InitializeContext26 0 ([byte[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        Emit-Cli39InitializeContext26 1 ([IntPtr]) $true
        Emit-Cli39InitializeContext26 0 ([byte[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldlen)
        $il.Emit([Reflection.Emit.OpCodes]::Conv_I4)
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('Copy',
          [Type[]]@([byte[]],[int],[IntPtr],[int])))
      }
    }
    $il.BeginCatchBlock([Exception]); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
    Emit-Cli39InitializeContext26 3 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failure); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock(); $il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false
    return $w
  } catch { $w.constructionFailure=$_; throw }
}
