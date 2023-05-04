#pragma  once
#include "vkdefines.h"
#include "vkdevices.h"
#include "vkrenderer.h"

class BufferBuilder {

public:
	void 							copybuffer(RenderBuilder& renderer, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
	void 							allocbuffer(RenderBuilder& renderer, BufferHandler& bufhandler, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void 							createbuffer(RenderBuilder& renderer, BufferHandler& bufferhandler, VkBufferUsageFlags flags[2], VkDeviceSize buffersize, const void *datasrc);

	void 							createuniformbuffer(RenderBuilder& renderer, BufferHandler& bufferhandler, VkDeviceSize buffersize);

	void  							crearetexturebuffer(RenderBuilder& renderer, BufferHandler& bufferhandler, VkDeviceSize buffersize, void *pixels);
	uint32_t findmemorytype(VkPhysicalDevice& physicaldevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

};