cd libRdk
rootcint -f RDKLinuxDict.cpp -c RDKHeaders.h RDKLinkDef.h
bjam release debug optimization=speed warnings=off -j 32 $1
cd ..