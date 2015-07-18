// speedup.pwn by Slice

#include <a_samp>

// The speed will be multiplied by this value
#define SPEED_MULTIPLIER 1.025

// The speed will only be increased if velocity is larger than this value
#define SPEED_THRESHOLD  0.4

new
	      g_SpeedUpTimer = -1,
	Float:g_SpeedThreshold,
	      PLAYER_SLOTS
;

new const
	      KEY_VEHICLE_FORWARD  = 0b001000,
	      KEY_VEHICLE_BACKWARD = 0b100000
;

public OnFilterScriptInit() {
	PLAYER_SLOTS = GetMaxPlayers();
	
	g_SpeedUpTimer = SetTimer("SpeedUp", 220, true);
	
	// Cache this value for speed
	// This can not be done during compilation because of a limitation with float values
	g_SpeedThreshold = SPEED_THRESHOLD * SPEED_THRESHOLD;
}

public OnFilterScriptExit() {
	KillTimer(g_SpeedUpTimer);
}

forward SpeedUp();
public SpeedUp() {
	new
		vehicleid,
		keys,
		Float:vx,
		Float:vy,
		Float:vz
	;
	
	// Loop all players
	for (new playerid = 0; playerid < PLAYER_SLOTS; playerid++) {
		if (!IsPlayerConnected(playerid))
			continue;
		
		// Store the value from GetPlayerVehicleID and continue if it's not 0
		if ((vehicleid = GetPlayerVehicleID(playerid))) {
			// Get the player keys (vx is used here because we don't need updown/leftright)
			GetPlayerKeys(playerid, keys, _:vx, _:vx);
			
			// If KEY_VEHICLE_FORWARD is pressed, but not KEY_VEHICLE_BACKWARD or KEY_HANDBRAKE.
			if ((keys & (KEY_VEHICLE_FORWARD | KEY_VEHICLE_BACKWARD | KEY_HANDBRAKE)) == KEY_VEHICLE_FORWARD) {
				// Get the velocity
				GetVehicleVelocity(vehicleid, vx, vy, vz);
				
				// Don't do anything if the vehicle is going slowly
				if (vx * vx + vy * vy < g_SpeedThreshold)
					continue;
				
				// Increase the X and Y velocity
				vx *= SPEED_MULTIPLIER;
				vy *= SPEED_MULTIPLIER;
				
				// Increase the Z velocity to make up for lost gravity, if needed.
				if (vz > 0.04 || vz < -0.04)
					vz -= 0.020;
				
				// Now set it
				SetVehicleVelocity(vehicleid, vx, vy, vz);
			}
		}
	}
}