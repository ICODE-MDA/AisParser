;Request execution level for Windows Vista and Windows 7 Installs
; This should not affect Windows XP installs
RequestExecutionLevel admin

;---------------------------------------------------
; Define the location of the installer base
!define COPY_FROM_DIR "D:\libraries\icode\installer\"

Name "ICODE-MDA"

# define name of installer
outFile "AisParserInstaller.exe"
BrandingText "ICODE-MDA"
 
# define installation directory
InstallDir $PROGRAMFILES\ICODE-MDA
AllowRootDirInstall true

LicenseData license.txt
!include LogicLib.nsh
;---------------------------------------------------
; Installer Pages
;
Page license  ; Displays the RAPIER License.  The license is defined by LicenseData variable.

Page directory ; Choose the Installation Folder.  The default path is defined by InstallDir variable.
 
Page instfiles ; This page performs the install

;---------------------------------------------------
; Uninstall Pages
UninstPage uninstConfirm  ; Ask user if he really wants to uninstall
UninstPage instfiles      ; Perform the uninstall

!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights are required to install this application. Please re-run this installer as a user who is a member of the Administrators group."
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
 
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd

# start default section
section 
    # set the installation directory as the destination for the following actions
    setOutPath $INSTDIR
    File /r "${COPY_FROM_DIR}"

    # Top directory Folder
    CreateDirectory "$SMPROGRAMS\ICODE-MDA"
   
    CreateShortCut "$SMPROGRAMS\ICODE-MDA\ICODE-MDA Ais Parser.lnk" "$INSTDIR\bin\AisParserApp.exe" "" "$INSTDIR\bin\AisParserApp.exe" 0
    ShellLink::SetShortCutIconLocation "$SMPROGRAMS\ICODE-MDA\ICODE-MDA Ais Parser.lnk" "$INSTDIR\icode-mda.ico"
    ShellLink::SetShortCutWorkingDirectory "$SMPROGRAMS\ICODE-MDA\ICODE-MDA Ais Parser.lnk" '$INSTDIR\bin' 

    CreateShortCut "$DESKTOP\ICODE-MDA Ais Parser.lnk" "$INSTDIR\bin\AisParserApp.exe"  
    ShellLink::SetShortCutIconLocation '$DESKTOP\ICODE-MDA Ais Parser.lnk' "$INSTDIR\icode-mda.ico"
    ShellLink::SetShortCutWorkingDirectory '$DESKTOP\ICODE-MDA Ais Parser.lnk' '$INSTDIR\bin'

    #write uninstall information to the registry
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ICODE-MDA" "DisplayName" "ICODE-MDA Ais Parser (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ICODE-MDA" "UninstallString" "$INSTDIR\uninstall.exe" 

    # create the uninstaller
    writeUninstaller "$INSTDIR\uninstall.exe"
 
    # create a shortcut named "new shortcut" in the start menu programs directory
    # point the new shortcut at the program uninstaller
    createShortCut "$SMPROGRAMS\ICODE-MDA\Uninstall ICODE-MDA Ais Parser.lnk" "$INSTDIR\uninstall.exe"
sectionEnd
 
# uninstaller section start
section "uninstall"
    setShellVarContext all
    !insertmacro VerifyUserIsAdmin

    #Empty install directory  
    RMDir /r "$INSTDIR\*.*"
    #Remove the install directory 
    RMDir "$INSTDIR"
 
    #Delete Start Menu Shortcuts
    RmDir /r "$SMPROGRAMS\ICODE-MDA"
    #Delete Desktop shortcut
    Delete "$DESKTOP\ICODE-MDA Ais Parser.lnk"
  
    #Delete Uninstaller And Unistall Registry Entries
    DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\ICODE-MDA"
    DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ICODE-MDA" 

# uninstaller section end
sectionEnd

;-------------------------------------------------------------
; this function is auto called when installer is complete
;
Function .onInstSuccess 
    MessageBox MB_YESNO "Installion Complete! Would you like to view readme.txt file?" IDNO NoReadme
      ExecShell 'open' "$INSTDIR\readme.txt"
    NoReadme:
FunctionEnd

