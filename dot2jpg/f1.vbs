 'Create DOT Object 
                Dim dot
                Set dot = CreateObject("Wingraphviz.dot")
                
                
                if dot is Nothing then
                                Msgbox "Unable to load the object Wingraphviz.dot"      
                end if
                
        'Sample data of DOT     
                strCMD2 = "digraph G {A -> B -> C -> D;}"                              
        
        
        'Check DOT Format
                if(not dot.Validate(strCMD2)) then
                        msgbox "DOT Format Error"
                End if                          
                
        'Generate SVG
                Dim strSVG
                strSVG = dot.ToSVG(strCMD2)
        
                if(len(strSVG) > 0) then
                        Msgbox "DOT as SVG output is " + vbcrlf + strSVG
                end if
        
        'Generate TextImage(PostScript)
                Dim strPS
                strPS = dot.ToPS(strCMD2)
        
                if(len(strPS) > 0) then
                        Msgbox "DOT as PostScript output is " + vbcrlf + strPS
                end if
                
        'Generate BinaryImage(PNG)      
                Dim Img         
                Set Img = dot.ToPNG(strCMD2)
                                
                if Img is Nothing then
                        Msgbox "Unable to generate to PNG"      
                end if                          
        
                Dim FilePath
                FilePath = "c:\test.PNG"
                                                
                if Img.save(FilePath) then
                        
                        Msgbox "BinaryImage Type is " + Img.Type + vbcrlf + _
                                        "Save Image at " + FilePath
                else
                        Msgbox "Unable to save image at " + FilePath
                end if          
                                        
        
        'Release Resource
                Set Img = Nothing
                
                Set dot = Nothing
      