//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹?
//?                                                                        ?
//?Module: Internals Example Header File                                   ?
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

#ifndef _INTERNALS_EXAMPLE_H
#define _INTERNALS_EXAMPLE_H

#include "InternalsPlugin.hpp"


// This is used for the app to use the plugin for its intended purpose
class ExampleInternalsPlugin : public InternalsPluginV01
{

 public:

  // Constructor/destructor
  ExampleInternalsPlugin() {}
  ~ExampleInternalsPlugin() {}

  // These are the functions derived from base class InternalsPlugin
  // that can be implemented.
  void Startup( long version );  // game startup
  void Shutdown();               // game shutdown

  void EnterRealtime();          // entering realtime
  void ExitRealtime();           // exiting realtime

  void StartSession();           // session has started
  void EndSession();             // session has ended

  // GAME OUTPUT
  virtual long WantsTelemetryUpdates() { return( false ); } // CHANGE TO 1 TO ENABLE TELEMETRY EXAMPLE!
  virtual void UpdateTelemetry( const TelemInfoV01 &info ){};

  virtual bool WantsGraphicsUpdates() { return( false ); } // CHANGE TO TRUE TO ENABLE GRAPHICS EXAMPLE!
  virtual void UpdateGraphics( const GraphicsInfoV01 &info ){};

  // GAME INPUT
  virtual bool HasHardwareInputs() { return( false ); } // CHANGE TO TRUE TO ENABLE HARDWARE EXAMPLE!
  virtual void UpdateHardware( const float fDT ) {  } // update the hardware with the time between frames
  virtual void EnableHardware() {  }             // message from game to enable hardware
  virtual void DisableHardware() {  }           // message from game to disable hardware

  // See if the plugin wants to take over a hardware control.  If the plugin takes over the
  // control, this method returns true and sets the value of the float pointed to by the
  // second arg.  Otherwise, it returns false and leaves the float unmodified.
  virtual bool CheckHWControl( const char * const controlName, float &fRetVal ){return false;};

  virtual bool ForceFeedback( float &forceValue ){return false;};  // SEE FUNCTION BODY TO ENABLE FORCE EXAMPLE

  // SCORING OUTPUT
  virtual bool WantsScoringUpdates() { return( true ); } // CHANGE TO TRUE TO ENABLE SCORING EXAMPLE!
  virtual void UpdateScoring( const ScoringInfoV01 &info );

  // COMMENTARY INPUT
  virtual bool RequestCommentary( CommentaryRequestInfoV01 &info ){ return false;};  // SEE FUNCTION BODY TO ENABLE COMMENTARY EXAMPLE

  // VIDEO EXPORT (sorry, no example code at this time)
  virtual bool WantsVideoOutput() { return( false ); }         // whether we want to export video
  virtual bool VideoOpen( const char * const szFilename, float fQuality, unsigned short usFPS, unsigned long fBPS,
                          unsigned short usWidth, unsigned short usHeight, char *cpCodec = NULL ) { return( false ); } // open video output file
  virtual void VideoClose() {}                                 // close video output file
  virtual void VideoWriteAudio( const short *pAudio, unsigned int uNumFrames ) {} // write some audio info
  virtual void VideoWriteImage( const unsigned char *pImage ) {} // write video image

  virtual void Error( const char * const msg );
};


#endif // _INTERNALS_EXAMPLE_H

