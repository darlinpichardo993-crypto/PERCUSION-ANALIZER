; Instalador de PERCUSSION ANALIZER (VST3) - ORYMY And YMY Records
#define MyAppName "PERCUSSION ANALIZER"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "ORYMY And YMY Records"
#ifndef VST3Src
  #define VST3Src "..\build\PercussionAnalizer_artefacts\Release\VST3\PERCUSSION ANALIZER.vst3"
#endif

[Setup]
AppId={{7E2B6C1A-4D3F-4B8A-9C55-0A1F2D3E4B71}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commoncf64}\VST3\PERCUSSION ANALIZER.vst3
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyPage=no
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupIconFile=..\assets\icon.ico
UninstallDisplayIcon={app}\icon.ico
WizardImageFile=..\assets\wizard_large.bmp
WizardSmallImageFile=..\assets\wizard_small.bmp
OutputDir=..\dist
OutputBaseFilename=PERCUSSION_ANALIZER_Setup
VersionInfoVersion={#MyAppVersion}
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription=Instalador de PERCUSSION ANALIZER VST3

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Messages]
spanish.WelcomeLabel2=Se instalará [name/ver] (VST3) en la carpeta de plugins VST3 de Windows.%n%nDespués abre FL Studio y haz un escaneo de plugins (Options > Manage plugins > Find installed plugins).

[Files]
Source: "{#VST3Src}\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion
Source: "..\assets\icon.ico"; DestDir: "{app}"; Flags: ignoreversion
