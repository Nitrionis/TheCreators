#pragma once

#include "ChunksRenderer.h"
#include "ChunksBuilder.h"

class ChunksController :
	public ChunksRenderer,
	public ChunksBuilder
{
public:
	ChunksController();
	~ChunksController();

	void Initialize();

	void SetVertices(vk::shared_array<uint16_t> vertices, uint32_t count, uint32_t srcOffset = 0, uint32_t dstOffset = 0);
	void SetIndices(vk::shared_array<uint16_t> indices, uint32_t count, uint32_t srcOffset = 0, uint32_t dstOffset = 0);

	void SetVerticesCount(uint32_t count);
	void SetIndicesCount(uint32_t count);

	void UpdateChunk(uint8_t chunk_x, uint8_t chunk_y);
	void UpdateOneBlock(uint8_t chunk_x, uint8_t chunk_y, uint8_t block_x, uint8_t block_y, uint8_t block_z);
};


