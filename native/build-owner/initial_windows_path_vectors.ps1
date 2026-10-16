# Pure source/path syntax vectors only, no emitter/native call/owner/ROOT.
# UNRUN: target parser/fixture execution still requires exact admission.
function Test-SlcliInitialWindowsPathVectors {
    foreach ($path in @('C:\','C:\Program Files\SDK\cl.exe','D:\source\native\file.c')) {
        if (-not (Test-SlcliInitialWindowsCanonicalPath -Path $path)) { throw 'Canonical path fixture rejected.' }
    }
    foreach ($path in @('', 'cl.exe','C:cl.exe','\\server\share\cl.exe','C:/tools/cl.exe','C:\tools\\cl.exe',
        'C:\tools\..\cl.exe','C:\tools\.\cl.exe','C:\tools.\cl.exe','C:\tools \cl.exe','C:\tools\cl.exe:stream',
        'C:\tools\*.exe','C:\tools\"cl.exe','C:\tools\',("C:\tools\cl.exe"+[char]0),('C:\'+[char]10+'\cl.exe'))) {
        if (Test-SlcliInitialWindowsCanonicalPath -Path $path) { throw 'Invalid path fixture accepted.' }
    }
}
