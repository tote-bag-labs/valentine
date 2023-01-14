#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define Year GetDateTimeString("yyyy","","")

[Setup]
AppName=Valentine
OutputBaseFilename=Valentine-{#Version}-Windows
AppCopyright=Copyright (C) {#Year} Tote Bag Labs
AppPublisher=Tote Bag Labs
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3"
DisableStartupPrompt=yes

[Files]
Source: "{src}..\Builds\Pamplejuce_artefacts\Release\VST3\Valentine.vst3\*.*"; DestDir: "{commoncf64}\VST3\Valentine.vst3\"; Check: Is64BitInstallMode; Flags: external overwritereadonly ignoreversion; Attribs: hidden system;
