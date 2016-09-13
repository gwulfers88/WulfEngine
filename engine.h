#ifndef ENGINE_H
#define ENGINE_H

#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))
#define Assert(expression) if(!expression) {*(int*)0 = 0; }

#define Kilobytes(size) (size * 1024LL)
#define Megabytes(size) (Kilobytes(size) * 1024LL)
#define Gigabytes(size) (Megabytes(size) * 1024LL)
#define Terabytes(size) (Gigabytes(size) * 1024LL)

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

struct TileMap
{
	u32 CountX;
	u32 CountY;
	u32 Width;
	u32 Height;
	u32* Tiles;
};

struct GameState
{
	TileMap tileMap[2][2];
	f32 posX;
	f32 posY;
	u32 AbsTileX;
	u32 AbsTileY;
	u32 TileX;
	u32 TileY;
};

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

u32 GetTileValue(TileMap* tileMap, u32 tileX, u32 tileY)
{
	u32 Result = tileMap->Tiles[tileY * tileMap->CountX + tileX];
	return Result;
}

b32 IsTileValid(TileMap* tileMap, f32 newPlayerPX, f32 newPlayerPY)
{
	b32 Result = false;
	
	u32 TileX = RoundF32ToU32(newPlayerPX) / tileMap->Width;
	u32 TileY = RoundF32ToU32(newPlayerPY) / tileMap->Height;
	u32 TileID = GetTileValue(tileMap, TileX, TileY);

	Result = (TileID < 1);

	return Result;
}

u32 tile00[12][16] =
{
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
};

u32 tile10[12][16] =
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

u32 tile01[12][16] =
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

u32 tile11[12][16] =
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

void GameUpdateAndRender(f32 dt, Buffer* buffer, GameMemory* memory, GameController* input)
{
	Assert((sizeof(GameState) < memory->permanentSize));
	GameState* gameState = (GameState*)memory->permanentStorage;

	f32 playerWidth = 50.0f * 0.75f;
	f32 playerHeight = 50.0f;

	if (!memory->isInitialized)
	{
		gameState->posX = 100.0f;
		gameState->posY = 100.0f;
		
		TileMap* tileMap00 = &gameState->tileMap[0][0];

		tileMap00->CountX = 16;
		tileMap00->CountY = 12;
		tileMap00->Width = 50;
		tileMap00->Height = 50;
		tileMap00->Tiles = tile00[0];

		TileMap* tileMap10 = &gameState->tileMap[1][0];

		tileMap10 = tileMap00;
		tileMap10->Tiles = tile10[0];

		TileMap* tileMap01 = &gameState->tileMap[0][1];

		tileMap01 = tileMap00;
		tileMap01->Tiles = tile01[0];

		TileMap* tileMap11 = &gameState->tileMap[1][1];

		tileMap11 = tileMap00;
		tileMap11->Tiles = tile11[0];

		memory->isInitialized = true;
	}

	TileMap* tileMap = &gameState->tileMap[gameState->TileY][gameState->TileY];

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

	dpX *= 100;
	dpY *= 100;

	f32 newPlayerPX = gameState->posX + dpX * dt;
	f32 newPlayerPY = gameState->posY + dpY * dt;

	f32 playerLeftX = (gameState->posX - (playerWidth * 0.5f)) + dpX * dt;
	f32 playerLeftY = (gameState->posY) + dpY * dt;

	f32 playerRightX = (gameState->posX + (playerWidth * 0.5f)) + dpX * dt;
	f32 playerRightY = (gameState->posY) + dpY * dt;

	// Simple Tile Collision code
	if (IsTileValid(tileMap, newPlayerPX, newPlayerPY) &&
		IsTileValid(tileMap, playerLeftX, playerLeftY) &&
		IsTileValid(tileMap, playerRightX, playerRightY))
	{
		gameState->posX = newPlayerPX;
		gameState->posY = newPlayerPY;
	}

	// RENDER
	DrawRect(buffer, 0, 0, (f32)buffer->width, (f32)buffer->height, 0.25f, 0.5f, 1.0f);

	for (u32 row = 0;
		row < tileMap->CountY;
		row++)
	{
		for (u32 col = 0;
			col < tileMap->CountX;
			col++)
		{
			u32 tileID = GetTileValue(tileMap, col, row);

			f32 Color = 0.4f;

			if (tileID > 0)
			{
				Color = 0.8f;
			}

			DrawRect(buffer, col * tileMap->Width, row * tileMap->Height, tileMap->Width, tileMap->Height, Color, Color, Color);
		}
	}

	// Move position to center of image
	f32 centerX = gameState->posX - (playerWidth * 0.5f);
	f32 centerY = gameState->posY - (playerHeight);
	DrawRect(buffer, centerX, centerY, playerWidth, playerHeight, 1.0f, 0.0f, 0.0f);
}

#endif
