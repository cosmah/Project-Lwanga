; Lwanga Windows Installer Script (NSIS)
; Cosmc Technologies
; Creates Windows installer for Lwanga compiler

!define PRODUCT_NAME "Lwanga"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Cosmc Technologies"
!define PRODUCT_WEB_SITE "https://github.com/cosmah/Project-Lwanga"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI Settings
!include "MUI2.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON "..\..\assets\icons\lwanga-file.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\lwangac.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--version"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Installer attributes
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "lwanga-${PRODUCT_VERSION}-windows-x64-installer.exe"
InstallDir "$PROGRAMFILES64\Lwanga"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "InstallLocation"
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR\bin"
  SetOverwrite ifnewer
  
  ; Install binaries
  File "..\..\build-windows\install\bin\lwangac.exe"
  File "..\..\build-windows\install\bin\lwangafmt.exe"
  File "..\..\build-windows\install\bin\lwangacat.exe"
  
  ; Install LLVM DLLs (if needed)
  File /nonfatal "C:\Program Files\LLVM\bin\LLVM-C.dll"
  
  ; Install documentation
  SetOutPath "$INSTDIR\docs"
  File "..\..\README.md"
  File "..\..\INSTALL.md"
  File "..\..\CHANGELOG.md"
  File "..\..\LICENSE"
  File "..\..\docs\language-reference.md"
  File "..\..\docs\syscalls.md"
  
  ; Install examples
  SetOutPath "$INSTDIR\examples"
  File "..\..\examples\*.lwanga"
  
  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\Lwanga"
  CreateShortCut "$SMPROGRAMS\Lwanga\Lwanga Compiler.lnk" "$INSTDIR\bin\lwangac.exe"
  CreateShortCut "$SMPROGRAMS\Lwanga\Documentation.lnk" "$INSTDIR\docs"
  CreateShortCut "$SMPROGRAMS\Lwanga\Examples.lnk" "$INSTDIR\examples"
  CreateShortCut "$SMPROGRAMS\Lwanga\Uninstall.lnk" "$INSTDIR\uninst.exe"
  
  ; Add to PATH
  EnVar::SetHKLM
  EnVar::AddValue "PATH" "$INSTDIR\bin"
  
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\Lwanga\Website.lnk" "${PRODUCT_WEB_SITE}"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\lwangac.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Section Uninstall
  ; Remove from PATH
  EnVar::SetHKLM
  EnVar::DeleteValue "PATH" "$INSTDIR\bin"
  
  ; Remove files
  Delete "$INSTDIR\bin\lwangac.exe"
  Delete "$INSTDIR\bin\lwangafmt.exe"
  Delete "$INSTDIR\bin\lwangacat.exe"
  Delete "$INSTDIR\bin\LLVM-C.dll"
  Delete "$INSTDIR\uninst.exe"
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\Lwanga\*.*"
  RMDir "$SMPROGRAMS\Lwanga"
  
  ; Remove directories
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\examples"
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR"
  
  ; Remove registry keys
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  
  SetAutoClose true
SectionEnd
