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

.int iAmmo_SMOKEGRENADE;

// This is to keep track of us holding down the nade
#ifdef SSQC
.int iMode_SMOKEGRENADE;
#endif

// Weapon Info
weaponinfo_t wptSMOKEGRENADE = { 
	WEAPON_SMOKEGRENADE, 	// Identifier
	SLOT_GRENADE,		// Slot
	200, 				// Price
	EXPLOSIVE_SMOKE, 	// Caliber ID
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
	iAmmo_SMOKEGRENADE,	// Clip Pointer
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
	ANIM_SMOKEGRENADE_IDLE1,
	ANIM_SMOKEGRENADE_PULLPIN,
	ANIM_SMOKEGRENADE_THROW,
	ANIM_SMOKEGRENADE_DRAW,
};

void WeaponSMOKEGRENADE_Draw( void ) {
#ifdef SSQC
	BaseMelee_Draw();
	self.iMode_SMOKEGRENADE = GRENADE_UNREADY;
#else
	View_PlayAnimation( ANIM_SMOKEGRENADE_DRAW );
#endif
}

void WeaponSMOKEGRENADE_PrimaryFire( void ) {
#ifdef SSQC
	static void WeaponSMOKEGRENADE_Release_Ready( void ) {
		self.iMode_SMOKEGRENADE = GRENADE_READY;
	}
	
	if ( self.iMode_SMOKEGRENADE != GRENADE_UNREADY ) {
		return;
	}
	
	Client_SendEvent( self, EV_WEAPON_PRIMARYATTACK );
	Animation_ShootWeapon( self );
	self.fAttackFinished = time + wptSMOKEGRENADE.fAttackFinished;
	
	// Mark the nade as ready once the pin has been pulled
	self.iMode_SMOKEGRENADE = GRENADE_PULLING;
	self.think = WeaponSMOKEGRENADE_Release_Ready;
	self.nextthink = self.fAttackFinished;
#else
	View_PlayAnimation( ANIM_SMOKEGRENADE_PULLPIN );
#endif
}

#ifdef SSQC
void WeaponSMOKEGRENADE_Throw( void ) {
	static void WeaponSMOKEGRENADE_Die( void ) {
		remove( self );
	}
	static vector Caliber_Reflect( vector v1, vector v2 ) {
		return v1 - 2 * dotproduct( v1, v2 ) * v2;
	}
	static void WeaponSMOKEGRENADE_Explode( void ) {
		Effect_CreateSmoke( self.origin );
		
		self.solid = SOLID_NOT;
		self.movetype = MOVETYPE_NONE;
		
		self.think = WeaponSMOKEGRENADE_Die;
		self.nextthink = time + 5.0f;
	}
	static void Weapon_SMOKEGRENADE_Touch( void ) {
		if ( other.solid == SOLID_TRIGGER ) {
			return;
		}
		if ( other == self.owner ) {
			return;
		}
		if ( other.classname == "func_breakable" ) {
			Damage_Apply( other, self, 50, self.origin, FALSE );
			self.velocity = Caliber_Reflect( self.velocity, trace_plane_normal );
		}
		sound( self, CHAN_WEAPON, sprintf( "weapons/grenade_hit%d.wav", floor( random() * 3 ) + 1 ), 1, ATTN_NORM );
	}
	
	makevectors( self.v_angle );
	entity eNade = spawn();
	setorigin( eNade, ( self.origin + self.view_ofs ) + ( v_forward * 16 ) );
	setmodel( eNade, "models/w_smokegrenade.mdl" );
	setsize( eNade, '-1 -1 -1', '1 1 1' );
	
	vector vDir = aim ( self, 100000 );
	eNade.classname = "remove_me";
	eNade.owner = self;
	eNade.solid = SOLID_TRIGGER;
	eNade.angles = vectoangles( vDir );
	eNade.velocity = ( vDir * 800 );
	eNade.avelocity = ( v_forward * 600 );
	eNade.movetype = MOVETYPE_BOUNCE;
	eNade.touch = Weapon_SMOKEGRENADE_Touch;
	eNade.think = WeaponSMOKEGRENADE_Explode;
	eNade.nextthink = time + 3.0f;
	
	self.iAmmo_SMOKEGRENADE--;
	
	if ( !self.iAmmo_SMOKEGRENADE ) {
		Weapon_SwitchBest();
	} else {
		Weapon_Draw( WEAPON_SMOKEGRENADE );	
	}
}
#endif

void WeaponSMOKEGRENADE_Release( void ) {
#ifdef SSQC
	if ( self.iMode_SMOKEGRENADE == GRENADE_READY ) {
		// Throw immediately
		WeaponSMOKEGRENADE_Throw();
		self.iMode_SMOKEGRENADE = GRENADE_UNREADY;
	} else if ( self.iMode_SMOKEGRENADE == GRENADE_PULLING ) {
		// Trying to release the grenade before it's done pulling, throw asap
		self.iMode_SMOKEGRENADE = GRENADE_UNREADY;
		self.think = WeaponSMOKEGRENADE_Throw;
	}
#endif
}
