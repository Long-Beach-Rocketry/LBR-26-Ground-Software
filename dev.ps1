param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$PassThroughArgs
)

$scriptPath = Join-Path $PSScriptRoot "tools\dev.ps1"
& $scriptPath @PassThroughArgs
exit $LASTEXITCODE
