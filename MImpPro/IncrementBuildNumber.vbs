    Set fso = CreateObject("Scripting.FileSystemObject")

    ' Get build number from file
    SET in_file = fso.OpenTextFile("Include\version.h", 1, False)
    ReadLine = in_file.ReadLine
    Set r = new RegExp
    r.Pattern = ".*\((.*)\)"
    r.IgnoreCase = True
    r.Global = True
    Set Matches =  r.Execute(ReadLine)
    '  in_file = Nothing

    NumberStr = Matches(0).Submatches(0)
    Number = CInt(NumberStr)

    SET version_file = fso.OpenTextFile("Include\version.h", 2, True)

    ' Write command file
    version_file.Write ("#define BUILD_NUMBER (" + CStr(Number+1) +")")
    version_file.WriteBlankLines(1)
    version_file.Write ("#define BUILD_NUMBER_STR """ + CStr(Number+1) + """")
    version_file.WriteBlankLines(1)
    version_file.Close

    SET wix_version_file = fso.OpenTextFile("MImpSetup\version.wxi", 2, True)

    ' Write command file
    wix_version_file.Write ("<Include>")
    wix_version_file.Write (" <?define Version=""7.0.0." + CStr(Number+1) +""" ?>")
    wix_version_file.Write ("</Include>")
    wix_version_file.Close

    
