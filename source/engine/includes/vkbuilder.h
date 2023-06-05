#pragma once

#include "vkdefines.h"
#include "vkdebug.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkpipeline.h"
#include "vkmesh.h"
#include "vkbuffer.h"
#include "vkdescriptors.h"
#include "vktexture.h"

struct RenderObject {
	Mesh* mesh;
	Material* material;
	//Texture* texture;
	VkDescriptorSet* textureset;
	
	glm::mat4 transformmatrix;
};

class VkBuilder {
public:
	DeletionQueue deletorhandler;

	void cleanall()				{ deletorhandler.cleanall(); };

	VkInstance&					getinstance() { return instance; };
	VkSurfaceKHR&				getsurface() {	return surface; };
	VkPhysicalDevice&  			getphysicaldevice() { return devicehandler.getphysicaldevice(); };
	VkDevice&					getdevice() { return devicehandler.getlogicaldevice(); };
	QueueBuilder&				getqueue() { return devicehandler.getqueue();};
	VkSwapchainKHR& 			getswapchain() { return devicehandler.getswapchain();};
	VkFormat&					getswapchainformat() { return devicehandler.getswapchainformat();};
	std::vector<VkImage>& 		getswapchainimage() { return devicehandler.getswapchainimage();};
	std::vector<VkImageView>& 	getswapchainimageview() { return devicehandler.getswapchainimageview();};
	VkExtent2D&	 				getswapchainextent() { return devicehandler.getswapchainextent();};
	VkRenderPass& 				getrenderpass() { return renderer.getrenderpass();};
	std::vector<VkFramebuffer>& getframebuffers() { return renderer.getframebuffers();};
	FrameArray&					getframes() { return renderer.getframedata();};
	VkPipeline&					getpipeline() { return pipeline.getpipeline();};

	std::vector<VkDescriptorSet>&  			getsamplerset() { return descriptors.getmainsamplerdescriptor(); };

	std::vector<VkDescriptorSet>& getdescriptorset() { return descriptors.getdescriptorset();};
	UboArray& getcamerabuffer() { return descriptors.getcamerabuffer();};			

	std::vector<RenderObject> 	renderqueue;
	std::vector<RenderObject>& getrenderqueue() {return renderqueue;};

	void pushrenderobject(RenderObject& object) { renderqueue.push_back(object); };

//------------------------------------------------------------------------------------------

	VkDebugUtilsMessengerEXT debugmessenger;
	VkInstance instance;
	void buildinstance();

	VkSurfaceKHR surface;
#ifdef OS_WINDOWS
	void buildwinsurface(HWND& hwnd, HINSTANCE& hinstance);
#endif
#ifdef OS_LINUX
	void buildlinuxsurface(xcb_connection_t* connection, xcb_window_t& window);
#endif

	DeviceBuilder devicehandler;
#ifdef OS_WINDOWS
	void initdevicebuilder(HWND& hwnd)	{ devicehandler.init(hwnd, instance, surface, deletorhandler);};
#endif
#ifdef OS_LINUX
	void initdevicebuilder()	{ devicehandler.init(instance, surface, deletorhandler);};
#endif
	void buildphysicaldevice()	{ devicehandler.findphysicaldevice();	};
	void buildlogicaldevice()	{ devicehandler.buildlogicaldevice();	};
	void buildswapchain() 		{ devicehandler.buildswapchain();		};
	void buildimagesview()		{ devicehandler.buildimagesview();		};

	RenderBuilder renderer;
	void initrenderbuilder()	{ renderer.init(devicehandler, deletorhandler);};
	void buildcommandpool()		{ renderer.buildcommandpool();		};
	void buildrenderpass() 		{ renderer.buildrenderpass();		};
	void builframebuffers() 	{ renderer.builframebuffers();		};
	void startsync() 			{ renderer.sync();					};

	TextureBuilder texturehandler;
	void inittexture() 			{ texturehandler.init(renderer, devicehandler, deletorhandler);};
	void loadimages() 		{ texturehandler.loadimages();	};

	DescriptorBuilder descriptors;
	void initdescriptors() 		{ descriptors.init(renderer, deletorhandler, texturehandler); };
	void builddescriptors()		{ descriptors.builddescriptors();				};

	PipelineBuilder pipeline;
	void initpipelinebuilder()	{ pipeline.init(devicehandler, renderer, descriptors, deletorhandler);};
	void buildpipeline()		{ pipeline.buildpipeline();			};

	MeshBuilder meshhandler;
	void initmeshbuilder()		{ meshhandler.init(pipeline, texturehandler, deletorhandler);		};
	void loadmeshes()			{ meshhandler.loadmeshes();			};

	Material* 	getmaterial(const std::string& name) { return pipeline.getmaterial(name);};
	Mesh* 		getmesh(const std::string& name) { return meshhandler.getmesh(name);};
	Texture* 		gettexture(const std::string& name) { return texturehandler.gettexture(name);};


	
	bool instanceextensionssupport();
	bool validationlayerssupport();
};