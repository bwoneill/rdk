cd libRdk
rootcint -f RDKDict.cpp -c RDKHeaders.h RDKLinkDef.h
bjam release debug optimization=speed warnings=off -j 8 %1
cd ..