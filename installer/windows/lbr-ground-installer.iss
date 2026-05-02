#define AppName "LBR-26 Ground Software"
#define AppPublisher "Long Beach Rocketry"
#define AppExeName "lbr-ground-gui.exe"

#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif

#ifndef SourceDir
  #define SourceDir "..\..\installer-input"
#endif

#ifndef OutputDir
  #define OutputDir "..\..\dist"
#endif

#define CliBinDir SourceDir + "\windows-ucrt64-x64\bin"
#define GuiBinDir SourceDir + "\gui-windows-x64\bin"
#define DocsDir SourceDir + "\docs"

[Setup]
AppId={{8C4E88D0-43F2-455A-A701-93C2A2E8E55F}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={localappdata}\Programs\LBR-26 Ground Software
DefaultGroupName=LBR-26 Ground Software
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=lbr-nightly-windows-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=lowest
CloseApplications=yes
RestartApplications=no
UsePreviousAppDir=yes
ChangesEnvironment=yes
UninstallDisplayName={#AppName}
UninstallDisplayIcon={app}\gui\{#AppExeName}
VersionInfoCompany={#AppPublisher}
VersionInfoDescription={#AppName} installer
VersionInfoProductName={#AppName}
VersionInfoProductVersion={#AppVersion}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Shortcuts:"; Flags: unchecked
Name: "addtopath"; Description: "Add command-line tools to the current user's PATH"; GroupDescription: "Command-line integration:"; Flags: unchecked

[Files]
Source: "{#GuiBinDir}\*"; DestDir: "{app}\gui"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#CliBinDir}\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#DocsDir}\*"; DestDir: "{app}\docs"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Ground Station GUI"; Filename: "{app}\gui\{#AppExeName}"; WorkingDir: "{app}\gui"
Name: "{group}\Ground CLI"; Filename: "{app}\bin\lbr_ground.exe"; WorkingDir: "{app}\bin"
Name: "{group}\HIL Runner"; Filename: "{app}\bin\lbr_hil_runner.exe"; WorkingDir: "{app}\bin"
Name: "{group}\Documentation"; Filename: "{app}\docs\html\index.html"; WorkingDir: "{app}\docs\html"
Name: "{group}\Uninstall LBR-26 Ground Software"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Ground Station GUI"; Filename: "{app}\gui\{#AppExeName}"; WorkingDir: "{app}\gui"; Tasks: desktopicon

[Run]
Filename: "{app}\gui\{#AppExeName}"; Description: "Launch Ground Station GUI"; Flags: nowait postinstall skipifsilent

[InstallDelete]
Type: filesandordirs; Name: "{app}\bin"
Type: filesandordirs; Name: "{app}\gui"
Type: filesandordirs; Name: "{app}\docs"

[Code]
const
  EnvironmentKey = 'Environment';
  AppRegistryKey = 'Software\Long Beach Rocketry\LBR-26 Ground Software';
  AddedToUserPathValue = 'AddedToUserPath';

function TrimSemicolons(Value: string): string;
begin
  while (Length(Value) > 0) and (Copy(Value, 1, 1) = ';') do
    Delete(Value, 1, 1);

  while (Length(Value) > 0) and (Copy(Value, Length(Value), 1) = ';') do
    Delete(Value, Length(Value), 1);

  Result := Value;
end;

function PathEntryExists(PathValue: string; Entry: string): Boolean;
begin
  Result := Pos(';' + Lowercase(Entry) + ';', ';' + Lowercase(PathValue) + ';') > 0;
end;

procedure AddToUserPath(Entry: string);
var
  PathValue: string;
begin
  if not RegQueryStringValue(HKCU, EnvironmentKey, 'Path', PathValue) then
    PathValue := '';

  if not PathEntryExists(PathValue, Entry) then
  begin
    if PathValue = '' then
      PathValue := Entry
    else
      PathValue := PathValue + ';' + Entry;

    if not RegWriteStringValue(HKCU, EnvironmentKey, 'Path', PathValue) then
      RaiseException('Failed to update the current user PATH.');

    RegWriteDWordValue(HKCU, AppRegistryKey, AddedToUserPathValue, 1);
  end;
end;

procedure RemoveFromUserPath(Entry: string);
var
  PathValue: string;
begin
  if not RegQueryStringValue(HKCU, EnvironmentKey, 'Path', PathValue) then
    Exit;

  PathValue := ';' + PathValue + ';';
  StringChangeEx(PathValue, ';' + Entry + ';', ';', True);
  PathValue := TrimSemicolons(PathValue);

  if PathValue = '' then
    RegDeleteValue(HKCU, EnvironmentKey, 'Path')
  else
    RegWriteStringValue(HKCU, EnvironmentKey, 'Path', PathValue);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssPostInstall) and WizardIsTaskSelected('addtopath') then
    AddToUserPath(ExpandConstant('{app}\bin'));
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  AddedToUserPath: Cardinal;
begin
  if CurUninstallStep <> usPostUninstall then
    Exit;

  if RegQueryDWordValue(HKCU, AppRegistryKey, AddedToUserPathValue, AddedToUserPath) and
     (AddedToUserPath = 1) then
  begin
    RemoveFromUserPath(ExpandConstant('{app}\bin'));
    RegDeleteValue(HKCU, AppRegistryKey, AddedToUserPathValue);
  end;
end;
