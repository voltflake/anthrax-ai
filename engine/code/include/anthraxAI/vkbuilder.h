#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdebug.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"
#include "anthraxAI/vkmesh.h"
#include "anthraxAI/vkmodel.h"
#include "anthraxAI/vkbuffer.h"
#include "anthraxAI/vkdescriptors.h"
#include "anthraxAI/vktexture.h"
#include "anthraxAI/animator.h"

enum ClearFlags {
	CLEAR_COLOR 	= 1 << 0,	/* 0000 0001 */
	CLEAR_DEPTH 	= 1 << 1, 	/* 0000 0010 */
};

struct RenderObject {
	Model* model;
	Mesh* mesh;
	Material* material;
	VkDescriptorSet* textureset;
	VkDescriptorSet* texturesetread;
	
	Positions3 pos;
	glm::mat4 transformmatrix;

	int ID = -1;
	DataTypes type = TYPE_UNDEFINED;
	bool debugcollision;
	bool selected = false;
	bool debug = false;
	bool animated = false;
};

class VkBuilder {
public:
	DeletionQueue 				deletorhandler;

	void cleanall()				{ deletorhandler.cleanall(); };

	void procscene(ResourcesMap& resmap, Animator& animator);

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
	VkPipeline&					getreadpipeline() { return pipeline.getreadpipeline();};
	VkPipelineLayout&			getreadpipelayout() { return pipeline.getreadpipelayout();};

	std::vector<VkDescriptorSet>& getsamplerset() { return descriptors.getmainsamplerdescriptor(); };
	VkDescriptorSet&			getattachmentset() { return descriptors.getattachmentdescriptor(); };

	std::vector<VkDescriptorSet>& 			getstorageset() { return descriptors.getstorageset();};
	VkDescriptorSet 			gettranformset() { return descriptors.gettranformset(); }
	std::vector<VkDescriptorSet>& 			getdescriptorset() { return descriptors.getdescriptorset();};
	UboArray& 								getcamerabuffer() { return descriptors.getcamerabuffer();};			

	std::vector<RenderObject> 				renderqueue;
	std::vector<RenderObject>& 				getrenderqueue() {return renderqueue;};
	void 									pushrenderobject(RenderObject& object) { renderqueue.push_back(object); };

//------------------------------------------------------------------------------------------

	VkDebugUtilsMessengerEXT debugmessenger;
	VkInstance instance;
	void buildinstance();

	VkSurfaceKHR surface;
#if defined(AAI_WINDOWS)
	void buildwinsurface(HWND& hwnd, HINSTANCE& hinstance);
#endif
#ifdef AAI_LINUX
	void buildlinuxsurface(xcb_connection_t* connection, xcb_window_t& window);
#endif

	DeviceBuilder devicehandler;
#if defined(AAI_WINDOWS)
	void initdevicebuilder(VkExtent2D windowextend, HWND& hwnd)	{ devicehandler.init(windowextend, hwnd, instance, surface, &deletorhandler);};
#endif
#ifdef AAI_LINUX
	void initdevicebuilder(VkExtent2D windowextend)	{ devicehandler.init(windowextend, instance, surface, &deletorhandler);};
#endif
	
	void buildphysicaldevice()	{ devicehandler.findphysicaldevice();	};
	void buildlogicaldevice()	{ devicehandler.buildlogicaldevice();	};
	void buildswapchain() 		{ devicehandler.buildswapchain();		};
	void buildimagesview()		{ devicehandler.buildimagesview();		};
	void cleanswapchain() 		{ devicehandler.cleanswapchain(); 		};

	void resizewindow(bool& winprepared, VkExtent2D windowextend, bool check);

	RenderBuilder renderer;
	void initrenderbuilder()	{ renderer.init(&devicehandler, &deletorhandler);};
	void buildcommandpool()		{ renderer.buildcommandpool();		};
	void buildrenderpass() 		{ renderer.buildrenderpass();		};
	void builframebuffers() 	{ renderer.builframebuffers(devicehandler);	};
	void clearframebuffers() 	{ renderer.clearframebuffers();		};
	void startsync() 			{ renderer.sync();					};

	TextureBuilder texturehandler;
	void inittexture(std::unordered_map<int, Data>& resources) { texturehandler.init(renderer, devicehandler, &deletorhandler, resources);};
	void loadimages() 		{ texturehandler.loadimages();	};
	void clearimages() 		{ texturehandler.clearimages();	};
	void clearattachments() { texturehandler.clearattachments(devicehandler); }
	void builddepthbuffer() { texturehandler.createdepthbuffer(devicehandler); };
	void buildmainimage() 	{ texturehandler.createmainimage(devicehandler); };

	DescriptorBuilder descriptors;
	void initdescriptors() 		{ descriptors.init(devicehandler, renderer, &deletorhandler, texturehandler); };
	void builddescriptors()		{ descriptors.builddescriptors();				};
	void cleartextureset() 		{ descriptors.cleartextureset();};

	PipelineBuilder pipeline;
	void initpipelinebuilder()	{ pipeline.init(&devicehandler, renderer, &descriptors, &deletorhandler);};
	void buildpipeline(bool check)		{ pipeline.buildpipeline(check);			};
	void clearpipeline()		{ pipeline.clearpipeline(getdevice());			};

	MeshBuilder meshhandler;
	ModelBuilder modelhandler;
	void initmeshbuilder()		{ meshhandler.init(pipeline, texturehandler, &deletorhandler); modelhandler.init(&deletorhandler);};
	void loadmeshes()			{ meshhandler.loadmeshes();			};
	void loadmeshfromobj(const char* filename, int id)			{ meshhandler.loadmeshfromobj(filename, id);			};
	void clearmeshes()			{ meshhandler.clearmeshes();			};

	void updatemesh(Mesh* mesh, Positions size, Positions newpos) 		{ meshhandler.updatemesh(mesh, size, newpos);	};
	void updatemodel(Model* model) 		{ meshhandler.updatemodel(model);	};

	void updatemodeldescriptors() 	{ modelhandler.updatedescriptors(renderer, &descriptors); }

	void loadmodel(std::string path, int id) { modelhandler.loadmodel(path, id); }

	Model* 		getmodel(int id) 								{ return modelhandler.getmodel(id); }
	Material* 	getmaterial(const std::string& name)			{ return pipeline.getmaterial(name);};
	Mesh* 		getmesh(int id) 								{ return meshhandler.getmesh(id);};
	Texture* 	gettexture(const std::string& name) 			{ return texturehandler.gettexture(name);};

	void copycheck(uint32_t swapchainimageindex);
	bool instanceextensionssupport();
	bool validationlayerssupport();
};