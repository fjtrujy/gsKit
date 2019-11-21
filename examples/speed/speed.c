//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2017 - Rick "Maximus32" Gaiser <rgaiser@gmail.com>
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// coverflow.c - Example demonstrating gsKit texture manager.
//

#include <stdio.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>
#include <ps2sdkapi.h>

#define NTSC_WIDTH 640
#define NTSC_HEIGHT 448
#define TEXTURE_WIDTH (320)
#define TEXTURE_HEIGHT (240)

#define GS_TEXT GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00) // turn white GS Screen
#define GS_BLACK GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00) // turn white GS Screen

#define FONTM_TEXTURE_COLOR GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00)
#define FONTM_TEXTURE_SCALED 0.5f
#define FONTM_TEXTURE_LEFT_MARGIN 0
#define FONTM_TEXTURE_BOTTOM_MARGIN 15
#define FONTM_TEXTURE_ZPOSITION 3

unsigned int __attribute__((aligned(16))) tex256[TEXTURE_WIDTH*TEXTURE_HEIGHT];

static void prepareTexture(void) {
	// initialize texture
	unsigned int i,j;

	for (j = 0; j < TEXTURE_HEIGHT; ++j)
	{
		for (i = 0; i < TEXTURE_WIDTH; ++i)
		{
			tex256[i + j * TEXTURE_WIDTH] = j * i;
		}
	}
}

static GSGLOBAL *init_GSGlobal(void)
{
   GSGLOBAL *gsGlobal = gsKit_init_global();

   gsGlobal->Mode = GS_MODE_NTSC;
   gsGlobal->Interlace = GS_INTERLACED;
   gsGlobal->Field = GS_FIELD;
   gsGlobal->Width = NTSC_WIDTH;
   gsGlobal->Height = NTSC_HEIGHT;

   gsGlobal->PSM = GS_PSM_CT16;
   gsGlobal->PSMZ = GS_PSMZ_16;
   gsGlobal->DoubleBuffering = GS_SETTING_OFF;
   gsGlobal->ZBuffering = GS_SETTING_OFF;
   gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;

   dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
               D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

   /* Initialize the DMAC */
   dmaKit_chan_init(DMA_CHANNEL_GIF);

   gsKit_init_screen(gsGlobal);
   gsKit_mode_switch(gsGlobal, GS_ONESHOT);
   gsKit_clear(gsGlobal, GS_BLACK);

   return gsGlobal;
}

static void set_texture(GSTEXTURE *texture, const void *frame,
      int width, int height, int PSM, int filter)
{
   texture->Width = width;
   texture->Height = height;
   texture->PSM = PSM;
   texture->Filter = filter;
   texture->Mem = (void *)frame;
}

static float curr_ms = 1.0f;
static clock_t reference = 0;

static void calculateFPS() {
	// simple frame rate counter
		clock_t old = reference;
		reference = ps2_clock();

		curr_ms =  ((float)(reference - old)) / ((float)PS2_CLOCKS_PER_SEC);
}

int main(int argc, char *argv[])
{
	int frames = 0;
	int checkEvery = 5000;
	char message[80];
	GSGLOBAL *gsGlobal = init_GSGlobal();
	GSTEXTURE *texture = calloc(1, sizeof(*texture));
	gsKit_TexManager_init(gsGlobal);

	GSFONTM *gsFontM = gsKit_init_fontm();
	gsKit_fontm_upload(gsGlobal, gsFontM);

	// Load textures
	prepareTexture();
	set_texture(texture, tex256, TEXTURE_WIDTH, TEXTURE_HEIGHT, GS_PSM_CT32, GS_FILTER_LINEAR);
	gsKit_TexManager_invalidate(gsGlobal, texture);
    gsKit_TexManager_bind(gsGlobal, texture);

	while(1)
	{
		gsKit_clear(gsGlobal, GS_BLACK);

		gsKit_prim_sprite_texture(gsGlobal, texture,
                              0, //X1
                              0,  // Y1
                              0,  // U1
                              0,  // V1
                              gsGlobal->Width, // X2
                              gsGlobal->Height, // Y2
                              texture->Width, // U2
                              texture->Height, // V2
                              0,
                              GS_TEXT);


		if (frames % checkEvery < 120) {
			gsKit_fontm_print_scaled(gsGlobal, gsFontM, 0, 0, FONTM_TEXTURE_ZPOSITION,
                                 FONTM_TEXTURE_SCALED , FONTM_TEXTURE_COLOR, message);

			gsKit_sync_flip(gsGlobal);
		} else {
			calculateFPS();
			float curr_fps = 1.0f / curr_ms;
			sprintf(message, "%d.%03d",(int)curr_fps,(int)((curr_fps-(int)curr_fps) * 1000.0f));
		}
		
		gsKit_queue_exec(gsGlobal);
   		gsKit_TexManager_nextFrame(gsGlobal);
		gsKit_TexManager_nextFrame(gsGlobal);

		frames++;
	}

	return 0;
}
