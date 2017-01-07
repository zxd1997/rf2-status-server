#pragma once

#ifndef _RF2_STATUS_SERVER_H
#define _RF2_STATUS_SERVER_H

#define WIN32_LEAN_AND_MEAN

#include "InternalsPlugin.hpp"

// This is used for the app to use the plugin for its intended purpose
class RF2StatusHttpServerPlugin : public InternalsPluginV03  // REMINDER: exported function GetPluginVersion() should return 1 if you are deriving from this InternalsPluginV01, 2 for InternalsPluginV02, etc.
{

public:

	// Constructor/destructor
	RF2StatusHttpServerPlugin() {}
	~RF2StatusHttpServerPlugin() {}

	// These are the functions derived from base class InternalsPlugin
	// that can be implemented.
	void Startup(long version);  // game startup
	void Shutdown();               // game shutdown

	void EnterRealtime();          // entering realtime
	void ExitRealtime();           // exiting realtime

	void StartSession();           // session has started
	void EndSession();             // session has ended

								   // GAME OUTPUT
	long WantsTelemetryUpdates() { return(false); } // CHANGE TO 1 TO ENABLE TELEMETRY EXAMPLE!
	void UpdateTelemetry(const TelemInfoV01 &info) {};

	bool WantsGraphicsUpdates() { return(true); } // CHANGE TO TRUE TO ENABLE GRAPHICS EXAMPLE!
	void UpdateGraphics(const GraphicsInfoV02 &info); // update plugin with extended graphics info
	unsigned char WantsToViewVehicle(CameraControlInfoV01 &camControl); // return values: 0=do nothing, 1=set ID and camera type, 2=replay controls, 3=both
	bool WantsToDisplayMessage(MessageInfoV01 &msgInfo) { return(false); } // set message and return true

																		   // GAME INPUT
	bool HasHardwareInputs() { return(false); } // CHANGE TO TRUE TO ENABLE HARDWARE EXAMPLE!
	void UpdateHardware(const double fDT) {} // update the hardware with the time between frames
	void EnableHardware() {}             // message from game to enable hardware
	void DisableHardware() {}           // message from game to disable hardware

										// See if the plugin wants to take over a hardware control.  If the plugin takes over the
										// control, this method returns true and sets the value of the double pointed to by the
										// second arg.  Otherwise, it returns false and leaves the double unmodified.
	bool CheckHWControl(const char * const controlName, double &fRetVal) { return(false); };

	bool ForceFeedback(double &forceValue) { return(false); };  // SEE FUNCTION BODY TO ENABLE FORCE EXAMPLE

																// SCORING OUTPUT
	bool WantsScoringUpdates() { return(true); } // CHANGE TO TRUE TO ENABLE SCORING EXAMPLE!
	void UpdateScoring(const ScoringInfoV01 &info);

	// COMMENTARY INPUT
	bool RequestCommentary(CommentaryRequestInfoV01 &info) { return(false); };  // SEE FUNCTION BODY TO ENABLE COMMENTARY EXAMPLE

};


#endif // !_RF2_STATUS_SERVER_H
