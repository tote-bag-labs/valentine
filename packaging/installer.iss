#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define MyAppName "Valentine"
#define MyAppPublisher "Tote Bag Labs"
#define Year GetDateTimeString("yyyy","","")

[Setup]
AppName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3\{#MyAppPublisher}\Valentine.vst3\"
DisableStartupPrompt=yes
OutputBaseFilename=valentine-{#Version}-windows

; MSVC adds a .ilk when building the plugin. Let's not include that.
[Files]
Source: "..\Builds\Valentine_artefacts\Release\VST3\Valentine.vst3\*"; DestDir: "{commoncf64}\VST3\{#MyAppPublisher}\Valentine.vst3\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs;
