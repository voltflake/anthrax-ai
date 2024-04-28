ifeq ($(OS),Windows_NT)
    uname := Windows
else
    uname := $(shell uname -s)
endif

ifeq ($(uname), Windows)
    NAME := prog.exe
    LIBS :=  -LD:/Vulkan-Game/Vulkan/1.3.239.0/Lib -lvulkan-1
endif
ifeq ($(uname), Linux)
    NAME := prog
    LIBS :=  -L./source/vulkan-linux/1.3.239.0/x86_64/lib -lvulkan -lxcb -lxcb-keysyms
endif

INCLUDES := includes/vulkan 
SRC  := vkmain.cpp window.cpp vkengine.cpp vkbuilder.cpp vkdebug.cpp vkdevices.cpp vkrenderer.cpp vkdescriptors.cpp vkpipeline.cpp vkmesh.cpp vkbuffer.cpp vktexture.cpp levelmanager.cpp vkdraw.cpp vkui.cpp processtrigger.cpp processanimation.cpp vkmove.cpp parser.cpp
SRC := $(addprefix src/, $(SRC))

IMGUI_INCLUDES := includes/imgui
SRC += includes/imgui/imgui.cpp includes/imgui/imgui_demo.cpp includes/imgui/imgui_draw.cpp includes/imgui/imgui_widgets.cpp includes/imgui/imgui_impl_vulkan.cpp includes/imgui/imgui_impl_x11.cpp includes/imgui/imgui_tables.cpp 

FLAGS := -m64 -std=c++17 -g
LINK :=  -m64 -std=c++17 

OBJS := $(SRC:.cpp=.obj)

%.obj:%.cpp 
	g++ $(FLAGS) -I$(INCLUDES) -Ilib/stb -I$(IMGUI_INCLUDES) -c -o $@ $<

all: $(NAME)
$(NAME): $(OBJS) $(OBJS_IMG)
	g++  $(LINK) -I$(INCLUDES) -I$(IMGUI_INCLUDES) -o $@ $^ $(LIBS)

clean:
ifeq ($(uname), Windows)
    del /Q /F $(OBJS) prog.exe
endif
ifeq ($(uname), Linux)
	rm -rf $(OBJS)  $(NAME)
endif

re: clean all
