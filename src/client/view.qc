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

var bool autocvar_cg_viewmodelLag = 0;
var bool autocvar_cg_muzzleDLight = 1;

void
View_Init(void)
{
	for (int s = g_seats.length; s-- > numclientseats;) {
		pSeat = &g_seats[s];
		if(!pSeat->m_eViewModel) {
			/* right side */
			pSeat->m_eViewModel = spawn(NSRenderableEntity);
			pSeat->m_eViewModel.classname = "vm";
			pSeat->m_eViewModel.renderflags = RF_DEPTHHACK | RF_FIRSTPERSON;
			pSeat->m_eViewModel.effects |= EF_NOSHADOW;
			pSeat->m_eViewModel.alpha = 1.0f;
			pSeat->m_eViewModel.drawmask = 0;

			setsize(pSeat->m_eViewModel, [0,0,0], [0,0,0]);

			pSeat->m_eMuzzleflash = spawn();
			pSeat->m_eMuzzleflash.classname = "mflash";
			pSeat->m_eMuzzleflash.renderflags = RF_ADDITIVE | RF_FIRSTPERSON;
			pSeat->m_eMuzzleflash.effects |= EF_NOSHADOW;
			setsize(pSeat->m_eMuzzleflash, [0,0,0], [0,0,0]);

			/* left side */
			pSeat->m_eViewModelL = spawn(NSRenderableEntity);
			pSeat->m_eViewModelL.classname = "vm";
			pSeat->m_eViewModelL.renderflags = RF_DEPTHHACK | RF_FIRSTPERSON;
			pSeat->m_eViewModelL.effects |= EF_NOSHADOW;
			pSeat->m_eViewModelL.alpha = 1.0f;
			pSeat->m_eViewModelL.drawmask = 0;
			setsize(pSeat->m_eViewModelL, [0,0,0], [0,0,0]);

			pSeat->m_eMuzzleflashL = spawn();
			pSeat->m_eMuzzleflashL.classname = "mflash";
			pSeat->m_eMuzzleflashL.renderflags = RF_ADDITIVE | RF_FIRSTPERSON;
			pSeat->m_eMuzzleflashL.effects |= EF_NOSHADOW;
			setsize(pSeat->m_eMuzzleflashL, [0,0,0], [0,0,0]);
		}
	}
}

void
View_SetMuzzleflash(int index)
{
	pSeat->m_eMuzzleflash.modelindex = (float)index;
}

void
View_AddEvent(void(void) pCallback, float flTime)
{
	pSeat->m_pEventCall = pCallback;
	pSeat->m_flEventTime = flTime;
	pSeat->m_flEventFrame = pSeat->m_eViewModel.frame;
	pSeat->m_flEventMdl = pSeat->m_eViewModel.modelindex;
	NSClientPlayer pl = (NSClientPlayer)pSeat->m_ePlayer;
	pSeat->m_iEventWeapon = pl.activeweapon;
}

void
View_ClearEvents(void)
{
	pSeat->m_pEventCall = __NULL__;
	pSeat->m_flEventTime = 0;
	pSeat->m_flEventFrame = 0;
	pSeat->m_flEventMdl = 0;
	pSeat->m_iEventWeapon = 0;

	pSeat->m_eViewModelL.frame1time = 0.0f;
	pSeat->m_eViewModel.frame1time = 0.0f;
	pSeat->m_eMuzzleflash.modelindex =
	pSeat->m_eMuzzleflashL.modelindex =
	pSeat->m_eMuzzleflash.alpha =
	pSeat->m_eMuzzleflashL.alpha =
	pSeat->m_eMuzzleflash.frame1time =
	pSeat->m_eMuzzleflashL.frame1time = 0;
}

void
View_CalcViewport(int s, float fWinWidth, float fWinHeight)
{
	switch (numclientseats) {
	case 3:
		if (!s) {
	case 2:
		g_viewSeats[s].SetViewSize([fWinWidth, fWinHeight * 0.5]);
		g_viewSeats[s].SetViewPosition([0, (s & 1) * g_viewSeats[s].GetViewHeight()]);
		break;
		}
		s++;
	case 4:
		g_viewSeats[s].SetViewSize([fWinWidth, fWinHeight] * 0.5);
		g_viewSeats[s].SetViewPosition([(s&1) * g_viewSeats[s].GetViewWidth(), (s / 2i) * g_viewSeats[s].GetViewHeight()]);
		break;
	default:
		g_viewSeats[s].SetViewSize([fWinWidth, fWinHeight]);
		//g_viewSeats[s].SetViewPosition([0, 0]);
		break;
	}

	video_res = g_viewSeats[s].m_vecSize;
	video_mins = g_viewSeats[s].m_vecPosition;
	g_hudmins = g_viewSeats[s].GetHUDCanvasPos();
	g_hudres = g_viewSeats[s].GetHUDCanvasSize();
}

void
View_DrawViewmodel_Single(int weapon, float weapontime)
{
	
}

/*
====================
View_DrawViewModel

Really convoluted function that makes the gun,
muzzleflash, dynamic lights and so on appear
====================
*/
void
View_DrawViewModel(void)
{
	NSRenderableEntity m_eViewModel = (NSRenderableEntity)pSeat->m_eViewModel;
	entity m_eMuzzleflash = pSeat->m_eMuzzleflash;
	NSRenderableEntity m_eViewModelL = (NSRenderableEntity)pSeat->m_eViewModelL;
	entity m_eMuzzleflashL = pSeat->m_eMuzzleflashL;

	player pl = __NULL__;
	NSClient cl = (NSClient)pSeat->m_ePlayer;

	/* it's either us or us spectating */
	if (Client_IsSpectator(cl)) {
		NSClientSpectator spec = (NSClientSpectator)pSeat->m_ePlayer;
		pl = (player)findfloat(world, ::entnum, spec.spec_ent);

		if (spec.spec_mode != SPECMODE_FIRSTPERSON)
			return;
	} else {
		pl = (player)pSeat->m_ePlayer;
	}

	if (!pl)
		return;

	if (pl.health <= 0) {
		return;
	}

	if (pl.vehicle) {
		NSVehicle veh = (NSVehicle)pl.vehicle;

		if (veh.HideViewWeapon() == true)
			return;
	}

	if (cvar("r_drawviewmodel") == 0 || autocvar_pm_thirdPerson == TRUE) {
		return;
	}

	m_eViewModel.SetRenderMode(pl.GetRenderMode());
	m_eViewModel.SetRenderFX(pl.GetRenderFX());
	m_eViewModel.SetRenderColor(pl.GetRenderColor());
	m_eViewModel.SetRenderAmt(pl.GetRenderAmt());

	m_eViewModelL.SetRenderMode(pl.GetRenderMode());
	m_eViewModelL.SetRenderFX(pl.GetRenderFX());
	m_eViewModelL.SetRenderColor(pl.GetRenderColor());
	m_eViewModelL.SetRenderAmt(pl.GetRenderAmt());

	View_UpdateWeapon(pl, m_eViewModel, m_eMuzzleflash);
	float fBaseTime2 = m_eViewModel.frame1time;
	float fBaseTime = m_eViewModel.frame1time;
	m_eViewModelL.frame = m_eViewModel.frame = pl.weaponframe;
	m_eViewModelL.frame1time =
	m_eViewModelL.frame2time =
	m_eViewModel.frame2time =
	m_eViewModel.frame1time = pl.weapontime;
	Event_Callback(m_eViewModel.frame1time, fBaseTime2);
	processmodelevents(m_eViewModel.modelindex, m_eViewModel.frame, fBaseTime,
		m_eViewModel.frame1time, ClientGame_ModelEvent);

	makevectors(view_angles);

	if (autocvar_cg_viewmodelLag == 0)
		m_eViewModel.angles = view_angles;
	else {
		float limit;
		float speed;
		makevectors(pSeat->m_vecLag);
		pSeat->m_vecLag = v_forward;
		makevectors(view_angles);

		if (autocvar_cg_viewmodelLag == 2) {
			float pitchfix = fabs(view_angles[0] / 180);
			limit = dotproduct(pSeat->m_vecLag, v_forward) * (1.0 - pitchfix);
			speed = (1.0 - limit) * clframetime;
			speed *= 90;
		} else {
			speed = clframetime * 20;
		}

		pSeat->m_vecLag[0] = Math_Lerp(pSeat->m_vecLag[0], v_forward[0], speed);
		pSeat->m_vecLag[1] = Math_Lerp(pSeat->m_vecLag[1], v_forward[1], speed);
		pSeat->m_vecLag[2] = Math_Lerp(pSeat->m_vecLag[2], v_forward[2], speed);
		pSeat->m_vecLag = vectoangles(pSeat->m_vecLag);
		m_eViewModel.angles = pSeat->m_vecLag;
	}

	/* apply to the left side */
	m_eViewModelL.angles = m_eViewModel.angles;
	m_eViewModelL.colormap = m_eViewModel.colormap = pSeat->m_ePlayer.colormap;

	/* now apply the scale hack */
	m_eViewModelL.scale = m_eViewModel.scale = autocvar_cg_viewmodelScale;

	if (Client_IsSpectator(cl) || XR_Available(pl) == false) {
		m_eViewModelL.origin = g_view.GetCameraOrigin();
		m_eViewModel.origin = g_view.GetCameraOrigin();
		m_eViewModel.angles = g_view.GetCameraAngle();
		m_eViewModelL.angles = g_view.GetCameraAngle();

		/* we only calculate bob on the right model, to avoid double speed bobbing */
		Viewmodel_CalcBob();
		Viewmodel_ApplyBob(m_eViewModel);
		Viewmodel_ApplyBob(m_eViewModelL);
	} else {
		m_eViewModelL.origin = pl.m_xrInputLeft.GetOrigin();
		m_eViewModel.origin = pl.m_xrInputRight.GetOrigin();
		m_eViewModel.angles = pl.m_xrInputRight.GetAngles();
		m_eViewModelL.angles = pl.m_xrInputLeft.GetAngles();
	}

	/* this is currently broken */
#if 0
	// Left-handed weapons
	if (autocvar_cg_viewmodelFlip) {
		v_right *= -1;
		m_eViewModel.renderflags |= RF_USEAXIS;
		//m_eViewModel.forceshader = SHADER_CULLED;
	} else {
		if (m_eViewModel.forceshader) {
			m_eViewModel.forceshader = 0;
			m_eViewModel.renderflags &= ~RF_USEAXIS;
		}
	}
#endif

	/* only draw the model when it's 'enabled'... */
	if (m_eViewModel.alpha != 0.0f) {
		if (m_eMuzzleflash.alpha > 0.0f) {
			makevectors(getproperty(VF_ANGLES));
			m_eMuzzleflash.origin = gettaginfo(m_eViewModel, m_eMuzzleflash.skin);
			m_eMuzzleflash.angles = m_eViewModel.angles;
			m_eMuzzleflash.angles[2] += (random() * 10) - 5;

			if (autocvar_cg_muzzleDLight == true)
				dynamiclight_add(pSeat->m_vecPredictedOrigin + (v_forward * 32), 256, [1,0.45,0]);

			setorigin(m_eMuzzleflash, m_eMuzzleflash.origin);
			addentity(m_eMuzzleflash);
		}
		if (m_eMuzzleflashL.alpha > 0.0f) {
			makevectors(getproperty(VF_ANGLES));
			m_eMuzzleflashL.origin = gettaginfo(m_eViewModelL, m_eMuzzleflashL.skin);
			m_eMuzzleflashL.angles = m_eViewModelL.angles;
			m_eMuzzleflashL.angles[2] += (random() * 10) - 5;

			if (autocvar_cg_muzzleDLight == true)
				dynamiclight_add(pSeat->m_vecPredictedOrigin + (v_forward * 32), 256, [1,0.45,0]);

			setorigin(m_eMuzzleflashL, m_eMuzzleflashL.origin);
			addentity(m_eMuzzleflashL);
		}
		setorigin(m_eViewModel, m_eViewModel.origin);
		setorigin(m_eViewModelL, m_eViewModel.origin);
		m_eViewModel.RenderFXPass();
		m_eViewModelL.RenderFXPass();

		m_eViewModelL.renderflags = RF_DEPTHHACK | RF_FIRSTPERSON;
		m_eViewModel.renderflags = RF_DEPTHHACK | RF_FIRSTPERSON;

		if (m_eViewModel.GetRenderMode() != RM_DONTRENDER)
			addentity(m_eViewModel);
		if (m_eViewModelL.GetRenderMode() != RM_DONTRENDER)
			addentity(m_eViewModelL);
	}
}

void
View_PreDraw(void)
{
	ClientGame_PreDraw();
}

void
View_PostDraw(void)
{
	// Take away alpha once it has drawn fully at least once
	if (pSeat->m_eMuzzleflash.alpha > 0.0f) {
		pSeat->m_eMuzzleflash.alpha -= (clframetime * 16);
	}
	if (pSeat->m_eMuzzleflashL.alpha > 0.0f) {
		pSeat->m_eMuzzleflashL.alpha -= (clframetime * 16);
	}

	ClientGame_PostDraw();
}

/*
====================
View_PlayAnimation

Resets the timeline and plays a new sequence
onto the view model
====================
*/
void
View_PlayAnimation(int iSequence)
{
	pSeat->m_eViewModel.frame =
	pSeat->m_eViewModelL.frame = (float)iSequence;
}
void
View_PlayAnimationLeft(int iSequence)
{
	pSeat->m_eViewModelL.frame = (float)iSequence;
}
void
View_PlayAnimationRight(int iSequence)
{
	pSeat->m_eViewModel.frame = (float)iSequence;
}

int
View_GetAnimation(void)
{
	return pSeat->m_eViewModel.frame;
}

void
View_EnableViewmodel(void)
{
	pSeat->m_eViewModel.alpha =
	pSeat->m_eViewModelL.alpha = 1.0f;
}

void
View_DisableViewmodel(void)
{
	pSeat->m_eViewModel.alpha =
	pSeat->m_eViewModelL.alpha = 0.0f;
}
