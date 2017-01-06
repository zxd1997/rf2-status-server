//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹?
//?                                                                        ?
//?Module: Internals Example Source File                                   ?
//?                                                                        ?
//?Description: Declarations for the Internals Example Plugin              ?
//?                                                                        ?
//?                                                                        ?
//?This source code module, and all information, data, and algorithms      ?
//?associated with it, are part of CUBE technology (tm).                   ?
//?                PROPRIETARY AND CONFIDENTIAL                            ?
//?Copyright (c) 1996-2008 Image Space Incorporated.  All rights reserved. ?
//?                                                                        ?
//?                                                                        ?
//?Change history:                                                         ?
//?  tag.2005.11.30: created                                               ?
//?                                                                        ?
//ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ?


#include "Example.hpp"          // corresponding header file
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <time.h>
#include <errno.h>
#include "cJSON.h"
#include "mongoose.h"

//#pragma comment (lib, "wsock32.lib")

// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "ServerStatusQuery v0.1" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 3 ); } // InternalsPluginV01 functionality

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( (PluginObject *) new ExampleInternalsPlugin ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( (ExampleInternalsPlugin *) obj ); }


//################  for guage ###############
char* debugLogFilePath=new char[512];
char* accessLogFilePath=new char[512];
unsigned int BUFFER_SIZE = 51200;
ScoringInfoV01 *currentScoringInfo = NULL;
GraphicsInfoV02 *currentGraphicsInfo = NULL;

bool isDebug = false;


#define accessLog(fmt,...) log(accessLogFilePath,fmt,__VA_ARGS__)
#define debugLogToFile(fmt,...) log(debugLogFilePath,fmt,__VA_ARGS__)
#define debugLog(fmt,...) if(isDebug){printf(fmt,__VA_ARGS__);printf("\n");}

void log(const char* filePath,const char* fmt,...)
{
    time_t t = time( 0 );
    char timeBuf[20];
    strftime( timeBuf, sizeof(timeBuf), "%H:%M:%S",localtime(&t) );

    FILE *fo = fopen( filePath, "a" );
    if( fo != NULL )
    {
        fprintf( fo, "[%s]:",timeBuf);

        va_list ap;
        va_start(ap, fmt);//Ω´ap÷∏œÚfmt∫Ûµƒµ⁄“ª∏ˆ≤Œ ˝
        vfprintf(fo,fmt,ap);
        va_end(ap);//Ω´ap÷√Œ™NULL

        fprintf( fo, "\n");
        fflush(fo);
        fclose( fo );
    }
    delete [] timeBuf;
}

class Session{
public:
        int socket;
};



void getScoringInfo(struct mg_connection *c, struct http_message *hm)
{
    cJSON *root = cJSON_CreateObject();

    if( currentScoringInfo != NULL )
    {
        cJSON_AddItemToObject(root, "mTrackName", cJSON_CreateString(currentScoringInfo->mTrackName));
        cJSON_AddItemToObject(root, "mSession", cJSON_CreateNumber(currentScoringInfo->mSession));
        cJSON_AddItemToObject(root, "mCurrentET", cJSON_CreateNumber(currentScoringInfo->mCurrentET));
        cJSON_AddItemToObject(root, "mEndET", cJSON_CreateNumber(currentScoringInfo->mEndET));
        cJSON_AddItemToObject(root, "mMaxLaps", cJSON_CreateNumber(currentScoringInfo->mMaxLaps));
        cJSON_AddItemToObject(root, "mLapDist", cJSON_CreateNumber(currentScoringInfo->mLapDist));
        cJSON_AddItemToObject(root, "mResultsStream", cJSON_CreateString(currentScoringInfo->mResultsStream));
        cJSON_AddItemToObject(root, "mNumVehicles", cJSON_CreateNumber(currentScoringInfo->mNumVehicles));
        cJSON_AddItemToObject(root, "mGamePhase", cJSON_CreateNumber(currentScoringInfo->mGamePhase));
        cJSON_AddItemToObject(root, "mYellowFlagState", cJSON_CreateNumber(currentScoringInfo->mYellowFlagState));
        cJSON_AddItemToObject(root, "mDarkCloud", cJSON_CreateNumber(currentScoringInfo->mDarkCloud));
        cJSON_AddItemToObject(root, "mRaining", cJSON_CreateNumber(currentScoringInfo->mRaining));
        cJSON_AddItemToObject(root, "mAmbientTemp", cJSON_CreateNumber(currentScoringInfo->mAmbientTemp));
        cJSON_AddItemToObject(root, "mTrackTemp", cJSON_CreateNumber(currentScoringInfo->mTrackTemp));
        cJSON_AddItemToObject(root, "mWind_X", cJSON_CreateNumber(currentScoringInfo->mWind.x));
        cJSON_AddItemToObject(root, "mWind_Y", cJSON_CreateNumber(currentScoringInfo->mWind.y));
        cJSON_AddItemToObject(root, "mWind_Z", cJSON_CreateNumber(currentScoringInfo->mWind.z));

        cJSON *mVehiclesArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "mVehicles", mVehiclesArray);
        for( long i = 0; i < currentScoringInfo->mNumVehicles; ++i )
        {
            VehicleScoringInfoV01 &vinfo = currentScoringInfo->mVehicle[ i ];

            cJSON *v = cJSON_CreateObject();
            cJSON_AddItemToObject(v, "mID", cJSON_CreateNumber(vinfo.mID));
            cJSON_AddItemToObject(v, "mDriverName", cJSON_CreateString(vinfo.mDriverName));
            cJSON_AddItemToObject(v, "mVehicleName", cJSON_CreateString(vinfo.mVehicleName));
            cJSON_AddItemToObject(v, "mTotalLaps", cJSON_CreateNumber(vinfo.mTotalLaps));
            cJSON_AddItemToObject(v, "mSector", cJSON_CreateNumber(vinfo.mSector));
            cJSON_AddItemToObject(v, "mFinishStatus", cJSON_CreateNumber(vinfo.mFinishStatus));
            cJSON_AddItemToObject(v, "mLapDist", cJSON_CreateNumber(vinfo.mLapDist));
            cJSON_AddItemToObject(v, "mPathLateral", cJSON_CreateNumber(vinfo.mPathLateral));
            cJSON_AddItemToObject(v, "mTrackEdge", cJSON_CreateNumber(vinfo.mTrackEdge));
            cJSON_AddItemToObject(v, "mBestSector1", cJSON_CreateNumber(vinfo.mBestSector1));
            cJSON_AddItemToObject(v, "mBestSector2", cJSON_CreateNumber(vinfo.mBestSector2));
            cJSON_AddItemToObject(v, "mBestLapTime", cJSON_CreateNumber(vinfo.mBestLapTime));
            cJSON_AddItemToObject(v, "mLastSector1", cJSON_CreateNumber(vinfo.mLastSector1));
            cJSON_AddItemToObject(v, "mLastSector2", cJSON_CreateNumber(vinfo.mLastSector2));
            cJSON_AddItemToObject(v, "mLastLapTime", cJSON_CreateNumber(vinfo.mLastLapTime));
            cJSON_AddItemToObject(v, "mCurSector1", cJSON_CreateNumber(vinfo.mCurSector1));
            cJSON_AddItemToObject(v, "mCurSector2", cJSON_CreateNumber(vinfo.mCurSector2));
            cJSON_AddItemToObject(v, "mNumPitstops", cJSON_CreateNumber(vinfo.mNumPitstops));
            cJSON_AddItemToObject(v, "mNumPenalties", cJSON_CreateNumber(vinfo.mNumPenalties));
            cJSON_AddItemToObject(v, "mIsPlayer", cJSON_CreateNumber(vinfo.mIsPlayer));
            cJSON_AddItemToObject(v, "mControl", cJSON_CreateNumber(vinfo.mControl));
            cJSON_AddItemToObject(v, "mInPits", cJSON_CreateNumber(vinfo.mInPits));
            cJSON_AddItemToObject(v, "mPlace", cJSON_CreateNumber(vinfo.mPlace));
            cJSON_AddItemToObject(v, "mVehicleClass", cJSON_CreateString(vinfo.mVehicleClass));
            cJSON_AddItemToObject(v, "mTimeBehindNext", cJSON_CreateNumber(vinfo.mTimeBehindNext));
            cJSON_AddItemToObject(v, "mLapsBehindNext", cJSON_CreateNumber(vinfo.mLapsBehindNext));
            cJSON_AddItemToObject(v, "mTimeBehindLeader", cJSON_CreateNumber(vinfo.mTimeBehindLeader));
            cJSON_AddItemToObject(v, "mLapsBehindLeader", cJSON_CreateNumber(vinfo.mLapsBehindLeader));
            cJSON_AddItemToObject(v, "mLapStartET", cJSON_CreateNumber(vinfo.mLapStartET));

            cJSON_AddItemToArray(mVehiclesArray,v);
        }

    }else{
        cJSON_AddItemToObject(root, "session", cJSON_CreateNumber(-1));
    }

    char *out =cJSON_Print(root);
	mg_printf(c,
		"HTTP/1.1 200 OK\r\n"
		"Server: rFactor2ServerStatQuery\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		(int)strlen(out), out);
    
}

void getGraphicsInfo(struct mg_connection *c, struct http_message *hm)
{
	cJSON *root = cJSON_CreateObject();

	if (currentGraphicsInfo != NULL)
	{
		cJSON_AddItemToObject(root, "mID", cJSON_CreateNumber(currentGraphicsInfo->mID));
		cJSON_AddItemToObject(root, "mCameraType", cJSON_CreateNumber(currentGraphicsInfo->mCameraType));
	}
	else {
		cJSON_AddItemToObject(root, "session", cJSON_CreateNumber(-1));
	}

	char *out = cJSON_Print(root);
	mg_printf(c,
		"HTTP/1.1 200 OK\r\n"
		"Server: rFactor2ServerStatQuery\r\n"
		"Content-Type: text/json\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s",
		(int)strlen(out), out);
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
	debugLog("http server start: %d", ev)
	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;
		if (mg_vcmp(&hm->uri, "/getScoringInfo") == 0) {
			getScoringInfo(nc, hm); /* Handle RESTful call */
		} else if (mg_vcmp(&hm->uri, "/getGraphicsInfo") == 0) {
			getGraphicsInfo(nc, hm); /* Handle RESTful call */
		} else {
			mg_printf(nc,
				"HTTP/1.1 200 OK\r\n"
				"Server: rFactor2ServerStatQuery\r\n"
				"Content-Type: text/json\r\n"
				"Content-Length: %d\r\n"
				"\r\n"
				"%s",
				(int)strlen("hello"), "hello");
		}
	}
}

char address[100] = { '\0' };
DWORD WINAPI __stdcall startHttpServer(LPVOID lpParameter)
{
	debugLog("http server start")
	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, address, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}

	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	//mg_enable_multithreading(nc);

	for (;;) {
		mg_mgr_poll(&mgr, 100000);
	}
	mg_mgr_free(&mgr);
	debugLog("http server exit")
	return 0;
}

void ExampleInternalsPlugin::UpdateScoring( const ScoringInfoV01 &info )
{
    if(currentScoringInfo == NULL)
        currentScoringInfo = new ScoringInfoV01();
    memcpy(currentScoringInfo,(void *)&info,sizeof(info));
}

void ExampleInternalsPlugin::Startup( long version )
{
	isDebug = GetPrivateProfileInt("config", "is_debug", 1, ".\\rf2_dedi_state_http_query.ini");
	if (isDebug) {
		AllocConsole();
		freopen("CONOUT$", "w+t", stdout);
	}
	
	
	GetPrivateProfileString("config", "http_port", ":34297", address, 100, ".\\rf2_dedi_state_http_query.ini");
	
    char logDir[512]={'\0'};
    GetPrivateProfileString("config", "log_dir","rf2_server_status_query_log",logDir,512,".\\rf2_dedi_state_http_query.ini");

    mkdir(logDir);

    time_t t = time( 0 );
    char tmp[20];
    strftime( tmp, sizeof(tmp), "%Y%m%d",localtime(&t) );
    sprintf(accessLogFilePath, "%s/%s.log",logDir,tmp);
    sprintf(debugLogFilePath, "%s/debug.log",logDir);


    debugLog("Plugin Running.....Port:%d", address);
    CreateThread(NULL,NULL, startHttpServer,NULL,0,NULL);

}

void ExampleInternalsPlugin::UpdateGraphics(const GraphicsInfoV02 &info)
{
	if (currentGraphicsInfo == NULL)
		currentGraphicsInfo = new GraphicsInfoV02();
	memcpy(currentGraphicsInfo, (void *)&info, sizeof(info));
}

unsigned char ExampleInternalsPlugin::WantsToViewVehicle(CameraControlInfoV01 &camControl)
{
	return (3);
}

void ExampleInternalsPlugin::Shutdown()
{
	if (isDebug) {
		fclose(stdout);
	}
}


void ExampleInternalsPlugin::StartSession()
{

}


void ExampleInternalsPlugin::EndSession()
{
    delete currentScoringInfo;
    currentScoringInfo = NULL;
}


void ExampleInternalsPlugin::EnterRealtime()
{

}


void ExampleInternalsPlugin::ExitRealtime()
{

}


