//RNPC ZOMBIE BY VIN PURE

#include <a_samp>
#include <rnpc>
#define Z_WHITE "{FFFFFF}"
#define DRANGEZ 20
// PRESSED(keys)
#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))
new Text3D:ZLabel[MAX_PLAYERS];
new rnpcZTimer[MAX_PLAYERS];
new RangeZ[MAX_PLAYERS];
public OnFilterScriptInit()
{
	print("\n--------------------------------------");
	print("ZOMBIE");
	print("--------------------------------------\n");
	MapAndreas_Init(MAP_ANDREAS_MODE_NOBUFFER);
	ConnectZombieBots(1);
	return 1;
}
public OnRNPCDeath(npcid, killerid, reason)
{
    
    if(GetPVarInt(npcid,"HS") == 1)
    {
    PlayAnim(npcid, "PED", "BIKE_fall_off", 4.1, 0, 1, 1, 1, 0, 1);
    }
    else
    {
    PlayAnim(npcid, "PED", "KO_shot_stom", 4.0, 0, 1, 1, 1, 0, 1);
    }
    SetPVarInt(npcid,"HS",0);
    SetTimerEx("ReSpawnZombie",10000,0,"i",npcid);
	return 1;
}
forward ReSpawnZombie(npcid);public ReSpawnZombie(npcid)
{
    Delete3DTextLabel(ZLabel[npcid]);
    RespawnRNPC(npcid);
	return 1;
}
public OnPlayerSpawn(playerid)
{
	RangeZ[playerid] = DRANGEZ;
	if (IsPlayerNPC(playerid))
	{
		RNPC_SetShootable(playerid, 1);
		RNPC_ToggleVehicleCollisionCheck(playerid, 1);

		SetPlayerSkin(playerid,162);

		new name[24];
		format(name, 24, "Zombie_%d", playerid);
		SetPlayerName(playerid, name);

		SetPlayerPos(playerid,1958.3783, 1343.1572, 15.3746);
		if (rnpcZTimer[playerid] > 0) KillTimer(rnpcZTimer[playerid]);

		rnpcZTimer[playerid] = SetTimerEx("RFollowPlayer", 1000, 1, "i", playerid);
		SetTimerEx("Puch",500,1,"i",playerid);
		ZLabel[playerid] = Create3DTextLabel("HP:100",0xFFFFFFFF, 30.0, 40.0, 50.0, 40.0, 0);
		Attach3DTextLabelToPlayer(ZLabel[playerid], playerid, 0.0, 0.0, 0.7);
		SetPlayerColor(playerid,0xFF0000FF);
	}
	return 1;
}
forward Puch(npcid);
public Puch(npcid)
{
	for(new i = 0 ;i<MAX_PLAYERS;i++)
	{
	    if(!IsPlayerNPC(i))
	    {
			if (IsPlayerConnected(i))
			{
			    new Float:zx,Float:zy,Float:zz;
			    GetPlayerPos(npcid,zx,zy,zz);
				if(IsPlayerInRangeOfPoint(i,1,zx,zy,zz))
				{
					if(GetRNPCHealth(npcid) > 0)
					{
					RNPC_CreateBuild(npcid,PLAYER_RECORDING_TYPE_ONFOOT);
					RNPC_AddPause(500);
					RNPC_SetKeys(KEY_FIRE);
					RNPC_AddPause(500);
					for (new iT = 0; iT < 360; iT+=20)
					{
				    RNPC_SetAngleQuats(0.0, iT, 0.0);
				    RNPC_AddPause(150);
					}
					RNPC_SetKeys(0); 
					RNPC_FinishBuild();
					RNPC_StartBuildPlayback(npcid);
					}
				}
			}
		}
	}
	return 1;
}
public OnPlayerUpdate(playerid)
{
    return 1;
}
stock CheckMauZombie(npcid)
{
	new Float:zhp;
	zhp = GetRNPCHealth(npcid);
	if(zhp == 100) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"**********");
	if(zhp >= 90) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"*********"Z_WHITE"*");
	if(zhp >= 80) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"********"Z_WHITE"**");
	if(zhp >= 70) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"*******"Z_WHITE"***");
	if(zhp >= 60) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"******"Z_WHITE"****");
	if(zhp >= 50) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"*****"Z_WHITE"*****");
	if(zhp >= 40) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"****"Z_WHITE"******");
	if(zhp >= 30) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"***"Z_WHITE"*******");
	if(zhp >= 20) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"**"Z_WHITE"********");
	if(zhp >= 10) return Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,"*"Z_WHITE"*********");
	return 1;
}
forward RFollowPlayer(npcid);
public RFollowPlayer(npcid)
{
	new Float:zx, Float:zy, Float:zz,Float:x, Float:y, Float:z;
	new Float:zhp = GetRNPCHealth(npcid);
	if(zhp <= 0)
	{
		Update3DTextLabelText(ZLabel[npcid],0xFFFFFFFF,"(~~DEATH~~)");
	}
	else
	{
	    CheckMauZombie(npcid);
		//format(string,sizeof(string),"Máu:%f",zhp);
		//Update3DTextLabelText(ZLabel[npcid],0xFF0000FF,string);
	}
	for(new i = 0 ;i<MAX_PLAYERS;i++)
	{
	    if(!IsPlayerNPC(i))
	    {
			if (IsPlayerConnected(i))
			{
				GetPlayerPos(npcid, zx, zy, zz);
				if(IsPlayerInRangeOfPoint(i,2,zx,zy,zz))
				{
					new Float:hhp;
					GetPlayerHealth(i,hhp);
					SetPlayerHealth(i,hhp-1);
				}
				if(IsPlayerInRangeOfPoint(i,RangeZ[i],zx,zy,zz))
				{
					GetPlayerPos(i,x,y,z);
					RNPC_SetUDKeys(KEY_UP);
					MoveRNPC(npcid,x,y,z,RNPC_SPEED_RUN,1);
					
				}
				else
				{
				    RangeZ[i] = DRANGEZ;
				}
			}
		}
	}
	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(PRESSED(KEY_FIRE))
	{
		if(GetPlayerWeapon(playerid) > 21)
		{
			RangeZ[playerid] = 70;
		}
	}
	if(PRESSED(KEY_SPRINT))
	{
	RangeZ[playerid] = 50;
	}
	if(PRESSED(KEY_JUMP ))
	{
	RangeZ[playerid] = 50;
	}
	return 1;
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart)
{
    if(IsPlayerNPC(damagedid) && bodypart == 9)
    {
        SetPVarInt(damagedid,"HS",1);
        SetRNPCHealth(damagedid, 0.0);
        GameTextForPlayer(playerid, "~r~HEADSHOOT", 2000,4);
	}
    return 1;
}
public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	new Float:hp;GetPlayerHealth(playerid,hp);
	if(IsPlayerNPC(issuerid))
	{
	SetPlayerHealth(playerid,hp-random(30));
	}
	return 1;
}
public OnPlayerDeath(playerid,killerid,reason)
{
    SendDeathMessage(killerid,playerid, reason);
    if(IsPlayerNPC(killerid))
    {
    PlayAnim(killerid, "BOMBER", "BOM_Plant", 4.0, 0, 0, 0, 0, 0, 1);
	SetRNPCHealth(killerid,100);
    }
	return 1;
}

stock ConnectZombieBots(amount)
{
	new name[24];
	while(amount-- > 0)
	{
		format(name, 24, "Zombie_%d", 500-amount);
		ConnectRNPC(name);
	}
}
PlayAnim(playerid, animlib[], animname[], Float:fDelta, loop, lockx, locky, freeze, time, forcesync)
{
	ApplyAnimation(playerid, animlib, animname, fDelta, loop, lockx, locky, freeze, time, forcesync);
}
