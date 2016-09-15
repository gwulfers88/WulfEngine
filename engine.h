#ifndef ENGINE_H
#define ENGINE_H

#include "platform.h"

struct Buffer
{
	void* data;
	i32 width;
	i32 height;
	i32 bpp;
	i32 pitch;
};

struct GameMemory
{
	b32 isInitialized;
	void* permanentStorage;
	void* transientStorage;
	i64 permanentSize;
	i64 transientSize;
};

struct GameButton
{
	b32 isDown;
};

struct GameController
{
	GameButton moveRight;
	GameButton moveUp;
	GameButton moveLeft;
	GameButton moveDown;

	GameButton start;
	GameButton back;
};

struct tile_map
{
	u32* Tiles;
};

struct world
{
	i32 TileMapCountX;
	i32 TileMapCountY;
	i32 OffsetX;
	i32 OffsetY;
	i32 CountX;
	i32 CountY;
	i32 TileWidth;
	i32 TileHeight;
	tile_map *TileMap;
};

struct raw_position
{
	i32 TileMapX;
	i32 TileMapY;
	f32 X;
	f32 Y;
};

struct canonical_position
{
	i32 TileMapX;
	i32 TileMapY;
	i32 TileX;
	i32 TileY;
	f32 RelX;
	f32 RelY;
};

struct GameState
{
	f32 PlayerPX;
	f32 PlayerPY;
	i32 PlayerTileMapX;
	i32 PlayerTileMapY;
};

void GameUpdateAndRender(f32 dt, Buffer* buffer, GameMemory* memory, GameController* input);

#endif
