#include "InspectorWindow.h"

#include "../RenderObject.h"

InspectorWindow::InspectorWindow()
{
}

InspectorWindow::~InspectorWindow()
{
}

void InspectorWindow::drawUI(ImGuiID& dockspaceId)
{

    if (m_inspectedObject == nullptr) return;
    
    ImGui::Begin("Object Inspector");

    std::string position;
    ImGui::Text("Transform");
    position = "X : " + std::to_string(m_inspectedObject->getPosition().x);
    ImGui::Text(position.c_str());
    ImGui::SameLine();
    position = "Y : " + std::to_string(m_inspectedObject->getPosition().y);
    ImGui::Text(position.c_str());
    ImGui::SameLine();
    position = "Z : " + std::to_string(m_inspectedObject->getPosition().z);
    ImGui::Text(position.c_str());

    if (ImGui::Button("Move forward"))
    {
        m_inspectedObject->offsetPosition(m_inspectedObject->forward());
        m_inspectedObject->update();
    }
    ImGui::SameLine();
    if (ImGui::Button("Move right"))
    {
        m_inspectedObject->offsetPosition(m_inspectedObject->right());
        m_inspectedObject->update();
    }
    ImGui::SameLine();
    if (ImGui::Button("Move up"))
    {
        m_inspectedObject->offsetPosition(m_inspectedObject->up());
        m_inspectedObject->update();
    }
    if (ImGui::Button("Rotate"))
    {
        m_inspectedObject->rotateYPR( { 0.0f, 0.0f, 0.25f } );
        m_inspectedObject->update();
    }
    
    ImGui::End();
    
}

void InspectorWindow::setInspectedObject(RenderObject* renderObject)
{
    m_inspectedObject = renderObject;
}
