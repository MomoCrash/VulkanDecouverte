/**
* This is a minimal implementation of the Simple Sum example
 */
#pragma once

#include "../framework.h"

/* The simple sum basic node */
class SimpleSum : public ImFlow::BaseNode
{

public:
    SimpleSum()
    {
        setTitle("Simple sum");
        setStyle(ImFlow::NodeStyle::green());
        addIN<int>("In", 0, ImFlow::ConnectionFilter::SameType());
        addOUT<int>("Out", nullptr)->behaviour([this](){ return getInVal<int>("In") + m_valB; });
    }

    void draw() override
    {
        if(isSelected()) {
            ImGui::SetNextItemWidth(100.f);
            ImGui::InputInt("##ValB", &m_valB);
            ImGui::Button("Hello");
        }

        if (isHovered())
        {
            ImGui::SetNextWindowPos(ImGui::GetMousePos());
            ImGui::Begin("Infos");
            ImGui::Text("Description");
            ImGui::End();
        }
        
    }

private:
    int m_valB = 0;
};