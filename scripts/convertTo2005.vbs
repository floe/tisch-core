' Converting project and solution files from VisualStudio2008 to 2005
' Based on:
' Textkonvertierung 
' Dr. Michael Luthardt 2008
' http://dr-luthardt.de/
'******************************************************************
set args=WScript.Arguments
set fso=CreateObject("Scripting.FileSystemObject")
Set WshS=WScript.CreateObject("WScript.Shell")


OldSolutionVersionString = "Format Version 10.00"
NewSolutionVersionString = "Format Version 9.00"
Solution2008String = "Visual Studio 2008"
Solution2005String = "Visual Studio 2005"
OldProjectVersion = "Version=""9,00"""
NewProjectVersion = "Version=""8,00"""
Set regEx = New RegExp
regEx.Pattern = OldString
regEx.Global = True

Dim files(9)
files(0) = "..\libtisch.sln"
files(1) = "..\scripts\calibd.vcproj"
files(2) = "..\scripts\calibtool.vcproj"
files(3) = "..\scripts\gestured.vcproj"
files(4) = "..\scripts\libgestures.vcproj"
files(5) = "..\scripts\libsimplecv.vcproj"
files(6) = "..\scripts\libtools.vcproj"
files(7) = "..\scripts\libwidgets.vcproj"
files(8) = "..\scripts\sudoku.vcproj"
files(9) = "..\scripts\touchd.vcproj"

for each file in files
	Input = file
	counter = 0
	
	if  not fso.FileExists(Input) then
		wshs.popup "Die Datei '"&file&"' existiert nicht!", 5, "Convert VS2008 -> VS2005", vbCritical
		set args = Nothing
		set fso  = Nothing
		set WshS  = Nothing
		wsh.quit(2)
	end if 
	
	if fso.GetParentFolderName(Input) = "" then
		Input = replace(WScript.ScriptFullName,  WScript.ScriptName, "")&fso.GetFileName(Input)
		Backup = replace(WScript.ScriptFullName,  WScript.ScriptName, "")&fso.GetBaseName(Input)&"_bak."&fso.GetExtensionName(Input)
		Output = replace(WScript.ScriptFullName,  WScript.ScriptName, "")&fso.GetBaseName(Input)&".tmp"
		fso.copyFile Input, Backup, true
	else
		Input = fso.GetParentFolderName(Input)&"\"&fso.getFileName(input)
		Backup = fso.GetParentFolderName(Input)&"\"&fso.GetBaseName(Input)&"_bak."&fso.GetExtensionName(Input)
		Output = fso.GetParentFolderName(Input)&"\"&fso.GetBaseName(Input)&".tmp"
		fso.copyFile Input, Backup, true
	end if
		
	if fso.FileExists(Output) then
			fso.DeleteFile Output, true
	end if

	set InStream = fso.OpenTextFile(Input, 1, False, -2)
	set OutStream = fso.OpenTextFile(Output, 8, True, -2)
	
	do until InStream.AtEndOfStream
		line = InStream.ReadLine
		regEx.Pattern = OldSolutionVersionString
		line = regEx.Replace(line, NewSolutionVersionString)
		regEx.Pattern = Solution2008String
		line = regEx.Replace(line, Solution2005String)
		regEx.Pattern = OldProjectVersion
		line = regEx.Replace(line, NewProjectVersion)
		OutStream.WriteLine line
	loop
	
	InStream.Close
	OutStream.Close
	fso.copyFile Output, Input, true
	fso.deleteFile Output, true
next

set RegEx = Nothing
set args = Nothing
set fso  = Nothing
set WshS  = Nothing
set InStream = Nothing
set OutStream  = Nothing

wsh.quit(0)
