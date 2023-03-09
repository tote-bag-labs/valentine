#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define MyAppName "Valentine"
#define MyAppPublisher "Tote Bag Labs"
#define Year GetDateTimeString("yyyy","","")

[Setup]
AppName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3\{#MyAppPublisher}\Valentine.vst3\"
OutputBaseFilename=valentine-{#Version}-windows
LicenseFile="../LICENSE"
UninstallDisplayIcon={uninstallexe}
UninstallFilesDir={commonappdata}\{#MyAppName}\uninstall

; MSVC adds a .ilk when building the plugin. Let's not include that.
[Files]
Source: "..\Builds\Valentine_artefacts\Release\VST3\Valentine.vst3\*"; DestDir: "{commoncf64}\VST3\{#MyAppPublisher}\Valentine.vst3\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs;

[Run]
Filename: "{cmd}"; \
    WorkingDir: "{commoncf64}\VST3"; \
    Parameters: "/C mklink /D /J  ""{commoncf64}\VST3\{#MyAppPublisher}\{#MyAppName}Data"" ""{commonappdata}\{#MyAppName}"""; \
    Flags: runascurrentuser;
