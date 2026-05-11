# Fix line endings CRLF to LF
$files = @(
    "C:\Users\PLM_104\Desktop\PLEMv5\standalone_devices\nanostage\src\main.c",
    "C:\Users\PLM_104\Desktop\PLEMv5\standalone_devices\nanostage\include\E816_DLL.h",
    "C:\Users\PLM_104\Desktop\PLEMv5\standalone_devices\nanostage\E816_DLL\E816_DLL.h"
)

foreach ($file in $files) {
    $content = [System.IO.File]::ReadAllText($file)
    $content = $content -replace "`r`n", "`n"
    [System.IO.File]::WriteAllText($file, $content)
    Write-Host "Fixed: $file"
}

Write-Host "All files converted to LF line endings"
