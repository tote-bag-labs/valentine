#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define Year GetDateTimeString("yyyy","","")

[Setup]
AppName=Valentine
OutputBaseFilename=Valentine-{#Version}-Windows
AppCopyright=Copyright (C) {#Year} Tote Bag Labs
AppPublisher=Tote Bag Labs
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3\Valentine.vst3"
DisableStartupPrompt=yes

; MSVC adds a .ilk when building the plugin. Let's not include that.
[Files]
Source: "..\Builds\Valentine_artefacts\Release\VST3\Valentine.vst3\*"; DestDir: "{commoncf64}\VST3\Valentine.vst3\"; Flags: ignoreversion recursesubdirs;
