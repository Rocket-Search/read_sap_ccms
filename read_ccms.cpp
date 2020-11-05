//Compile
//g++ -g -std=c++0x -DSAPwithUNICODE -I./nwrfcsdk/include -c -o read_ccms.o read_ccms.cpp
//Link
//g++ -g -o read_ccms read_ccms.o ./nwrfcsdk/lib/libsapnwrfc.so ./nwrfcsdk/lib/libsapucum.so
//Einzeiler
//original Compile laut SAP
//clear; rm -frv read_ccms.o ; g++ -g -std=c++0x -DSAPwithUNICODE -I./nwrfcsdk/include -c -o read_ccms.o read_ccms.cpp ; g++ -g -fPIC -Wall -o read_ccms read_ccms.o ./nwrfcsdk/lib/libsapnwrfc.so ./nwrfcsdk/lib/libsapucum.so
//C++17 Standard (gcc 7)
//clear; rm -frv read_ccms.o ; g++-7 -fPIC -Wall -g -std=c++17 -DSAPwithUNICODE -I./nwrfcsdk/include -c -o read_ccms.o read_ccms.cpp ; g++-7 -g -fPIC -Wall -o read_ccms read_ccms.o ./nwrfcsdk/lib/libsapnwrfc.so ./nwrfcsdk/lib/libsapucum.so

//Version
//strings libsapnwrfc.so | grep Patch

//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:'/nwrfcsdk/lib'
//export PATH=$PATH:'/nwrfcsdk/lib'

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "nwrfcsdk/include/sapnwrfc.h"
#include "nwrfcsdk/include/sapucrfc.h"

using namespace std;

//Public Variablen für Probleme
RFC_ERROR_INFO errorInfo;
RFC_RC rc = RFC_OK;

//Signal Handler
void signalHandler( int signum)
{
		cout<<"PROBLEM RFC SDK"<<endl;
        cout << "Interrupt signal (" << signum << ") received.\n";
        
		printfU(cU("signalHandler key# %s: %d\n"),errorInfo.key);
		printfU(cU("signalHandler message# %s: %d\n"),errorInfo.message);
		printfU(cU("signalHandler code# %d\n"),errorInfo.code);
		printfU(cU("signalHandler rc # %d\n"),rc);
        
		//exit (666);
        exit (- 1);
}

//int main(int argc, char *argv[])
int mainU(int argc, SAP_UC** argv)
{


//SAP Login #################################
	#define NUM_PARAMS 9
	cout<<"SAP Login"<<endl;
	
	//RFC_ERROR_INFO errorInfo;
	RFC_CONNECTION_HANDLE conn;
	RFC_CONNECTION_PARAMETER loginParams[NUM_PARAMS];
	//RFC_RC rc = RFC_OK;

	loginParams[0].name = cU("dest");
	loginParams[0].value = cU("DEV");
	loginParams[1].name =cU("user");
	loginParams[1].value = cU("RFC_Test");
	loginParams[2].name = cU("passwd");
	loginParams[2].value = cU("Pa$$w0rD");
	loginParams[3].name = cU("ASHOST");
	loginParams[3].value = cU("hostname");
	loginParams[4].name = cU("SYSID");
	loginParams[4].value = cU("SAP-SID");
	loginParams[5].name = cU("SYSNR");
	loginParams[5].value = cU("System-Nr");
	loginParams[6].name = cU("CLIENT");
	loginParams[6].value = cU("Client-Nr");
	loginParams[7].name = cU("lang");
	loginParams[7].value = cU("EN");
	//##
	loginParams[8].name = cU("TRACE");
	loginParams[8].value = cU("1");
	
	conn = RfcOpenConnection(loginParams, NUM_PARAMS, &errorInfo);
	
	/*
	if (conn == NULL) 
	{
		cout<<"FEHLER#"<<endl;
		printfU(cU("OUTPUT %s: %d\n"),errorInfo.key);
		printfU(cU("OUTPUT %s: %d\n"),errorInfo.message);
	}
	*/								  
	if (errorInfo.code != RFC_OK) 
	{
		cout<<"Login PROBLEM"<<endl;
		printfU(cU("key# %s: %d\n"),errorInfo.key);
		printfU(cU("message# %s: %d\n"),errorInfo.message);
		printfU(cU("code# %d\n"),errorInfo.code);
				
	}
	if (errorInfo.code == RFC_OK) 
	{
		cout<<"Login erfolgreich"<<endl;
		//printfU(cU("message# %s: %d\n"),errorInfo.message);
		//printfU(cU("code# %d\n"),errorInfo.code);
		
	}
cout<<"##########################################################################################"<<endl;
//###########################################################################
//Im SAP Gui den BAPI explorer öffnen /nbapi
//	-> Hirachical View -> Basis Components -> Use Subcomponents

//Zur Verfügung stehneden Monitore anzeigen
	RFC_FUNCTION_DESC_HANDLE ccms_bapi_handle;
	RFC_FUNCTION_HANDLE rfc_handle_1;
	RFC_FUNCTION_HANDLE rfc_handle_2;
	
//####BAPI_XMI_LOGON Login. Login für CCMS notwendig
/*Ausgabe laut trace file
Repository::queryFunctionMetaData(BAPI_XMI_LOGON) via handle 26909520 returns function meta 19cf480
name=BAPI_XMI_LOGON (origin=SAP-SID) with 6 parameters
parameter 0 (RETURN, RFCTYPE_STRUCTURE, RFC_EXPORT, 548, 1088, 0, 19e7530, , , (nil))
parameter 1 (SESSIONID, RFCTYPE_CHAR, RFC_EXPORT, 24, 48, 0, (nil), , Unique XMI session identification, (nil))
parameter 2 (EXTCOMPANY, RFCTYPE_CHAR, RFC_IMPORT, 16, 32, 0, (nil), , Manufacturer of the tool you should log onto, (nil))
parameter 3 (EXTPRODUCT, RFCTYPE_CHAR, RFC_IMPORT, 16, 32, 0, (nil), , Name of the tool to be logged on, (nil))
parameter 4 (INTERFACE, RFCTYPE_CHAR, RFC_IMPORT, 3, 6, 0, (nil), , Interface onto which you should log on, (nil))
parameter 5 (VERSION, RFCTYPE_CHAR, RFC_IMPORT, 10, 20, 0, (nil), , Version of the interface, (
*/
	RFC_STRUCTURE_HANDLE returnStructure;
	SAP_UC message[8192] = iU("");
	unsigned resultLen = 8192;
	ccms_bapi_handle = RfcGetFunctionDesc(conn, cU("BAPI_XMI_LOGON"), &errorInfo);
	cout<<"BAPI_XMI_LOGON"<<endl;
	rfc_handle_1 = RfcCreateFunction(ccms_bapi_handle, &errorInfo);
	RfcSetChars(rfc_handle_1, cU("EXTCOMPANY"), cU("TESTCOPMANY"), 11, &errorInfo);
	RfcSetChars(rfc_handle_1, cU("EXTPRODUCT"), cU("TESTPRODUKT"), 11, &errorInfo);
	RfcSetChars(rfc_handle_1, cU("INTERFACE"), cU("XAL"), 3, &errorInfo);
	RfcSetChars(rfc_handle_1, cU("VERSION"), cU("1.0"), 3, &errorInfo);
	rc = RfcInvoke(conn, rfc_handle_1, &errorInfo);
	//####
cout<<"##########################################################################################"<<endl;
	
/*Ausgabe laut trace file
Repository::queryFunctionMetaData(BAPI_SYSTEM_MON_GETLIST) via handle 26909520 returns function meta 19a5ca0
	 name=BAPI_SYSTEM_MON_GETLIST (origin=SAP-SID) with 4 parameters
	 parameter 0 (RETURN, RFCTYPE_STRUCTURE, RFC_EXPORT, 548, 1088, 0, 19e7530, , Return Messages, (nil))
	 parameter 1 (EXTERNAL_USER_NAME, RFCTYPE_CHAR, RFC_IMPORT, 16, 32, 0, (nil), , Name of the SAP-External User, (nil))
	 parameter 2 (MONI_SET_NAME, RFCTYPE_STRUCTURE, RFC_IMPORT, 60, 120, 0, 19d4ae0, , Monitor Set Name, (nil))
	 parameter 3 (MONITOR_NAMES, RFCTYPE_TABLE, RFC_TABLES, 120, 240, 0, 19d4780, , Names of the Monitors, (nil))
*/
	//####BAPI_SYSTEM_MON_GETLIST. Einlesen aller Monitore einer Monitorsammlung
	cout<<"BAPI_SYSTEM_MON_GETLIST"<<endl;
	ccms_bapi_handle = RfcGetFunctionDesc(conn, cU("BAPI_SYSTEM_MON_GETLIST"), &errorInfo);
	rfc_handle_2 = RfcCreateFunction(ccms_bapi_handle, &errorInfo);
	RfcSetChars(rfc_handle_2, cU("EXTERNAL_USER_NAME"), cU("RFC_TEST"), 8, &errorInfo);
	
	//####Daten an SAP senden
	rc = RfcInvoke(conn, rfc_handle_2, &errorInfo);
	
	//###Daten ausgeben, Tabllenzeile für Tabellenzeile
	unsigned rowCount = 0;
	RFC_TABLE_HANDLE table;
	rc = RfcGetTable(rfc_handle_2, cU("MONITOR_NAMES"),&table,&errorInfo);
	RfcGetRowCount(table, &rowCount, &errorInfo);
	printfU(cU("RfcGetRowCount# %d\n"), rowCount);
	
	//SAP_UC buffer[256] = iU("");
	SAP_UC buffer[4096] = iU("");
	RfcGetString(table, cU("MS_NAME"), buffer,sizeofU(buffer), &resultLen, &errorInfo);
		//printfU(cU("RfcGetString MS_NAME# %s\n"), buffer);
	RfcGetString(table, cU("MONI_NAME"), buffer,sizeofU(buffer), &resultLen, &errorInfo);
		//printfU(cU("RfcGetString MONI_NAME# %s\n"), buffer);
	
	//##57 Tabllen Zeilen weiterspringen
	RfcMoveTo(table , 57,&errorInfo);
	//for (int i = 0; i < 57; i++)
	//{
	//	RfcMoveToNextRow(table, &errorInfo);
	//}
	RfcGetString(table, cU("MS_NAME"), buffer,sizeofU(buffer), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MS_NAME# %s\n"), buffer);
	RfcGetString(table, cU("MONI_NAME"), buffer,sizeofU(buffer), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MONI_NAME# %s\n"), buffer);	
		
/*			
	for (unsigned i=0; i<rowCount; ++i) 
	{
		RfcMoveTo(table, i, NULL);
		RfcGetString(table, cU("MS_NAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				//#Template Name
			//printfU(cU("MS_NAME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MONI_NAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				//#Monitor Name
			//printfU(cU("MONI_NAME in row %d = %s\n"), i, buffer);
	}
*/
cout<<"##########################################################################################"<<endl;
//#####################################################################################################
//CCMS Monitor name
//BAPI_SYSTEM_MON_GETTREE
/*
Repository::queryFunctionMetaData(BAPI_SYSTEM_MON_GETTREE) via handle 40209232 returns function meta 2654b40
name=BAPI_SYSTEM_MON_GETTREE (origin=SAP-SID) with 7 parameters
parameter 0 (RETURN, RFCTYPE_STRUCTURE, RFC_EXPORT, 548, 1088, 0, 2696530, , BAPI Return Structure, (nil))
parameter 1 (EXTERNAL_USER_NAME, RFCTYPE_CHAR, RFC_IMPORT, 16, 32, 0, (nil), , Name of the SAP-External User, (nil))
parameter 2 (MAX_TREE_DEPTH, RFCTYPE_INT, RFC_IMPORT, 4, 4, 0, (nil), 0, , (nil))
parameter 3 (MONITOR_NAME, RFCTYPE_STRUCTURE, RFC_IMPORT, 120, 240, 0, 2683780, , Name of the Monitor, (nil))
parameter 4 (VIS_ON_USR_LEVEL, RFCTYPE_INT, RFC_IMPORT, 4, 4, 0, (nil), 3, , (nil))
parameter 5 (TREE_NODES, RFCTYPE_TABLE, RFC_TABLES, 440, 828, 0, 26b96e0, , Nodes of the Monitor Tree, (nil))
parameter 6 (TREE_NODES2, RFCTYPE_TABLE, RFC_TABLES, 696, 1340, 0, 26a7290, , The Nodes of a Monitoring Tree (Extended with the Long Monitor Names), (nil))
*/
	cout<<"BAPI_SYSTEM_MON_GETTREE"<<endl;

	ccms_bapi_handle = RfcGetFunctionDesc(conn, cU("BAPI_SYSTEM_MON_GETTREE"), &errorInfo);
		printfU(cU("RfcGetFunctionDesc BAPI_SYSTEM_MON_GETTREE key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcGetFunctionDesc BAPI_SYSTEM_MON_GETTREE message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcGetFunctionDesc BAPI_SYSTEM_MON_GETTREE code# %d\n"),errorInfo.code);
		
	rfc_handle_2 = RfcCreateFunction(ccms_bapi_handle, &errorInfo);
		printfU(cU("RfcCreateFunction ccms_bapi_handle key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcCreateFunction ccms_bapi_handle message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcCreateFunction ccms_bapi_handle code# %d\n"),errorInfo.code);

	RfcSetInt(rfc_handle_2, cU("MAX_TREE_DEPTH"), 0,&errorInfo);
		printfU(cU("RfcSetInt MAX_TREE_DEPTH key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcSetInt MAX_TREE_DEPTH message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcSetInt MAX_TREE_DEPTH code# %d\n"),errorInfo.code);
	
	RfcSetInt(rfc_handle_2, cU("VIS_ON_USR_LEVEL"), 3,&errorInfo);
		printfU(cU("RfcSetInt VIS_ON_USR_LEVEL key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcSetInt VIS_ON_USR_LEVEL message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcSetInt VIS_ON_USR_LEVEL code# %d\n"),errorInfo.code);

	RfcSetChars(rfc_handle_2, cU("EXTERNAL_USER_NAME"), cU("RFC_TEST"), 8, &errorInfo);
		printfU(cU("RfcSetChars EXTERNAL_USER_NAME key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcSetChars EXTERNAL_USER_NAME message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcSetChars EXTERNAL_USER_NAME code# %d\n"),errorInfo.code);

//################################################################################################################################
	//Die Import (senden) Struktur festlegen. 
	//1. die Imp. Struktur mittels "RfcGetStructure" erfassen. 
	//2. die Struktur mittels "RfcSetStructure" zum senden freigeben.
	//3. Die Werte der zusendenen Struktur mittels "RfcSetChars" festlegen.
	RfcGetStructure(rfc_handle_2, cU("MONITOR_NAME"), &returnStructure, &errorInfo);
			printfU(cU("RfcGetStructure MONITOR_NAME key# %s: %d\n"),errorInfo.key);
			printfU(cU("RfcGetStructure MONITOR_NAME message# %s: %d\n"),errorInfo.message);
			printfU(cU("RfcGetStructure MONITOR_NAME code# %d\n"),errorInfo.code);

	RfcSetStructure(rfc_handle_2, cU("MONITOR_NAME"),returnStructure, &errorInfo);
			printfU(cU("RfcSetStructure MONITOR_NAME key# %s: %d\n"),errorInfo.key);
			printfU(cU("RfcSetStructure MONITOR_NAME message# %s: %d\n"),errorInfo.message);
			printfU(cU("RfcSetStructure MONITOR_NAME code# %d\n"),errorInfo.code);

	RfcSetChars(returnStructure, cU("MS_NAME"), cU("SAP CCMS Monitor Templates"),26, &errorInfo);
			printfU(cU("RfcSetChars MS_NAME key# %s: %d\n"),errorInfo.key);
			printfU(cU("RfcSetChars MS_NAME message# %s: %d\n"),errorInfo.message);
			printfU(cU("RfcSetChars MS_NAME code# %d\n"),errorInfo.code);
	RfcSetChars(returnStructure, cU("MONI_NAME"), cU("Filesystems"),11, &errorInfo);
			printfU(cU("RfcSetChars MONI_NAME key# %s: %d\n"),errorInfo.key);
			printfU(cU("RfcSetChars MONI_NAME message# %s: %d\n"),errorInfo.message);
			printfU(cU("RfcSetChars MONI_NAME code# %d\n"),errorInfo.code);
//################################################################################################################################
		
	rc = RfcInvoke(conn, rfc_handle_2, &errorInfo);
		printfU(cU("RfcInvoke key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcInvoke message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcInvoke code# %d\n"),errorInfo.code);
		printfU(cU("RfcInvoke rc # %d\n"),rc);
	
	RfcGetStructure(rfc_handle_2, cU("RETURN"), &returnStructure, &errorInfo);
		printfU(cU("RfcGetStructure RETURN# %s: \n"),returnStructure[0]);
		printfU(cU("RfcGetStructure RETURN key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcGetStructure RETURN message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcGetStructure RETURN code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("MESSAGE"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MESSAGE message# %s\n"), message);
		printfU(cU("RfcGetString MESSAGE key# %s: %d\n"),errorInfo.key);
		printfU(cU("RfcGetString MESSAGE message# %s: %d\n"),errorInfo.message);
		printfU(cU("RfcGetString MESSAGE code# %d\n"),errorInfo.code);
			
	rowCount = 0;
	
	rc = RfcGetTable(rfc_handle_2, cU("TREE_NODES"),&table,&errorInfo);
	RfcGetRowCount(table, &rowCount, &errorInfo);
	printfU(cU("RfcGetRowCount TREE_NODES# %d\n"), rowCount);

	for (unsigned i=0; i<rowCount; ++i) 
	{
		RfcMoveTo(table, i, NULL);
		
		RfcGetString(table, cU("MTSYSID"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTSYSID in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MTMCNAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTMCNAME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MTNUMRANGE"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTNUMRANGE in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MTUID"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTUID in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MTCLASS"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTCLASS in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MTINDEX"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTINDEX in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("EXTINDEX"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("EXTINDEX in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALTREENUM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALTREENUM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALIDXINTRE"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALIDXINTRE in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALLEVINTRE"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALLEVINTRE in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALPARINTRE"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALPARINTRE in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("OBJECTNAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("OBJECTNAME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MTNAMESHRT"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MTNAMESHRT in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("CUSGRPNAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("CUSGRPNAME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("DELIVERSTA"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("DELIVERSTA in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("HIGHALVAL"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("HIGHALVAL in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("HIGHALSEV"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("HIGHALSEV in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALSYSID"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALSYSID in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("MSEGNAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("MSEGNAME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALUNIQNUM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALUNIQNUM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALINDEX"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALINDEX in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALERTDATE"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALERTDATE in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ALERTTIME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALERTTIME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("DUMMYALIGN"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("DUMMYALIGN in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("LASTVALDAT"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("LASTVALDAT in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("LASTVALTIM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("LASTVALTIM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("LASTVALDUM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("LASTVALDUM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ACTUALVAL"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ACTUALVAL in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("ACTUALSEV"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ACTUALSEV in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VALSYSID"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VALSYSID in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VMSEGNAME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VMSEGNAME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VALUNIQNUM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VALUNIQNUM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VALINDEX"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VALINDEX in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VALERTDATE"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VALERTDATE in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VALERTTIME"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VALERTTIME in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VALERTDUM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VALERTDUM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("COUNTOFACT"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("COUNTOFACT in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("COUNTSUM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("COUNTSUM in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("VISUSERLEV"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("VISUSERLEV in row %d = %s\n"), i, buffer);
		RfcGetString(table, cU("TDSTATUS"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("TDSTATUS in row %d = %s\n"), i, buffer);
			
	}
	
	rowCount = 0;
	//RFC_TABLE_HANDLE table;
	rc = RfcGetTable(rfc_handle_2, cU("TREE_NODES2"),&table,&errorInfo);
	RfcGetRowCount(table, &rowCount, &errorInfo);
	printfU(cU("RfcGetRowCount TREE_NODES2# %d\n"), rowCount);
	for (unsigned i=0; i<rowCount; ++i) 
	{
		RfcMoveTo(table, i, NULL);
		
		RfcGetString(table, cU("ALMTFULLNM"), buffer,sizeofU(buffer), NULL, &errorInfo);				
			printfU(cU("ALMTFULLNM in row %d = %s\n"), i, buffer);
	}


//#####################################################################################################
cout<<"##########################################################################################"<<endl;

//#####################################################################################################
//BAPI_SYSTEM_MTE_GETTIDBYNAME
//##
/* //Ausgabe laut trace File
Repository::queryFunctionMetaData(BAPI_SYSTEM_MTE_GETTIDBYNAME) via handle 36539216 returns function meta 22d4b40
name=BAPI_SYSTEM_MTE_GETTIDBYNAME (origin=SAP-SID) with 7 parameters
parameter 0 (RETURN, RFCTYPE_STRUCTURE, RFC_EXPORT, 548, 1088, 0, 2316530, , Return Messages, (nil))
parameter 1 (TID, RFCTYPE_STRUCTURE, RFC_EXPORT, 84, 168, 0, 232e160, , The returned MTE ID (TID), (nil))
parameter 2 (CONTEXT_NAME, RFCTYPE_CHAR, RFC_IMPORT, 40, 80, 0, (nil), , The context name, (nil))
parameter 3 (EXTERNAL_USER_NAME, RFCTYPE_CHAR, RFC_IMPORT, 16, 32, 0, (nil), , Name of the SAP-External User, (nil))
parameter 4 (MTE_NAME, RFCTYPE_CHAR, RFC_IMPORT, 40, 80, 0, (nil), , MTE Name, (nil))
parameter 5 (OBJECT_NAME, RFCTYPE_CHAR, RFC_IMPORT, 40, 80, 0, (nil), , Object Name, (nil))
parameter 6 (SYSTEM_ID, RFCTYPE_CHAR, RFC_IMPORT, 8, 16, 0, (nil), , System ID, (nil))
*/
	cout<<"BAPI_SYSTEM_MTE_GETTIDBYNAME"<<endl;

	ccms_bapi_handle = RfcGetFunctionDesc(conn, cU("BAPI_SYSTEM_MTE_GETTIDBYNAME"), &errorInfo);
		
	rfc_handle_2 = RfcCreateFunction(ccms_bapi_handle, &errorInfo);

/*	
	//#Aufbau laut CCMS im SAP "SAP-SID\hostname_SAP-SID_System-Nr\R3Services\Background\AbortedJobs" entspricht -> SYSTEM_ID \ CONTEXT_NAME \ <Nichts> \  OBJECT_NAME \ MTE_NAME
	RfcSetChars(rfc_handle_2, cU("CONTEXT_NAME"), cU("hostname_SAP-SID_System-Nr"), 13, &errorInfo);	
	RfcSetChars(rfc_handle_2, cU("EXTERNAL_USER_NAME"), cU("RFC_TEST"), 8, &errorInfo);
	RfcSetChars(rfc_handle_2, cU("MTE_NAME"), cU("AbortedJobs"), 11, &errorInfo);	
	RfcSetChars(rfc_handle_2, cU("OBJECT_NAME"), cU("Background"), 10, &errorInfo);	
	RfcSetChars(rfc_handle_2, cU("SYSTEM_ID"), cU("SAP-SID"), 3, &errorInfo);	
*/	
	cout<<"CCMS: \'SAP-SID\\hostname_SAP-SID_System-Nr\\OperatingSystem\\Filesystems\\/tmp/\\Freespace\'"<<endl;
	//#Darstellung in der CCMS Menu im SAP # SAP-SID\hostname_SAP-SID_System-Nr\OperatingSystem\Filesystems\/tmp\Freespace
	RfcSetChars(rfc_handle_2, cU("CONTEXT_NAME"), cU("hostname_SAP-SID_System-Nr"), 13, &errorInfo);	
	RfcSetChars(rfc_handle_2, cU("EXTERNAL_USER_NAME"), cU("RFC_TEST"), 8, &errorInfo);
	RfcSetChars(rfc_handle_2, cU("MTE_NAME"), cU("Freespace"), 9, &errorInfo);	
	RfcSetChars(rfc_handle_2, cU("OBJECT_NAME"), cU("/tmp"), 4, &errorInfo);	
	//RfcSetChars(rfc_handle_2, cU("OBJECT_NAME"), cU("/usr"), 4, &errorInfo);	
	RfcSetChars(rfc_handle_2, cU("SYSTEM_ID"), cU("SAP-SID"), 3, &errorInfo);	
	
	rc = RfcInvoke(conn, rfc_handle_2, &errorInfo);

//RFC_STRUCTURE_HANDLE returnStructure;
//SAP_UC message[8192] = iU("");
//unsigned utf8Len = 8192, resultLen;

	RfcGetStructure(rfc_handle_2, cU("RETURN"), &returnStructure, &errorInfo);
		//printfU(cU("RfcGetStructure RETURN# %s: \n"),returnStructure[0]);
		//printfU(cU("RfcGetStructure RETURN key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetStructure RETURN message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetStructure RETURN code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("MESSAGE"), message, sizeofU(message), &resultLen, &errorInfo);
		//printfU(cU("RfcGetString MESSAGE message %s\n"), message);
		//printfU(cU("RfcGetString MESSAGE key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetString MESSAGE message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetString MESSAGE code# %d\n"),errorInfo.code);

	RfcGetStructure(rfc_handle_2, cU("TID"), &returnStructure, &errorInfo);
		//printfU(cU("RfcGetStructure TID# %s: \n"),returnStructure[0]);
		//printfU(cU("RfcGetStructure TID key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetStructure TID message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetStructure TID code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("MTSYSID"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MTSYSID message %s\n"), message);
		//printfU(cU("RfcGetString MTSYSID key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetString MTSYSID message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetString MTSYSID code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("MTMCNAME"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MTMCNAME message %s\n"), message);
	RfcGetString(returnStructure, cU("MTNUMRANGE"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MTNUMRANGE message %s\n"), message);
	RfcGetString(returnStructure, cU("MTUID"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MTUID message %s\n"), message);
	RfcGetString(returnStructure, cU("MTCLASS"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MTCLASS message %s\n"), message);
	RfcGetString(returnStructure, cU("MTINDEX"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MTINDEX message %s\n"), message);
	RfcGetString(returnStructure, cU("EXTINDEX"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString EXTINDEX message %s\n"), message);
cout<<"##########################################################################################"<<endl;
//#####################################################################################################
//BAPI_SYSTEM_MTE_GETPERFCURVAL
//##
/*
RfcGetFunctionDesc(BAPI_SYSTEM_MTE_GETPERFCURVAL) via handle 36285264
Repository::queryFunctionMetaData(BAPI_SYSTEM_MTE_GETPERFCURVAL) via handle 36285264 returns function meta 22c8740
name=BAPI_SYSTEM_MTE_GETPERFCURVAL (origin=SAP-SID) with 4 parameters
parameter 0 (CURRENT_VALUE, RFCTYPE_STRUCTURE, RFC_EXPORT, 104, 140, 0, 22ea8f0, , Current Performance Attribute Values, (nil))
parameter 1 (RETURN, RFCTYPE_STRUCTURE, RFC_EXPORT, 548, 1088, 0, 22d8530, , Return Messages, (nil))
parameter 2 (EXTERNAL_USER_NAME, RFCTYPE_CHAR, RFC_IMPORT, 16, 32, 0, (nil), , Name of the SAP-External User, (nil))
parameter 3 (TID, RFCTYPE_STRUCTURE, RFC_IMPORT, 84, 168, 0, 22f0160, , MTE ID (TID), (nil))
*/
	cout<<"BAPI_SYSTEM_MTE_GETPERFCURVAL"<<endl;
	ccms_bapi_handle = RfcGetFunctionDesc(conn, cU("BAPI_SYSTEM_MTE_GETPERFCURVAL"), &errorInfo);

	rfc_handle_2 = RfcCreateFunction(ccms_bapi_handle, &errorInfo);

	RfcSetChars(rfc_handle_2, cU("EXTERNAL_USER_NAME"), cU("RFC_TEST"), 8, &errorInfo);
	//RfcSetChars(tabLine, cU("EXTERNAL_USER_NAME"), buffer, strlenU(buffer), &errorInfo);
		//printfU(cU("RfcSetChars EXTERNAL_USER_NAME key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcSetChars EXTERNAL_USER_NAME message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcSetChars EXTERNAL_USER_NAME code# %d\n"),errorInfo.code);
		
	/* TID aus vorherigen BAPI (//SAP-SID\hostname_SAP-SID_System-Nr\OperatingSystem\Filesystems\/tmp\Freespace)
	FieldName: MTSYSID              Value: SAP-SID
	ieldName: MTMCNAME             Value: hostname_SAP-SID_System-Nr
	FieldName: MTNUMRANGE           Value: 005
	FieldName: MTUID                Value: 0000000146
	FieldName: MTCLASS              Value: 100
	FieldName: MTINDEX              Value: 0000000093
	FieldName: EXTINDEX             Value: 0000000028
	*/
	//sapnwrfc.h:    RfcSetStructure(DATA_CONTAINER_HANDLE dataHandle, SAP_UC const* name, const RFC_STRUCTURE_HANDLE value, RFC_ERROR_INFO* errorInfo);

	//##Verknüpft durch das RFC handle "rfc_handle_2" und der Struktur "returnStructure"
	//###RfcGetStructure(rfc_handle_2, cU("TID"), &returnStructure, &errorInfo);
	rc = RfcSetStructure(rfc_handle_2, cU("TID"),returnStructure, &errorInfo);
		//printfU(cU("RfcSetStructure RETURN key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcSetStructure RETURN message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcSetStructure RETURN code# %d\n"),errorInfo.code);
		//printfU(cU("RfcSetStructure rc # %d\n"),rc);
		
	rc = RfcInvoke(conn, rfc_handle_2, &errorInfo);
		//printfU(cU("RfcInvoke key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcInvoke message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcInvoke code# %d\n"),errorInfo.code);
		//printfU(cU("RfcInvoke rc # %d\n"),rc);
		
	RfcGetStructure(rfc_handle_2, cU("RETURN"), &returnStructure, &errorInfo);
		//printfU(cU("RfcGetStructure RETURN# %s: \n"),returnStructure[0]);
		//printfU(cU("RfcGetStructure RETURN key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetStructure RETURN message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetStructure RETURN code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("MESSAGE"), message, sizeofU(message), &resultLen, &errorInfo);
		//printfU(cU("RfcGetString MESSAGE message %s\n"), message);
		//printfU(cU("RfcGetString MESSAGE key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetString MESSAGE message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetString MESSAGE code# %d\n"),errorInfo.code);
	
	RfcGetStructure(rfc_handle_2, cU("CURRENT_VALUE"), &returnStructure, &errorInfo);
		//printfU(cU("RfcGetStructure CURRENT_VALUE# %s: \n"),returnStructure[0]);
		//printfU(cU("RfcGetStructure CURRENT_VALUE key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetStructure CURRENT_VALUE message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetStructure CURRENT_VALUE code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("ALRELEVVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString ALRELEVVAL message %s\n"), message);
		//printfU(cU("RfcGetString ALRELEVVAL key# %s: %d\n"),errorInfo.key);
		//printfU(cU("RfcGetString ALRELEVVAL message# %s: %d\n"),errorInfo.message);
		//printfU(cU("RfcGetString ALRELEVVAL code# %d\n"),errorInfo.code);
	RfcGetString(returnStructure, cU("ALRELVALDT"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString ALRELVALDT message %s\n"), message);
	RfcGetString(returnStructure, cU("ALRELVALTI"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString ALRELVALTI message %s\n"), message);
	RfcGetString(returnStructure, cU("ALRELVALDT"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString LASTALSTAT message %s\n"), message);
	RfcGetString(returnStructure, cU("LASTALSTAT"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString LASTPERVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("LASTPERVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG01PVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG01PVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG05PVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG05PVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG15PVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG15PVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString ALRELVALDT message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG01SVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG01SVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG05SVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG05SVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG15SVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG15SVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG01CVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG01CVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG05CVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG05CVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("AVG15CVAL"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString AVG15CVAL message %s\n"), message);
	RfcGetString(returnStructure, cU("MAXPFVALUE"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MAXPFVALUE message %s\n"), message);
	RfcGetString(returnStructure, cU("MAXPFDATE"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MAXPFDATE message %s\n"), message);
	RfcGetString(returnStructure, cU("MAXPFTIME"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MAXPFTIME message %s\n"), message);
	RfcGetString(returnStructure, cU("MINPFVALUE"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MINPFVALUE message %s\n"), message);
	RfcGetString(returnStructure, cU("MINPFDATE"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MINPFDATE message %s\n"), message);
	RfcGetString(returnStructure, cU("MINPFTIME"), message, sizeofU(message), &resultLen, &errorInfo);
		printfU(cU("RfcGetString MINPFTIME message %s\n"), message);
	
	
//#####################################################################################################


	RfcDestroyFunction(rfc_handle_2, &errorInfo);
	RfcDestroyFunction(rfc_handle_1, &errorInfo);
	RfcCloseConnection(conn, NULL);
	
//###########################################################################	

	return(0);
}


