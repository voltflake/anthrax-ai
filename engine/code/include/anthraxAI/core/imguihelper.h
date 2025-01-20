#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
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
    };
   
    class Element
    {
        public:

            Element(ElementType type, const std::string& label)
            : Type(type), Label(label) { if (type == UI::TAB) { ID = IDCounter; IDCounter++;} }
          
            template<typename T, typename... Args>
            Element(ElementType type, const std::string& label, T t, Args... args, std::function<void (std::string)> func) 
            : Type(type), Label(label), Definition(func) { EvaluateArgs(t, args...); }
            
            template<typename T, typename... Args>
            Element(ElementType type, const std::string& label, T t, Args... args, std::function<void (bool)> func) 
            : Type(type), Label(label), DefinitionBool(func) { EvaluateArgs(t, args...); }

            Element(ElementType type, const std::string& label,std::function<float ()> func) 
            : Type(type), Label(label), DefinitionFloat(func) { }

            Element(ElementType type, const std::string& label, std::function<std::string ()> func) 
            : Type(type), Label(label), DefinitionString(func) { }

            int GetID() const { return ID; }
            ElementType GetType() const { return Type; }
            std::string GetLabel() const { return Label; }

            std::vector<std::string> GetComboList() { return ComboList;}
            
            std::function<void (std::string)> Definition;
            std::function<bool ()> DefinitionBoolRet;
            std::function<void (bool)> DefinitionBool;
            std::function<float ()> DefinitionFloat;
            std::function<std::string ()> DefinitionString;

            inline bool operator<(const UI::Element& elem) const { return GetID() < elem.GetID(); }
            bool GetCheckbox() const { return Checkbox; }
            void SetCheckbox(bool b) { Checkbox = b;}

            int ComboInd = 0;
            
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

            void UpdateObjectInfo();
            void Init();
            void InitUIElements();
            void Render();
            void UpdateFrame();
            
            void Add(UI::Element tab, const UI::Element& element) { UITabs[tab].emplace_back(element); }
            void Add(const std::string& scene, const UI::Window& window) { UIWindows[scene].emplace_back(window); }
            void Add(const std::string& scene, const UI::Element& element) { UIElements[scene].emplace_back(element); }
#ifdef AAI_LINUX
            void CatchEvent(xcb_generic_event_t *event) { ImGui_ImplX11_Event(event); }
#endif
        private:
            void Combo(UI::Element& element);
            void ListBox(UI::Element& element);
            void ProcessUI(UI::Element& element);
            ImGuiStyle 	EditorStyle;
            std::string EditorWindow;
            
            UI::UITabsElementsMap UITabs;
            UI::UIElementsMap UIElements;
            UI::UIWindowsMap UIWindows;
    };
}
