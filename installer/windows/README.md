# Windows Installer

The nightly Windows setup executable is built with Inno Setup from
`lbr-ground-installer.iss`.

The installer:

- installs per user under `%LOCALAPPDATA%\Programs\LBR-26 Ground Software`
- installs the GUI under `gui`
- installs `lbr_ground.exe`, `lbr_hil_runner.exe`, and runtime DLLs under `bin`
- installs generated HTML documentation under `docs`
- creates Start Menu shortcuts
- optionally creates a desktop shortcut
- optionally adds `bin` to the current user's `PATH`
- removes its `PATH` entry during uninstall

Build locally after preparing `installer-input` with:

```powershell
iscc /DSourceDir="$PWD\installer-input" /DOutputDir="$PWD\installer-output" installer\windows\lbr-ground-installer.iss
```
