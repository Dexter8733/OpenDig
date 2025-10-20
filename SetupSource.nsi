!include "MUI2.nsh"

Name "OpenDig Installer"
OutFile "OpenDigSetup.exe"
InstallDir "$PROGRAMFILES\OpenDig"
InstallDirRegKey HKLM "Software\OpenDig" "Install_Dir"
ShowInstDetails show
ShowUnInstDetails show

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

Section "Install"
  SetOutPath "$INSTDIR"
  File "HEADER.BMP"
  File "libgcc_s_dw2-1.dll"
  File "libstdc++-6.dll"
  File "OpenDigGUI.exe"
  File "OpenDigHelper.exe"
  File "OpenDigIcon.ico"

  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenDig" "DisplayName" "OpenDig"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenDig" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\OpenDig" "Install_Dir" "$INSTDIR"
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\HEADER.BMP"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\OpenDigGUI.exe"
  Delete "$INSTDIR\OpenDigHelper.exe"
  Delete "$INSTDIR\OpenDigIcon.ico"
  Delete "$INSTDIR\Uninstall.exe"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenDig"
  DeleteRegKey HKLM "Software\OpenDig"
  RMDir "$INSTDIR"
SectionEnd