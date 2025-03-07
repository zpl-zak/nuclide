/*
 * Copyright (c) 2016-2022 Vera Visions LLC.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

enumflags
{
	SF_TRIGGER,
	SF_TOUCH,
	SF_PRESSURE
};

enum
{
	BREAKMT_GLASS,
	BREAKMT_WOOD,
	BREAKMT_METAL,
	BREAKMT_FLESH,
	BREAKMT_CINDER,
	BREAKMT_TILE,
	BREAKMT_COMPUTER,
	BREAKMT_GLASS_UNBREAKABLE,
	BREAKMT_ROCK,
	BREAKMT_NONE
};

#ifdef VALVE
const string funcbreakable_objtable[22] = {
	"",
	"spawnfunc_item_battery",
	"spawnfunc_item_healthkit",
	"weapon_9mmhandgun",
	"weapon_9mmclip",
	"weapon_9mmAR",
	"spawnfunc_ammo_9mmAR",
	"spawnfunc_ammo_ARgrenades",
	"weapon_shotgun",
	"spawnfunc_ammo_buckshot",
	"weapon_crossbow",
	"spawnfunc_ammo_crossbow",
	"weapon_357",
	"spawnfunc_ammo_357",
	"weapon_rpg",
	"spawnfunc_ammo_rpgclip",
	"spawnfunc_ammo_gaussclip",
	"weapon_hegrenade",
	"weapon_tripmine",
	"weapon_satchel",
	"weapon_snark",
	"weapon_hornetgun"
};
#else
const string funcbreakable_objtable[] = {
	""
};
#endif

const string funcbreakable_surftable[] = {
"gs_material_glass",
"gs_material_wood",
"gs_material_metal",
"gs_material_flesh",
"gs_material_cinderblock",
"gs_material_tile",
"gs_material_computer",
"gs_material_unbreakableglass",
"gs_material_rocks"
};

/*!QUAKED func_breakable (0 .5 .8) ? SF_TRIGGER SF_TOUCH SF_PRESSURE
# OVERVIEW
Brush volume that can break into lots of little pieces.

# KEYS
- "targetname" : Name
- "target" : Target when triggered.
- "killtarget" : Target to kill when triggered.
- "health" : Health of the entity.
- "material" : Material it's made of (See material-list below).
- "delay" : Delay in seconds of when it breaks under pressure.
- "explodemagnitude" : Strength of the explosion.

# SPAWNFLAGS
- SF_TRIGGER (1) : Breakable is invincible and has to be triggered in order to break.
- SF_TOUCH (2) : Break when an entity collides into it at high speed (damage is speed in units * 0.01).
- SF_PRESSURE (4) : Initiate break once someone is standing on top of it.

# NOTES
The strength of the explosion decides the radius (magnitude * 2.5) and the
maximum damage the explosion will do in the center. It has a linear fall-off.

Material-list:
	- 0 : Glass
	- 1 : Wood
	- 2 : Metal
	- 3 : Flesh
	- 4 : Cinder
	- 5 : Tile
	- 6 ; Computer
	- 7 : Glass (Unbreakable)
	- 8 : Rock
	- 9 : None

# TRIVIA
This entity was introduced in Half-Life (1998).
*/
class
func_breakable:NSSurfacePropEntity
{
	float m_flDelay;
	float m_flExplodeMag;
	float m_flExplodeRad;
	string m_strBreakSpawn;
	bool m_bCanTouch;

	/*entity m_pressAttacker;
	int m_pressType;
	int m_pressDamage;*/

public:
	void func_breakable(void);
	
	/* overrides */
	virtual void Save(float);
	virtual void Restore(string,string);
	virtual void SpawnKey(string,string);
	virtual void Spawned(void);
	virtual void Respawn(void);
	virtual void Pain(void);
	virtual void Death(void);
	virtual void Trigger(entity, triggermode_t);
	
	virtual void Explode(void);
	virtual void Touch(entity);

	/*virtual void(void) PressureDeath;*/
};

void
func_breakable::func_breakable(void)
{
	m_flDelay = 0.0f;
	m_flExplodeMag = 0.0f;
	m_flExplodeRad = 0.0f;
	m_strBreakSpawn = __NULL__;
	m_bCanTouch = false;

	/* func_breakable defaults to glass */
	if (classname == "func_breakable") {
		material = 0;
	} else {
		material = 1;
	}
}

void
func_breakable::Save(float handle)
{
	super::Save(handle);
	SaveFloat(handle, "m_flDelay", m_flDelay);
	SaveFloat(handle, "m_flExplodeMag", m_flExplodeMag);
	SaveFloat(handle, "m_flExplodeRad", m_flExplodeRad);
	SaveString(handle, "m_strBreakSpawn", m_strBreakSpawn);
	SaveBool(handle, "m_bCanTouch", m_bCanTouch);
}

void
func_breakable::Restore(string strKey, string strValue)
{
	switch (strKey) {
	case "m_flDelay":
		m_flDelay = ReadFloat(strValue);
		break;
	case "m_flExplodeMag":
		m_flExplodeMag = ReadFloat(strValue);
		break;
	case "m_flExplodeRad":
		m_flExplodeRad = ReadFloat(strValue);
		break;
	case "m_strBreakSpawn":
		m_strBreakSpawn = ReadString(strValue);
		break;
	case "m_bCanTouch":
		m_bCanTouch = ReadBool(strValue);
		break;
	default:
		super::Restore(strKey, strValue);
	}
}

void
func_breakable::SpawnKey(string strKey, string strValue)
{
	switch (strKey) {
	case "material":
		material = stof(strValue);
		break;
	case "explodemagnitude":
		m_flExplodeMag = stof(strValue);
		m_flExplodeRad = m_flExplodeMag * 2.5f;
		break;
	case "spawnobject":
		int oid = stoi(strValue);

		if (oid >= funcbreakable_objtable.length) {
			m_strBreakSpawn = "";
			print(sprintf("^1func_breakable^7:" \
			      "spawnobject %i out of bounds! fix your mod!\n", oid));
		} else {
			m_strBreakSpawn = funcbreakable_objtable[oid];
		}
		break;
	case "spawnonbreak":
		m_strBreakSpawn = strValue;
		break;
	default:
		super::SpawnKey(strKey, strValue);
		break;
	}
}

void
func_breakable::Spawned(void)
{
	/* needs to be called before super function */
	SetSurfaceData(funcbreakable_surftable[material]);
	SetPropData(funcbreakable_surftable[material]);

	super::Spawned();
}

void
func_breakable::Respawn(void)
{
	SetMovetype(MOVETYPE_NONE);
	SetSolid(SOLID_BSP);

	SetModel(GetSpawnModel());
	SetOrigin(GetSpawnOrigin());
	ClearAngles();
	ReleaseThink();
	m_bCanTouch = true;

	if (HasSpawnFlags(SF_TRIGGER)) {
		SetTakedamage(DAMAGE_NO);
	} else {
		SetTakedamage(DAMAGE_YES);
	}

	/* initially set the health to that of the ent-data */
	float spawnhealth = GetSpawnHealth();

	if (HasPropData() == true && spawnhealth <= 0) {
		/* assign propdata health */
		SetHealth(GetPropData(PROPINFO_HEALTH));
		m_flExplodeMag = GetPropData(PROPINFO_EXPLOSIVE_DMG);
		m_flExplodeRad = GetPropData(PROPINFO_EXPLOSIVE_RADIUS);
	} else {
		SetHealth(spawnhealth);
	}

	/* unassigned health isn't valid */
	if (GetHealth() <= 0)
		SetHealth(15);
}

void
func_breakable::Pain(void)
{
	string painSound;

	if (HasSpawnFlags(SF_TRIGGER)) {
		return;
	}

	painSound = GetSurfaceData(SURFDATA_SND_BULLETIMPACT);

	if (painSound)
		StartSoundDef(painSound, CHAN_VOICE, true);
}


void
func_breakable::Explode(void)
{
	vector rp = WorldSpaceCenter();
	vector vecDir = vectoangles(rp - g_dmg_vecLocation);
	string breakModel = GetPropData(PROPINFO_BREAKMODEL);

	BreakModel_Spawn(absmin, absmax, [0,0,0], m_flExplodeMag, vlen(size) / 10, breakModel);
	FX_Explosion(rp);
	Damage_Radius(rp, this, m_flExplodeMag, m_flExplodeRad, TRUE, 0);
	UseTargets(this, TRIG_TOGGLE, 0.0f); /* delay... ignored. */
	SetTakedamage(DAMAGE_NO);
	Disappear();
}

void
func_breakable::Death(void)
{
	static void break_spawnobject(void) {
		/* these might get overwritten by the entity spawnfunction */
		vector neworg = self.origin;

		/* become the classname assigned */
		NSEntity t = (NSEntity)self;
		callfunction(self.classname);

		/* apply the saved values back */
		t.origin = t.GetSpawnOrigin() = neworg;

		/* spawn anew */
		if (t.Respawn)
			t.Respawn();
	}

	if (material == BREAKMT_GLASS_UNBREAKABLE) {
		return;
	}

	eActivator = g_dmg_eAttacker;

	if (m_strBreakSpawn != "" && isfunction(m_strBreakSpawn)) {
		entity unit = spawn();
		unit.classname = m_strBreakSpawn;
		unit.think = break_spawnobject;
		unit.nextthink = time;
		unit.real_owner = this;
		setorigin(unit, WorldSpaceCenter());
	}

	/* This may seem totally absurd. That's because it is. It's very
	 * unreliable but exploding breakables in close proximity it WILL cause
	 * an OVERFLOW because we'll be busy running through thousands
	 * of entities in total when one breakable damages another in a frame. 
	 * The only way around this is to set a hard-coded limit of loops per
	 * frame and that would break functionality. */
	if (m_flExplodeMag) {
		ScheduleThink(Explode, random(0.0f, 0.5f));
	} else {
		vector vecDir = vectoangles(WorldSpaceCenter() - g_dmg_vecLocation);
		string sndBreak = GetSurfaceData(SURFDATA_SND_BREAK);
		string breakModel = GetPropData(PROPINFO_BREAKMODEL);

		if (breakModel != "")
			BreakModel_Spawn(absmin, absmax, vecDir, g_dmg_iDamage * 2.5, vlen(size) / 10, breakModel);
		else
			NSLog("func_breakable (%s) does not have a propdata break model", funcbreakable_surftable[material]);

		if (sndBreak != "")
			StartSoundDef(sndBreak, CHAN_BODY, true);
		else
			NSLog("func_breakable (%s) does not have a surfaceproperty for break", funcbreakable_surftable[material]);

		Disappear();
		SetTakedamage(DAMAGE_NO);
		UseTargets(eActivator, TRIG_TOGGLE, 0.0f);
	}
}

void
func_breakable::Trigger(entity act, triggermode_t state)
{
	if (GetHealth() > 0)
		Death();
}

void
func_breakable::Touch(entity eToucher)
{
	static void TriggerWrap(void) {
		/* TODO: 'this' should be the person who touched the ent instead */
		Trigger(this, TRIG_TOGGLE);
	}

	if (m_bCanTouch == false)
		return;

	if (eToucher.classname == classname) {
		return;
	}

	if (eToucher.solid == SOLID_CORPSE) {
		if (vlen(eToucher.velocity) > 100) {
			Trigger(this, TRIG_ON);
		}
	}

	if (HasSpawnFlags(SF_TOUCH)) {
		int fDamage = (float)(vlen(eToucher.velocity) * 0.01f);

		if (fDamage >= health) {
			m_bCanTouch = false;
			Damage_Apply(this, eToucher, fDamage, 0, DMG_CRUSH);

			if ((GetSurfaceData(SURFDATA_MATERIAL) == GSMATERIAL_GLASS) || (GetSurfaceData(SURFDATA_MATERIAL) == GSMATERIAL_COMPUTER)) {
				Damage_Apply(eToucher, this, fDamage / 4, 0, DMG_CRUSH);
			}
		}
	}

	if (HasSpawnFlags(SF_PRESSURE) && (eToucher.absmin[2] >= maxs[2] - 2)) {
		m_bCanTouch = false;

		if (m_flDelay <= 0) {
			m_flDelay = 0.1f;
		}

		ScheduleThink(TriggerWrap, m_flDelay);
	}
}
