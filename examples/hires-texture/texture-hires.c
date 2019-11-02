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

#define HIRES_MODE

#define MIN(a,b)	((a) < (b) ? (a) : (b))

void loadTexture(GSTEXTURE *tex)
{
	int color_b = 0xffff0000;
   	int color_g = 0xff00ff00;
   	int color_r = 0xff0000ff;
	int width = 320;
	int height = 240;

	tex->Width = width;
    tex->Height = height;
    tex->PSM = GS_PSM_CT32;
	tex->Filter = GS_FILTER_NEAREST;

	size_t mem_size = gsKit_texture_size_ee(tex->Width, tex->Height, tex->PSM);
	tex->Mem = memalign(128, mem_size);
   	
	int *line = tex->Mem;
	unsigned x = 0, y = 0;

   	for (y = 0; y < height; y++, line += width)
   	{
      	unsigned index_y = (y >> 4) & 1;
      	for (x = 0; x < width; x++)
      	{
         	unsigned index_x = (x >> 4) & 1;
         	line[x] = (index_y ^ index_x) ? color_r : color_g;
      	}
   	}
}

void drawTexture(GSGLOBAL *gsGlobal, GSTEXTURE *tex)
{
	u64 TexCol1 = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x80);

	gsKit_TexManager_bind(gsGlobal, tex);

	// Draw normal
	gsKit_prim_sprite_texture(gsGlobal, tex,
						0.0f,  // X1
						0.0f,  // Y2
						0.0f,  // U1
						0.0f,  // V1
						tex->Width, // X2
						tex->Height, // Y2
						tex->Width, // U2
						tex->Height, // V2
						2,
						TexCol1);
}

// void drawTexture(GSGLOBAL *gsGlobal, GSTEXTURE *texture)
// {
// 	u64 TexCol1 = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x80);
// 	float x1, y1, x2, y2;
// 	float visible_width =  texture->Width;
// 	float visible_height =  texture->Height;
// 	float width_proportion = (float)gsGlobal->Width / (float)visible_width;
// 	float height_proportion = (float)gsGlobal->Height / (float)visible_height;
// 	float delta = MIN(width_proportion, height_proportion);
// 	float newWidth = visible_width * delta;
// 	float newHeight = visible_height * delta;

// 	x1 = (gsGlobal->Width - newWidth) / 2.0f;
// 	y1 = (gsGlobal->Height - newHeight) / 2.0f;
// 	x2 = newWidth + x1;
// 	y2 = newHeight + y1;

//    gsKit_prim_sprite_texture( gsGlobal, texture,
//                               x1, //X1
//                               y1,  // Y1
//                               0,  // U1
//                               0,  // V1
//                               x2, // X2
//                               y2, // Y2
//                               texture->Width, // U2
//                               texture->Height, // V2
//                               2,
//                               TexCol1);
// }

int main(int argc, char *argv[])
{
	GSTEXTURE *texture;
	u64 Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
    int iPassCount;

#ifdef HIRES_MODE
	GSGLOBAL *gsGlobal = gsKit_hires_init_global();
#else
	GSGLOBAL *gsGlobal = gsKit_init_global();
#endif

#if 0
	gsGlobal->Mode = GS_MODE_DTV_720P;
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width = 1280;
	gsGlobal->Height = 720;
	iPassCount = 3;
#endif
#if 1
	gsGlobal->Mode = GS_MODE_DTV_1080I;
	gsGlobal->Interlace = GS_INTERLACED;
	//gsGlobal->Field = GS_FIELD;
	gsGlobal->Field = GS_FRAME;
	gsGlobal->Width  = 1920;
	gsGlobal->Height = 1080;
	iPassCount = 3;
#endif
#if 0
	gsGlobal->Mode = GS_MODE_NTSC;
	gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field = GS_FIELD;
	gsGlobal->Width = 640;
	gsGlobal->Height = 448;
#endif

	gsGlobal->PSM = GS_PSM_CT16S;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->Dithering = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->ZBuffering = GS_SETTING_ON;

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

#ifdef HIRES_MODE
	gsKit_hires_init_screen(gsGlobal, iPassCount);
#else
	gsKit_init_screen(gsGlobal);
	gsKit_TexManager_init(gsGlobal);
#endif

	// init texture
	texture = malloc(sizeof(GSTEXTURE));
	memset(texture, 0, sizeof(GSTEXTURE));

	// Load texture
	loadTexture(texture);

	// Clear static loaded textures, use texture manager instead
	gsKit_vram_clear(gsGlobal);

	gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 128), 0);
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	while(1)
	{
#ifndef HIRES_MODE
		gsKit_clear(gsGlobal, Black);
#endif

		drawTexture(gsGlobal, texture);

#ifdef HIRES_MODE
		gsKit_hires_sync(gsGlobal);
		gsKit_hires_flip(gsGlobal);
#else
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
#endif
		gsKit_TexManager_nextFrame(gsGlobal);
	}

#ifdef HIRES_MODE
	gsKit_hires_deinit_global(gsGlobal);
#else
	gsKit_deinit_global(gsGlobal);
#endif

	return 0;
}
