# Microsoft Developer Studio Project File - Name="Client App" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Client App - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Client App.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Client App.mak" CFG="Client App - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Client App - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Client App - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Client App - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 user32.lib dplayx.lib dxguid.lib ddraw.lib kernel32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib dinput.lib dsound.lib winmm.lib fmodvc.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Client App - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib dplayx.lib dxguid.lib ddraw.lib kernel32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib dinput.lib dsound.lib winmm.lib fmodvc.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Client App - Win32 Release"
# Name "Client App - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "App"

# PROP Default_Filter ""
# Begin Group "OpenGL"

# PROP Default_Filter ""
# Begin Group "Text"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\GLText.cpp
# End Source File
# Begin Source File

SOURCE=.\TextObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\TextObjMethods.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\API_OpenGL.cpp
# End Source File
# Begin Source File

SOURCE=.\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Blt2DopenGL.cpp
# End Source File
# Begin Source File

SOURCE=.\glParticles.cpp
# End Source File
# Begin Source File

SOURCE=.\RedrawOpenGL.cpp
# End Source File
# Begin Source File

SOURCE=.\tga.cpp
# End Source File
# End Group
# Begin Group "DDraw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\API_DDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\asm.cpp
# End Source File
# Begin Source File

SOURCE=.\DDParticles.cpp
# End Source File
# Begin Source File

SOURCE=.\DDrawText.cpp
# End Source File
# Begin Source File

SOURCE=.\RedrawDDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\RGB16_DDraw.cpp
# End Source File
# End Group
# Begin Group "DInput"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dinput.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\App.cpp
# End Source File
# Begin Source File

SOURCE=.\DPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\dxerror.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\UserSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# End Group
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Group "3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\board.cpp
# End Source File
# Begin Source File

SOURCE=.\camera3d.cpp
# End Source File
# Begin Source File

SOURCE=.\face3d.cpp
# End Source File
# Begin Source File

SOURCE=.\GEM3D.CPP
# End Source File
# Begin Source File

SOURCE=.\object3d.cpp
# End Source File
# Begin Source File

SOURCE=.\vector3d.cpp
# End Source File
# End Group
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChatInput.cpp
# End Source File
# Begin Source File

SOURCE=.\GameButtons.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemenu.cpp
# End Source File
# Begin Source File

SOURCE=.\IPInput.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuButtons.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\GameData.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=.\Score.cpp
# End Source File
# Begin Source File

SOURCE=.\Timer.cpp
# End Source File
# End Group
# Begin Group "soundlatest"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gsound.cpp
# End Source File
# Begin Source File

SOURCE=.\GSoundSamples.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundFile.cpp
# End Source File
# End Group
# Begin Group "database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Database.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\mmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\mmgr.h
# End Source File
# Begin Source File

SOURCE=.\WndProc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Game Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\App.h
# End Source File
# Begin Source File

SOURCE=.\board.h
# End Source File
# Begin Source File

SOURCE=.\camera3d.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\gamemenu.h
# End Source File
# Begin Source File

SOURCE=.\gem3d.h
# End Source File
# Begin Source File

SOURCE=.\model.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Group "OpenGL Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\Blt2DopenGL.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\GLSurfDef.h
# End Source File
# Begin Source File

SOURCE=.\text.h
# End Source File
# Begin Source File

SOURCE=.\tga.h
# End Source File
# End Group
# Begin Group "DirectX Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dinput.h
# End Source File
# Begin Source File

SOURCE=.\DPlay.h
# End Source File
# Begin Source File

SOURCE=.\dxerror.h
# End Source File
# Begin Source File

SOURCE=.\utility.h
# End Source File
# End Group
# Begin Group "soundlatest Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fmod.h
# End Source File
# Begin Source File

SOURCE=.\fmod_errors.h
# End Source File
# Begin Source File

SOURCE=.\gsound.h
# End Source File
# Begin Source File

SOURCE=.\wincompat.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TimerUpdate.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\win_utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\win_utilities.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Data\Menu\GameData.txt
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Data\Menu\MenuData.txt
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# End Target
# End Project
