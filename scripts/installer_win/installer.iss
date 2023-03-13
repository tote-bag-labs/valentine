#define AppUrl "https://github.com/Tote-Bag-Labs/valentine"
#define MyAppName "Valentine"
#define MyAppPublisher "Tote Bag Labs"
#define Version Trim(FileRead(FileOpen("..\..\VERSION")))

[Setup]
AppName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppVersion={#Version}
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
DefaultDirName="{commoncf64}\VST3\{#MyAppPublisher}\Valentine.vst3\"
DisableDirPage=yes
LicenseFile="..\..\LICENSE"
OutputBaseFilename=valentine-{#Version}-windows
UninstallFilesDir={commonappdata}\{#MyAppName}\uninstall

; MSVC adds a .ilk when building the plugin. Let's not include that.
[Files]
Source: "..\..\Builds\Valentine_artefacts\Release\VST3\Valentine.vst3\*"; DestDir: "{commoncf64}\VST3\{#MyAppPublisher}\Valentine.vst3\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs;

[Run]
Filename: "{cmd}"; \
    WorkingDir: "{commoncf64}\VST3"; \
    Parameters: "/C mklink /D /J  ""{commoncf64}\VST3\{#MyAppPublisher}\{#MyAppName}Data"" ""{commonappdata}\{#MyAppName}"""; \
    Flags: runascurrentuser;
