/*
 * Copyright (c) 2016-2021 Marco Hladik <marco@icculus.org>
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

void Weapons_PickupNotify(base_player pl, int w);
void Weapons_RefreshAmmo(base_player pl);
void Weapons_SwitchBest(base_player pl);
int Weapons_AddItem(base_player pl, int w, int startammo);
void Weapons_RemoveItem(base_player pl, int w);
void Weapons_InitItem(int w);
void Weapons_UpdateAmmo(base_player pl, int a1, int a2, int a3);
void Weapons_ReloadWeapon(base_player pl, .int mag, .int ammo, int max);
void Weapon_DropCurrentWeapon(base_player pl);
int Weapon_GetCount();
int Weapon_GetBitID(int);
