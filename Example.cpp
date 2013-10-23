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
#pragma comment (lib, "Ws2_32.lib")

#include "Example.hpp"          // corresponding header file
#include <math.h>
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>
#include <errno.h>
#include "cJSON.h"

// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "ExamplePlugin - 2008.02.13" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 1 ); } // InternalsPluginV01 functionality

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( (PluginObject *) new ExampleInternalsPlugin ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( (ExampleInternalsPlugin *) obj ); }


//################  for guage ###############
char* debugLogFilePath=new char[512];
char* accessLogFilePath=new char[512];
unsigned int BUFFER_SIZE = 51200;
char* buffer = new char[BUFFER_SIZE];
ScoringInfoV01 *currentScoringInfo = NULL;
int httpPort = 0;



#define accessLog(fmt,...) log(accessLogFilePath,fmt,__VA_ARGS__)
#define debugLog(fmt,...) log(debugLogFilePath,fmt,__VA_ARGS__)

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



DWORD WINAPI __stdcall service(LPVOID lpParameter)
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

    Session *s=(Session *)lpParameter;
    int socket = s->socket;

    memset(buffer,0,BUFFER_SIZE);
    sprintf(buffer, "HTTP/1.1 200 OK\nServer: rFactor2ServerStatQuery\nContent-Type: text/html\nContent-Length: %d\n\n%s",strlen(out),out);

    if (send(socket, buffer, strlen(buffer), 0 ) <= 0) {
        debugLog("Send Failed");
    }
    closesocket(socket);
    delete s;
    free(out);
    return 0;
}

DWORD WINAPI __stdcall startHttpServer(LPVOID lpParameter)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        debugLog("Error at WSAStartup()");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(httpPort);

    int server_socket = socket(PF_INET,SOCK_STREAM,0);
    if( server_socket < 0)
    {
        debugLog("Create Socket Failed!");
        return 0;
    }

    if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        debugLog("Server Bind Port Failed!");
        return 0;
    }

    //server_socket”√”⁄º‡Ã˝
    if ( listen(server_socket, 20) )
    {
        debugLog("Server Listen Failed!");
        return 0;
    }

    while(1) //∑˛ŒÒ∆˜∂À“™“ª÷±‘À––
    {
        struct sockaddr_in client_addr;
        int length = sizeof(client_addr);

        Session *s=new Session();
        s->socket = accept(server_socket,(struct sockaddr*)&client_addr,&length);
        linger m_sLinger;
        m_sLinger.l_onoff = 1; // (‘⁄closesocket()µ˜”√,µ´ «ªπ”– ˝æ›√ª∑¢ÀÕÕÍ±œµƒ ±∫Ú»›–Ì∂∫¡Ù)
        m_sLinger.l_linger = 30; // (»›–Ì∂∫¡Ùµƒ ±º‰Œ™0√Î)
        setsockopt(s->socket,SOL_SOCKET,SO_LINGER,(const char*)&m_sLinger,sizeof(linger));
        if ( s->socket < 0)
        {
            debugLog("Server Accept Failed!/n");
            break;
        }else{
            accessLog("%s",inet_ntoa(client_addr.sin_addr));
            CreateThread(NULL,NULL, service,(void *)s,0,NULL);
        }

    }
    //πÿ±’º‡Ã˝”√µƒsocket
    closesocket(server_socket);

    return 0;
}

void ExampleInternalsPlugin::UpdateScoring( const ScoringInfoV01 &info )
{
    if(currentScoringInfo == NULL)
        currentScoringInfo = new ScoringInfoV01();
    memcpy(currentScoringInfo,(void *)&info,sizeof(info));
}

// ExampleInternalsPlugin class


void ExampleInternalsPlugin::Startup( long version )
{
    char logDir[512]={'\0'};
    httpPort=GetPrivateProfileInt("config", "http_port",8199,".\\rf2_server_status_query_config.ini");
    GetPrivateProfileString("config", "log_dir","rf2_server_status_query_log",logDir,512,".\\rf2_server_status_query_config.ini");

    mkdir(logDir);

    time_t t = time( 0 );
    char tmp[20];
    strftime( tmp, sizeof(tmp), "%Y%m%d",localtime(&t) );
    sprintf(accessLogFilePath, "%s/%s.log",logDir,tmp);
    sprintf(debugLogFilePath, "%s/debug.log",logDir);

    CreateThread(NULL,NULL, startHttpServer,NULL,0,NULL);

    debugLog("plugin startup");
}


void ExampleInternalsPlugin::Shutdown()
{

}


void ExampleInternalsPlugin::StartSession()
{

}


void ExampleInternalsPlugin::EndSession()
{

}


void ExampleInternalsPlugin::EnterRealtime()
{

}


void ExampleInternalsPlugin::ExitRealtime()
{

}


void ExampleInternalsPlugin::UpdateTelemetry( const TelemInfoV01 &info )
{

}


void ExampleInternalsPlugin::UpdateGraphics( const GraphicsInfoV01 &info )
{

}


bool ExampleInternalsPlugin::CheckHWControl( const char * const controlName, float &fRetVal )
{
  return( false );
}


bool ExampleInternalsPlugin::ForceFeedback( float &forceValue )
{
  // Note that incoming value is the game's computation, in case you're interested.

  // CHANGE COMMENTS TO ENABLE FORCE EXAMPLE
  return( false );

  // I think the bounds are -11500 to 11500 ...
//  forceValue = 11500.0f * sinf( mET );
//  return( true );
}




bool ExampleInternalsPlugin::RequestCommentary( CommentaryRequestInfoV01 &info )
{
  // COMMENT OUT TO ENABLE EXAMPLE
  return( false );
}

