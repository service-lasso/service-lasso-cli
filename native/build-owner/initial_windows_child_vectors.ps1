# Pure native-width PID relation cases, no native call/event/token/ROOT.
# Source fixtures remain UNRUN.
function Test-SlcliInitialWindowsChildVectors {
    foreach ($case in @(@([UInt32]7,[UInt32]9,[Int64]7,[Int64]9,$true),
        @([UInt32]7,[UInt32]9,[Int64]8,[Int64]9,$false),
        @([UInt32]7,[UInt32]9,[Int64]7,[Int64]10,$false),
        @([UInt32]0,[UInt32]9,[Int64]0,[Int64]9,$false),
        @([UInt32]7,[UInt32]0,[Int64]7,[Int64]0,$false),
        @([UInt32]7,[UInt32]9,[Int64]-1,[Int64]9,$false),
        @([UInt32]7,[UInt32]9,[Int64]7,[Int64]-1,$false),
        @([UInt32]7,[UInt32]9,[Int64]4294967303,[Int64]9,$false),
        @([UInt32]7,[UInt32]9,[Int64]7,[Int64]4294967305,$false),
        @([UInt32]::MaxValue,[UInt32]9,[Int64]4294967295,[Int64]9,$true))) {
        $actual=Test-SlcliInitialWindowsChildParentIdentity -ExpectedChild $case[0] -ExpectedParent $case[1] -ActualChild $case[2] -ActualParent $case[3]
        if ($actual -ne $case[4]) { throw 'Actual native-width child/parent identity relation differs.' }
    }
}
