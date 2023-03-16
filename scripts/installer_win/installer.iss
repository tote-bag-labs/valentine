#define PluginName "Valentine"
#define Publisher "Tote Bag Labs"
#define Version Trim(FileRead(FileOpen("..\..\VERSION")))

[Setup]
AppName={#PluginName}
AppPublisher={#Publisher}
AppVersion={#Version}
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
DefaultDirName="{commoncf64}\VST3\{#PluginName}.vst3\"
DisableDirPage=yes
LicenseFile="..\..\LICENSE"
OutputBaseFilename=valentine-{#Version}-windows
UninstallFilesDir={commonappdata}\{#PluginName}\uninstall

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf64}\VST3\{#PluginName}Data"

; MSVC adds a .ilk when building the plugin. Let's not include that.
[Files]
Source: "..\..\Builds\Valentine_artefacts\Release\VST3\{#PluginName}.vst3\*"; DestDir: "{commoncf64}\VST3\{#PluginName}.vst3\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs;

[Run]
Filename: "{cmd}"; \
    WorkingDir: "{commoncf64}\VST3"; \
    Parameters: "/C mklink /D ""{commoncf64}\VST3\{#PluginName}Data"" ""{commonappdata}\{#PluginName}"""; \
    Flags: runascurrentuser;
