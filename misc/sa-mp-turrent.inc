// ================================================================
//  rTurret
//      Version 1.5.1
//      Created for SA-MP 0.3.7
//      Copyright © 2014 - 2015 by Corekt
//
//      This program is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  Changelog:
//      Version 1.5.1 (7/18/2015)
//          Fixed bugs and removed warnings
//          Added TurretDamagePlayer
//          Added (Get/Set)TurretBulletOffset
//          Added (Get/Set)TurretBulletRot
//      Version 1.5 (7/1/2015)
//          Improved predictive aim algorithm
//          Added Turret Health system:
//              GetTurretHealth
//              SetTurretHealth
//              ToggleTurretHealth
//              OnPlayerShootTurret
//              OnTurretDeath
//          Added LoS feature (requires ColAndreas):
//              ToggleTurretLOS
//              ToggleTurretLOSAim
//              OnTurretBulletCollision
//          Minor bug fixes/tweaks
//      Version 1.1 (7/19/2014)
//          Added ToggleTurretTargetFocus,
//          Added GetTurretOwner, SetTurretOwner,
//          Added SetTurretMaterial, GetTurretMaterial
//          Added definitions for British English spelling
//          Minor bug fixes
//      Version 1.0 (7/14/2014)
//          First release
//
//  Description:
//      rTurret allows you to create highly customizable interactive turrets
//
//  Installation:
//      Requires -
//          Streamer Plugin -   http://forum.sa-mp.com/showthread.php?t=102865
//          ColAndreas Plugin - http://forum.sa-mp.com/showthread.php?t=560218
//          foreach include -   http://forum.sa-mp.com/showthread.php?t=570868
//      If using this in a filterscript, define FILTERSCRIPT
//      before including this file.
//
//  Thanks to:
//      Y_Less                                  foreach
//      Incognito                               Streamer Plugin
//      [uL]Slice, [uL]Chris420, [uL]Pottus     ColAndreas Plugin
//      Kye/Kalcor and the SA-MP Team           SA-MP
//
//  Core Functions:
//      CreateTurret(behavior = TURRET_BEHAVIOR_STATIONARY, playerid = INVALID_PLAYER_ID, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 1000.0, world = -1, interior = -1, rate = 100, Float:speed = 20.0, Float:dmg = 25.0, Float:range = 50.0, Float:streamdistance = 200.0);
//      DestroyTurret(turretid, &iter_next = -1);
//      IsValidTurret(turretid);
//
//  Callbacks:
//      OnTurretCreate(turretid);
//      OnTurretDestroy(turretid);
//      OnTurretDeath(turretid, killerid, weaponid);
//      OnTurretHitPlayer(turretid, playerid);
//      OnPlayerTakeTurretDamage(playerid, turretid, Float:damage);
//      OnPlayerShootTurret(playerid, weaponid, turretid, Float:fX, Float:fY, Float:fZ);
//      OnTurretMoved(turretid);
//      OnTurretCustomShoot(turretid);
//      OnTurretBulletCollision(turretid, Float:x, Float:y, Float:z)
//
//  For more documentation and updates visit:
//      http://forum.sa-mp.com/showthread.php?t=525988
// ================================================================

#if !defined _samp_included
    #error You must include a_samp before rTurret.
#endif

#if defined _rTurret_included
    #endinput
#endif  
#define _rTurret_included

#if !defined COLANDREAS
    #include <colandreas>
#endif

#include <streamer>
#include <foreach>

#if !defined MAX_TURRETS
    #define MAX_TURRETS                             (256)
#endif

#if !defined MIN_TURRET_RATE
    #define MIN_TURRET_RATE                         (100)
#endif

#if !defined MAX_TURRET_VISIBLE_BULLETS
    #define MAX_TURRET_VISIBLE_BULLETS              (32)
#endif

#define INVALID_TURRET                              (-1)
#define INVALID_TURRET_ID                           INVALID_TURRET

// Streamer data identifiers
// Do not reuse these values!
#define TURRET_INT                                  (-706637)
#define TURRET_BIG_AREA_INT                         (-316)
#define TURRET_SHOOT_AREA_INT                       (-56007)
#define TURRET_BULLET_AREA_INT                      (-4234)
#define TURRET_BULLET_OBJECT_INT                    (-801137)

// Turret moveids
#define TURRET_MOVE_ROTATE_CLOCKWISE_1              (-1)
#define TURRET_MOVE_ROTATE_CLOCKWISE_2              (-2)
#define TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1       (-3)
#define TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_2       (-4)
#define TURRET_MOVE_OTHER                           (-5)

// Turret Behaviors
#define TURRET_BEHAVIOR_STATIONARY                  (0)
#define TURRET_BEHAVIOR_ROTATE_CLOCKWISE            (1)
#define TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE     (2)
#define TURRET_BEHAVIOR_AIM_CLOSEST                 (3)
#define TURRET_BEHAVIOR_AIM_FARTHEST                (4)
#define TURRET_BEHAVIOR_AIM_WEAKEST                 (5)
#define TURRET_BEHAVIOR_AIM_STRONGEST               (6)
#define TURRET_BEHAVIOR_AIM_RANDOM                  (7)
#define TURRET_BEHAVIOR_AIM_SINGLE                  (8)
#define TURRET_BEHAVIOR_AIM_CUSTOM                  (9)
#define TURRET_BEHAVIOR_MAX                         (10)

// ======================================================
// Callbacks
// ======================================================
forward OnTurretCreate(turretid);
forward OnTurretDestroy(turretid);
forward OnTurretDeath(turretid, killerid, weaponid);
forward OnTurretHitPlayer(turretid, playerid);
forward OnPlayerTakeTurretDamage(playerid, turretid, Float:damage);
forward OnPlayerShootTurret(playerid, weaponid, turretid, Float:fX, Float:fY, Float:fZ);
forward OnTurretMoved(turretid);
forward OnTurretCustomShoot(turretid);
forward OnTurretBulletCollision(turretid, Float:x, Float:y, Float:z);

enum E_TURRET_DATA
{
    e_turr_objectid,
    e_turr_behavior,
    e_turr_owner,
    e_turr_team,
    e_turr_world,
    e_turr_interior,
    e_turr_area,
    e_turr_bigarea,
    e_turr_rate,
    Text3D:e_turr_3dtext,
    Float:e_turr_3dtext_xoffset,
    Float:e_turr_3dtext_yoffset,
    Float:e_turr_3dtext_zoffset,
    Float:e_turr_3dtext_draw,
    e_turr_3dtext_testlos,
    Float:e_turr_health,
    Float:e_turr_speed,
    Float:e_turr_dmg,
    Float:e_turr_range,
    Float:e_turr_stream,
    Float:e_turr_rotspeed,
    Float:e_turr_bullet_radius,
    e_turr_bullet_model,
    e_turr_bullet_color,
    Float:e_turr_bullet_xoffset,
    Float:e_turr_bullet_yoffset,
    Float:e_turr_bullet_zoffset,
    Float:e_turr_bullet_rx,
    Float:e_turr_bullet_ry,
    Float:e_turr_bullet_rz,
    e_turr_timer,
    e_turr_visible_bullets,
    e_turr_max_visible_bullets,
    bool:e_turr_active,
    bool:e_turr_has_health,
    bool:e_turr_3dspace,
    bool:e_turr_predict,
    bool:e_turr_los,
    bool:e_turr_los_aim,
    bool:e_turr_checkarea,
    bool:e_turr_playsound,
    bool:e_turr_target_focus,
    e_turr_soundid,
    e_turr_target
}

static
    rTurret_Killed[MAX_PLAYERS char],
    rTurret_KilledBy[MAX_PLAYERS],
    rTurretData[MAX_TURRETS][E_TURRET_DATA];
    
static stock
    bool:rTurret_Is_Duplicate = false;
    
new
    Iterator:rTurrets<MAX_TURRETS>,
    Iterator:rTurretTargets[MAX_TURRETS]<MAX_PLAYERS>,
    Iterator:rTurretPlayersInBigArea[MAX_TURRETS]<MAX_PLAYERS>;

// ======================================================
// Core Functions
// ======================================================

stock CreateTurret(behavior = TURRET_BEHAVIOR_STATIONARY, playerid = INVALID_PLAYER_ID, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 1000.0, world = -1, interior = -1, rate = 100, Float:speed = 20.0, Float:dmg = 25.0, Float:range = 50.0, Float:streamdistance = 200.0)
{
    new
        turretid = Iter_Free(rTurrets);
        
    if (turretid == -1) {
        printf("rTurret error: Max turrets of %d reached. Please define the #MAX_TURRETS macro to a higher value.", MAX_TURRETS);
        return INVALID_TURRET;
    }
    
    if (behavior < 0 || behavior >= TURRET_BEHAVIOR_MAX) {
        printf("rTurret warning: Invalid turret behavior! Turret %d's behavior has been changed from %d to TURRET_BEHAVIOR_STATIONARY (%d).", turretid, behavior, TURRET_BEHAVIOR_STATIONARY);
        behavior = TURRET_BEHAVIOR_STATIONARY;
    }
    
    if (rate < MIN_TURRET_RATE) {
        printf("rTurret warning: Turret %d's rate of %d has been set to MIN_TURRET_RATE %d.", turretid, rate, MIN_TURRET_RATE);
        rate = MIN_TURRET_RATE;
    }
    
    Iter_Add(rTurrets, turretid);
    rTurretData[turretid][e_turr_behavior] =            behavior;
    rTurretData[turretid][e_turr_team] =                255;
    rTurretData[turretid][e_turr_owner] =               playerid;
    rTurretData[turretid][e_turr_world] =               world;
    rTurretData[turretid][e_turr_interior] =            interior;
    rTurretData[turretid][e_turr_rate] =                rate;
    rTurretData[turretid][e_turr_3dtext_xoffset] =      0;
    rTurretData[turretid][e_turr_3dtext_yoffset] =      0;
    rTurretData[turretid][e_turr_3dtext_zoffset] =      0;
    rTurretData[turretid][e_turr_3dtext_draw] =         25;
    rTurretData[turretid][e_turr_3dtext_testlos] =      0;
    rTurretData[turretid][e_turr_health] =              100.0;
    rTurretData[turretid][e_turr_speed] =               floatabs(speed);
    rTurretData[turretid][e_turr_dmg] =                 dmg;
    rTurretData[turretid][e_turr_range] =               range;
    rTurretData[turretid][e_turr_stream] =              streamdistance;
    rTurretData[turretid][e_turr_rotspeed] =            0.0005;
    rTurretData[turretid][e_turr_bullet_radius] =       2.0;
    rTurretData[turretid][e_turr_bullet_model] =        2352;
    rTurretData[turretid][e_turr_bullet_color] =        0;
    rTurretData[turretid][e_turr_bullet_xoffset] =      0.0;
    rTurretData[turretid][e_turr_bullet_yoffset] =      0.0;
    rTurretData[turretid][e_turr_bullet_zoffset] =      0.0;
    rTurretData[turretid][e_turr_bullet_rx] =           0.0;
    rTurretData[turretid][e_turr_bullet_ry] =           270.0;
    rTurretData[turretid][e_turr_bullet_rz] =           0.0;
    rTurretData[turretid][e_turr_max_visible_bullets] = MAX_TURRET_VISIBLE_BULLETS;
    rTurretData[turretid][e_turr_active] =              true;
    rTurretData[turretid][e_turr_has_health] =          false;
    rTurretData[turretid][e_turr_3dspace] =             true;
    rTurretData[turretid][e_turr_predict] =             true;
    rTurretData[turretid][e_turr_los] =                 true;
    rTurretData[turretid][e_turr_los_aim] =             true;
    rTurretData[turretid][e_turr_checkarea] =           true;
    rTurretData[turretid][e_turr_playsound] =           true;
    rTurretData[turretid][e_turr_soundid] =             1130;
    rTurretData[turretid][e_turr_target] =              INVALID_PLAYER_ID;
    rTurretData[turretid][e_turr_target_focus] =        false;
    
    rTurretData[turretid][e_turr_objectid] =    CreateDynamicObject(2985, x, y, z, rx, ry, (behavior != TURRET_BEHAVIOR_ROTATE_CLOCKWISE && behavior != TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE) ? rz : 0.0, world, interior, .streamdistance = streamdistance);
    rTurretData[turretid][e_turr_area] =        CreateDynamicSphere(0, 0, 0, range, world, interior);
    rTurretData[turretid][e_turr_bigarea] =     CreateDynamicSphere(0, 0, 0, 60.0, world, interior);
    rTurretData[turretid][e_turr_3dtext] =      CreateDynamic3DTextLabel(" ", -1, x, y, z + 2, 25, .testlos = 0, .worldid = world, .interiorid = interior, .streamdistance = 50);
    
    AttachDynamicAreaToObject(rTurretData[turretid][e_turr_area], rTurretData[turretid][e_turr_objectid]);
    AttachDynamicAreaToObject(rTurretData[turretid][e_turr_bigarea], rTurretData[turretid][e_turr_objectid]);
    // Turret object array data (identifier, turretid, moveid (below))
    Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_INT);
    Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, turretid);
    // Shoot area array data (turretid, identifier)
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_area], E_STREAMER_EXTRA_ID, turretid);
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_area], E_STREAMER_EXTRA_ID, TURRET_SHOOT_AREA_INT);
    // Big area array data (turretid, identifier)
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_bigarea], E_STREAMER_EXTRA_ID, turretid);
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_bigarea], E_STREAMER_EXTRA_ID, TURRET_BIG_AREA_INT);
    
    CallLocalFunction("OnTurretCreate", "d", turretid);
    
    if (behavior == TURRET_BEHAVIOR_ROTATE_CLOCKWISE) {
        Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_1);
        MoveDynamicObject(rTurretData[turretid][e_turr_objectid], x, y, z + 0.001, rTurretData[turretid][e_turr_rotspeed], rx, ry, -179);
    } else if (behavior == TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE) {
        Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1);
        MoveDynamicObject(rTurretData[turretid][e_turr_objectid], x, y, z + 0.001, rTurretData[turretid][e_turr_rotspeed], rx, ry, 179);
    } else
        Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_OTHER);

    rTurretData[turretid][e_turr_timer] = SetTimerEx("Timer_TurretShoot", rate, true, "d", turretid);
    
    return turretid;
}

stock DestroyTurret(turretid, &iter_next = -1)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    CallLocalFunction("OnTurretDestroy", "d", turretid);
    
    DestroyDynamicArea(rTurretData[turretid][e_turr_area]);
    DestroyDynamicArea(rTurretData[turretid][e_turr_bigarea]);
        
    if (rTurretData[turretid][e_turr_timer] != -1) {
        KillTimer(rTurretData[turretid][e_turr_timer]);
        rTurretData[turretid][e_turr_timer] = -1;
    }
    
    //rTurretData[turretid][e_turr_visible_bullets] = -1;
    DestroyDynamicObject(rTurretData[turretid][e_turr_objectid]);
    //DestroyDynamicArea(rTurretData[turretid][e_turr_area]);
    //DestroyDynamicArea(rTurretData[turretid][e_turr_bigarea]);
    DestroyDynamic3DTextLabel(rTurretData[turretid][e_turr_3dtext]);
    rTurretData[turretid][e_turr_objectid] = INVALID_OBJECT_ID;
    Iter_Clear(rTurretTargets[turretid]);
    Iter_Clear(rTurretPlayersInBigArea[turretid]);
    Iter_SafeRemove(rTurrets, turretid, iter_next);
    return 1;
}

stock IsValidTurret(turretid)
{
    return Iter_Contains(rTurrets, turretid);
}

// ======================================================
// Getter Functions
// ======================================================

stock GetTurretObject(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return INVALID_OBJECT_ID;
        
    return rTurretData[turretid][e_turr_objectid];
}

stock Text3D:GetTurret3DTextLabel(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return Text3D:INVALID_3DTEXT_ID;
        
    return rTurretData[turretid][e_turr_3dtext];
}

stock GetTurretOwner(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return INVALID_PLAYER_ID;
        
    return rTurretData[turretid][e_turr_owner];
}

stock GetTurretBehavior(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return -1;
        
    return rTurretData[turretid][e_turr_behavior];
}

stock GetTurretTeam(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 255;
        
    return rTurretData[turretid][e_turr_team];
}

stock Float:GetTurretHealth(turretid)
{
    if (!Iter_Contains(rTurrets, turretid) || !rTurretData[turretid][e_turr_has_health])
        return -1.0;
        
    return rTurretData[turretid][e_turr_health];
}

stock GetTurretPos(turretid, &Float:x, &Float:y, &Float:z)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], x, y, z);
    return 1;
}

stock GetTurretRot(turretid, &Float:rotx, &Float:roty, &Float:rotz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    GetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], rotx, roty, rotz);
    return 1;
}

stock Float:GetTurretRotSpeed(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0.0;
        
    return rTurretData[turretid][e_turr_rotspeed] / speed;
}

stock Float:GetTurretBulletRadius(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0.0;
        
    return rTurretData[turretid][e_turr_bullet_radius];
}

stock GetTurretWorld(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return -2;
        
    return rTurretData[turretid][e_turr_world];
}

stock GetTurretInterior(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return -2;
        
    return rTurretData[turretid][e_turr_interior];
}

stock GetTurretText(turretid, text[], maxlength = sizeof(text))
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    GetDynamic3DTextLabelText(rTurretData[turretid][e_turr_3dtext], text, sizeof(text));
    return 1;
}

stock GetTurretTextOffset(turretid, &Float:offsetx, &Float:offsety, &Float:offsetz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    offsetx = rTurretData[turretid][e_turr_3dtext_xoffset];
    offsety = rTurretData[turretid][e_turr_3dtext_yoffset];
    offsetz = rTurretData[turretid][e_turr_3dtext_zoffset];
    return 1;
}

stock GetTurretFireRate(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    return rTurretData[turretid][e_turr_rate];
}

stock Float:GetTurretBulletSpeed(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0.0;
        
    return rTurretData[turretid][e_turr_speed];
}

stock Float:GetTurretDamage(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0.0;
        
    return rTurretData[turretid][e_turr_dmg];
}

stock Float:GetTurretRange(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0.0;
        
    return rTurretData[turretid][e_turr_range];
}

stock Float:GetTurretStreamDistance(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0.0;
        
    return rTurretData[turretid][e_turr_stream];
}

stock GetTurretMaterial(turretid, materialindex, &modelid, txdname[], texturename[], &materialcolor, maxtxdname = sizeof txdname, maxtexturename = sizeof texturename)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    GetDynamicObjectMaterial(rTurretData[turretid][e_turr_objectid], materialindex, &modelid, txdname, texturename, &materialcolor, maxtxdname, maxtexturename);
    return 1;
}

stock GetTurretBulletModel(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return -1;
        
    return rTurretData[turretid][e_turr_bullet_model];
}

stock GetTurretBulletColor(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return -1;
        
    return rTurretData[turretid][e_turr_bullet_color];
}

stock GetTurretBulletOffset(turretid, &Float:offsetx, &Float:offsety, &Float:offsetz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    offsetx = rTurretData[turretid][e_turr_bullet_offsetx];
    offsety = rTurretData[turretid][e_turr_bullet_offsety];
    offsetz = rTurretData[turretid][e_turr_bullet_offsetz];
    return 1;
}

stock GetTurretBulletRot(turretid, &Float:rx, &Float:ry, &Float:rz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rx = rTurretData[turretid][e_turr_bullet_rx];
    ry = rTurretData[turretid][e_turr_bullet_ry];
    rz = rTurretData[turretid][e_turr_bullet_rz];
    return 1;
}

stock GetTurretVisibleBullets(turretid)
{
    if (!Iter_Contains(rTurrtts, turretid))
        return 0;
        
    return rTurretData[turretid][e_turr_visible_bullets];
}

stock GetTurretMaxVisibleBullets(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;

    return rTurretData[turretid][e_turr_max_visible_bullets];
}

stock GetTurretSound(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    return rTurretData[turretid][e_turr_soundid];
}

stock GetTurretKiller(playerid)
{
    return rTurret_KilledBy[playerid];
}

// ======================================================
// Setter Functions
// ======================================================

stock SetTurretOwner(turretid, playerid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (playerid == rTurretData[turretid][e_turr_owner])
        return 0;
    else
        rTurretData[turretid][e_turr_owner] = playerid;
    return 1;
}

stock SetTurretBehavior(turretid, behavior)
{
    if (!Iter_Contains(rTurrets, turretid) || behavior < 0 || behavior >= TURRET_BEHAVIOR_MAX)
        return 0;
    
    if (IsDynamicObjectMoving(rTurretData[turretid][e_turr_objectid]))
        StopDynamicObject(rTurretData[turretid][e_turr_objectid]);
        
    switch(rTurretData[turretid][e_turr_behavior]) {
        case TURRET_BEHAVIOR_ROTATE_CLOCKWISE: {
            Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_1);
            Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_2);
        }
        case TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE: {
            Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1);
            Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_2);
        }
        default:
            Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_OTHER);
    }
    
    rTurretData[turretid][e_turr_behavior] = behavior;
    
    switch(behavior) {
        case TURRET_BEHAVIOR_ROTATE_CLOCKWISE: {
            new
                Float:turPX,
                Float:turPY,
                Float:turPZ,
                Float:turRX,
                Float:turRY,
                Float:turRZ;
                
            GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ);
            GetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, turRZ);
            SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, 0.0);
            Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_1);
            MoveDynamicObject(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ + 0.001, rTurretData[turretid][e_turr_rotspeed], turRX, turRY, -179.0);
        }
        case TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE: {
            new
                Float:turPX,
                Float:turPY,
                Float:turPZ,
                Float:turRX,
                Float:turRY,
                Float:turRZ;
                
            GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ);
            GetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, turRZ);
            SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, 0.0);
            Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1);
            MoveDynamicObject(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ + 0.001, rTurretData[turretid][e_turr_rotspeed], turRX, turRY, 179.0);
        }
        default:
            Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_OTHER);
    }
    return 1;
}

stock SetTurretTeam(turretid, team)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_team] = team;
    return 1;
}

stock SetTurretHealth(turretid, Float:health)
{
    if (!Iter_Contains(rTurrets, turretid) || floatcmp(0.0, health) == 1)
        return 0;

    rTurretData[turretid][e_turr_health] = health;
    return 1;
}

stock SetTurretPos(turretid, Float:x, Float:y, Float:z)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;

    SetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], x, y, z);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_X, turPX + rTurretData[turretid][e_turr_3dtext_xoffset]);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_Y, turPY + rTurretData[turretid][e_turr_3dtext_yoffset]);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_Z, turPZ + 2 + rTurretData[turretid][e_turr_3dtext_zoffset]);
    return 1;
}

stock SetTurretRot(turretid, Float:rotx, Float:roty, Float:rotz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;

    SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], rotx, roty, rotz);
    return 1;
}

stock SetTurretRotSpeed(turretid, Float:speed)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_rotspeed] = 0.0005 * floatabs(speed);
    return 1;
}

stock SetTurretBulletRadius(turretid, Float:radius)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_bullet_radius] = radius;
    return 1;
}

stock SetTurretWorld(turretid, world)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_world] = world;
    Streamer_SetIntData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_WORLD_ID, world);
    Streamer_SetIntData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_WORLD_ID, world);
    return 1;
}

stock SetTurretInterior(turretid, interior)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_interior] = interior;
    Streamer_SetIntData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_INTERIOR_ID, interior);
    Streamer_SetIntData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_INTERIOR_ID, interior);
    return 1;
}

stock SetTurretText(turretid, color, const text[])
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    UpdateDynamic3DTextLabelText(rTurretData[turretid][e_turr_3dtext], color, text);
    return 1;
}

stock SetTurretTextOffset(turretid, Float:offsetx, Float:offsety, Float:offsetz, Float:drawdistance = -1.0, testlos = -1)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    new
        Float:turPX,
        Float:turPY,
        Float:turPZ;
        
    GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ);
        
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_X, turPX + offsetx);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_Y, turPY + offsety);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_Z, turPZ + 2 + offsetz);
    
    if (floatcmp(drawdistance, -1.0) != 0) {
        Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_DRAW_DISTANCE, drawdistance);
        Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_STREAM_DISTANCE, drawdistance + 25);
        rTurretData[turretid][e_turr_3dtext_draw] = drawdistance;
    }
    if (testlos != -1) {
        Streamer_SetIntData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_TEST_LOS, testlos);
        rTurretData[turretid][e_turr_3dtext_testlos] = testlos;
    }
        
    rTurretData[turretid][e_turr_3dtext_xoffset] = offsetx;
    rTurretData[turretid][e_turr_3dtext_yoffset] = offsety;
    rTurretData[turretid][e_turr_3dtext_zoffset] = offsetz;
    return 1;
}

stock SetTurretFireRate(turretid, rate)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (rate < MIN_TURRET_RATE) {
        printf("rTurret warning: Turret %d's rate of %d has been set to MIN_TURRET_RATE %d.", turretid, rate, MIN_TURRET_RATE);
        rate = MIN_TURRET_RATE;
    }
    rTurretData[turretid][e_turr_rate] = rate;
    if (rTurretData[turretid][e_turr_timer] != -1 && rTurretData[turretid][e_turr_active]) {
        KillTimer(rTurretData[turretid][e_turr_timer]);
        rTurretData[turretid][e_turr_timer] = SetTimerEx("Timer_TurretShoot", rate, true, "d", turretid);
    }
    return 1;
}

stock SetTurretBulletSpeed(turretid, Float:speed)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_speed] = floatabs(speed);
    return 1;
}

stock SetTurretDamage(turretid, Float:damage)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_dmg] = damage;
    return 1;
}

stock SetTurretRange(turretid, Float:range)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_range] = range;
    return 1;
}

stock SetTurretStreamDistance(turretid, Float:streamdistance)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_stream] = streamdistance;
    Streamer_SetFloatData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_STREAM_DISTANCE, streamdistance);
    return 1;
}

stock SetTurretMaterial(turretid, materialindex, modelid, const txdname[], const texturename[], materialcolor = 0)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    SetDynamicObjectMaterial(rTurretData[turretid][e_turr_objectid], materialindex, modelid, txdname, texturename, materialcolor);
    return 1;
}

stock SetTurretBulletModel(turretid, modelid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_bullet_model] = modelid;
    return 1;
}

stock SetTurretBulletColor(turretid, color = 0)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_bullet_color] = color;
    return 1;
}

stock SetTurretBulletOffset(turretid, Float:offsetx, Float:offsety, Float:offsetz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_bullet_xoffset] = offsetx;
    rTurretData[turretid][e_turr_bullet_yoffset] = offsety;
    rTurretData[turretid][e_turr_bullet_zoffset] = offsetz;
    return 1;
}

stock SetTurretBulletRot(turretid, Float:rx, Float:ry, Float:rz)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_bullet_rx] = rx;
    rTurretData[turretid][e_turr_bullet_ry] = ry;
    rTurretData[turretid][e_turr_bullet_rz] = rz;
    return 1;
}

stock SetTurretMaxVisibleBullets(turretid, maxbullets)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
    if (maxbullets >= MAX_TURRET_VISIBLE_BULLETS) {
        printf("rTurret Warning: Max visible bullets for turret %d has been lowered from %d to MAX_TURRET_VISIBLE_BULLETS %d", turretid, maxbullets, MAX_TURRET_VISIBLE_BULLETS);
        maxbullets = MAX_TURRET_VISIBLE_BULLETS;
    } else if (maxbullets < 1) {
        printf("rTurret Warning: Max visible bullets for turret %d has been raised from %d to %d", turretid, maxbullets, 1);
        maxbullets = 1;
    }
    rTurretData[turretid][e_turr_max_visible_bullets] = maxbullets;
    return 1;
}

stock SetTurretSound(turretid, soundid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    rTurretData[turretid][e_turr_soundid] = soundid;
    return 1;
}

stock SetTurretSingleTarget(turretid, targetid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (!IsPlayerConnected(targetid))
        return 0;
        
    rTurretData[turretid][e_turr_target] = targetid;
    return 1;
}

// ======================================================
// Extra Functions
// ======================================================

stock ToggleTurretActive(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 1;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_active])
            return 0;
        if (rTurretData[turretid][e_turr_timer] == -1)
            rTurretData[turretid][e_turr_timer] = SetTimerEx("Timer_TurretShoot", rTurretData[turretid][e_turr_rate], true, "d", turretid);
        else {
            KillTimer(rTurretData[turretid][e_turr_timer]);
            rTurretData[turretid][e_turr_timer] = SetTimerEx("Timer_TurretShoot", rTurretData[turretid][e_turr_rate], true, "d", turretid);
        }
        rTurretData[turretid][e_turr_active] = true;
    } else {
        if (!rTurretData[turretid][e_turr_active])
            return 0;
        if (rTurretData[turretid][e_turr_timer] != -1) {
            KillTimer(rTurretData[turretid][e_turr_timer]);
            rTurretData[turretid][e_turr_timer] = -1;
        }
        rTurretData[turretid][e_turr_active] = false;
    }
    return 1;
}

stock ToggleTurretHealth(turretid, toggle, Float:sethealth = -1.0)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_has_health])
            return 0;
        rTurretData[turretid][e_turr_has_health] = true;
    } else {
        if (!rTurretData[turretid][e_turr_has_health])
            return 0;
        rTurretData[turretid][e_turr_has_health] = false;
    }
    if (floatcmp(0.0, sethealth) != 1)
        rTurretData[turretid][e_turr_health] = sethealth;
        
    return 1;
}

stock ToggleTurretCheckArea(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_checkarea])
            return 0;
        rTurretData[turretid][e_turr_checkarea] = true;
        Streamer_RemoveArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_area], E_STREAMER_EXTRA_ID, -1);
        Streamer_AppendArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_area], E_STREAMER_EXTRA_ID, TURRET_SHOOT_AREA_INT);
    } else {
        if (!rTurretData[turretid][e_turr_checkarea])
            return 0;
        rTurretData[turretid][e_turr_checkarea] = false;
        Streamer_RemoveArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_area], E_STREAMER_EXTRA_ID, TURRET_SHOOT_AREA_INT);
        Streamer_AppendArrayData(STREAMER_TYPE_AREA, rTurretData[turretid][e_turr_area], E_STREAMER_EXTRA_ID, -1);
    }
    return 1;
}

stock ToggleTurretPlaySound(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_playsound])
            return 0;
        rTurretData[turretid][e_turr_playsound] = true;
    } else {
        if (!rTurretData[turretid][e_turr_playsound])
            return 0;
        rTurretData[turretid][e_turr_playsound] = false;
    }
    return 1;
}

stock ToggleTurretVisible(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    new
        Float:sd;
        
    Streamer_GetFloatData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_STREAM_DISTANCE, sd);
    
    if (toggle) {
        if (floatcmp(sd, rTurretData[turretid][e_turr_stream]) == 0)
            return 0;
        Streamer_SetFloatData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_STREAM_DISTANCE, rTurretData[turretid][e_turr_stream]);
    } else {
        if (floatcmp(sd, 0.0) == 0)
            return 0;
        Streamer_SetFloatData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_STREAM_DISTANCE, 0.0);
    }
    return 1;
}

stock ToggleTurretAim3dSpace(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_3dspace])
            return 0;
        rTurretData[turretid][e_turr_3dspace] = true;
    } else {
        if (!rTurretData[turretid][e_turr_3dspace])
            return 0;
        rTurretData[turretid][e_turr_3dspace] = false;
    }
    return 1;
}

stock ToggleTurretPredictiveAim(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_predict])
            return 0;
        rTurretData[turretid][e_turr_predict] = true;
    } else {
        if (!rTurretData[turretid][e_turr_predict])
            return 0;
        rTurretData[turretid][e_turr_predict] = false;
    }
    return 1;
}

stock ToggleTurretLOS(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_los])
            return 0;
        rTurretData[turretid][e_turr_los] = true;
    } else {
        if (!rTurretData[turretid][e_turr_los])
            return 0;
        rTurretData[turretid][e_turr_los] = false;
    }
    return 1;
}

stock ToggleTurretLOSAim(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_los_aim])
            return 0;
        rTurretData[turretid][e_turr_los_aim] = true;
    } else {
        if (!rTurretData[turretid][e_turr_los_aim])
            return 0;
        rTurretData[turretid][e_turr_los_aim] = false;
    }
    return 1;
}

stock ToggleTurretTargetFocus(turretid, toggle)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    if (toggle) {
        if (rTurretData[turretid][e_turr_target_focus])
            return 0;
        rTurretData[turretid][e_turr_target_focus] = true;
    } else {
        if (!rTurretData[turretid][e_turr_target_focus])
            return 0;
        rTurretData[turretid][e_turr_target_focus] = false;
    }
    return 1;
}

stock UpdateTurretTextPos(turretid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
        
    new
        Float:turPX,
        Float:turPY,
        Float:turPZ;
        
    GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_X, turPX + rTurretData[turretid][e_turr_3dtext_xoffset]);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_Y, turPY + rTurretData[turretid][e_turr_3dtext_yoffset]);
    Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, rTurretData[turretid][e_turr_3dtext], E_STREAMER_Z, turPZ + 2 + rTurretData[turretid][e_turr_3dtext_zoffset]);
    return 1;
}

stock MoveTurret(turretid, Float:X, Float:Y, Float:Z, Float:Speed, Float:RotX = -1000.0, Float:RotY = -1000.0, Float:RotZ = -1000.0)
{
    if (!Iter_Contains(rTurrets, turretid) ||
        rTurretData[turretid][e_turr_behavior] == TURRET_BEHAVIOR_ROTATE_CLOCKWISE ||
        rTurretData[turretid][e_turr_behavior] == TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE)
            return 0;
            
    MoveDynamicObject(rTurretData[turretid][e_turr_objectid], X, Y, Z, Speed, RotX, RotY, RotZ);
    return 1;
}

stock TurretDamagePlayer(turretid, playerid)
{
    if (!Iter_Contains(rTurrets, turretid))
        return 0;
    if (funcidx("OnTurretHitPlayer") == -1 || CallLocalFunction("OnTurretHitPlayer", "dd", turretid, playerid)) {
        // Damage player
        new
            Float:a,
            Float:h,
            Float:dif;
            
        GetPlayerArmour(playerid, a);
        GetPlayerHealth(playerid, h);
        if (floatcmp(a, 0.0) != 1) {
            // No armor
            dif = h - rTurretData[turretid][e_turr_dmg];
            if (floatcmp(dif, 0.0) == -1 || floatcmp(dif, 0.0) == 0) {
                // Dead
                rTurret_Killed{playerid} = true;
                rTurret_KilledBy[playerid] = turretid;
                SetPlayerHealth(playerid, 0.0);
            } else
                SetPlayerHealth(playerid, dif);
        } else {
            // Has armor
            dif = a - rTurretData[turretid][e_turr_dmg];
            if (floatcmp(dif, 0.0) == -1) {
                // Damage health
                new
                    Float:newdif = h - floatabs(dif);
                    
                SetPlayerArmour(playerid, 0.0);
                
                if (floatcmp(newdif, 0.0) == -1 || floatcmp(newdif, 0.0) == 0) {
                    // Dead
                    rTurret_Killed{playerid} = true;
                    rTurret_KilledBy[playerid] = turretid;
                    SetPlayerHealth(playerid, 0.0);
                } else
                    SetPlayerHealth(playerid, newdif);
            } else
                SetPlayerArmour(playerid, dif);
        }
        CallLocalFunction("OnPlayerTakeTurretDamage", "ddf", playerid, turretid, rTurretData[turretid][e_turr_dmg]);
    }
    return 1;
}

// ======================================================
// Hooks
// ======================================================

#if defined FILTERSCRIPT
public OnFilterScriptInit()
{
    new dupli_value = getproperty(.name = "rTurret_Duplicate_Prop");
    if (dupli_value == 126382) {
        printf("Warning: Duplicate instance of rTurret detected! Remove the include from your filterscript or gamemode.");
        rTurret_Is_Duplicate = true;
    } else {
        setproperty(.name = "rTurret_Duplicate_Prop", .value = 126382);
    }
        
    Iter_Init(rTurretTargets);
    Iter_Init(rTurretPlayersInBigArea);
    foreach(new i : Player) {
        rTurret_Killed{i} = false;
        rTurret_KilledBy[i] = INVALID_TURRET;
    }
    for (new i = 0; i < MAX_TURRETS; i++) {
        rTurretData[i][e_turr_timer] = -1;
        rTurretData[i][e_turr_visible_bullets] = -1;
        rTurretData[i][e_turr_objectid] = INVALID_OBJECT_ID;
    }
    return CallLocalFunction("rTurret_OnFilterScriptInit", "");
}
    #if defined _ALS_OnFilterScriptInit
        #undef OnFilterScriptInit
    #else
        #define _ALS_OnFilterScriptInit
    #endif
    #define OnFilterScriptInit rTurret_OnFilterScriptInit
    forward rTurret_OnFilterScriptInit();
    
public OnFilterScriptExit()
{
    if (!rTurret_Is_Duplicate) {
        deleteproperty(0, "", 126382);
    }
    foreach(new i : rTurrets)
        DestroyTurret(i);
        
    Streamer_DestroyAllItems(STREAMER_TYPE_OBJECT, 0);
    Streamer_DestroyAllItems(STREAMER_TYPE_AREA, 0);
    Streamer_DestroyAllItems(STREAMER_TYPE_3D_TEXT_LABEL, 0);
    return CallLocalFunction("rTurret_OnFilterScriptExit", "");
}
    #if defined _ALS_OnFilterScriptExit
        #undef OnFilterScriptExit
    #else
        #define _ALS_OnFilterScriptExit
    #endif
    #define OnFilterScriptExit rTurret_OnFilterScriptExit
    forward rTurret_OnFilterScriptExit();
    
#else
public OnGameModeInit()
{
    new dupli_value = getproperty(.name = "rTurret_Duplicate_Prop");
    if (dupli_value == 126382) {
        printf("Warning: Duplicate instance of rTurret detected! Remove the include from your filterscript or gamemode.");
        rTurret_Is_Duplicate = true;
    } else {
        setproperty(.name = "rTurret_Duplicate_Prop", .value = 126382);
    }
    Iter_Init(rTurretTargets);
    Iter_Init(rTurretPlayersInBigArea);
    foreach(new i : Player) {
        rTurret_Killed{i} = false;
        rTurret_KilledBy[i] = INVALID_TURRET;
    }
    for (new i = 0; i < MAX_TURRETS; i++) {
        rTurretData[i][e_turr_timer] = -1;
        rTurretData[i][e_turr_visible_bullets] = -1;
        rTurretData[i][e_turr_objectid] = INVALID_OBJECT_ID;
    }
    return CallLocalFunction("rTurret_OnGameModeInit", "");
}
    #if defined _ALS_OnGameModeInit
        #undef OnGameModeInit
    #else
        #define _ALS_OnGameModeInit
    #endif
    #define OnGameModeInit rTurret_OnGameModeInit
    forward rTurret_OnGameModeInit();
    
public OnGameModeExit()
{
    if (!rTurret_Is_Duplicate) {
        deleteproperty(0, "", 126382);
    }
    foreach(new i : rTurrets)
        DestroyTurret(i);
        
    Streamer_DestroyAllItems(STREAMER_TYPE_OBJECT, 0);
    Streamer_DestroyAllItems(STREAMER_TYPE_AREA, 0);
    Streamer_DestroyAllItems(STREAMER_TYPE_3D_TEXT_LABEL, 0);
    return CallLocalFunction("rTurret_OnGameModeExit", "");
}
    #if defined _ALS_OnGameModeExit
        #undef OnGameModeExit
    #else
        #define _ALS_OnGameModeExit
    #endif
    #define OnGameModeExit rTurret_OnGameModeExit
    forward rTurret_OnGameModeExit();
#endif

public OnPlayerConnect(playerid)
{
    rTurret_Killed{playerid} = false;
    rTurret_KilledBy[playerid] = INVALID_TURRET;
    return CallLocalFunction("rTurret_OnPlayerConnect", "d", playerid);
}
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect rTurret_OnPlayerConnect
forward rTurret_OnPlayerConnect(playerid);

public OnPlayerSpawn(playerid)
{
    if (rTurret_Killed{playerid})
        // Prevent taking turret damage upon spawning
        SetTimerEx("Timer_ResetTurretKilled", 1000, false, "d", playerid);
    return CallLocalFunction("rTurret_OnPlayerSpawn", "d", playerid);
}
#if defined _ALS_OnPlayerSpawn
    #undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn rTurret_OnPlayerSpawn
forward rTurret_OnPlayerSpawn(playerid);

public OnPlayerDeath(playerid, killerid, reason)
{       
    if (rTurret_Killed{playerid}) {
        if (Iter_Contains(rTurrets, rTurret_KilledBy[playerid])) {
            new
                turretid = rTurret_KilledBy[playerid];
                
            killerid = rTurretData[turretid][e_turr_owner];
            reason = 47;
            if (Iter_Contains(rTurretTargets[turretid], playerid))
                Iter_Remove(rTurretTargets[turretid], playerid);
        }
    }
    return CallLocalFunction("rTurret_OnPlayerDeath", "ddd", playerid, killerid, reason);
}
#if defined _ALS_OnPlayerDeath
    #undef OnPlayerDeath
#else
    #define _ALS_OnPlayerDeath
#endif
#define OnPlayerDeath rTurret_OnPlayerDeath
forward rTurret_OnPlayerDeath(playerid, killerid, reason);

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
    if (hittype == BULLET_HIT_TYPE_PLAYER_OBJECT) {
        new
            streamerid = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, hitid);
        if (IsValidDynamicObject(streamerid)) {
            if (Streamer_IsInArrayData(STREAMER_TYPE_OBJECT, streamerid, E_STREAMER_EXTRA_ID, TURRET_INT)) {
                // Turret object
                new
                    data[3],
                    turretid;
                    
                Streamer_GetArrayData(STREAMER_TYPE_OBJECT, streamerid, E_STREAMER_EXTRA_ID, data);
                turretid = data[1];
                
                if (rTurretData[turretid][e_turr_has_health]) {
                    // Deplete health
                    // Credits to Slice for weapon damage values - https://github.com/oscar-broman/samp-weapon-config
                    static Float:rTurret_weapon_damage[] = {
                        1.0, // 0 - Fist
                        1.0, // 1 - Brass knuckles
                        1.0, // 2 - Golf club
                        1.0, // 3 - Nitestick
                        1.0, // 4 - Knife
                        1.0, // 5 - Bat
                        1.0, // 6 - Shovel
                        1.0, // 7 - Pool cue
                        1.0, // 8 - Katana
                        1.0, // 9 - Chainsaw
                        1.0, // 10 - Dildo
                        1.0, // 11 - Dildo 2
                        1.0, // 12 - Vibrator
                        1.0, // 13 - Vibrator 2
                        1.0, // 14 - Flowers
                        1.0, // 15 - Cane
                        82.5, // 16 - Grenade
                        0.0, // 17 - Teargas
                        1.0, // 18 - Molotov
                        9.9, // 19 - Vehicle M4 (custom)
                        46.2, // 20 - Vehicle minigun (custom)
                        0.0, // 21
                        8.25, // 22 - Colt 45
                        13.2, // 23 - Silenced
                        46.2, // 24 - Deagle
                        3.3, // 25 - Shotgun
                        3.3, // 26 - Sawed-off
                        4.95, // 27 - Spas
                        6.6, // 28 - UZI
                        8.25, // 29 - MP5
                        9.9, // 30 - AK47
                        9.9, // 31 - M4
                        6.6, // 32 - Tec9
                        24.75, // 33 - Cuntgun
                        41.25, // 34 - Sniper
                        82.5, // 35 - Rocket launcher
                        82.5, // 36 - Heatseeker
                        1.0, // 37 - Flamethrower
                        46.2, // 38 - Minigun
                        82.5, // 39 - Satchel
                        0.0, // 40 - Detonator
                        0.33, // 41 - Spraycan
                        0.33, // 42 - Fire extinguisher
                        0.0, // 43 - Camera
                        0.0, // 44 - Night vision
                        0.0, // 45 - Infrared
                        0.0, // 46 - Parachute
                        0.0, // 47 - Fake pistol
                        2.64, // 48 - Pistol whip (custom)
                        9.9, // 49 - Vehicle
                        330.0, // 50 - Helicopter blades
                        82.5, // 51 - Explosion
                        1.0, // 52 - Car park (custom)
                        1.0, // 53 - Drowning
                        165.0  // 54 - Splat
                    };
                    
                    if (CallLocalFunction("OnPlayerShootTurret", "dddfff", playerid, weaponid, turretid, fX, fY, fZ) == 1) {
                        new
                            Float:diff = rTurretData[turretid][e_turr_health] - rTurret_weapon_damage[weaponid];
                        if (floatcmp(0.0, diff) == 1) {
                            CallLocalFunction("OnTurretDeath", "ddd", turretid, playerid, weaponid);
                            DestroyTurret(turretid);
                        } else {
                            rTurretData[turretid][e_turr_health] = diff;
                        }
                    } else {
                        if (floatcmp(0.0, rTurretData[turretid][e_turr_health]) == 1) {
                            CallLocalFunction("OnTurretDeath", "ddd", turretid, playerid, weaponid);
                            DestroyTurret(turretid);
                        }
                    }
                }
            }
        }
    }
    return CallLocalFunction("rTurret_OnPlayerWeaponShot", "ddddfff", playerid, weaponid, hittype, hitid, fX, fY, fZ);
}
#if defined _ALS_OnPlayerWeaponShot
    #undef OnPlayerWeaponShot
#else
    #define _ALS_OnPlayerWeaponShot
#endif
#define OnPlayerWeaponShot rTurret_OnPlayerWeaponShot
forward rTurret_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);

public OnDynamicObjectMoved(objectid)
{
    if (!IsValidDynamicObject(objectid)) {
        return CallLocalFunction("rTurret_OnDynamicObjectMoved", "d", objectid);
    }
    if (Streamer_IsInArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, TURRET_BULLET_OBJECT_INT)) {
        // Destroy bullet and its area
        new
            data[4],
            turretid,
            bullet_areaid,
            colliPath;
            
        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, data);
        turretid = data[1];
        bullet_areaid = data[2];
        colliPath = data[3];
        
        if (colliPath == 1) {
            new
                Float:bX, Float:bY, Float:bZ;
                
            GetDynamicObjectPos(objectid, bX, bY, bZ);
            CallLocalFunction("OnTurretBulletCollision", "dfff", turretid, bX, bY, bZ);
        }
            
        DestroyDynamicArea(bullet_areaid);
        DestroyDynamicObject(objectid);
        rTurretData[turretid][e_turr_visible_bullets]--;
        //return CallLocalFunction("rTurret_OnDynamicObjectMoved", "d", objectid);
        return 1;
    } else if (Streamer_IsInArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, TURRET_INT)) {
        // Turret object
        new
            data[3],
            turretid,
            Float:oX,
            Float:oY,
            Float:oZ,
            Float:oRX,
            Float:oRY,
            Float:oRZ,
            movestate;
            
        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, data);
        turretid = data[1];
        movestate = data[2];
        GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], oX, oY, oZ);
        GetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], oRX, oRY, oRZ);
        switch(movestate) {
            case TURRET_MOVE_ROTATE_CLOCKWISE_1:{
                Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_1);
                Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_2);
                MoveDynamicObject(rTurretData[turretid][e_turr_objectid], oX, oY, oZ - 0.001, rTurretData[turretid][e_turr_rotspeed], oRX, oRY, -359.0);
            }
            case TURRET_MOVE_ROTATE_CLOCKWISE_2: {
                Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_2);
                Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_CLOCKWISE_1);
                SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], oRX, oRY, 0);
                MoveDynamicObject(rTurretData[turretid][e_turr_objectid], oX, oY, oZ + 0.001, rTurretData[turretid][e_turr_rotspeed], oRX, oRY, -179.0);
            }
            case TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1: {
                Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1);
                Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_2);
                MoveDynamicObject(rTurretData[turretid][e_turr_objectid], oX, oY, oZ - 0.001, rTurretData[turretid][e_turr_rotspeed], oRX, oRY, 359.0);
            }
            case TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_2: {
                Streamer_RemoveArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_2);
                Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], E_STREAMER_EXTRA_ID, TURRET_MOVE_ROTATE_COUNTERCLOCKWISE_1);
                SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], oRX, oRY, 0);
                MoveDynamicObject(rTurretData[turretid][e_turr_objectid], oX, oY, oZ + 0.001, rTurretData[turretid][e_turr_rotspeed], oRX, oRY, 179.0);
            }
            default: CallLocalFunction("OnTurretMoved", "d", turretid);
        }
        return 1;
    }
    return CallLocalFunction("rTurret_OnDynamicObjectMoved", "d", objectid);
}
#if defined _ALS_OnDynamicObjectMoved
    #undef OnDynamicObjectMoved
#else
    #define _ALS_OnDynamicObjectMoved
#endif
#define OnDynamicObjectMoved rTurret_OnDynamicObjectMoved
forward rTurret_OnDynamicObjectMoved(objectid);

public OnPlayerEnterDynamicArea(playerid, areaid)
{
    // Check if player is dead or not spawned
    if (rTurret_Killed{playerid})
        return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
        
    if (Streamer_IsInArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_BULLET_AREA_INT)) {
        // Destroy bullet and its area
        new
            data[4],
            turretid = INVALID_TURRET,
            bulletid,
            target_focusid;
            
        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);
        turretid = data[1];
        bulletid = data[2];
        target_focusid = data[3];
        
        if (!Iter_Contains(rTurrets, turretid))
            return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
            
        if (rTurretData[turretid][e_turr_owner] == playerid || (rTurretData[turretid][e_turr_team] != 255 && rTurretData[turretid][e_turr_team] == GetPlayerTeam(playerid)))
            return 1;
            
        if (rTurretData[turretid][e_turr_target_focus])
            if (target_focusid != INVALID_PLAYER_ID && target_focusid != playerid)
                // playerid is not focus target and is therefore immune to damage
                return 1;
            
        // "*** Streamer_GetArrayData: Invalid ID specified" message shows if you destroy area here.
        // To avoid this, hide object and efficiently "disable" area. Object and area will be destroyed under OnDynamicObjectMoved.
        
        // DestroyDynamicArea(areaid);
        // DestroyDynamicObject(bulletid);
        Streamer_RemoveArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_BULLET_AREA_INT);
        Streamer_SetFloatData(STREAMER_TYPE_OBJECT, bulletid, E_STREAMER_STREAM_DISTANCE, 0.0);
            
        rTurretData[turretid][e_turr_visible_bullets]--;
        TurretDamagePlayer(turretid, playerid);
        return 1;
    } else if (Streamer_IsInArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_BIG_AREA_INT)) {
        // Turret within hearing range of player
        new
            data[2],
            turretid = INVALID_TURRET;
            
        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);
        turretid = data[0];
        if (!Iter_Contains(rTurrets, turretid))
            return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
        Iter_Add(rTurretPlayersInBigArea[data[0]], playerid);
        return 1;
    } else if (Streamer_IsInArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_SHOOT_AREA_INT)) {
        // Player within target range of turret
        new
            data[2],
            turretid = INVALID_TURRET;
            
        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);
        turretid = data[0];
        if (!Iter_Contains(rTurrets, turretid))
            return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
        if (rTurretData[turretid][e_turr_owner] == playerid || (rTurretData[turretid][e_turr_team] != 255 && rTurretData[turretid][e_turr_team] == GetPlayerTeam(playerid)))
            return 1;
        Iter_Add(rTurretTargets[turretid], playerid);
        if (rTurretData[turretid][e_turr_timer] == -1 && rTurretData[turretid][e_turr_active])
            rTurretData[turretid][e_turr_timer] = SetTimerEx("Timer_TurretShoot", rTurretData[turretid][e_turr_rate], true, "d", turretid);
        return 1;
    }
    return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
}
#if defined _ALS_OnPlayerEnterDynamicArea
    #undef OnPlayerEnterDynamicArea
#else
    #define _ALS_OnPlayerEnterDynamicArea
#endif
#define OnPlayerEnterDynamicArea rTurret_OnPlayerEnterArea
forward rTurret_OnPlayerEnterArea(playerid, areaid);

public OnPlayerLeaveDynamicArea(playerid, areaid)
{
    if (!IsValidDynamicArea(areaid)) {
        return CallLocalFunction("rTurret_OnPlayerLeaveArea", "dd", playerid, areaid);
    }
    if (Streamer_IsInArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_BIG_AREA_INT)) {
        // Turret out of hearing range for player
        new
            data[2],
            turretid = INVALID_TURRET;
            
        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);
        turretid = data[0];
        if (!Iter_Contains(rTurrets, turretid))
            return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
        Iter_Remove(rTurretPlayersInBigArea[turretid], playerid);
    } else if (Streamer_IsInArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_SHOOT_AREA_INT)) {
        // Player out of turret target range
        new
            data[2],
            turretid = INVALID_TURRET;
            
        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);
        turretid = data[0];
        if (!Iter_Contains(rTurrets, turretid))
            return CallLocalFunction("rTurret_OnPlayerEnterArea", "dd", playerid, areaid);
        Iter_Remove(rTurretTargets[turretid], playerid);
        if (!Iter_Count(rTurretTargets[turretid]) && rTurretData[turretid][e_turr_checkarea]) {
            if (rTurretData[turretid][e_turr_timer] != -1) {
                KillTimer(rTurretData[turretid][e_turr_timer]);
                rTurretData[turretid][e_turr_timer] = -1;
            }
        }
    }
    return CallLocalFunction("rTurret_OnPlayerLeaveArea", "dd", playerid, areaid);
}
#if defined _ALS_OnPlayerLeaveDynamicArea
    #undef OnPlayerLeaveDynamicArea
#else
    #define _ALS_OnPlayerLeaveDynamicArea
#endif
#define OnPlayerLeaveDynamicArea rTurret_OnPlayerLeaveArea
forward rTurret_OnPlayerLeaveArea(playerid, areaid);

// ======================================================
// Internal Functions
// ======================================================

forward Timer_TurretShoot(turretid);
forward Timer_ResetTurretKilled(playerid);

public Timer_TurretShoot(turretid)
{
    if (!Iter_Contains(rTurrets, turretid) || (!Iter_Count(rTurretTargets[turretid]) && rTurretData[turretid][e_turr_checkarea])) {
        // Turret doesn't "exist" or no targets within range to shoot for aim behavior
        if (rTurretData[turretid][e_turr_timer] != -1) {
            KillTimer(rTurretData[turretid][e_turr_timer]);
            rTurretData[turretid][e_turr_timer] = -1;
        }
        return 0;
    }
        
    new
        areaid,
        bulletid,
        targetid = INVALID_PLAYER_ID,
        // Player Pos
        Float:pX,
        Float:pY,
        Float:pZ,
        // Turret Pos and Rot
        Float:turPX,
        Float:turPY,
        Float:turPZ,
        Float:turRX,
        Float:turRY,
        Float:turRZ,
        Float:newPX,
        Float:newPY,
        Float:newPZ,
        target_focusid = INVALID_PLAYER_ID,
        // LoS
        Float:colliX, Float:colliY, Float:colliZ;
    
    GetDynamicObjectPos(rTurretData[turretid][e_turr_objectid], turPX, turPY, turPZ);
    GetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, turRZ);
    newPZ = turPZ + 1.15;
    
    if (TURRET_BEHAVIOR_AIM_CLOSEST <= rTurretData[turretid][e_turr_behavior] <= TURRET_BEHAVIOR_AIM_CUSTOM) {
        // Set turret facing target
        new
            Float:targDist;
                        
        switch(rTurretData[turretid][e_turr_behavior]) {
            case TURRET_BEHAVIOR_AIM_CLOSEST: {
                targDist = 100000.0;
                foreach(new i : rTurretTargets[turretid]) {
                    if (floatcmp(targDist, GetPlayerDistanceFromPoint(i, turPX, turPY, newPZ)) == 1) {
                        // Closest player so far
                        targetid = i;
                        targDist = GetPlayerDistanceFromPoint(i, turPX, turPY, newPZ);
                        GetPlayerPos(i, pX, pY, pZ);
                    }
                }
            }
            case TURRET_BEHAVIOR_AIM_FARTHEST: {
                foreach(new i : rTurretTargets[turretid]) {
                    if (floatcmp(targDist, GetPlayerDistanceFromPoint(i, turPX, turPY, newPZ)) == -1) {
                        // Farthest player so far
                        targetid = i;
                        targDist = GetPlayerDistanceFromPoint(i, turPX, turPY, newPZ);
                        GetPlayerPos(i, pX, pY, pZ);
                    }
                }
            }
            case TURRET_BEHAVIOR_AIM_WEAKEST: {
                new
                    Float:a,
                    Float:h,
                    Float:smallSum = 1000.0;
                    
                foreach(new i : rTurretTargets[turretid]) {
                    GetPlayerArmour(i, a);
                    GetPlayerHealth(i, h);
                    if (floatcmp(smallSum, a + h) == 1) {
                        // Player with lowest health and armour so far
                        smallSum = a + h;
                        targetid = i;
                        targDist = GetPlayerDistanceFromPoint(i, turPX, turPY, newPZ);
                        GetPlayerPos(i, pX, pY, pZ);
                    }
                }
            }
            case TURRET_BEHAVIOR_AIM_STRONGEST: {
                new
                    Float:a,
                    Float:h,
                    Float:bigSum = 0.0;
                    
                foreach(new i : rTurretTargets[turretid]) {
                    GetPlayerArmour(i, a);
                    GetPlayerHealth(i, h);
                    if (floatcmp(bigSum, a + h) == -1) {
                        // Player with highest health and armour so far
                        bigSum = a + h;
                        targetid = i;
                        targDist = GetPlayerDistanceFromPoint(i, turPX, turPY, newPZ);
                        GetPlayerPos(i, pX, pY, pZ);
                    }
                }
            }
            case TURRET_BEHAVIOR_AIM_SINGLE: {
                if (IsPlayerConnected(rTurretData[turretid][e_turr_target])) {
                    targetid = rTurretData[turretid][e_turr_target];
                    targDist = GetPlayerDistanceFromPoint(targetid, turPX, turPY, newPZ);
                    GetPlayerPos(targetid, pX, pY, pZ);
                }
            }
            case TURRET_BEHAVIOR_AIM_CUSTOM: {
                if (funcidx("OnTurretCustomShoot") != -1) {
                    new
                        rtn = CallLocalFunction("OnTurretCustomShoot", "d", turretid);
                        
                    if (IsPlayerConnected(rtn)) {
                        targetid = rtn;
                        targDist = GetPlayerDistanceFromPoint(targetid, turPX, turPY, newPZ);
                        GetPlayerPos(targetid, pX, pY, pZ);
                    }
                }
            }
            case TURRET_BEHAVIOR_AIM_RANDOM: {
                if (Iter_Count(rTurretTargets[turretid]) > 0) {
                    targetid = Iter_Random(rTurretTargets[turretid]);
                    targDist = GetPlayerDistanceFromPoint(targetid, turPX, turPY, newPZ);
                    GetPlayerPos(targetid, pX, pY, pZ);
                }
            }
        }
            
        if (targetid != INVALID_PLAYER_ID) {
            // Set turret facing target
            new
                Float:vX,
                Float:vY,
                Float:vZ,
                Float:Speed;
                
            if (rTurretData[turretid][e_turr_target_focus])
                // Set turret's target focus (all other players will be immune to damage)
                target_focusid = targetid;
                
            if (rTurretData[turretid][e_turr_3dspace])
                // Turret aims in 3d space
                // Bullet's destination z value changes
                newPZ = newPZ + (rTurretData[turretid][e_turr_range] * ((pZ - newPZ) / (targDist)));

            turRZ = atan2(pY - turPY, pX - turPX);
            
            if (IsPlayerInAnyVehicle(targetid))
                GetVehicleVelocity(GetPlayerVehicleID(targetid), vX, vY, vZ);
            else
                GetPlayerVelocity(targetid, vX, vY, vZ);
                
            // player speed in units per second
            Speed = floatsqroot(((vX * vX) + (vY * vY) + (vZ * vZ))) * 81.137;
            
            if (floatcmp(Speed, 0.0) != 0) {
                if (!rTurretData[turretid][e_turr_predict]) {
                    newPX = turPX + (rTurretData[turretid][e_turr_range] * floatsin(-(turRZ + 270), degrees));
                    newPY = turPY + (rTurretData[turretid][e_turr_range] * floatcos(-(turRZ + 270), degrees));
                } else {
                    // Predictive aim algorithm (improved)
                    // Issues: Overcompensates angle adjustment while target is jumping due to short burst of high velocity
                    new
                        Float:pA;
                        
                    GetPlayerFacingAngle(targetid, pA);
                    pA = -pA + 360;
                    
                    new
                        Float:ppX = pX - turPX,
                        Float:ppY = pY - turPY;
                    
                    new
                        Float:bulletCross = ppX * (vY * 40.5685) - ppY * (vX * 40.5685),
                        Float:magR = floatsqroot(ppX * ppX + ppY * ppY),
                        Float:angleAdjust = asin(bulletCross / (rTurretData[turretid][e_turr_speed] * magR)),
                        Float:finalAngle = angleAdjust + atan2(ppY, ppX);

                    if (angleAdjust == angleAdjust) {
                        turRZ = finalAngle;
                        newPX = turPX + (rTurretData[turretid][e_turr_range] * floatsin(-(turRZ + 270), degrees));
                        newPY = turPY + (rTurretData[turretid][e_turr_range] * floatcos(-(turRZ + 270), degrees));
                    } else {
                        // NaN
                        newPX = pX;
                        newPY = pY;
                    }
                }
            } else {
                // Idle targets don't get hit by very fast bullets (speed >= 20 && rate >= 250)
                // This part won't matter anyway
                newPX = pX;
                newPY = pY;
                newPZ = pZ;
            }
            SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, turRZ);
        } else {
            // Turret doesn't change direction
            newPX = turPX + (rTurretData[turretid][e_turr_range] * floatsin(-(turRZ + 270), degrees));
            newPY = turPY + (rTurretData[turretid][e_turr_range] * floatcos(-(turRZ + 270), degrees));
        }
    } else {
        // Redundant? I don't remember
        newPX = turPX + (rTurretData[turretid][e_turr_range] * floatsin(-(turRZ + 270), degrees));
        newPY = turPY + (rTurretData[turretid][e_turr_range] * floatcos(-(turRZ + 270), degrees));
    }
    
    new
        // Bullet origin
        Float:bOriginX = turPX + (1.0 * floatsin(-(turRZ + 270), degrees)),
        Float:bOriginY = turPY + (1.0 * floatcos(-(turRZ + 270), degrees)),
        Float:bOriginZ = turPZ + 1.15;
    
    // LoS aim
    if (rTurretData[turretid][e_turr_los]) {
        // LOS detection - if collision coordinates stored, bullet path is blocked
        CA_RayCastLine(bOriginX, bOriginY, bOriginZ, newPX, newPY, newPZ, colliX, colliY, colliZ);
        if (rTurretData[turretid][e_turr_los_aim]) {
            // Don't shoot if there is no LoS
            if (floatcmp(colliX, 0.0) != 0 || floatcmp(colliY, 0.0) != 0 || floatcmp(colliZ, 0.0) != 0) {
                // Path leads to object collision, check if any targets ahead of collision
                new
                    Float:distToColli;
                    
                Streamer_GetDistanceToItem(colliX, colliY, colliZ, STREAMER_TYPE_OBJECT, rTurretData[turretid][e_turr_objectid], distToColli, 3);
                
                if (floatcmp(GetPlayerDistanceFromPoint(targetid, turPX, turPY, turPZ), distToColli) == 1) {
                    // Turret to player > Turret to obj collision
                    return 0;
                }
            }
        }
    }
    
    // Bullet creation set to go!
    new
        bool:colliPath = false,
        visibleBullets = rTurretData[turretid][e_turr_visible_bullets] + 1;
    
    if (visibleBullets >= rTurretData[turretid][e_turr_max_visible_bullets])
        // Cancel bullet creation
        return 0;
    else
        rTurretData[turretid][e_turr_visible_bullets]++;
        
    if (!rTurretData[turretid][e_turr_playsound]) {
        // Stream in bullets for players in range
        foreach(new i : rTurretPlayersInBigArea[turretid]) {
            Streamer_Update(i, 0);
        }
    } else {
        // Same as above, but also play sound
        foreach(new i : rTurretPlayersInBigArea[turretid]) {
            Streamer_Update(i, 0);
            if (IsPlayerInRangeOfPoint(i, 20.0, turPX, turPY, turPZ))
                PlayerPlaySound(i, rTurretData[turretid][e_turr_soundid], 0, 0, 0);
            else {
                GetPlayerPos(i, pX, pY, pZ);
                PlayerPlaySound(i, rTurretData[turretid][e_turr_soundid], pX, pY, pZ + 7);
            }
        }
    }
    
    if (targetid != INVALID_PLAYER_ID) {
        new
            Float:vX, Float:vY, Float:vZ,
            Float:Speed;
        
        if (IsPlayerInAnyVehicle(targetid))
            GetVehicleVelocity(GetPlayerVehicleID(targetid), vX, vY, vZ);
        else
            GetPlayerVelocity(targetid, vX, vY, vZ);
            
        // player speed in units per second
        Speed = floatsqroot(((vX * vX) + (vY * vY) + (vZ * vZ))) * 81.137;
        if (floatcmp(Speed, 0.0) == 0) {
            // Idle players don't take damage so we'll have to force damage
            TurretDamagePlayer(turretid, targetid);
            return 0;
        }
    }
        
    if (floatcmp(colliX, 0.0) == 0 && floatcmp(colliY, 0.0) == 0 && floatcmp(colliZ, 0.0) == 0) {
        // LOS detection - if collision coordinates stored, bullet path is blocked
        colliPath = false;
    } else {
        colliPath = true;
    }
        
    if ((TURRET_BEHAVIOR_AIM_CLOSEST <= rTurretData[turretid][e_turr_behavior] <= TURRET_BEHAVIOR_AIM_CUSTOM) && targetid != INVALID_PLAYER_ID)
        // Don't want the turret rotating when it's not shooting due to no LoS
        SetDynamicObjectRot(rTurretData[turretid][e_turr_objectid], turRX, turRY, turRZ);
        
    bulletid = 
        CreateDynamicObject(rTurretData[turretid][e_turr_bullet_model],
            bOriginX + rTurretData[turretid][e_turr_bullet_xoffset],
            bOriginY + rTurretData[turretid][e_turr_bullet_yoffset],
            bOriginZ + rTurretData[turretid][e_turr_bullet_zoffset],
            rTurretData[turretid][e_turr_bullet_rx],
            rTurretData[turretid][e_turr_bullet_ry],
            turRZ + rTurretData[turretid][e_turr_bullet_rz],
            rTurretData[turretid][e_turr_world],
            rTurretData[turretid][e_turr_interior],
            .streamdistance = rTurretData[turretid][e_turr_stream]);
            
    if (rTurretData[turretid][e_turr_bullet_color] != 0)
        SetDynamicObjectMaterial(bulletid, 0, -1, "none", "none", rTurretData[turretid][e_turr_bullet_color]);

    areaid = CreateDynamicSphere(0, 0, 0, rTurretData[turretid][e_turr_bullet_radius], rTurretData[turretid][e_turr_world], rTurretData[turretid][e_turr_interior]);
    // Replace when cuboids become attachable to objects
    //areaid = CreateDynamicCuboid(0 - rTurretData[turretid][e_turr_bullet_radius], 0 - rTurretData[turretid][e_turr_bullet_radius], -3.0, rTurretData[turretid][e_turr_bullet_radius], 0 - rTurretData[turretid][e_turr_bullet_radius], 3.0, rTurretData[turretid][e_turr_world], rTurretData[turretid][e_turr_interior]);
    AttachDynamicAreaToObject(areaid, bulletid);
        
    // Bullet object array data (identifier, turretid, areaid)
    Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, bulletid, E_STREAMER_EXTRA_ID, TURRET_BULLET_OBJECT_INT);
    Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, bulletid, E_STREAMER_EXTRA_ID, turretid);
    Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, bulletid, E_STREAMER_EXTRA_ID, areaid);
    Streamer_AppendArrayData(STREAMER_TYPE_OBJECT, bulletid, E_STREAMER_EXTRA_ID, (colliPath) ? 1 : 0);
    
    // Area array data (identifier, turretid, bullet objectid)
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, TURRET_BULLET_AREA_INT);
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, turretid);
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, bulletid);
    Streamer_AppendArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, target_focusid);
    
    if (rTurretData[turretid][e_turr_los]) {
        // LOS detection - if collision coordinates stored, bullet path is blocked
        if (!colliPath)
            MoveDynamicObject(bulletid, newPX, newPY, newPZ, rTurretData[turretid][e_turr_speed]);
        else
            MoveDynamicObject(bulletid, colliX, colliY, colliZ, rTurretData[turretid][e_turr_speed]);
    } else {
        MoveDynamicObject(bulletid, newPX, newPY, newPZ, rTurretData[turretid][e_turr_speed]);
    }
    return 1;
}

public Timer_ResetTurretKilled(playerid)
{
    rTurret_Killed{playerid} = false;
    rTurret_KilledBy[playerid] = INVALID_TURRET;
}

// ======================================================
// British English Macros
// ======================================================

#define TURRET_BEHAVIOUR_STATIONARY                 TURRET_BEHAVIOR_STATIONARY
#define TURRET_BEHAVIOUR_ROTATE_CLOCKWISE           TURRET_BEHAVIOR_ROTATE_CLOCKWISE
#define TURRET_BEHAVIOUR_ROTATE_COUNTERCLOCKWISE    TURRET_BEHAVIOR_ROTATE_COUNTERCLOCKWISE
#define TURRET_BEHAVIOUR_AIM_CLOSEST                TURRET_BEHAVIOR_AIM_CLOSEST
#define TURRET_BEHAVIOUR_AIM_FARTHEST               TURRET_BEHAVIOR_AIM_FARTHEST
#define TURRET_BEHAVIOUR_AIM_WEAKEST                TURRET_BEHAVIOR_AIM_WEAKEST
#define TURRET_BEHAVIOUR_AIM_STRONGEST              TURRET_BEHAVIOR_AIM_STRONGEST
#define TURRET_BEHAVIOUR_AIM_RANDOM                 TURRET_BEHAVIOR_AIM_RANDOM
#define TURRET_BEHAVIOUR_AIM_SINGLE                 TURRET_BEHAVIOR_AIM_SINGLE
#define TURRET_BEHAVIOUR_AIM_CUSTOM                 TURRET_BEHAVIOR_AIM_CUSTOM

#define GetTurretBehaviour                          GetTurretBehavior
#define SetTurretBehaviour                          SetTurretBehavior
#define GetTurretBulletColour                       GetTurretBulletColor
#define SetTurretBulletColour                       SetTurretBulletColor