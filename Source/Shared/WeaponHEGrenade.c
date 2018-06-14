/*
	Copyright 2016-2018 Marco "eukara" Hladik
	
	MIT LICENSE

	Permission is hereby granted, free of charge, to any person 
	obtaining a copy of this software and associated documentation 
	files (the "Software"), to deal in the Software without 
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/
.int iAmmo_HEGRENADE;

// This is to keep track of us holding down the nade
#ifdef SSQC
.int iMode_HEGRENADE;
#endif

// Weapon Info
weaponinfo_t wptHEGRENADE = { 
	WEAPON_HEGRENADE, 	// Identifier
	SLOT_GRENADE,		// Slot
	200, 				// Price
	EXPLOSIVE_HE, 		// Caliber ID
	1.0, 				// Max Player Speed
	1, 					// Bullets Per Shot
	1, 					// Clip/MagSize
	50, 				// Damage Per Bullet
	1, 					// Penetration Multiplier
	64, 				// Bullet Range
	0.75, 				// Range Modifier
	TYPE_SEMI,			// Firing Type
	1.0, 				// Attack-Delay
	1.0, 				// Reload-Delay
	iAmmo_9MM, 			// Caliber Pointer
	iAmmo_HEGRENADE,	// Clip Pointer
	1,					// Accuracy Divisor
	1.0,				// Accuracy Offset
	1.0,				// Max Inaccuracy
	7,					// Minimum Crosshair Distance
	3,					// Crosshair Movement Delta
	1.0,				// Armor penetration ratio
	ATYPE_GRENADE,		// Animation Type
	FALSE
};

// Anim Table
enum {
	ANIM_HEGRENADE_IDLE1,
	ANIM_HEGRENADE_PULLPIN,
	ANIM_HEGRENADE_THROW,
	ANIM_HEGRENADE_DRAW,
};

void WeaponHEGRENADE_Draw( void ) {
#ifdef SSQC
	BaseMelee_Draw();
	self.iMode_HEGRENADE = GRENADE_UNREADY;
#else
	View_PlayAnimation( ANIM_HEGRENADE_DRAW );
#endif
}

void WeaponHEGRENADE_PrimaryFire( void ) {
#ifdef SSQC
	static void WeaponHEGRENADE_Release_Ready( void ) {
		self.iMode_HEGRENADE = GRENADE_READY;
	}
	
	if ( self.iMode_HEGRENADE != GRENADE_UNREADY ) {
		return;
	}
	
	Client_SendEvent( self, EV_WEAPON_PRIMARYATTACK );
	Animation_ShootWeapon( self );
	self.fAttackFinished = time + wptHEGRENADE.fAttackFinished;
	
	// Mark the nade as ready once the pin has been pulled
	self.iMode_HEGRENADE = GRENADE_PULLING;
	self.think = WeaponHEGRENADE_Release_Ready;
	self.nextthink = self.fAttackFinished;
#else
	View_PlayAnimation( ANIM_HEGRENADE_PULLPIN );
#endif
}

#ifdef SSQC
void WeaponHEGRENADE_Throw( void ) {
	static void WeaponHEGRENADE_Explode( void ) {
		Effect_CreateExplosion( self.origin );
		Damage_Radius( self.origin, self, 100, 512, TRUE );
		sound( self, CHAN_WEAPON, sprintf( "weapons/explode%d.wav", floor( random() * 2 ) + 3 ), 1, ATTN_NORM );
		remove( self );
	}
	static void Weapon_HEGRENADE_Touch( void ) {
		if ( other.solid == SOLID_TRIGGER ) {
			return;
		}
		if ( other == self.owner ) {
			return;
		}
		if ( ( other.classname == "func_breakable" ) && ( other.material == MATERIAL_GLASS ) ) {
			Damage_Apply( other, self, other.health, self.origin, FALSE );
		}
		
		sound( self, CHAN_WEAPON, "weapons/he_bounce-1.wav", 1, ATTN_NORM );
	}
	
	makevectors( self.v_angle );
	entity eNade = spawn();
	setorigin( eNade, ( self.origin + self.view_ofs ) + ( v_forward * 16 ) );
	setmodel( eNade, "models/w_hegrenade.mdl" );
	setsize( eNade, '-1 -1 -1', '1 1 1' );
	
	vector vDir = aim ( self, 100000 );
	eNade.owner = self;
	eNade.classname = "remove_me";
	eNade.solid = SOLID_TRIGGER; // This is so grenades will not get slowed down by windows they touch
	eNade.angles = vectoangles( vDir );
	eNade.velocity = ( vDir * 1000 );
	eNade.avelocity = ( v_forward * 1000 );
	eNade.movetype = MOVETYPE_BOUNCE;
	eNade.touch = Weapon_HEGRENADE_Touch;
	
	eNade.think = WeaponHEGRENADE_Explode;
	eNade.nextthink = time + 3.0f;
	
	self.iAmmo_HEGRENADE--;
	
	if ( !self.iAmmo_HEGRENADE ) {
		Weapon_SwitchBest();
	} else {
		Weapon_Draw( WEAPON_HEGRENADE );
	}
}
#endif

void WeaponHEGRENADE_Release( void ) {
#ifdef SSQC
	if ( self.iMode_HEGRENADE == GRENADE_READY ) {
		// Throw immediately
		WeaponHEGRENADE_Throw();
		self.iMode_HEGRENADE = GRENADE_UNREADY;
	} else if ( self.iMode_HEGRENADE == GRENADE_PULLING ) {
		// Trying to release the grenade before it's done pulling, throw asap
		self.iMode_HEGRENADE = GRENADE_UNREADY;
		self.think = WeaponHEGRENADE_Throw;
	}
#endif
}
