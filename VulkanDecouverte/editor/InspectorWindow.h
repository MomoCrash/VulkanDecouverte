#pragma once

#include "../framework.h"

class RenderObject;

class InspectorWindow
{
public:
    InspectorWindow();
    ~InspectorWindow();

    void drawUI(ImGuiID& dockspaceId);
    void setInspectedObject(RenderObject* renderObject);

private:
    RenderObject* m_inspectedObject;
};
