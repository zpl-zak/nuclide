/*
 * Copyright (c) 2016-2019 Marco Hladik <marco@icculus.org>
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

/*QUAKED monster_alien_grunt (0 0.8 0.8) (-32 -32 0) (32 32 64)

HALF-LIFE (1998) ENTITY

Alien Grunt

*/

enum {
	AG_IDLE,
	AG_THREAT,
	AG_WALK,
	AG_RUN,
	AG_LEFT,
	AG_RIGHT,
	AG_FLINCH,
	AG_FLINCHBIG,
	AG_ATTACK,
	AG_ATTACK2,
	AG_VICTORYSQUAT,
	AG_VICTORYEAT,
	AG_VICTORYSTAND,
	AG_FLINCHARML,
	AG_FLINCHLEGL,
	AG_FLINCHARMR,
	AG_FLINCHLEGR,
	AG_SHOOTUP,
	AG_SHOOTDOWN,
	AG_SHOOT,
	AG_SHOOTQUICK,
	AG_SHOOTLONG,
	AG_DIEHS,
	AG_DIEGUT,
	AG_DIEFORWARD,
	AG_DIE,
	AG_DIEBACK,
	AG_FLOAT,
	AG_SCARE,
	AG_OPEN,
	AG_SMASHRAIL,
	AG_LAND
};

string ag_sndattack[] = {
	"agrunt/ag_attack1.wav",
	"agrunt/ag_attack2.wav",
	"agrunt/ag_attack3.wav"
};

string ag_sndidle[] = {
	"agrunt/ag_idle1.wav",
	"agrunt/ag_idle2.wav",
	"agrunt/ag_idle3.wav",
	"agrunt/ag_idle4.wav",
	"agrunt/ag_idle5.wav"
};

string ag_sndpain[] = {
	"agrunt/ag_pain1.wav",
	"agrunt/ag_pain2.wav",
	"agrunt/ag_pain3.wav",
	"agrunt/ag_pain4.wav",
	"agrunt/ag_pain5.wav"
};

string ag_sndsee[] = {
	"agrunt/ag_alert1.wav",
	"agrunt/ag_alert2.wav",
	"agrunt/ag_alert3.wav",
	"agrunt/ag_alert4.wav",
	"agrunt/ag_alert5.wav"
};

class monster_alien_grunt:CBaseMonster
{
	float m_flIdleTime;
	float m_flPainTime;

	void() monster_alien_grunt;

	virtual void(int) Pain;
	virtual void(int) Death;
	virtual void(void) IdleNoise;
	virtual void(void) Respawn;
};

void
monster_alien_grunt::Pain(int iHitBody)
{
	CBaseMonster::Pain(iHitBody);

	if (m_flPainTime > time) {
		return;
	}

	if (random() < 0.25f) {
		return;
	}

	int rand = floor(random(0,ag_sndpain.length));
	Sound(ag_sndpain[rand]);
	frame = AG_FLINCH + floor(random(0, 2));
	m_flPainTime = time + 0.25f;
}

void
monster_alien_grunt::Death(int iHitBody)
{
	/* if we're already dead (corpse) don't change animations */
	if (style != MONSTER_DEAD) {
		/* headshots == different animation */
		if (iHitBody == BODY_HEAD) {
			if (random() < 0.5) {
				frame = AG_DIEHS;
			} else {
				frame = AG_DIEFORWARD;
			}
		} else {
			frame = AG_DIE + floor(random(0, 3));
		}

		/* the sound */
		int rand = floor(random(0,ag_sndpain.length));
		Sound(ag_sndpain[rand]);
	}

	/* set the functional differences */
	CBaseMonster::Death(iHitBody);
}

void
monster_alien_grunt::IdleNoise(void)
{
	/* don't make noise if we're dead (corpse) */
	if (style == MONSTER_DEAD) {
		return;
	}

	if (m_flIdleTime > time) {
		return;
	}
	m_flIdleTime = time + random(2,10);

	int rand = floor(random(0, ag_sndidle.length));
	Sound(ag_sndidle[rand]);
}

void
monster_alien_grunt::Respawn(void)
{
	CBaseMonster::Respawn();
	frame = AG_IDLE;
}

void
monster_alien_grunt::monster_alien_grunt(void)
{
	for (int i = 0; i < ag_sndattack.length; i++) {
		precache_sound(ag_sndattack[i]);
	}
	for (int i = 0; i < ag_sndidle.length; i++) {
		precache_sound(ag_sndidle[i]);
	}
	for (int i = 0; i < ag_sndpain.length; i++) {
		precache_sound(ag_sndpain[i]);
	}
	for (int i = 0; i < ag_sndsee.length; i++) {
		precache_sound(ag_sndsee[i]);
	}

	netname = "Alien Grunt";
	model = "models/agrunt.mdl";
	base_mins = [-32,-32,0];
	base_maxs = [32,32,64];
	base_health = Skill_GetValue("agrunt_health");
	CBaseMonster::CBaseMonster();
}
