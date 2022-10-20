//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2022 - Daniel Santos <danielsantos346@gmail.com>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// cube.c - Example demonstrating gsKit 3D with triangle list operation.
//

#include <kernel.h>
#include <stdlib.h>
#include <malloc.h>
#include <tamtypes.h>
#include <math3d.h>

#include <gsKit.h>
#include <gsInline.h>
#include <dmaKit.h>

#include <draw.h>
#include <draw3d.h>

#include "mesh_data.c"

static const u64 BLACK_RGBAQ   = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);

VECTOR object_position = { 0.00f, 0.00f, 0.00f, 1.00f };
VECTOR object_rotation = { 0.00f, 0.00f, 0.00f, 1.00f };

VECTOR camera_position = { 0.00f, 0.00f, 100.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f,   0.00f, 1.00f };

void flipScreen(GSGLOBAL* gsGlobal)
{	
	//gsKit_set_finish(gsGlobal);
	gsKit_queue_exec(gsGlobal);
	gsKit_finish();
	gsKit_sync_flip(gsGlobal);
	gsKit_TexManager_nextFrame(gsGlobal);
}

GSGLOBAL* init_graphics()
{
	GSGLOBAL* gsGlobal = gsKit_init_global();

	gsGlobal->Mode = gsKit_check_rom();
	if (gsGlobal->Mode == GS_MODE_PAL){
		gsGlobal->Height = 512;
	} else {
		gsGlobal->Height = 448;
	}

	gsGlobal->PSM  = GS_PSM_CT16S;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->ZBuffering = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->Dithering = GS_SETTING_OFF;

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);

	gsKit_vram_clear(gsGlobal);

	gsKit_init_screen(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	return gsGlobal;

}

void gsKit_prim_triangles_gouraud_3d(GSGLOBAL *gsGlobal, int count, void* vertices)
{
	u64* p_store;
	u64* p_data;

	int bytes = count * sizeof(GSPRIMTRIANGLE);
	int qsize = (count*2) + 2;

	p_store = p_data = gsKit_heap_alloc(gsGlobal, qsize, (qsize*16), GIF_AD);

	*p_data++ = GIF_TAG_AD(qsize);
	*p_data++ = GIF_AD;


	if(p_store == gsGlobal->CurQueue->last_tag)
	{
		*p_data++ = GIF_TAG_TRIANGLE_GOURAUD(count-1);
		*p_data++ = GIF_TAG_TRIANGLE_GOURAUD_REGS;
	}

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_TRIANGLE, 1, 0, gsGlobal->PrimFogEnable,
				gsGlobal->PrimAlphaEnable, gsGlobal->PrimAAEnable,
				0, gsGlobal->PrimContext, 0);

	*p_data++ = GS_PRIM;

	memcpy(p_data, vertices, bytes);
}

int render(GSGLOBAL* gsGlobal)
{

	// Matrices to setup the 3D environment and camera
	MATRIX local_world;
	MATRIX world_view;
	MATRIX view_screen;
	MATRIX local_screen;

	VECTOR *temp_vertices;

	xyz_t   *verts;
	color_t *colors;

	GSPRIMTRIANGLE *gs_vertices = (GSPRIMTRIANGLE *)memalign(128, sizeof(GSPRIMTRIANGLE) * points_count);

	VECTOR *c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * points_count);
	VECTOR* c_colours = (VECTOR *)memalign(128, sizeof(VECTOR) * points_count);


	for (int i = 0; i < points_count; i++)
	{
		c_verts[i][0] = vertices[points[i]][0];
		c_verts[i][1] = vertices[points[i]][1];
		c_verts[i][2] = vertices[points[i]][2];
		c_verts[i][3] = vertices[points[i]][3];

		c_colours[i][0] = colours[points[i]][0];
		c_colours[i][1] = colours[points[i]][1];
		c_colours[i][2] = colours[points[i]][2];
		c_colours[i][3] = colours[points[i]][3];
	}

	// Allocate calculation space.
	temp_vertices = memalign(128, sizeof(VECTOR) * points_count);

	// Allocate register space.
	verts  = memalign(128, sizeof(vertex_t) * points_count);
	colors = memalign(128, sizeof(color_t)  * points_count);

	// Create the view_screen matrix.
	create_view_screen(view_screen, 4.0f/3.0f, -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);

	if (gsGlobal->ZBuffering == GS_SETTING_ON)
		gsKit_set_test(gsGlobal, GS_ZTEST_ON);

	gsGlobal->PrimAAEnable = GS_SETTING_ON;

	// The main loop...
	for (;;)
	{

		// Spin the cube a bit.
		object_rotation[0] += 0.008f; //while (object_rotation[0] > 3.14f) { object_rotation[0] -= 6.28f; }
		object_rotation[1] += 0.012f; //while (object_rotation[1] > 3.14f) { object_rotation[1] -= 6.28f; }

		// Create the local_world matrix.
		create_local_world(local_world, object_position, object_rotation);

		// Create the world_view matrix.
		create_world_view(world_view, camera_position, camera_rotation);

		// Create the local_screen matrix.
		create_local_screen(local_screen, local_world, world_view, view_screen);

		// Calculate the vertex values.
		calculate_vertices(temp_vertices, points_count, c_verts, local_screen);

		// Convert floating point vertices to fixed point and translate to center of screen.
		draw_convert_xyz(verts, 2048+320, 2048-224, 16, points_count, (vertex_f_t*)temp_vertices);

		// Convert floating point colours to fixed point.
		draw_convert_rgbq(colors, points_count, (vertex_f_t*)temp_vertices, (color_f_t*)c_colours, 0x80);

		for (int i = 0, j = 0; i < points_count/3 && j < points_count; i++, j+=3)
		{
			gs_vertices[i].p1.rgbaq = color_to_RGBAQ(colors[j+0].r, colors[j+0].g, colors[j+0].b, colors[j+0].a);
			gs_vertices[i].p1.xyz2 = vertex_to_XYZ2(gsGlobal, temp_vertices[j+0][0], temp_vertices[j+0][1], verts[j+0].z);

			gs_vertices[i].p2.rgbaq = color_to_RGBAQ(colors[j+1].r, colors[j+1].g, colors[j+1].b, colors[j+1].a);
			gs_vertices[i].p2.xyz2 = vertex_to_XYZ2(gsGlobal, temp_vertices[j+1][0], temp_vertices[j+1][1], verts[j+1].z);

			gs_vertices[i].p3.rgbaq = color_to_RGBAQ(colors[j+2].r, colors[j+2].g, colors[j+2].b, colors[j+2].a);
			gs_vertices[i].p3.xyz2 = vertex_to_XYZ2(gsGlobal, temp_vertices[j+2][0], temp_vertices[j+2][1], verts[j+2].z);
		}

		gsKit_clear(gsGlobal, BLACK_RGBAQ);

		gsKit_prim_list_triangle_gouraud_3d(gsGlobal, points_count, gs_vertices);

		flipScreen(gsGlobal);

	}

	return 0;

}

int main(int argc, char *argv[])
{

	// Init GS.
	GSGLOBAL* gsGlobal = init_graphics();

	// Render the cube
	render(gsGlobal);

	// Sleep
	SleepThread();

	// End program.
	return 0;

}
