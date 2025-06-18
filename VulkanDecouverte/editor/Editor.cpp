#include "Editor.h"

#include "../GeometryFactory.h"
#include "../Mesh.h"
#include "../RenderObject.h"
#include "../Shader.h"
#include "../RenderPipeline.h"
#include "../Sampler.h"
#include "../Texture.h"
#include "../nodes/NodeEditor.h"

Editor::Editor(GuiHandler* guiHandler) : RenderWindow("SVE", 800, 800)
{
        
    m_mainWindowContext = guiHandler->inject(this);

    Shader sFragment("frag.spv", Shader::FRAGMENT);
    Shader sVertex("vert.spv", Shader::VERTEX);
    
    m_renderPipeline = new RenderPipeline({ &sFragment, &sVertex}, *this);
    
    m_nodeEditor = new NodeEditor(guiHandler);
    m_guiHandler = guiHandler;

    m_mesh = new Mesh(*this, GeometryFactory::GetPrimitive(Primitive::CUBE));
    m_testObject = new RenderObject(m_mesh);

    m_inspectorWindow.setInspectedObject(m_testObject);

    DS[0] = ImGui_ImplVulkan_AddTexture(m_defaultSampler->getSampler(), m_defaultTexture->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    DS[1] = ImGui_ImplVulkan_AddTexture(m_defaultSampler->getSampler(), m_defaultTexture->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
}

Editor::~Editor()
{
    delete m_mesh;
    delete m_renderPipeline;
    delete m_nodeEditor;
}

void Editor::draw()
{
    
    update();
    
    clear();

    m_guiHandler->setContext(m_mainWindowContext);
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiID dockspace_id;
    
    {

        bool p_open = true;
        
        // Variables to configure the Dockspace example.
        static bool opt_fullscreen = true; // Is the Dockspace full-screen?
        static bool opt_padding = false; // Is there padding (a blank space) between the window edge and the Dockspace?
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None; // Config flags for the Dockspace

        // In this example, we're embedding the Dockspace into an invisible parent window to make it more configurable.
        // We set ImGuiWindowFlags_NoDocking to make sure the parent isn't dockable into because this is handled by the Dockspace.
        //
        // ImGuiWindowFlags_MenuBar is to show a menu bar with config options. This isn't necessary to the functionality of a
        // Dockspace, but it is here to provide a way to change the configuration flags interactively.
        // You can remove the MenuBar flag if you don't want it in your app, but also remember to remove the code which actually
        // renders the menu bar, found at the end of this function.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        // Is the example in Fullscreen mode?
        if (opt_fullscreen)
        {
            // If so, get the main viewport:
            const ImGuiViewport* viewport = ImGui::GetMainViewport();

            // Set the parent window's position, size, and viewport to match that of the main viewport. This is so the parent window
            // completely covers the main viewport, giving it a "full-screen" feel.
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            // Set the parent window's styles to match that of the main viewport:
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // No corner rounding on the window
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // No border around the window

            // Manipulate the window flags to make it inaccessible to the user (no titlebar, resize/move, or navigation)
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            // The example is not in Fullscreen mode (the parent window can be dragged around and resized), disable the
            // ImGuiDockNodeFlags_PassthruCentralNode flag.
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so the parent window should not have its own background:
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // If the padding option is disabled, set the parent window's padding size to 0 to effectively hide said padding.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);

        // Remove the padding configuration - we pushed it, now we pop it:
        if (!opt_padding)
            ImGui::PopStyleVar();

        // Pop the two style rules set in Fullscreen mode - the corner rounding and the border size.
        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Check if Docking is enabled:
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            // If it is, draw the Dockspace with the DockSpace() function.
            // The GetID() function is to give a unique identifier to the Dockspace - here, it's "MyDockSpace".
            dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        // This is to show the menu bar that will change the config settings at runtime.
        // If you copied this demo function into your own code and removed ImGuiWindowFlags_MenuBar at the top of the function,
        // you should remove the below if-statement as well.
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                // Display a menu item for each Dockspace flag, clicking on one will toggle its assigned flag.
                if (ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

                // Display a menu item to close this example.
                if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                    if (p_open != NULL) // Remove MSVC warning C6011 (NULL dereference) - the `p_open != NULL` in MenuItem() does prevent NULL derefs, but IntelliSense doesn't analyze that deep so we need to add this in ourselves.
                        p_open = false; // Changing this variable to false will close the parent window, therefore closing the Dockspace as well.
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();

        }

        // End the parent window that contains the Dockspace:
        ImGui::End();

        ImGui::Begin("Image");
        ImGui::Image((ImTextureID)DS[0], ImGui::GetWindowSize());
        ImGui::End();
        
    }

    m_inspectorWindow.drawUI(dockspace_id);

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    
    ImGui_ImplVulkan_RenderDrawData(draw_data, getCommandBuffer());

    drawObject(*m_renderPipeline, *m_testObject);

    display();

    m_nodeEditor->draw();
    
}
