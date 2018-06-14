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

#include "VGUI.h"

// Radio Commands
#define VGUIRADIO_COMMANDS 6
float fRadioCommands[ VGUIRADIO_COMMANDS ] = {
	RADIO_CT_COVERME,
	RADIO_CT_POINT,
	RADIO_POSITION,
	RADIO_REGROUP,
	RADIO_FOLLOWME,
	RADIO_FIREASSIS
};

// Group Radio Commands
#define VGUIRADIO_GROUPCOMMANDS 6
float fRadioGroupCommands[ VGUIRADIO_GROUPCOMMANDS ] = {
	RADIO_GO,
	RADIO_FALLBACK,
	RADIO_STICKTOG,
	RADIO_CT_INPOS,
	RADIO_STORMFRONT,
	RADIO_COM_REPORTIN
};

// Radio Responses
#define VGUIRADIO_RESPONSES 9
float fRadioResponses[ VGUIRADIO_RESPONSES ] = {
	RADIO_CT_AFFIRM,
	RADIO_CT_ENEMYS,
	RADIO_CT_BACKUP,
	RADIO_CLEAR,
	RADIO_CT_INPOS,
	RADIO_CT_REPORTINGIN,
	RADIO_BLOW,
	RADIO_NEGATIVE,
	RADIO_ENEMYDOWN
};

/*
====================
VGUI_Radio_DrawCommand

Prints and acts as an input check for a single command
====================
*/
void VGUI_Radio_DrawCommand( float fIndex, float fMessage, vector vPos ) {
	VGUI_Text( sprintf( "%d) %s", fIndex + 1, sRadioChat[ fMessage ] ), vPos, '12 12', FONT_CON );
	
	if ( fInputKeyCode == ( fIndex + 49 ) ) {
		sendevent( "RadioMessage", "f", fMessage );
		pSeat->fVGUI_Display = VGUI_NONE;
	}
}

/*
====================
VGUI_Radio_Draw

Main drawing routine for the radio menus
====================
*/
void VGUI_Radio_Draw( void ) {
	vector vSize, vPos;
	
	if ( pSeat->fVGUI_Display == VGUI_RADIO1 ) {
		vSize_x = 312;
		vSize_y = ( 10 * VGUIRADIO_COMMANDS ) + 64;
		vPos = vVideoMins + [ 16, vVideoResolution_y - 148 - vSize_y ];
		
		VGUI_WindowSmall( _("RADIO_TITLE1"), vPos, vSize );
		
		vPos_y += 24;
		vPos_x += 8;
		for ( float i = 0; i < VGUIRADIO_COMMANDS; i++ ) {
			vPos_y += 10;
			VGUI_Radio_DrawCommand( i, fRadioCommands[ i ], vPos );
		}
	} else if ( pSeat->fVGUI_Display == VGUI_RADIO2 ) {
		vSize_x = 312;
		vSize_y = ( 10 * VGUIRADIO_GROUPCOMMANDS ) + 64;
		vPos = vVideoMins + [ 16, vVideoResolution_y - 148 - vSize_y ];
		
		VGUI_WindowSmall( _("RADIO_TITLE2"), vPos, vSize );
		
		vPos_y += 24;
		vPos_x += 8;
		for ( float i = 0; i < VGUIRADIO_GROUPCOMMANDS; i++ ) {
			vPos_y += 10;
			VGUI_Radio_DrawCommand( i, fRadioGroupCommands[ i ], vPos );
		}
	} else {
		vSize_x = 312;
		vSize_y = ( 10 * VGUIRADIO_RESPONSES ) + 64;
		vPos = vVideoMins + [ 16, vVideoResolution_y - 148 - vSize_y ];
		
		VGUI_WindowSmall( _("RADIO_TITLE3"), vPos, vSize );
		
		vPos_y += 24;
		vPos_x += 8;
		for ( float i = 0; i < VGUIRADIO_RESPONSES; i++ ) {
			vPos_y += 10;
			VGUI_Radio_DrawCommand( i, fRadioResponses[ i ], vPos );
		}
	}
	
	vPos_y += 20;
	VGUI_Text( sprintf( "0) %s", _("VGUI_BACK") ), vPos, '12 12', FONT_CON );
	
	if ( fInputKeyCode == 48 ) {
		pSeat->fVGUI_Display = VGUI_NONE;
	}
}

/*
====================
VGUI_Radio_Toggle
====================
*/
void VGUI_Radio_Toggle( float fMenu ) {
	if ( getstatf( STAT_HEALTH ) <= 0 ) {
		return;
	}
	
	if ( pSeat->fVGUI_Display == fMenu ) {
		pSeat->fVGUI_Display = VGUI_NONE;
	} else {
		pSeat->fVGUI_Display = fMenu;
	}
}
