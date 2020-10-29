# read_sap_ccms

This in example code, how to use BAPI and get values from CCMS.
Please feel free to modify the code, according to your wishes and needs.
If you have any question, don't hesitate to contact me software.moore@gmail.com

first download SAP RFC SDK

https://support.sap.com/en/product/connectors/nwrfcsdk.html#section_1291717368

SAP RFC SDK Guide

https://support.sap.com/content/dam/support/en_us/library/ssp/products/connectors/nwrfcsdk/NW_RFC_750_ProgrammingGuide.pdf

Enviroment Variables

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/sap_rfc_sdk/nwrfcsdk/lib

export PATH=$PATH:/sap_rfc_sdk/nwrfcsdk/lib

compile commands

g++-7 -fPIC -Wall -g -std=c++17 -DSAPwithUNICODE -I./nwrfcsdk/include -c -o read_ccms.o read_ccms.cpp

g++-7 -g -fPIC -Wall -o read_ccms read_ccms.o ./nwrfcsdk/lib/libsapnwrfc.so ./nwrfcsdk/lib/libsapucum.so

execute:  ./read_ccms
