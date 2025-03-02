#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "imgui.h"
#include <string>

#ifdef AAI_LINUX
#include <imgui_impl_x11.h>
static ImGui_ImplVulkanH_Window MainWindowData;
#endif

#include <map>
#include <functional>
#include <atomic>
namespace UI
{
    enum ElementType
    {
        TEXT,
        BUTTON,
        SEPARATOR,
        TAB,
        COMBO,
        FLOAT,
        CHECKBOX,
        LISTBOX,
        TREE,
        IMAGE,
        SLIDER,
    };
   
    class Element
    {
        public:

            Element(ElementType type, const std::string& label, bool isdyn = false)
            : Type(type), Label(label), IsDynamic(isdyn) { if (type == UI::TAB) { ID = IDCounter; IDCounter++;} }
          
            template<typename T, typename... Args>
            Element(ElementType type, const std::string& label, bool isdyn, T t, Args... args, std::function<void (std::string)> func, bool addempty = false) 
            : Type(type), Label(label), IsDynamic(isdyn), Definition(func), AddEmpty(addempty) { EvaluateArgs(t, args...); }

            template<typename T, typename... Args>
            Element(ElementType type, const std::string& label, bool isdyn, T t, Args... args, std::function<void (std::string, const UI::Element& elem)> func, bool addempty = false) 
            : Type(type), Label(label), IsDynamic(isdyn), DefinitionWithElem(func), AddEmpty(addempty) { EvaluateArgs(t, args...); }

            
            template<typename T, typename... Args>
            Element(ElementType type, const std::string& label, bool isdyn, T t, Args... args, std::function<void (bool)> func) 
            : Type(type), Label(label), IsDynamic(isdyn), DefinitionBool(func) { EvaluateArgs(t, args...); }

            Element(ElementType type, const std::string& label, bool isdyn, std::function<float (float)> func) 
            : Type(type), Label(label), IsDynamic(isdyn), DefinitionFloatArg(func) {  }

            Element(ElementType type, const std::string& label, bool isdyn,std::function<float ()> func) 
            : Type(type), Label(label), IsDynamic(isdyn), DefinitionFloat(func) { }

            Element(ElementType type, const std::string& label, bool isdyn, std::function<std::string ()> func) 
            : Type(type), Label(label), IsDynamic(isdyn), DefinitionString(func) { }

            int GetID() const { return ID; }
            ElementType GetType() const { return Type; }
            std::string GetLabel() const { return Label; }
            bool IsUIDynamic() const { return IsDynamic; }        

            std::vector<std::string> GetComboList() { return ComboList;}

            std::function<void (std::string, const UI::Element& elem)> DefinitionWithElem;
            std::function<void (std::string)> Definition;
            std::function<bool ()> DefinitionBoolRet;
            std::function<void (bool)> DefinitionBool;
            std::function<float (float)> DefinitionFloatArg;
            std::function<float ()> DefinitionFloat;
            std::function<std::string ()> DefinitionString;

            inline bool operator<(const UI::Element& elem) const { return GetID() < elem.GetID(); }
            bool GetCheckbox() const { return Checkbox; }
            void SetCheckbox(bool b) { Checkbox = b;}

            int ComboInd = 0;
            bool AddEmpty = false;

            void ClearComboList() { ComboList.clear(); }

        private:
            
            void GetArg(const std::vector<std::string>& vec);
            void GetArg(void* nu) { }
            template <typename T>
            void EvaluateArgs(T t) { GetArg(t); }
            template<typename T, typename... Args>
            void EvaluateArgs(T t, Args... args) { EvaluateArgs(args...); }

            std::vector<std::string> ComboList = {};
            inline static std::atomic_int IDCounter = 0;
            int ID = 0;
            ElementType Type;
            std::string Label;
            bool IsDynamic = false;
            bool Checkbox = true;
};
    
    class Window
    {
        public:
            Window(const std::string& name, Vector2<float> size, Vector2<float>pos, ImGuiWindowFlags flags)
            : Name(name), Size(size), Position(pos), Flags(flags) {}
           
            ImGuiWindowFlags GetFlags() const { return Flags; }
            std::string GetName() const { return Name; }
            float GetSizeX() const { return Size.x; }
            float GetSizeY() const { return Size.y; }
            float GetPosX() const { return Position.x; }
            float GetPosY() const { return Position.y; }
        private:
            Vector2<float> Size;
            Vector2<float> Position;
            ImGuiWindowFlags Flags;
            std::string Name;
    };

    typedef std::map<Element, std::vector<Element>> UITabsElementsMap;
    typedef std::map<std::string, std::vector<Element>> UIElementsMap;
    typedef std::map<std::string, std::vector<Window>> UIWindowsMap;
}


namespace Core 
{ 
    class ImGuiHelper : public Utils::Singleton<ImGuiHelper>
    {
        public:
            ~ImGuiHelper();
            
            void DisplayObjectInfo(const std::string& obj, const UI::Element& elem);
            void UpdateObjectInfo();
            void Init();
            bool IsInit() const { return Initialized; };
            void InitUIElements();
            void Render();
            void UpdateFrame();
            
            void Add(UI::Element tab, const UI::Element& element) { UITabs[tab].emplace_back(element); }
            void Add(const std::string& scene, const UI::Window& window) { UIWindows[scene].emplace_back(window); }
            void Add(const std::string& scene, const UI::Element& element) { UIElements[scene].emplace_back(element); }
#ifdef AAI_LINUX
            void CatchEvent(xcb_generic_event_t *event) { ImGui_ImplX11_Event(event); }
#endif
            bool TextureNeedsUpdate() { return TextureUpdate; }
            void ResetTextureUpdate() { TextureUpdate = false;}
            
            struct TextureForUpdate {
                int ID;
                std::string OldTextureName;
                std::string NewTextureName;
            };

            TextureForUpdate GetTextureForUpdate() { return TextureUpdateInfo; }
        private:
            Keeper::Objects* ParseObjectID(const std::string& id);
            void Image(UI::Element& element);
            void Combo(UI::Element& element);
            void Tree(UI::Element& element); 
            void ListBox(UI::Element& element);
            void ProcessUI(UI::Element& element);

            ImGuiStyle 	EditorStyle;
            std::string EditorWindow;
            std::string SelectedElement;
            UI::UITabsElementsMap UITabs;
            UI::UIElementsMap UIElements;
            UI::UIWindowsMap UIWindows;
            
            TextureForUpdate TextureUpdateInfo;
            bool TextureUpdate = false;
            bool IsDisplayInReset = false;
            bool Initialized = false;
    };
}
