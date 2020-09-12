// rf2-status-server.cpp : Defines the exported functions for the DLL application.
//

#include "rf2-status-server.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <time.h>
#include <errno.h>
#include <windows.h>
#include <comdef.h>
#include "cJSON.h"
#include "mongoose.h"

// plugin information

extern "C" __declspec(dllexport)
const char * __cdecl GetPluginName() { return("RF2StatusHttpServer v0.1"); }

extern "C" __declspec(dllexport)
PluginObjectType __cdecl GetPluginType() { return(PO_INTERNALS); }

extern "C" __declspec(dllexport)
int __cdecl GetPluginVersion() { return(3); } // InternalsPluginV03 functionality

extern "C" __declspec(dllexport)
PluginObject * __cdecl CreatePluginObject() { return((PluginObject *) new RF2StatusHttpServerPlugin); }

extern "C" __declspec(dllexport)
void __cdecl DestroyPluginObject(PluginObject *obj) { delete((RF2StatusHttpServerPlugin *)obj); }

ScoringInfoV01 *currentScoringInfo = NULL;
HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
GraphicsInfoV02 *currentGraphicsInfo = NULL;
bool isDebug = false;

#define debugLog(fmt,...) if(isDebug){printf(fmt,__VA_ARGS__);printf("\n");}
const char* DEFAULT_HEADER_FMT = "HTTP/1.1 200 OK\r\n"
"Server: rFactor2ServerStatQuery\r\n"
"Content-Type: text/json\r\n"
"Content-Length: %d\r\n"
"\r\n"
"%s";

void logToFile(const char* filePath, const char* fmt, ...)
{
	time_t t = time(0);
	char timeBuf[20];
	strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", localtime(&t));

	FILE *fo = fopen(filePath, "a");
	if (fo != NULL)
	{
		fprintf(fo, "[%s]:", timeBuf);

		va_list ap;
		va_start(ap, fmt);//将ap指向fmt后的第一个参数
		vfprintf(fo, fmt, ap);
		va_end(ap);//将ap置为NULL

		fprintf(fo, "\n");
		fflush(fo);
		fclose(fo);
	}
	delete[] timeBuf;
}

void getScoringInfo(struct mg_connection *nc, struct http_message *hm)
{
	cJSON *root = cJSON_CreateObject();

	WaitForSingleObject(mutex, INFINITE);
	if (currentScoringInfo != NULL)
	{
		cJSON_AddItemToObject(root, "mGameMode", cJSON_CreateNumber(currentScoringInfo->mGameMode));
		cJSON_AddItemToObject(root, "mIsPasswordProtected", cJSON_CreateNumber(currentScoringInfo->mIsPasswordProtected));
		cJSON_AddItemToObject(root, "mServerPort", cJSON_CreateNumber(currentScoringInfo->mServerPort));
		cJSON_AddItemToObject(root, "mServerPublicIP", cJSON_CreateNumber(currentScoringInfo->mServerPublicIP));
		cJSON_AddItemToObject(root, "mMaxPlayers", cJSON_CreateNumber(currentScoringInfo->mMaxPlayers));
		cJSON_AddItemToObject(root, "mServerName", cJSON_CreateString(currentScoringInfo->mServerName));
		cJSON_AddItemToObject(root, "mTrackName", cJSON_CreateString(currentScoringInfo->mTrackName));
		cJSON_AddItemToObject(root, "mSession", cJSON_CreateNumber(currentScoringInfo->mSession));
		cJSON_AddItemToObject(root, "mStartET", cJSON_CreateNumber(currentScoringInfo->mStartET));
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
		cJSON_AddItemToObject(root, "mAvgPathWetness", cJSON_CreateNumber(currentScoringInfo->mAvgPathWetness));
		cJSON_AddItemToObject(root, "mWind_X", cJSON_CreateNumber(currentScoringInfo->mWind.x));
		cJSON_AddItemToObject(root, "mWind_Y", cJSON_CreateNumber(currentScoringInfo->mWind.y));
		cJSON_AddItemToObject(root, "mWind_Z", cJSON_CreateNumber(currentScoringInfo->mWind.z));

		cJSON *mVehiclesArray = cJSON_CreateArray();
		cJSON_AddItemToObject(root, "mVehicles", mVehiclesArray);
		for (long i = 0; i < currentScoringInfo->mNumVehicles; ++i)
		{
			VehicleScoringInfoV01 &vinfo = currentScoringInfo->mVehicle[i];

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
			cJSON_AddItemToObject(v, "mBestLapSector1", cJSON_CreateNumber(vinfo.mBestLapSector1));
			cJSON_AddItemToObject(v, "mBestLapSector2", cJSON_CreateNumber(vinfo.mBestLapSector2));
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
			cJSON_AddItemToObject(v, "mPitState", cJSON_CreateNumber(vinfo.mPitState));
			cJSON_AddItemToObject(v, "mPitLapDist", cJSON_CreateNumber(vinfo.mPitLapDist));
			cJSON_AddItemToObject(v, "mPlace", cJSON_CreateNumber(vinfo.mPlace));
			cJSON_AddItemToObject(v, "mVehicleClass", cJSON_CreateString(vinfo.mVehicleClass));
			cJSON_AddItemToObject(v, "mTimeBehindNext", cJSON_CreateNumber(vinfo.mTimeBehindNext));
			cJSON_AddItemToObject(v, "mLapsBehindNext", cJSON_CreateNumber(vinfo.mLapsBehindNext));
			cJSON_AddItemToObject(v, "mTimeBehindLeader", cJSON_CreateNumber(vinfo.mTimeBehindLeader));
			cJSON_AddItemToObject(v, "mLapsBehindLeader", cJSON_CreateNumber(vinfo.mLapsBehindLeader));
			cJSON_AddItemToObject(v, "mLapStartET", cJSON_CreateNumber(vinfo.mLapStartET));
			cJSON_AddItemToObject(v, "mPOS_X", cJSON_CreateNumber(vinfo.mPos.x));
			cJSON_AddItemToObject(v, "mPOS_Y", cJSON_CreateNumber(vinfo.mPos.y));
			cJSON_AddItemToObject(v, "mPOS_Z", cJSON_CreateNumber(vinfo.mPos.z));
			cJSON_AddItemToArray(mVehiclesArray, v);
		}
		
	}
	else {
		cJSON_AddItemToObject(root, "error", cJSON_CreateString("session no started"));
	}
	ReleaseMutex(mutex);

	char *out = cJSON_Print(root);
	mg_printf(nc, DEFAULT_HEADER_FMT,
		(int)strlen(out), out);
	cJSON_Delete(root);
	free(out);

}

void getGraphicsInfo(struct mg_connection *nc, struct http_message *hm)
{
	cJSON *root = cJSON_CreateObject();

	if (currentGraphicsInfo != NULL)
	{
		cJSON_AddItemToObject(root, "mID", cJSON_CreateNumber(currentGraphicsInfo->mID));
		cJSON_AddItemToObject(root, "mCameraType", cJSON_CreateNumber(currentGraphicsInfo->mCameraType));
	}
	else {
		cJSON_AddItemToObject(root, "error", cJSON_CreateString("session no started"));
	}

	char *out = cJSON_Print(root);
	mg_printf(nc, DEFAULT_HEADER_FMT,
		(int)strlen(out), out);
	cJSON_Delete(root);
	free(out);
}

struct camera_control {
	long mID;
	long mCameraType;
};

camera_control *cameraControl = NULL;

void setCameraControl(struct mg_connection *nc, struct http_message *hm)
{
	char mID[10];
	char mCameraType[10];
	if (mg_get_http_var(&hm->query_string, "mID", mID, 10) < 0)
	{
		const char *out = "{\"error\":\"param mID not found\"}";
		mg_printf(nc, DEFAULT_HEADER_FMT,
			(int)strlen(out), out);
		return;
	}
	if (mg_get_http_var(&hm->query_string, "mCameraType", mCameraType, 10) < 0)
	{
		const char *out = "{\"error\":\"param mCameraType not found\"}";
		mg_printf(nc, DEFAULT_HEADER_FMT,
			(int)strlen(out), out);
		return;
	}

	WaitForSingleObject(mutex, INFINITE);
	cameraControl = (camera_control*)malloc(sizeof(camera_control));
	cameraControl->mID = atol(mID);
	cameraControl->mCameraType = atol(mCameraType);
	ReleaseMutex(mutex);

	const char *out = "{\"succ\":1}";
	mg_printf(nc, DEFAULT_HEADER_FMT,
		(int)strlen(out), out);
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;
		if (mg_vcmp(&hm->uri, "/getScoringInfo") == 0) {
			getScoringInfo(nc, hm); /* Handle RESTful call */
		}
		else if (mg_vcmp(&hm->uri, "/getGraphicsInfo") == 0) {
			getGraphicsInfo(nc, hm); /* Handle RESTful call */
		}
		else if (mg_vcmp(&hm->uri, "/cameraControl") == 0) {
			setCameraControl(nc, hm); /* Handle RESTful call */
		}
		else {
			const char *out = "{\"error\":\"api not found\"}";
			mg_printf(nc, DEFAULT_HEADER_FMT,
				(int)strlen(out), out);
		}
		nc->flags |= MG_F_SEND_AND_CLOSE;
	}
}

WCHAR address[100] = { '\0' };
DWORD WINAPI __stdcall startHttpServer(LPVOID lpParameter)
{
	debugLog("http server start")
		struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, _bstr_t(address), ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}

	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	mg_enable_multithreading(nc);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
	debugLog("http server exit")
		return 0;
}

void RF2StatusHttpServerPlugin::Startup(long version)
{
	isDebug = GetPrivateProfileInt(L"config", L"is_debug", 1, L".\\rf2-status-server-cfg.ini");
	if (isDebug) {
		AllocConsole();
		freopen("CONOUT$", "w+t", stdout);
	}

	GetPrivateProfileString(L"config", L"http_address", L":34297", address, 100, L".\\rf2-status-server-cfg.ini");

	debugLog("Plugin Running....listening:%s", (const char*)_bstr_t(address));
	CreateThread(NULL, NULL, startHttpServer, NULL, 0, NULL);

}

void RF2StatusHttpServerPlugin::UpdateScoring(const ScoringInfoV01 &info)
{
	WaitForSingleObject(mutex, INFINITE);
	if (currentScoringInfo == NULL)
		currentScoringInfo = new ScoringInfoV01();
	memcpy(currentScoringInfo, (void *)&info, sizeof(info));
	ReleaseMutex(mutex);
}

void RF2StatusHttpServerPlugin::UpdateGraphics(const GraphicsInfoV02 &info)
{
	WaitForSingleObject(mutex, INFINITE);
	if (currentGraphicsInfo == NULL)
		currentGraphicsInfo = new GraphicsInfoV02();
	memcpy(currentGraphicsInfo, (void *)&info, sizeof(info));
	ReleaseMutex(mutex);
}

unsigned char RF2StatusHttpServerPlugin::WantsToViewVehicle(CameraControlInfoV01 &camControl)
{
	WaitForSingleObject(mutex, INFINITE);
	if (cameraControl != NULL)
	{
		camControl.mID = cameraControl->mID;
		camControl.mCameraType = cameraControl->mCameraType;
		free(cameraControl);
		cameraControl = NULL;
		ReleaseMutex(mutex);
		return (3);
	}
	ReleaseMutex(mutex);
	return (0);// return values: 0=do nothing, 1=set ID and camera type, 2=replay controls, 3=both
}

void RF2StatusHttpServerPlugin::Shutdown()
{
	if (isDebug) {
		fclose(stdout);
	}
}


void RF2StatusHttpServerPlugin::StartSession()
{

}


void RF2StatusHttpServerPlugin::EndSession()
{
	free(currentScoringInfo);
	currentScoringInfo = NULL;
}


void RF2StatusHttpServerPlugin::EnterRealtime()
{

}


void RF2StatusHttpServerPlugin::ExitRealtime()
{

}