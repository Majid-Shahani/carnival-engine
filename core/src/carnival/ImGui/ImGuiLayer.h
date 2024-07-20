#pragma once
#include <carnival/Layer/Layer.h>

namespace Carnival {

    class CL_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;
        
        virtual void OnRender() override;

        void Begin();
        void End();
    private:
        void SetDarkThemeColors();
    };
}
