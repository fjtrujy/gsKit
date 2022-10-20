//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// basic.c - Example demonstrating basic gsKit operation.
//

#include <gsKit.h>
#include <gsInline.h>
#include <dmaKit.h>
#include <malloc.h>

#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	u64 White, Black, Red, Green, Blue, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSGLOBAL *gsGlobal = gsKit_init_global();

	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	float x = 10;
	float y = 10;
	float width = 150;
	float height = 150;

	float VHeight;

	VHeight = gsGlobal->Height;

	float *LineStrip;
	float *LineStripPtr;
	float *TriStrip;
	float *TriStripPtr;
	float *TriFanPtr;
	float *TriFan;

	LineStripPtr = LineStrip = malloc(12 * sizeof(float));
	*LineStrip++ = 75;	// Segment 1 X
	*LineStrip++ = 250;	// Segment 1 Y
	*LineStrip++ = 125;	// Segment 2 X
	*LineStrip++ = 290;	// Segment 2 Y
	*LineStrip++ = 100;	// Segment 3 X
	*LineStrip++ = 350;	// Segment 3 Y
	*LineStrip++ = 50;	// Segment 4 X
	*LineStrip++ = 350;	// Segment 4 Y
	*LineStrip++ = 25;	// Segment 6 X
	*LineStrip++ = 290;	// Segment 6 X
	*LineStrip++ = 75;	// Segment 6 Y
	*LineStrip++ = 250;	// Segment 6 Y

	TriStripPtr = TriStrip = malloc(12 * sizeof(float));
	*TriStrip++ = 550;
	*TriStrip++ = 100;
	*TriStrip++ = 525;
	*TriStrip++ = 125;
	*TriStrip++ = 575;
	*TriStrip++ = 125;
	*TriStrip++ = 550;
	*TriStrip++ = 150;
	*TriStrip++ = 600;
	*TriStrip++ = 150;
	*TriStrip++ = 575;
	*TriStrip++ = 175;

	TriFanPtr = TriFan = malloc(16 * sizeof(float));
	*TriFan++ = 300;
	*TriFan++ = 100;
	*TriFan++ = 225;
	*TriFan++ = 100;
	*TriFan++ = 235;
	*TriFan++ = 75;
	*TriFan++ = 265;
	*TriFan++ = 40;
	*TriFan++ = 300;
	*TriFan++ = 25;
	*TriFan++ = 335;
	*TriFan++ = 40;
	*TriFan++ = 365;
	*TriFan++ = 75;
	*TriFan++ = 375;
	*TriFan++ = 100;

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	Red = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
	Green = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x00,0x00);
	Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsKit_init_screen(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	gsKit_clear(gsGlobal, White);

	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);

	srand(time(NULL));   // Initialization, should only be called once.
	
	GSPRIMPOINT vertices[100];



	for (int i = 0; i < 100; i++) {
		int rx = rand() % gsGlobal->Width;
		int ry = rand() % gsGlobal->Height;
		vertices[i].vertex = vertex_to_XYZ2(gsGlobal, (float)rx, (float)ry, 1);
		vertices[i].color = Black;
	}
	gsKit_prim_list_points(gsGlobal, 100, &vertices);	

	// for (int i = 0; i < 100; i++) {
	// 	int rx = rand() % gsGlobal->Width;
	// 	int ry = rand() % gsGlobal->Height;
	// 	gsKit_prim_point(gsGlobal, rx, ry, 1, Black);	
	// }

	// gsKit_prim_point(gsGlobal, 50.0f, 50.0f, 1, Black);
	// gsKit_prim_point(gsGlobal, 100.0f, 100.0f, 1, Red);
	// gsKit_prim_point(gsGlobal, 200.0f, 200.0f, 1, Blue);

	// gsKit_prim_quad(gsGlobal, 150.0f, 150.0f,
	// 			   150.0f, 400.0f,
	// 			   450.0f, 150.0f,
	// 			   450.0f, 400.0f, 2, Green);

	// gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	// gsKit_prim_triangle_fan(gsGlobal, TriFanPtr, 8, 5, Black);

	// gsKit_prim_quad_gouraud(gsGlobal, 500.0f, 250.0f,
	// 				   500.0f, 350.0f,
	// 				   600.0f, 250.0f,
	// 				   600.0f, 350.0f, 2,
	// 				   Red, Green, Blue, Black);

	// gsKit_prim_triangle_gouraud(gsGlobal, 280.0f, 200.0f,
	// 				       280.0f, 350.0f,
	// 				       180.0f, 350.0f, 5,
	// 				       Blue, Red, White);

	// // Temporarily apply a scissor box that covers only part of the red triangle
	// gsKit_set_scissor(gsGlobal, GS_SETREG_SCISSOR(300.0f, 350.0f, 200.0f, 400.0f));
	// gsKit_prim_triangle(gsGlobal, 300.0f, 200.0f, 300.0f, 350.0f, 400.0f, 350.0f, 3, Red);
	// gsKit_set_scissor(gsGlobal, GS_SCISSOR_RESET);

	// gsKit_prim_sprite(gsGlobal, 400.0f, 100.0f, 500.0f, 200.0f, 5, Red);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	while(1)
	{
		// if( y <= 10  && (x + width) < (gsGlobal->Width - 10))
		// 	x+=10;
		// else if( (y + height)  <  (VHeight - 10) && (x + width) >= (gsGlobal->Width - 10) )
		// 	y+=10;
		// else if( (y + height) >=  (VHeight - 10) && x > 10 )
		// 	x-=10;
		// else if( y > 10 && x <= 10 )
		// 	y-=10;

		// gsKit_prim_sprite(gsGlobal, x, y, x + width, y + height, 4, BlueTrans);

		// // RedTrans must be a oneshot for proper blending!
		// gsKit_prim_sprite(gsGlobal, 100.0f, 100.0f, 200.0f, 200.0f, 5, RedTrans);
		// gsKit_prim_sprite(gsGlobal, 100.0f, 200.0f, 250.0f, 250.0f, 5, GreenTrans);
		// gsKit_prim_sprite(gsGlobal, 200.0f, 250.0f, 275.0f, 275.0f, 5, WhiteTrans);

		gsKit_queue_exec(gsGlobal);

		// Flip before exec to take advantage of DMA execution double buffering.
		gsKit_sync_flip(gsGlobal);

	}

	return 0;
}
