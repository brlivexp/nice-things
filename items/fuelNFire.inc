  


// INCLUDE FILEOVI
#include								<a_samp>
#include								<zcmd>


// DEFINE DEFINICIJE


#define SLOT_ID							5 // STAVLJA OBJEKAT U SLOT 5. OVO KASNIJE MOĹ˝ETE PROMJENITI
#define REFRESH_RATE					15000 // VRIJEME ZA KOLIKO Ä†E ODUZETI ''HEALTH'' VATRI. 15000ms = 15s.
#define MAX_GASOLINE						500 // MAKSIMALNO KOLIKO MOĹ˝E SERVER PRIMITI TOÄŚAKA GASOLINE. OVO SLOBODNO MOĹ˝ETE PROMIJENITI :)

#define DIALOG_Canister					6969 // OVO KASNIJE VI PROMJENITE AKO Ĺ˝ELITE
#define DIALOG_Canister_STATUS			3105 // OVO KASNIJE VI PROMJENITE AKO Ĺ˝ELITE


// VARIABLE
new tocaka_GASOLINE			=			false;

new Float:Canister						[MAX_PLAYERS];
new bool:lighter						[MAX_PLAYERS];
new bool:in_hand							[MAX_PLAYERS];
new Float:lastX							[MAX_PLAYERS];
new Float:lastY							[MAX_PLAYERS];
new Float:lastZ							[MAX_PLAYERS];
new bool:can_spill					[MAX_PLAYERS];


// ENUM
enum benzin
{
	Float:benzinX,
	Float:benzinY,
	Float:benzinZ,
	
	Float:playerX,
	Float:playerY,
	Float:playerZ,
	Objekt,
	bool:Fireup,
	Float:Quanity
}

new Benzin[MAX_GASOLINE][benzin];



// FORWARDI
forward inc300_DroppingGASOLINE		(playerid);
forward inc300_ZapaliBenzin				(playerid);
forward inc300_UnistiVatru				();


// PUBLICI
public OnPlayerConnect(playerid)
{
	Canister[playerid] = 0.0;
	lighter[playerid] = false;
	in_hand[playerid] = false;
	lastX[playerid] = 0.0;
	lastY[playerid] = 0.0;
	lastZ[playerid] = 0.0;
	can_spill[playerid] = false;
	return 1;
}

public inc300_UnistiVatru()
{
	for(new id = false; id < sizeof(Benzin); id++)
	{
		if(Benzin[id][Fireup] == true)
		{
			if(Benzin[id][Quanity] < 0.1)
			{
				Benzin[id][benzinX] = 0.0;
				Benzin[id][benzinY] = 0.0;
				Benzin[id][benzinZ] = 0.0;
				Benzin[id][Quanity] = 0.0;
				Benzin[id][Fireup] = false;
				DestroyObject(Benzin[id][Objekt]);
				tocaka_GASOLINE--;
			}
			else
			{
				new string[8];
				new broj = (random(99-100));

				format(string, 8, "0.0%d", broj);
				Benzin[id][Quanity] -= floatstr(string);
			}
		}
	}
	return 1;
}


public OnFilterScriptInit()
{
	SetTimer("inc300_DestroyFire", REFRESH_RATE, true);

	print("------------------------------------- INFO -------------------------------------");
	print("--------------------------------------------------------------------------------");

	AddStaticPickup(1239, 2, 1940.2311, -1774.2710, 13.3906, -1);
	Create3DTextLabel("Buying Gas\n{F00000}/gas", 0xFFFFFFFF, 1940.2311, -1774.2710, 14.6000, 40.0, 0, 0);
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if(dialogid == DIALOG_Canister)
	{
		if(response)
		{
			if(listitem == 0)
			{
				if(in_hand[playerid] == true)
				{
					RemovePlayerAttachedObject(playerid, SLOT_ID);
					in_hand[playerid] = false;
					SendClientMessage(playerid, 0xFFFFFFAA, "You have put down your Canister.");
					ShowPlayerDialog(playerid, DIALOG_Canister, DIALOG_STYLE_LIST, "Canister", "Izvadi Canister", "Odaberi", "Nazad");
				}
				else // UKOLIKO GA NEMA
				{
					SetPlayerAttachedObject(playerid, SLOT_ID, 1650, 5, 0.145482, 0.035119, 0.040793, 8.881844, 276.183959, 329.795593, 1.561557, 1.177534, 1.265636); // BONE ''5'' - POSTAVLJA Canister U IGRAÄŚEVU LIJEVU RUKU
					in_hand[playerid] = true;
					SendClientMessage(playerid, 0xFFFFFFAA, "You took out your Canister.");
					ShowPlayerDialog(playerid, DIALOG_Canister, DIALOG_STYLE_LIST, "Canister", "Put down Canister\nOpen Canister\nStatus  of theCanister", "Choose", "Back");
				}
			}

			if(listitem == 1)
			{
				if(tocaka_GASOLINE != MAX_GASOLINE) // UKOLIKO JOĹ  NIJE PREÄEN LIMIT
				{
					if(can_spill[playerid] == false)
					{
						SendClientMessage(playerid, 0xFFFFFFAA, "To spill the gasoline press {F00000}Y{FFFFFF}.To stop press {F00000}N");
						can_spill[playerid] = true;
					}
					else
					{
						SendClientMessage(playerid, 0xF00000AA, "You are already spilling gasoline");
						ShowPlayerDialog(playerid, DIALOG_Canister, DIALOG_STYLE_LIST, "Canister", "Put down Canister\nOpen Canister\nStatus of Canister", "Choose", "Back");
					}
				}
				else
				{
					SendClientMessage(playerid, 0xF00000AA, "There is already alot of gasoline in the server");
				}
			}
			if(listitem == 2)
			{
				for(new id = false; id < sizeof(Benzin); id++)
            	{
	       if(IsPlayerInRangeOfPoint(playerid, 0.5, Benzin[id][benzinX], Benzin[id][benzinY], Benzin[id][benzinZ]))
	        {
		             if(Benzin[id][Fireup] == true)
		             {
                       SendClientMessage(playerid, 0xF00000AA, "You threw the gas canister in the fire!");
                       new Float:x, Float:y, Float:z;
                       GetPlayerPos(playerid, x, y, z);
                       Benzin[playerid][playerX] = x;
                       Benzin[playerid][playerY] = y;
                       Benzin[playerid][playerZ] = z;
                       SetTimerEx("CanisterExplode", 3000, false, "i", playerid);
                       
					  }
				 }
		    }
		}

			if(listitem == 3)
			{
				new string[86];
				format(string, 86, "{FFFFFF}-  Gasoline Fireup: {F00000}Gas\n{FFFFFF}- Left over: {F00000}%.2f L", Canister[playerid]);
				ShowPlayerDialog(playerid, DIALOG_Canister_STATUS, DIALOG_STYLE_MSGBOX, "Canister - Status", string, "OK", "");
			}
		}
	}

	if(dialogid == DIALOG_Canister_STATUS)
	{
		ShowPlayerDialog(playerid, DIALOG_Canister, DIALOG_STYLE_LIST, "Canister", "Put down Canister\nOpen Canister\nStatus of the Canistera", "Choose", "Back");
	}
	return 1;
}

forward CanisterExplode(playerid);


public CanisterExplode(playerid)
{
            for(new id = false; id < sizeof(Benzin); id++)
         	{
            CreateExplosion(Benzin[playerid][playerX], Benzin[playerid][playerY], Benzin[playerid][playerZ], 6, 10.0);

	        }
    return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(newkeys & KEY_YES) // TIPKA ''Y''
	{
		if(can_spill[playerid] == true)
		{
			if(tocaka_GASOLINE != MAX_GASOLINE) // UKOLIKO JOĹ  IMA MJESTA DA SERVER PRIMI GASOLINE
			{
				new Float:x, Float:y, Float:z;
				GetPlayerPos(playerid, x, y, z);

				for(new id = false; id < sizeof(Benzin); id++)
				{
					if(x != lastX[playerid] && y != lastY[playerid] && y != lastZ[playerid]) //UKOLIKO IGRAÄŚ NIJE NA STAROJ POZICIJI
					{
						if(Benzin[id][benzinX] == 0.0) // UKOLIKO JE TAJ SLOT SLOBODAN
						{
							if(Canister[playerid] != 0.0) // UKOLIKO Canister NIJE PRAZAN
							{
								new string[8];
								new random_broj = (random(99-100));

								format(string, 8, "0.%d", random_broj);

								tocaka_GASOLINE++;
								Benzin[id][benzinX] = x;
								Benzin[id][benzinY] = y;
								Benzin[id][benzinZ] = z;

								lastX[playerid] = x;
								lastY[playerid] = y;
								lastZ[playerid] = z;

								if(Canister[playerid] >  floatstr(string)) // UKOLIKO IMA JOĹ  GASOLINE U CanisterU A DA JE KOLIÄŚINA GASOLINE VEÄŚA OD RANDOMA
								{
									Canister[playerid] -= floatstr(string);
									Benzin[id][Quanity] = floatstr(string);
								}
								else // UKOLIKO JE KOLIÄŚINA MANJA OD RANDOMA
								{
									Benzin[id][Quanity] = Canister[playerid];
									Canister[playerid] = 0.0;
								}
							}
							else // UKOLIKO JE Canister PRAZAN
							{
								can_spill[playerid] = false;
								SendClientMessage(playerid, 0xFFFFFFAA, "You don't have no gasoline in your canister,buy it at a gas station(/gas)");
							}
							return 1;
						}
					}
					else
					{
						if(Benzin[id][benzinX] == 0.0) // UKOLIKO JE TAJ SLOT SLOBODAN
						{
							if(Canister[playerid] != 0.0) // UKOLIKO Canister NIJE PRAZAN
							{
								new string[8];
								new random_broj = (random(55-20)+20);

								format(string, 8, "0.%d", random_broj);

								lastX[playerid] = x;
								lastY[playerid] = y;
								lastZ[playerid] = z;

								if(Canister[playerid] >  floatstr(string)) // UKOLIKO IMA JOĹ  GASOLINE U CanisterU A DA JE KOLIÄŚINA GASOLINE VEÄŚA OD RANDOMA
								{
									Canister[playerid] -= floatstr(string);

									SendClientMessage(playerid, 0xF00000AA, "Some gasoline has spilled on the floor");
								}
								else // UKOLIKO JE KOLIÄŚINA MANJA OD RANDOMA
								{
									Canister[playerid] = 0.0;

									SendClientMessage(playerid, 0xF00000AA, "Some gasoline has spilled on the floor");
								}
							}
							else // UKOLIKO JE Canister PRAZAN
							{
								can_spill[playerid] = false;
								SendClientMessage(playerid, 0xFFFFFFAA, "You don't have no gasoline in your canister,buy it at a gas station(/benzin)");
							}
							return 1;
						}
					}
				}
			}
			else // UKOLIKO VIĹ E NEMA MJESTA
			{
				SendClientMessage(playerid, 0xF00000AA, "There is already alot of gasoline in the server");
			}
		}
	}

	if(newkeys & KEY_NO) // TIPKA ''N''
	{
		can_spill[playerid] = false;
		SendClientMessage(playerid, 0xFFFFFFAA, "You have stopped spilling out gasoline");
	}
	return 1;
}


// ZCMD KOMANDE
CMD:can(playerid, params[])
{
	if(in_hand[playerid] == true)
	{
		ShowPlayerDialog(playerid, DIALOG_Canister, DIALOG_STYLE_LIST, "Canister", "Put in Canister\nOpen Canister\nDrop the can in gasoline\nStatus  of the Canistera", "Choose", "Back");
	}
	else
	{
		ShowPlayerDialog(playerid, DIALOG_Canister, DIALOG_STYLE_LIST, "Canister", "Take out Canister", "Choose", "Back");
	}
	return 1;
}



CMD:gas(playerid, params[])
{
	if(IsPlayerInRangeOfPoint(playerid, 2.0, 1940.2311, -1774.2710, 13.3906)) // PROVJERA DALI JE NA MJESTU ZA KUPOVINU GASOLINE
	{
		if(Canister[playerid] != 5.00) // UKOLIKO Canister NIJE PUN
		{
			Canister[playerid] = 5.00;
			SendClientMessage(playerid, 0xFFFFFFAA, "You bought {F00000}5{FFFFFF} gas.");
		}
		else // UKOLIKO JE PUN
		{
			SendClientMessage(playerid, 0xF00000AA, "Your canister is already full!");
		}
	}
	else // UKOLIKO NIJE NA MJESTU ZA KUPOVINU GASOLINE
	{
		SendClientMessage(playerid, 0xF00000AA, "You must be near a gas station to buy gas.");
	}
	return 1;
}

CMD:lightup(playerid, params[])
{
	for(new id = false; id < sizeof(Benzin); id++)
	{
		if(IsPlayerInRangeOfPoint(playerid, 2.5, Benzin[id][benzinX], Benzin[id][benzinY], Benzin[id][benzinZ])) // UKOLIKO JE IGRAÄŚ MAKSIMUM 1.5 METAR OD GASOLINE
		{
			if(Benzin[id][Fireup] == false)
			{
				Benzin[id][Fireup] = true;
				Benzin[id][Objekt] = CreateObject(18688, Benzin[id][benzinX], Benzin[id][benzinY], Benzin[id][benzinZ] - 2.80, 0.0, 0.0, 0.0, 200.0);
			}
		}
	}
	return 1;
}

CMD:gaspos(playerid, params[])
{
	SetPlayerPos(playerid,1940.2311, -1774.2710, 13.3906);
	return 1;
}

CMD:turnoffallfire(playerid, params[])
{
	if(IsPlayerAdmin(playerid)) // UKOLIKO JE LOGIRAN KAO RCON ADMIN
	{
		tocaka_GASOLINE = false;
		SendClientMessageToAll(0xF00000AA, "Admin deleted all fire made by gasoline.");

		for(new id = false; id < sizeof(Benzin); id++)
		{
			if(Benzin[id][Fireup] == true)
			{
				Benzin[id][benzinX] = 0.0;
				Benzin[id][benzinY] = 0.0;
				Benzin[id][benzinZ] = 0.0;
				Benzin[id][Quanity] = 0.0;
				DestroyObject(Benzin[id][Objekt]);
			}
		}
	}
	else // UKOLIKO NIJE RCON ADMIN
	{
	}
	return 1;
}