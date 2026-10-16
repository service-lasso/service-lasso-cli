# Private A stream owner. Backing is selected/charged before compiler birth;
# native EOF, root exit and descendant/debug closure are separate observations.
function New-SlcliInitialWindowsStreams {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capacity=$Owner.Source.StreamPollCapacity;$bytes=$Owner.Source.StreamByteCapacity
    if ($capacity -isnot [Int32] -or $capacity -le 0 -or $capacity -gt [Int32]::MaxValue/16 -or
        $bytes -isnot [Int32] -or $bytes -le 0 -or $null -ne $Birth.Birth -or $null -ne $Birth.Streams) {
        throw 'Initial stream backing must precede native child birth.'
    }
    $original=[pscustomobject]@{ Birth=$Birth; Capacity=$capacity; Polls=[object[]]::new($capacity);
        Used=0; Started=$false; Complete=$false; ParentEnds=[object[]]::new(3);
        StdinWriterClose=$null; Streams=[object[]]::new(2) }
    $Birth.Streams=$original
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    for ($index=0;$index -lt 2;$index++) {
        $stream=[pscustomobject]@{ Kind=@('stdout','stderr')[$index]; Reader=$null;
            Body=$null; Control=$null; BytesUsed=0; Rows=[object[]]::new($capacity);
            Eof=$null; ReaderClose=$null }
        $original.Streams[$index]=$stream
        $stream.Body=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $bytes
        $stream.Control=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($capacity*16) -Alignment 8
    }
    return $original
}

function Start-SlcliInitialWindowsStreams {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $original=$Birth.Streams
    if ($null -eq $original -or $original.Started -or $null -eq $Birth.Birth -or
        $Birth.Birth.Raw -eq 0 -or $null -eq $Birth.StdinWriterClose -or
        $Birth.StdinWriterClose.Raw -eq 0 -or
        -not [Object]::ReferenceEquals($Birth.Stdin.WriterReference.Close,$Birth.StdinWriterClose)) {
        throw 'Initial stream birth/empty-input source mismatch.'
    }
    $original.Started=$true;$original.StdinWriterClose=$Birth.StdinWriterClose
    $original.Streams[0].Reader=$Birth.Stdout.ReaderReference
    $original.Streams[1].Reader=$Birth.Stderr.ReaderReference
    # Only these parent-held references are retired. Child inherited handles,
    # process/debug aliases and all descendant stream writers stay distinct.
    $ends=@($Birth.Stdin.ReaderReference,$Birth.Stdout.WriterReference,$Birth.Stderr.WriterReference)
    for ($index=0;$index -lt 3;$index++) {
        $entry=[pscustomobject]@{ Reference=$ends[$index]; Close=$null }
        $original.ParentEnds[$index]=$entry
        if ($null -ne $entry.Reference.Close) { throw 'Initial child-end parent reference already consumed.' }
        $entry.Close=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'CloseHandle' -Arguments @($entry.Reference.Handle)
        $entry.Reference.Close=$entry.Close
        if ($entry.Close.Raw -eq 0) { $Owner.Pending=$true;throw 'Initial parent child-end close failed.' }
    }
}

function Get-SlcliInitialWindowsStreamExtent {
    param([Parameter(Mandatory)][Int32]$Used,[Parameter(Mandatory)][Int32]$Capacity,
        [Parameter(Mandatory)][UInt32]$Available)
    if ($Used -lt 0 -or $Capacity -le 0 -or $Used -gt $Capacity -or $Available -eq 0) {
        throw 'Invalid actual initial stream extent.'
    }
    $requested=[Int32][Math]::Min([Int64]16384,[Int64]$Available)
    if ($requested -gt $Capacity-$Used) { throw 'Initial complete stream byte budget exhausted.' }
    return $requested
}

function Read-SlcliInitialWindowsStreams {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $original=$Birth.Streams
    if ($null -eq $original -or -not $original.Started -or $original.Complete -or
        $original.Used -ge $original.Capacity) { throw 'Initial stream owner unavailable/exhausted.' }
    foreach ($end in $original.ParentEnds) {
        if ($null -eq $end.Close -or $end.Close.Raw -eq 0 -or
            -not [Object]::ReferenceEquals($end.Reference.Close,$end.Close)) {
            throw 'Initial stream parent writer/reference closure changed.'
        }
    }
    $ordinal=$original.Used;$original.Used++
    $poll=[pscustomobject]@{ Thread=$null; Streams=[object[]]::new(2); Finish=$null }
    $original.Polls[$ordinal]=$poll
    $poll.Thread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($poll.Thread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Initial stream poll left debug creator thread.' }
    for ($index=0;$index -lt 2;$index++) {
        $stream=$original.Streams[$index]
        if ($null -ne $stream.Eof) { continue }
        $control=[IntPtr]::Add($stream.Control.Pointer,$ordinal*16)
        $row=[pscustomobject]@{ Control=$control; Offset=$stream.BytesUsed; Peek=$null; Available=[UInt32]0;
            Requested=0; Read=$null; Received=[UInt32]0; Idle=$false; Eof=$false }
        $poll.Streams[$index]=$row;$stream.Rows[$ordinal]=$row
        $row.Peek=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'PeekNamedPipe' -Arguments @(
            $stream.Reader.Handle,[IntPtr]::Zero,[UInt32]0,[IntPtr]::Zero,$control,[IntPtr]::Zero)
        $row.Available=[UInt32]([Int64][Runtime.InteropServices.Marshal]::ReadInt32($control) -band [Int64]0xffffffff)
        if ($row.Peek.Raw -eq 0) {
            if ($row.Peek.CacheObserved -ne 1 -or $row.Peek.CachedError -ne 109 -or $row.Available -ne 0) {
                $Owner.Pending=$true;throw 'Initial native stream peek failed without observed clean EOF.'
            }
            $row.Eof=$true;$stream.Eof=$row
            continue
        }
        if ($row.Available -eq 0) { $row.Idle=$true;continue }
        $row.Requested=Get-SlcliInitialWindowsStreamExtent -Used $stream.BytesUsed -Capacity $stream.Body.Bytes -Available $row.Available
        # Sole parent reader consumes only an actually available prefix. Both
        # native data and received-count cells are retained before postclock.
        $row.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadFile' -Arguments @(
            $stream.Reader.Handle,[IntPtr]::Add($stream.Body.Pointer,$stream.BytesUsed),[UInt32]$row.Requested,
            [IntPtr]::Add($control,4),[IntPtr]::Zero)
        $row.Received=[UInt32]([Int64][Runtime.InteropServices.Marshal]::ReadInt32($control,4) -band [Int64]0xffffffff)
        if ($row.Read.Raw -eq 0 -or $row.Received -eq 0 -or $row.Received -gt $row.Requested) {
            $Owner.Pending=$true;throw 'Initial available stream prefix read failed/changed; not EOF.'
        }
        $stream.BytesUsed+=[Int32]$row.Received
    }
    $poll.Finish=Read-SlcliInitialWindowsClock -Owner $Owner
    $Owner.PriorTick=$poll.Finish.Tick
    $original.Complete=($null -ne $original.Streams[0].Eof -and $null -ne $original.Streams[1].Eof)
    return $poll
}

function Close-SlcliInitialWindowsStreamReaders {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $original=$Birth.Streams
    if ($null -eq $original -or -not $original.Complete) { throw 'Actual separate stream EOF still outstanding.' }
    foreach ($stream in $original.Streams) {
        if ($null -eq $stream.Eof -or $null -ne $stream.ReaderClose -or $null -ne $stream.Reader.Close) {
            throw 'Initial stream reader last borrower/one-use close mismatch.'
        }
        $stream.Reader.LastBorrower=$stream.Eof
        $stream.ReaderClose=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'CloseHandle' -Arguments @($stream.Reader.Handle)
        $stream.Reader.Close=$stream.ReaderClose
        if ($stream.ReaderClose.Raw -eq 0) { $Owner.Pending=$true;throw 'Initial EOF reader close failed; reference retained.' }
    }
}
