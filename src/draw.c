/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012 Damien Radtke - www.damienradtke.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * For more information, visit http://www.gnu.org/copyleft
 *
 *                               ---
 *
 * Helper methods for drawing the parsed map.
 */

#include "draw.h"

static void _al_draw_orthogonal_tile_layer(ALLEGRO_MAP_LAYER *layer, ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!layer->visible) {
		return;
	}

	float r, g, b, a;
	al_unmap_rgba_f(tint, &r, &g, &b, &a);
	ALLEGRO_COLOR color = al_map_rgba_f(r, g, b, a * layer->opacity);

	int mx, my;
	int ystart = sy / map->tile_height, yend = (sy + sh) / map->tile_height;
	int xstart = sx / map->tile_width, xend = (sx + sw) / map->tile_width;

	// defer rendering until everything is drawn
	al_hold_bitmap_drawing(true);
	
	for (my = ystart; my <= yend; my++) {
		for (mx = xstart; mx <= xend; mx++) {
			ALLEGRO_MAP_TILE *tile = al_get_single_tile(map, layer, mx, my);
			flags = 0;
			
			if (!tile) {
				continue;
			}
			
			float x = mx*(map->tile_width) - sx + dx;
			float y = my*(map->tile_height) - sy + dy;
			
			if (flipped_vertically(layer, mx, my)) flags ^= ALLEGRO_FLIP_VERTICAL;
			if (flipped_horizontally(layer, mx, my)) flags ^= ALLEGRO_FLIP_HORIZONTAL;
			
			if (flipped_diagonally(layer, mx, my)) {
				int tile_center_h = map->tile_width		/ 2;
				int tile_center_w = map->tile_height	/ 2;
				flags ^= ALLEGRO_FLIP_VERTICAL;
				al_draw_tinted_rotated_bitmap(tile->bitmap, color, tile_center_w, tile_center_h, x + tile_center_h, y + tile_center_w, -ALLEGRO_PI/2, flags);
			} else {
				al_draw_tinted_bitmap(tile->bitmap, color, x, y, flags);
			}
		}
	}
	
	al_hold_bitmap_drawing(false);
}

static void _al_draw_orthogonal_object_layer(ALLEGRO_MAP_LAYER *layer, ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!layer->visible) {
		return;
	}

	float r, g, b, a;
	al_unmap_rgba_f(tint, &r, &g, &b, &a);
	ALLEGRO_COLOR color = al_map_rgba_f(r, g, b, a * layer->opacity);
	
	// defer rendering until everything is drawn
	al_hold_bitmap_drawing(true);
	
	GSList *objects = layer->objects;
	while (objects) {
		ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)objects->data;
		objects = g_slist_next(objects);

		// no need to draw invisible objects
		if (!object->bitmap) {
			continue;
		}

		int x = object->x - sx;
		int y = object->y - sy;

		// make sure it's on-screen; if it's not, don't draw it
		if ((x + object->width) < 0 || x > sw || y < 0 || (y - object->height) > sh) {
			continue;
		}

		al_draw_tinted_bitmap(object->bitmap, color, x, y-object->height, flags);
	}
	
	al_hold_bitmap_drawing(false);
}

static void _al_draw_orthogonal_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	GSList *layers = map->layers;
	while (layers) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		layers = g_slist_next(layers);
		if (layer->type == TILE_LAYER) {
			_al_draw_orthogonal_tile_layer(layer, map, tint, sx, sy, sw, sh, dx, dy, flags);
		} else if (layer->type == OBJECT_LAYER) {
			_al_draw_orthogonal_object_layer(layer, map, tint, sx, sy, sw, sh, dx, dy, flags);
		}
	}
}

/*
 * Draw the whole map to the target backbuffer at the given location using the given tint.
 * NOTE: the tint will not override the layer opacity property; the two alpha values are combined.
 */
void al_draw_tinted_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float dx, float dy, int flags)
{
	if (!strcmp(map->orientation, "orthogonal")) {
		_al_draw_orthogonal_map(map, tint, 0, 0, map->width * map->tile_width, map->height * map->tile_width, dx, dy, flags);
	} else {
		fprintf(stderr, "Error: can't draw map with orientation \"%s\"\n", map->orientation);
	}
}

/*
 * Draw a region of the map to the target backbuffer using the given tint.
 * NOTE: the tint will not override the layer opacity property; the two alpha values are combined.
 */
void al_draw_tinted_map_region(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!strcmp(map->orientation, "orthogonal")) {
		_al_draw_orthogonal_map(map, tint, sx, sy, sw, sh, dx, dy, flags);
	} else {
		fprintf(stderr, "Error: can't draw map with orientation \"%s\"\n", map->orientation);
	}
}

/*
 * Draw a region of the map to the target backbuffer.
 */
void al_draw_map_region(ALLEGRO_MAP *map, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	al_draw_tinted_map_region(map, al_map_rgba_f(1, 1, 1, 1), sx, sy, sw, sh, dx, dy, flags);
}

/*
 * Draw the whole map to the target backbuffer at the given location.
 */
void al_draw_map(ALLEGRO_MAP *map, float dx, float dy, int flags)
{
	al_draw_tinted_map(map, al_map_rgba_f(1, 1, 1, 1), dx, dy, flags);
}

/*
 * Draw a layer to the target backbuffer using the given tint.
 */
void al_draw_tinted_tile_layer_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float dx, float dy, int flags)
{
	_al_draw_orthogonal_tile_layer(al_get_layer_for_name(map, name), map, tint, 0, 0, map->width * map->tile_width, map->height * map->tile_height, dx, dy, flags);
}

/*
 * Draw the whole layer to the target backbuffer at the given location.
 */
void al_draw_tile_layer_for_name(ALLEGRO_MAP *map, char *name, float dx, float dy, int flags)
{
	al_draw_tinted_tile_layer_for_name(map, name, al_map_rgba_f(1, 1, 1, 1), dx, dy, flags);
}

/*
 * Draw, tinted, a region of the layer with the given name.
 */
void al_draw_tinted_tile_layer_region_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!strcmp(map->orientation, "orthogonal")) {
		_al_draw_orthogonal_tile_layer(al_get_layer_for_name(map, name), map, tint, sx, sy, sw, sh, dx, dy, flags);
	} else {
		fprintf(stderr, "Error: can't draw layer with orientation \"%s\"\n", map->orientation);
	}
}

/*
 * Draw a region of the layer with the given name.
 */
void al_draw_tile_layer_region_for_name(ALLEGRO_MAP *map, char *name, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	al_draw_tinted_tile_layer_region_for_name(map, name, al_map_rgba_f(1, 1, 1, 1), sx, sy, sw, sh, dx, dy, flags);
}
