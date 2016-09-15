#include "engine.h"

#include <math.h>

i32 RoundF32ToI32(f32 value)
{
	i32 Result = (i32)(value + 0.5f);
	return Result;
}

u32 RoundF32ToU32(f32 value)
{
	u32 Result = (u32)(value + 0.5f);
	return Result;
}

i32 FloorF32ToI32(f32 Value)
{
	i32 Result = (i32)floorf(Value);
	return Result;
}

void DrawRect(Buffer* buffer, f32 X, f32 Y, f32 width, f32 height, f32 R, f32 G, f32 B)
{
	i32 minX = RoundF32ToI32(X);
	i32 maxX = RoundF32ToI32(X + width);
	i32 minY = RoundF32ToI32(Y);
	i32 maxY = RoundF32ToI32(Y + height);

	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	if (maxX > buffer->width)
		maxX = buffer->width;
	if (maxY > buffer->height)
		maxY = buffer->height;

	u8 Red = u8(R * 255.0f);
	u8 Green = u8(G * 255.0f);
	u8 Blue = u8(B * 255.0f);

	u32 Color = ((Red << 16) |
		(Green << 8) |
		(Blue << 0));

	u8* Row = ((u8*)buffer->data +
		minX * buffer->bpp +
		minY * buffer->pitch);

	for (int row = minY;
		row < maxY;
		row++)
	{
		u32* Pixels = (u32*)Row;
		for (int col = minX;
			col < maxX;
			col++)
		{
			*Pixels++ = Color;
		}
		Row += buffer->pitch;
	}
}

u32 GetTileValueUnchecked(world *World, tile_map *TileMap, i32 TileX, i32 TileY)
{
	u32 Result = TileMap->Tiles[TileY * World->CountX + TileX];
	return Result;
}

b32 IsTileMapPointEmpty(world* World, tile_map* TileMap, i32 TestTileX, i32 TestTileY)
{
	b32 Result = false;

	if (TileMap)
	{
		u32 TileValue = GetTileValueUnchecked(World, TileMap, TestTileX, TestTileY);
		Result = (TileValue == 0);
	}

	return Result;
}

tile_map* GetTileMap(world* World, i32 TileMapX, i32 TileMapY)
{
	tile_map* TileMap = &World->TileMap[TileMapY * World->TileMapCountX + TileMapX];

	return TileMap;
}

canonical_position GetCanonicalPosition(world* World, raw_position Pos)
{
	canonical_position Result = {};

	Result.TileMapX = Pos.TileMapX;
	Result.TileMapY = Pos.TileMapY;

	f32 X = Pos.X - World->OffsetX;
	f32 Y = Pos.Y - World->OffsetY;

	Result.TileX = FloorF32ToI32(X / World->TileWidth);
	Result.TileY = FloorF32ToI32(Y / World->TileHeight);

	Result.RelX = X - Result.TileX * World->TileWidth;
	Result.RelY = Y - Result.TileY * World->TileHeight;

	Assert((Result.RelX >= 0));
	Assert((Result.RelY >= 0));
	Assert((Result.RelX < World->TileWidth));
	Assert((Result.RelY < World->TileHeight));

	if (Result.TileX < 0)
	{
		Result.TileX = World->CountX + Result.TileX;
		Result.TileMapX--;
	}
	if (Result.TileY < 0)
	{
		Result.TileY = World->CountY + Result.TileY;
		Result.TileMapY--;
	}
	if (Result.TileX >= World->CountX)
	{
		Result.TileX = Result.TileX - World->CountX;
		Result.TileMapX++;
	}
	if (Result.TileY >= World->CountY)
	{
		Result.TileY = Result.TileY - World->CountY;
		Result.TileMapY++;
	}

	return Result;
}

b32 IsWorldPointEmpty(world* World, raw_position TestPosition)
{
	b32 Result = false;

	// find out if we have gone to a different tile map
	canonical_position CanPos = GetCanonicalPosition(World, TestPosition);
	tile_map* TileMap = GetTileMap(World, CanPos.TileMapX, CanPos.TileMapY);
	Result = IsTileMapPointEmpty(World, TileMap, CanPos.TileX, CanPos.TileY);

	return Result;
}

void GameUpdateAndRender(f32 dt, Buffer* buffer, GameMemory* memory, GameController* input)
{
	Assert((sizeof(GameState) < memory->permanentSize));
	GameState* gameState = (GameState*)memory->permanentStorage;

	// TODO: Make Sparse Storage for world
#define TILEMAP_COUNT_X 16
#define TILEMAP_COUNT_Y 12

	u32 tile00[TILEMAP_COUNT_Y][TILEMAP_COUNT_X] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
	};

	u32 tile10[TILEMAP_COUNT_Y][TILEMAP_COUNT_X] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};

	u32 tile01[TILEMAP_COUNT_Y][TILEMAP_COUNT_X] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
	};

	u32 tile11[TILEMAP_COUNT_Y][TILEMAP_COUNT_X] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};

	tile_map TileMaps[2][2];

	TileMaps[0][0].Tiles = (u32*)tile00;
	TileMaps[1][0].Tiles = (u32*)tile10;
	TileMaps[0][1].Tiles = (u32*)tile01;
	TileMaps[1][1].Tiles = (u32*)tile11;

	world World = {};
	World.TileMapCountX = 2;
	World.TileMapCountY = 2;
	World.CountX = TILEMAP_COUNT_X;
	World.CountY = TILEMAP_COUNT_Y;
	World.TileWidth = 50;
	World.TileHeight = 50;
	World.OffsetX = 0;
	World.OffsetY = 0;
	World.TileMap = (tile_map*)TileMaps;

	f32 playerWidth = 0.75f * World.TileWidth;
	f32 playerHeight = World.TileHeight;

	if (!memory->isInitialized)
	{
		gameState->PlayerPX = 100;
		gameState->PlayerPY = 100;

		memory->isInitialized = true;
	}

	tile_map* TileMap = GetTileMap(&World, gameState->PlayerTileMapX, gameState->PlayerTileMapY);
	Assert(TileMap);

	// UPDATE
	f32 dpX = 0;
	f32 dpY = 0;
	if (input->moveRight.isDown)
	{
		dpX = 1;
	}
	else if (input->moveLeft.isDown)
	{
		dpX = -1;
	}
	if (input->moveUp.isDown)
	{
		dpY = -1;
	}
	else if (input->moveDown.isDown)
	{
		dpY = 1;
	}

	dpX *= 50;
	dpY *= 50;

	// TODO: Use Equations of Motion.
	f32 NewPlayerX = gameState->PlayerPX + dpX * dt;
	f32 NewPlayerY = gameState->PlayerPY + dpY * dt;

	raw_position PlayerP = { gameState->PlayerTileMapX, gameState->PlayerTileMapY,
		NewPlayerX, NewPlayerY };

	raw_position PlayerLeft = PlayerP;
	PlayerLeft.X -= 0.5f*playerWidth;
	raw_position PlayerRight = PlayerP;
	PlayerRight.X += 0.5f*playerWidth;

	// TODO: Make better Collision system.
	if (IsWorldPointEmpty(&World, PlayerP) &&
		IsWorldPointEmpty(&World, PlayerLeft) &&
		IsWorldPointEmpty(&World, PlayerRight))
	{
		canonical_position CanPos = GetCanonicalPosition(&World, PlayerP);
		gameState->PlayerTileMapX = CanPos.TileMapX;
		gameState->PlayerTileMapY = CanPos.TileMapY;
		gameState->PlayerPX = World.OffsetX + World.TileWidth * CanPos.TileX + CanPos.RelX;
		gameState->PlayerPY = World.OffsetY + World.TileHeight * CanPos.TileY + CanPos.RelY;
	}

	// RENDER
	DrawRect(buffer, 0, 0, (f32)buffer->width, (f32)buffer->height, 0.25f, 0.5f, 1.0f);

	for (u32 row = 0;
		row < World.CountY;
		row++)
	{
		for (u32 col = 0;
			col < World.CountX;
			col++)
		{
			u32 tileID = GetTileValueUnchecked(&World, TileMap, col, row);

			f32 Color = 0.4f;

			if (tileID > 0)
			{
				Color = 0.8f;
			}

			f32 UpperLeftX = World.OffsetX + col * World.TileWidth;
			f32 UpperLeftY = World.OffsetY + row * World.TileHeight;
			DrawRect(buffer, UpperLeftX, UpperLeftY, World.TileWidth, World.TileHeight, Color, Color, Color);
		}
	}

	// Move position to center of image
	f32 centerX = gameState->PlayerPX - (playerWidth * 0.5f);
	f32 centerY = gameState->PlayerPY - (playerHeight);
	DrawRect(buffer, centerX, centerY, playerWidth, playerHeight, 1.0f, 0.0f, 0.0f);

	DrawRect(buffer, PlayerLeft.X, PlayerLeft.Y, 10, 10, 1.0f, 1.0f, 0.0f);
	DrawRect(buffer, PlayerRight.X - 10, PlayerRight.Y, 10, 10, 1.0f, 1.0f, 0.0f);
}
