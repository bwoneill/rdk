robocopy images %DROPBOX%\Public\images /b /copyall
robocopy .\ %DROPBOX%\Public pwasu.tex pwasu.sty pwasu.pdf pwasu.bib cclicence.xmp /b /copyall
7z u %DROPBOX%\Public\images.zip images
7z u %DROPBOX%\Public\thesis.zip images pwasu.tex pwasu.sty pwasu.bib cclicence.xmp
pause