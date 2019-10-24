#include <Nero/editor/EditorInterface.h>
#include <SFML/Window/Event.hpp>
#include <Nero/core/engine/EngineConstant.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <Nero/core/utility/FileUtil.h>
#include <nativefiledialog/include/nfd.h>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <Nero/editor/EditorConstant.h>
#include <functional>
#include <vector>

namespace  nero
{
    EditorInterface::EditorInterface(sf::RenderWindow& window):
        m_RenderWindow(window)
       ,m_InterfaceFirstDraw(true)
	   ,m_ResourceManager()
       ////////////////////////Project and Workspace////////////////////////
       ,m_WorksapceStatus(0)
       ,m_InputWorksapceFolder("")
       ,m_InputWorkspaceName("")
       ,m_InputWorkspaceCompany("")
       ,m_InputWorkspaceLead("")
       ,m_ProjectManagerWindowSize(800.f, 500.f)
       ,m_SelectedWorkpsapce(nullptr)
	  ,m_SelectedWorkpsapceIdex(0)
	  ,m_SelectedProjectTypeIdex(0)
	  ,m_SelectedCodeEditorIdex(0)
	   ,m_ProjectCreationStatus(0)
      ,m_AdvancedScene(new AdvancedScene())
	   ,g_Context(nullptr)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        m_ProjectManagerBannerTexture.loadFromFile("editor_project_manager.png");
        m_ProjectButtonTexture.loadFromFile("project_button.png");
        m_CompileButtonTexture.loadFromFile("compile_button.png");
        m_ReloadButtonTexture.loadFromFile("reload_button.png");
        m_EditButtonTexture.loadFromFile("edit_button.png");
        m_BlankButtonTexture.loadFromFile("blank_button.png");


        m_ProjectManager = std::make_unique<ProjectManager>();
        m_SceneManager = std::make_unique<SceneManager>();

        //old = std::cout.rdbuf(buffer.rdbuf());

		g_Context = ax::NodeEditor::CreateEditor();

		m_ResourceManager.texture.load();


    }

    EditorInterface::~EditorInterface()
    {
       //Empty
		ax::NodeEditor::DestroyEditor(g_Context);
    }

    void EditorInterface::handleEvent(const sf::Event& event)
    {
        if(event.type == sf::Event::Closed)
        {
            quitEditor();
        }

        m_AdvancedScene->handleEvent(event);

        ImGui::SFML::ProcessEvent(event);
    }

    void EditorInterface::update(const sf::Time& timeStep)
	{
        m_AdvancedScene->update(timeStep);
    }

    void EditorInterface::render()
    {

        ImGui::SFML::Update(m_RenderWindow, EngineConstant.TIME_PER_FRAME);

		//Create Interface Dockspace
        createDockSpace();

        showSceneWindow();
        showGameSettingWindow();
        showGameProjectWindow();


		showCurrentSceneWindow();


        ImGui::Begin("Engine Help");

        ImGui::End();


		showToolbarWindow();

        showLogWindow();

        showResourceCategoryWindow();

        showResourceWindow();

        ImGui::ShowDemoWindow();

        //upper left
		showUtilityWindow();
		showMusicWindow();

		showSceneLevelWindow();
        showSceneChunckWindow();
        showSceneScreenWindow();
		showSceneLayerWindow();




        interfaceFirstDraw();

		if(m_GameProject)
		{
			showBackgroundTaskWindow();

		}

		ImGui::SFML::Render(m_RenderWindow);

	}

    void EditorInterface::showSceneWindow()
    {
		ImGui::Begin(EditorConstant.SCENE.c_str());

            //if(ImGui::IsWindowFocused())
            {

                sf::Vector2f window_position    = ImGui::GetWindowPos();
                sf::Vector2f window_size        = ImGui::GetWindowSize();
                sf::Vector2f mouse_position     = ImGui::GetMousePos();
                float title_bar_height          = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
                sf::Vector2f window_padding     = ImGui::GetStyle().WindowPadding;

                AdvancedScene::RenderContext renderContext;
                renderContext.canvas_position   = sf::Vector2f(window_position.x + window_padding.x, window_position.y + title_bar_height + window_padding.y);
                renderContext.canvas_size       = sf::Vector2f(window_size.x - window_padding.x * 2, window_size.y - title_bar_height - window_padding.y * 2);
                renderContext.mouse_position    = sf::Vector2f(mouse_position.x - renderContext.canvas_position.x, mouse_position.y - renderContext.canvas_position.y);
                renderContext.focus             = ImGui::IsWindowFocused();

                sf::RenderTexture& renderTexture = m_AdvancedScene->render(renderContext);

                ImGui::Image(flipTexture(renderTexture.getTexture()));
            }

        ImGui::End();
    }

    void EditorInterface::showGameSettingWindow()
    {
		ImGui::Begin(EditorConstant.GAME_SETTING.c_str());

		ax::NodeEditor::SetCurrentEditor(g_Context);

		   ax::NodeEditor::Begin("My Editor");

		   int uniqueId = 1;

		   // Start drawing nodes.
		   ax::NodeEditor::BeginNode(uniqueId++);
			   ImGui::Text("Node A");
			   ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
				   ImGui::Text("-> In");
			   ax::NodeEditor::EndPin();
			   ImGui::SameLine();
			   ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
				   ImGui::Text("Out ->");
			   ax::NodeEditor::EndPin();
		   ax::NodeEditor::EndNode();

		   ax::NodeEditor::End();

        ImGui::End();
    }

    void EditorInterface::showGameProjectWindow()
    {
		ImGui::Begin(EditorConstant.GAME_PROJECT.c_str());

        ImGui::End();
    }

    void EditorInterface::updateFrameRate(const float& frameRate, const float& frameTime)
    {

    }

    void EditorInterface::quitEditor()
    {
        m_RenderWindow.close();
    }

    void EditorInterface::createDockSpace()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 viewport_size = viewport->Size;
        viewport_size.y -= 20.f;
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);



        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        if(!setup_dock)
        {
            dockspace_id = ImGui::GetID("MyDockSpace");

        }

        ImGui::DockSpace(dockspace_id, viewport_size, dockspace_flags);



        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("action_1", nullptr, nullptr);
                ImGui::MenuItem("action_2", nullptr, nullptr);

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Views"))
            {
                ImGui::MenuItem("action_3", nullptr, nullptr);
                ImGui::MenuItem("action_4", nullptr, nullptr);

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Download"))
            {
                ImGui::MenuItem("action_3", nullptr, nullptr);
                ImGui::MenuItem("action_4", nullptr, nullptr);

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Helps"))
            {
                ImGui::MenuItem("action_3", nullptr, nullptr);
                ImGui::MenuItem("action_4", nullptr, nullptr);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();

        if(!setup_dock && !fileExist(getPath({"imgui"}, ".ini")))
        {
            ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.

            //split main dock in five
            //ImGuiID dock_id_bottom        = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, nullptr, &dock_main_id);
            dock_id_upper_left          = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);

            auto left_node = ImGui::DockBuilderGetNode(dock_id_upper_left);
            left_node->SizeRef.x = 180.f;

			dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_upper_left, ImGuiDir_Down, 0.20f, nullptr, &dock_id_upper_left);
            dock_id_right               = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);
            actionBarId                 = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.20f, nullptr, &dock_main_id);
            ImGuiID dock_id_down        = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow(EditorConstant.SCENE.c_str(), ImGui::DockBuilderGetCentralNode(dockspace_id)->ID);
			ImGui::DockBuilderDockWindow(EditorConstant.GAME_SETTING.c_str(), ImGui::DockBuilderGetCentralNode(dockspace_id)->ID);
			ImGui::DockBuilderDockWindow(EditorConstant.GAME_PROJECT.c_str(), ImGui::DockBuilderGetCentralNode(dockspace_id)->ID);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", ImGui::DockBuilderGetCentralNode(dockspace_id)->ID);
			//upper left
			ImGui::DockBuilderDockWindow(EditorConstant.UTILITY.c_str(),      dock_id_upper_left);
			ImGui::DockBuilderDockWindow(EditorConstant.Music.c_str(), dock_id_upper_left);

			ImGui::DockBuilderDockWindow(EditorConstant.SCENE_LEVEL.c_str(), dock_id_left_bottom);
			ImGui::DockBuilderDockWindow(EditorConstant.SCENE_CHUNCK.c_str(), dock_id_left_bottom);
			ImGui::DockBuilderDockWindow(EditorConstant.SCENE_SCREEN.c_str(), dock_id_left_bottom);
			ImGui::DockBuilderDockWindow(EditorConstant.SCENE_LAYER.c_str(), dock_id_left_bottom);

			ImGui::DockBuilderDockWindow(EditorConstant.SCENE_EXPLORER.c_str(), dock_id_right);
            ImGui::DockBuilderDockWindow("Engine Help", dock_id_right);
            ImGui::DockBuilderDockWindow("Resource Browser", dock_id_right);
			ImGui::DockBuilderDockWindow(EditorConstant.Toolbar.c_str(), actionBarId);
            ImGui::DockBuilderDockWindow("Logging", dock_id_down);
            ImGui::DockBuilderDockWindow("Resource", dock_id_down);
            //ImGui::DockBuilderDockWindow("Bottom", dock_id_bottom);


            auto node = ImGui::DockBuilderGetNode(actionBarId);
            node->SizeRef.y = 16.8f;
            node->LocalFlags = ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_SingleDock;

            auto right_node = ImGui::DockBuilderGetNode(dock_id_right);
			right_node->SizeRef.x = 300;



            auto bottom_node = ImGui::DockBuilderGetNode(dock_id_down);
            bottom_node->SizeRef.y = 150;

            auto left_bottom_node = ImGui::DockBuilderGetNode(dock_id_left_bottom);
			left_bottom_node->SizeRef.y = 580.f;

            //auto center_node = ImGui::DockBuilderGetCentralNode(dockspace_id);
            //left_bottom_node->SizeRef.x = 600.f;

            ImGui::DockBuilderFinish(dockspace_id);


            setup_dock = true;

        }
    }

    void EditorInterface::showToolbarWindow()
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        window_flags |= ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 2.f));
		ImGui::Begin(EditorConstant.Toolbar.c_str(), nullptr, window_flags);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.5f, 2.5f));

            float width = ImGui::GetWindowContentRegionWidth();

            int i = 0;
            float baseWidth = 24.f + 6.f + 20.f;

            /*ImGui::SameLine(baseWidth * i++);

            if(ImGui::ImageButton(m_BlankButtonTexture))
            {

            }

            ImGui::SameLine(baseWidth * i++);

            if(ImGui::ImageButton(m_BlankButtonTexture))
            {

            }

            ImGui::SameLine(baseWidth * i++);

            if(ImGui::ImageButton(m_BlankButtonTexture))
            {

            }

            ImGui::SameLine(baseWidth * i++);

            if(ImGui::ImageButton(m_BlankButtonTexture))
            {

            }*/

             ImGui::SameLine(width - 72.f);


             if(ImGui::ImageButton(m_ProjectButtonTexture))
             {
				ImGui::OpenPopup(EditorConstant.PROJECT_MANAGER.c_str());
             }

             ImGui::SameLine(width - 72.f - 24.f - 3.f - 10.f);

             if(ImGui::ImageButton(m_ReloadButtonTexture))
             {
                reloadProject();
             }

             ImGui::SameLine(width - 72.f - 48.f - 6.f - 20.f);

             if(ImGui::ImageButton(m_EditButtonTexture))
             {
                editProject();
             }

             ImGui::SameLine(width - 72.f - 72.f - 9.f - 30.f);

             if(ImGui::ImageButton(m_CompileButtonTexture))
             {
                compileProject();
             }

             ImGui::PopStyleVar();

			 //show project manager window
			 showProjectManagerWindow();

        ImGui::End();
		ImGui::PopStyleVar();
    }


    ////////////////////////Project and Workspace////////////////////////
    void EditorInterface::showProjectManagerWindow()
    {
        //Window flags
        ImGuiWindowFlags window_flags   = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize  | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
        //Winsow size
        ImVec2 winsow_size              = m_ProjectManagerWindowSize;

        //Project manager window
        ImGui::SetNextWindowSize(winsow_size);
        //Begin window
		if(ImGui::BeginPopupModal(EditorConstant.PROJECT_MANAGER.c_str(), nullptr, window_flags))
        {
            //Save cursor position
            ImVec2 cursor = ImGui::GetCursorPos();

            //Panel 1 : Engine Banner
            ImGui::BeginChild("##project_manager_panel_1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.33f, winsow_size.y - 20.f));
                ImGui::Image(m_ProjectManagerBannerTexture);
            ImGui::EndChild();

            //Panel 2 : Window tabs
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * 0.33f);
            ImGui::SetCursorPosY(cursor.y);
            ImGui::BeginChild("##project_manager_panel_2", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.67f, winsow_size.y * 0.85f));

                if (ImGui::BeginTabBar("##project_manager_tabbar"))
                {
                    ImGui::Dummy(ImVec2(0.0f, 10.0f));

					if (ImGui::BeginTabItem(EditorConstant.CREATE_PROJECT.c_str()))
                    {
                        showCreateProjectWindow();

                        ImGui::EndTabItem();
                    }

					if (ImGui::BeginTabItem(EditorConstant.OPEN_PROJECT.c_str()))
                    {
                        showOpenPorjectWindow();

                        ImGui::EndTabItem();
                    }

					if (ImGui::BeginTabItem(EditorConstant.RECENT_PROJECT.c_str()))
                    {
                        showRecentProjectWindow();

                        ImGui::EndTabItem();
                    }

					if (ImGui::BeginTabItem(EditorConstant.WORKSPACE.c_str()))
                    {
                        showWorkspaceWindow();

                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }

            ImGui::EndChild();

            ImGui::SetCursorPosX(winsow_size.x/2.f);
            ImGui::SetCursorPosY(winsow_size.y - 30.f);
            if (ImGui::Button("Close##close_project_manager_window", ImVec2(100, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
            /*float wording_width = 130.f;

                float input_width = ImGui::GetWindowContentRegionWidth() - 150.f;

                ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs;
                if (ImGui::BeginTabBar("ProjectManagerTab", tab_bar_flags))
                {
                    ImGui::Dummy(ImVec2(0.0f, 10.0f));

                    if (ImGui::BeginTabItem("Create Project"))
                    {

                        ImGui::EndTabItem();

                    }

                    if (ImGui::BeginTabItem("Open Project"))
                    {

                        ImGui::EndTabItem();

                    }

                    if (ImGui::BeginTabItem("Recent Project"))
                    {
                        ImGui::EndTabItem();

                    }

                    ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;
                    if(m_WorksapceStatus == 1)
                    {
                       flags = ImGuiTabItemFlags_SetSelected;
                       m_WorksapceStatus = 0;
                    }

                    if (ImGui::BeginTabItem("Workspace", nullptr, flags))
                    {
                        wording_width = 150.f;
                        input_width = ImGui::GetWindowContentRegionWidth() - wording_width;

                        ImGui::Text("Workspace Folder");
                        ImGui::SameLine(wording_width);
                        ImGui::SetNextItemWidth(input_width - 65.f);
                        ImGui::InputText("##workspace_folder", m_InputWorksapceFolder, IM_ARRAYSIZE(m_InputWorksapceFolder));
                        ImGui::SameLine(wording_width + input_width - 60.f);

                        if (ImGui::Button("Browse##choose_folder", ImVec2(60, 0)))
                        {
                            nfdchar_t *outPath = nullptr;
                            nfdresult_t result = NFD_PickFolder(nullptr, &outPath);


                            if ( result == NFD_OKAY ) {
                               //puts("Success!");
                               //puts(outPath);
                                free(outPath);

                                strncpy(m_InputWorksapceFolder, outPath, sizeof(m_InputWorksapceFolder) - 1);
                                m_InputWorksapceFolder[sizeof(m_InputWorksapceFolder) - 1] = 0;

                            }
                            else if ( result == NFD_CANCEL ) {
                               //puts("User pressed cancel.");
                            }
                            else {
                               //printf("Error: %s\n", NFD_GetError() );
                            }
                        }
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));


                        ImGui::Text("Workspace Name");
                        ImGui::SameLine(wording_width);
                        ImGui::SetNextItemWidth(input_width);
                        ImGui::InputText("##workspace_name", m_InputWorkspaceName, IM_ARRAYSIZE(m_InputWorkspaceName));
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Default Project Lead");
                        ImGui::SameLine(wording_width);
                        ImGui::SetNextItemWidth(input_width);
                        ImGui::InputText("##workspace_lead", m_InputWorkspaceLead, IM_ARRAYSIZE(m_InputWorkspaceLead));
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Default Company Name");
                        ImGui::SameLine(wording_width);
                        ImGui::SetNextItemWidth(input_width);
                        ImGui::InputText("##workspace_company", m_InputWorkspaceCompany, IM_ARRAYSIZE(m_InputWorkspaceCompany));
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::SameLine(wording_width + input_width - 130.f);
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
                        if (ImGui::Button("Create Workspace##create_workspace_button", ImVec2(130.f, 0)))
                        {

                        }

                        ImGui::Dummy(ImVec2(0.0f, 10.0f));

                        ImGui::BeginChild("workspace list", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

                        ImGui::EndChild(),


                        ImGui::EndTabItem();

                    }

                    ImGui::EndTabBar();
                }
            ImGui::EndChild();*/

            /*//Panel 3 : existing project list
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * 0.33f);
            ImGui::SetCursorPosY(cursor.y + size.y * 0.5f);
            ImGui::BeginChild("Open Project", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.67f, size.y * 0.5f - 75.f));

            ImGui::Text("Open Project");
            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::BeginChild("project list", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

                for(const nlohmann::json& project : m_ProjectManager->getProjectTable())
                {
                    std::string project_name = project["project_name"].get<std::string>();

                    if (ImGui::CollapsingHeader(project_name.c_str()))
                    {
                                    project_name        = ": " + project_name;
                        std::string project_id          = ": " + project["project_id"].get<std::string>();
                        std::string project_lead        = ": " + project["project_lead"].get<std::string>();
                        std::string project_company     = ": " + project["project_company"].get<std::string>();
                        std::string project_description = ": " + project["project_description"].get<std::string>();
                        std::string creation_date       = ": " + project["creation_date"].get<std::string>();
                        std::string modification_date   = ": " + project["modification_date"].get<std::string>();

                        ImGui::Text("Project Name");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(project_name.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Project Id");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(project_id.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Project Lead");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(project_lead.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Project Company");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(project_company.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Create Date");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(creation_date.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Modification Date");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(modification_date.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 2.0f));

                        ImGui::Text("Project Description");
                        ImGui::SameLine(wording_width);
                        ImGui::Text(project_description.c_str());
                        ImGui::Dummy(ImVec2(0.0f, 5.0f));


                        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 100.f);
                        if (ImGui::Button("Open Project", ImVec2(100, 0)))
                        {
                            openProject(project);

                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::Dummy(ImVec2(0.0f, 10.0f));

                    }
                }
             ImGui::EndChild();

            ImGui::EndChild();*/

            /*ImGui::Dummy(ImVec2(0.0f, 7.0f));

            cursor = ImGui::GetCursorPos();*/
            /*ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * 0.33f);
            if (ImGui::Button("Open Folder", ImVec2(100, 0)))
            {
                nfdchar_t *outPath = nullptr;
                nfdresult_t result = NFD_PickFolder(nullptr, &outPath); //NFD_OpenDialog( nullptr, nullptr, &outPath );


                if ( result == NFD_OKAY ) {
                   //puts("Success!");
                   //puts(outPath);
                    free(outPath);

                    nero_log(outPath);

                    openProject(std::string(outPath));
                }
                else if ( result == NFD_CANCEL ) {
                   //puts("User pressed cancel.");
                }
                else {
                   //printf("Error: %s\n", NFD_GetError() );
                }

                //ImGui::CloseCurrentPopup();
            }*/



    void EditorInterface::showCreateProjectWindow()
    {
        //Window flags
        ImGuiWindowFlags window_flags   = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize  | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
        //Winsow size
        ImVec2 winsow_size              = m_ProjectManagerWindowSize;

        float wording_width = 130.f;
        float input_width = ImGui::GetWindowContentRegionWidth() - 150.f;

        ImGui::Text("Create a new Project and start a new Adventure");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::BeginChild("project form", ImVec2(0.f, 0.f), true);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

		ImGui::Text("Project Name");
		ImGui::SameLine(wording_width);
		ImGui::SetNextItemWidth(input_width);
		ImGui::InputText("##project_name", m_InputProjectName, IM_ARRAYSIZE(m_InputProjectName));
		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		ImGui::Separator();

		ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Workspace");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);

        //load workpsace
        std::vector<std::string> workspaceNameTable = m_ProjectManager->getWorkspaceNameTable();

        if(workspaceNameTable.empty())
        {
            workspaceNameTable.push_back("There is no Workspace availabled");
        }

        std::size_t worskpaceCount = workspaceNameTable.size();
        const char** workspaceComboTable = new const char* [worskpaceCount];

        fillCharTable(workspaceComboTable, workspaceNameTable);

        m_SelectedWorkpsapce = workspaceComboTable[m_SelectedWorkpsapceIdex];
        if (ImGui::BeginCombo("##combo", m_SelectedWorkpsapce, ImGuiComboFlags())) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < worskpaceCount; n++)
            {
                bool is_selected = (m_SelectedWorkpsapce == workspaceComboTable[n]);


                if (ImGui::Selectable(workspaceComboTable[n], is_selected))
                {
                    m_SelectedWorkpsapce = workspaceComboTable[n];
                    m_SelectedWorkpsapceIdex = n;

                    auto workspace = m_ProjectManager->findWorkspace(workspaceNameTable[n]);

                    fillCharArray(m_InputProjectCompany, IM_ARRAYSIZE(m_InputProjectCompany), workspace["default_company_name"].get<std::string>());
                    fillCharArray(m_InputProjectLead, IM_ARRAYSIZE(m_InputProjectLead), workspace["default_project_lead"].get<std::string>());
                    fillCharArray(m_InputProjectNamespace, IM_ARRAYSIZE(m_InputProjectNamespace), workspace["default_namespace"].get<std::string>());
                }

                if (is_selected)
                {
                     ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        //delete array
        for (std::size_t i = 0 ; i < worskpaceCount; i++)
        {
            delete[] workspaceComboTable[i] ;
        }
        delete[] workspaceComboTable ;
        workspaceComboTable = nullptr;

		ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Project Type");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        const char* projectTypeComboTable[] = {"CPP Project", "Lua Project", "CPP and Lua Project"};
		m_SelectedProjectType = projectTypeComboTable[m_SelectedProjectTypeIdex];            // Here our selection is a single pointer stored outside the object.
        if (ImGui::BeginCombo("##project_type_combo", m_SelectedProjectType, ImGuiComboFlags())) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(projectTypeComboTable); n++)
            {
                bool is_selected = (m_SelectedProjectType == projectTypeComboTable[n]);

                if (ImGui::Selectable(projectTypeComboTable[n], is_selected))
                {
                    m_SelectedProjectType = projectTypeComboTable[n];
					m_SelectedProjectTypeIdex = n;
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Dummy(ImVec2(0.0f, 2.0f));


		ImGui::Text("Code Editor");
		ImGui::SameLine(wording_width);
		ImGui::SetNextItemWidth(input_width);
		const char* codeEditorComboTable[] = {"Qt Creator", "Visual Studio 2019"};
		m_SelectedCodeEditor = codeEditorComboTable[m_SelectedCodeEditorIdex];
		if (ImGui::BeginCombo("##code-editor-combo", m_SelectedCodeEditor, ImGuiComboFlags()))
		{
			for (int n = 0; n < IM_ARRAYSIZE(codeEditorComboTable); n++)
			{
				bool is_selected = (m_SelectedCodeEditor == codeEditorComboTable[n]);

				if (ImGui::Selectable(codeEditorComboTable[n], is_selected))
				{
					m_SelectedCodeEditor = codeEditorComboTable[n];
					m_SelectedCodeEditorIdex = n;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Project Lead");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##project_lead", m_InputProjectLead, IM_ARRAYSIZE(m_InputProjectLead));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Company Name");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##project_company", m_InputProjectCompany, IM_ARRAYSIZE(m_InputProjectCompany));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Namesapce");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##project_namespace", m_InputProjectNamespace, IM_ARRAYSIZE(m_InputProjectNamespace));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));


        ImGui::Text("Description");
        ImGui::SameLine(wording_width);
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline("##project_description", m_InputProjectDescription, IM_ARRAYSIZE(m_InputProjectDescription), ImVec2(input_width, ImGui::GetTextLineHeight() * 5), flags);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 102.f);
        ImGui::SetCursorPosY(winsow_size.y * 0.85f - 100.f);
        bool onCreate = ImGui::Button("Create", ImVec2(100, 0));
        std::string project_name = StringPool.BLANK;

        std::string error_message = StringPool.BLANK;
        bool error = true;

        if(std::string(m_InputProjectName) == StringPool.BLANK)
        {
            error_message = "Please enter a Project Name";
        }
        else if(m_ProjectManager->isProjectExist(std::string(m_InputProjectName)))
        {
            error_message = "A project with the same signature already exist,\n"
                            "please choose another Project Name";
        }
        else if(std::string(m_InputProjectLead) == StringPool.BLANK)
        {
            error_message = "Please enter a Project Lead";
        }
        else if (std::string(m_InputProjectCompany) == StringPool.BLANK)
        {
            error_message = "Please enter a Company Name";
        }
        else
        {
            error = false;
        }

        if (onCreate && error)
        {
            ImGui::OpenPopup("Error Creating Project");
        }
        else if(onCreate)
        {
            nlohmann::json projectJson;

            m_LastCreatedProject = std::string(m_InputProjectName);


            projectJson["workspace_name"]       = std::string(m_SelectedWorkpsapce);;
            projectJson["project_name"]         = std::string(m_InputProjectName);
            projectJson["project_type"]       = std::string(m_SelectedProjectType);;
            projectJson["project_namspace"]         = std::string(m_InputProjectNamespace);
            projectJson["project_lead"]         = std::string(m_InputProjectLead);
            projectJson["project_company"]      = std::string(m_InputProjectCompany);
            projectJson["project_description"]  = std::string(m_InputProjectDescription);

            memset(m_InputProjectName, 0, sizeof m_InputProjectName);
            //memset(m_InputProjectLead, 0, sizeof m_InputProjectLead);
            //memset(m_InputProjectNamespace, 0, sizeof m_InputProjectNamespace);
            //memset(m_InputProjectCompany, 0, sizeof m_InputProjectCompany);
            //memset(m_InputProjectDescription, 0, sizeof m_InputProjectDescription);

            auto workspace = m_ProjectManager->findWorkspace(workspaceNameTable.front());

            fillCharArray(m_InputProjectCompany, IM_ARRAYSIZE(m_InputProjectCompany), workspace["default_company_name"].get<std::string>());
            fillCharArray(m_InputProjectLead, IM_ARRAYSIZE(m_InputProjectLead), workspace["default_project_lead"].get<std::string>());
            fillCharArray(m_InputProjectNamespace, IM_ARRAYSIZE(m_InputProjectNamespace), workspace["default_namespace"].get<std::string>());
            fillCharArray(m_InputProjectDescription, IM_ARRAYSIZE(m_InputProjectDescription), "My awsome Game Project !");

            //start new thread
            m_CreateProjectFuture = std::async(std::launch::async, &EditorInterface::createProject, this, projectJson, std::ref(m_ProjectCreationStatus));

            ImGui::OpenPopup("Wait Project Creation");
        }

        ImGui::SetNextWindowSize(ImVec2(300.f, 120.f));
        if(ImGui::BeginPopupModal("Error Creating Project", nullptr, window_flags))
        {
            ImGui::Text("%s", error_message.c_str());
            ImGui::Dummy(ImVec2(0.0f, 45.0f));
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 95.f);
            if (ImGui::Button("Close", ImVec2(100, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
         }

        ImGui::SetNextWindowSize(ImVec2(300.f, 120.f));
        if(ImGui::BeginPopupModal("Wait Project Creation", nullptr, window_flags))
        {
            std::string message = StringPool.BLANK;

            if(m_ProjectCreationStatus == 1)
            {
                message = "Creating project ... ";
            }
            else if(m_ProjectCreationStatus == 2)
            {
                 message = "Creating project ... ";
                 message += "\nStep 1/2 : Generating project files";
            }
            else if(m_ProjectCreationStatus == 3)
            {
                 message = "Creating project ... ";
                 message += "\nStep 1/2 : Generating project files ..";
                 message += "\nStep 2/2 : Compiling project ...";
            }
            else if(m_ProjectCreationStatus == 4)
            {
                 message = "Creating project ... ";
                 message += "\nStep 1/2 : Generating project files ..";
                 message += "\nStep 2/2 : Compiling project ...";
                  message += "\nCreation Complet !";
            }

            ImGui::TextWrapped("%s", message.c_str());

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            if(m_ProjectCreationStatus == 4)
            {
                if (ImGui::Button("Open project", ImVec2(100, 0)))
                {

                    ImGui::CloseCurrentPopup();
                    ImGui::ClosePopupToLevel(0, true);

                     openProject(m_LastCreatedProject);
                }

                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 95.f);

                if (ImGui::Button("Close", ImVec2(100, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
         }

        //check if a workspace exit
        if(false)//m_WorksapceStatus == 0)
        {
            ImGui::OpenPopup("Creat a Worksapce");
        }

        ImGui::SetNextWindowSize(ImVec2(200.f, 100.f));
        if(ImGui::BeginPopupModal("Creat a Worksapce", nullptr, window_flags))
        {

            if (ImGui::Button("Create Workspace##close_workspace", ImVec2(100, 0)))
            {
                m_WorksapceStatus = 1;

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
         }

        ImGui::EndChild();

    }

    void EditorInterface::showOpenPorjectWindow()
    {
        float wording_width = 130.f;
        float input_width = ImGui::GetWindowContentRegionWidth() - 150.f;

        ImGui::Text("Create a new Project and start a new Adventure");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));



        ImGui::BeginChild("project list", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

            ImGui::Dummy(ImVec2(0.f, 10.f));

            for(const std::string workspace : m_ProjectManager->getWorkspaceNameTable())
            {
                std::string header = workspace + " - workspace";
                if (ImGui::CollapsingHeader(header.c_str()))
                {
                    ImGui::SetCursorPosX(20.f);
                    std::string message = "List of Projects in workspace : " + workspace;
                    ImGui::Text(message.c_str());
                    ImGui::Separator();

                    ImGui::Dummy(ImVec2(0.0f, 5.0f));


                    for(const nlohmann::json& project : m_ProjectManager->getWorkspaceProjectTable(workspace))
                    {
                        std::string project_name = project["project_name"].get<std::string>();

                        ImGui::SetCursorPosX(20.f);
                        if (ImGui::CollapsingHeader(project_name.c_str()))
                        {
                                        project_name        = ": " + project_name;
                            std::string project_id          = ": " + project["project_id"].get<std::string>();
                            std::string project_lead        = ": " + project["project_lead"].get<std::string>();
                            std::string project_company     = ": " + project["project_company"].get<std::string>();
                            std::string project_description = ": " + project["project_description"].get<std::string>();
                            std::string creation_date       = ": " + project["creation_date"].get<std::string>();
                            std::string modification_date   = ": " + project["modification_date"].get<std::string>();

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Project Name");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(project_name.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 2.0f));

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Project Id");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(project_id.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 2.0f));

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Project Lead");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(project_lead.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 2.0f));

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Project Company");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(project_company.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 2.0f));

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Create Date");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(creation_date.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 2.0f));

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Modification Date");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(modification_date.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 2.0f));

                            ImGui::SetCursorPosX(40.f);
                            ImGui::Text("Project Description");
                            ImGui::SameLine(wording_width);
                            ImGui::Text(project_description.c_str());
                            ImGui::Dummy(ImVec2(0.0f, 5.0f));


                            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 100.f);
                            if (ImGui::Button("Open Project", ImVec2(100, 0)))
                            {
                                openProject(project["project_name"].get<std::string>());

                                ImGui::CloseCurrentPopup();
                            }

                            ImGui::Dummy(ImVec2(0.0f, 10.0f));

                        }
                    }
                 }

                ImGui::Dummy(ImVec2(0.f, 15.f));
            }

         ImGui::EndChild();
    }

    void EditorInterface::showRecentProjectWindow()
    {

    }

    void EditorInterface::showWorkspaceWindow()
    {
        float wording_width = 150.f;
        float input_width   = ImGui::GetWindowContentRegionWidth() - wording_width;

        ImGui::Text("Workspace Folder");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width - 65.f);
        ImGui::InputText("##workspace_folder", m_InputWorksapceFolder, IM_ARRAYSIZE(m_InputWorksapceFolder));
        ImGui::SameLine(wording_width + input_width - 60.f);

        if(ImGui::Button("Browse##choose_workspace_folder", ImVec2(60, 0)))
        {
            nfdchar_t *outPath = nullptr;
            nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

            if (result == NFD_OKAY)
            {
                free(outPath);

                //Copy outPath to m_InputWorksapceFolder
                strncpy(m_InputWorksapceFolder, outPath, sizeof(m_InputWorksapceFolder) - 1);
                m_InputWorksapceFolder[sizeof(m_InputWorksapceFolder) - 1] = 0;

            }
            else if (result == NFD_CANCEL)
            {

            }
            else
            {
               //printf("Error: %s\n", NFD_GetError() );
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Workspace Name");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##workspace_name", m_InputWorkspaceName, IM_ARRAYSIZE(m_InputWorkspaceName));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Default Project Lead");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##workspace_lead", m_InputWorkspaceLead, IM_ARRAYSIZE(m_InputWorkspaceLead));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Default Company Name");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##workspace_company", m_InputWorkspaceCompany, IM_ARRAYSIZE(m_InputWorkspaceCompany));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::Text("Default Namespace");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::InputText("##workspace_namespace", m_InputWorkspaceNamespace, IM_ARRAYSIZE(m_InputWorkspaceNamespace));
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::SameLine(wording_width + input_width - 130.f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

        bool onCreate               = ImGui::Button("Create Workspace", ImVec2(130.f, 0));
        std::string error_message   = StringPool.BLANK;
        bool error = false;

        /*if(false)
        {
            error_message = "Please enter a Project Name";
        }
        else if(false)
        {
            error_message = "A project with the same signature already exist,\n"
                            "please choose another Project Name";
        }
        else
        {
            error = false;
        }*/

        if (onCreate && error)
        {
			ImGui::OpenPopup(EditorConstant.ERROR_CREATING_WORKSPACE.c_str());
        }
        else if(onCreate)
        {
            nlohmann::json workspaceJson;

            workspaceJson["workspace_folder"]       = std::string(m_InputWorksapceFolder);
            workspaceJson["workspace_name"]         = std::string(m_InputWorkspaceName);
            workspaceJson["default_project_lead"]   = std::string(m_InputWorkspaceLead);
            workspaceJson["default_company_name"]   = std::string(m_InputWorkspaceCompany);
            workspaceJson["default_namespace"]      = std::string(m_InputWorkspaceNamespace);

            memset(m_InputWorksapceFolder,      0, sizeof m_InputWorksapceFolder);
            memset(m_InputWorkspaceName,        0, sizeof m_InputWorkspaceName);
            memset(m_InputWorkspaceLead,        0, sizeof m_InputWorkspaceLead);
            memset(m_InputWorkspaceCompany,     0, sizeof m_InputWorkspaceCompany);
            memset(m_InputWorkspaceNamespace,   0, sizeof m_InputWorkspaceNamespace);

            createWorkspace(workspaceJson);
        }



        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Separator();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));


        ImGui::Text("Workspace Folder");
        ImGui::SameLine(wording_width);
        ImGui::SetNextItemWidth(input_width - 130.f);
        ImGui::InputText("##workspace_folder_import", m_InputWorksapceImportFolder, IM_ARRAYSIZE(m_InputWorksapceImportFolder));
        ImGui::SameLine(wording_width + input_width - 124.f);

        if(ImGui::Button("Browse##choose_workspace_folder_import", ImVec2(60, 0)))
        {
            nfdchar_t *outPath = nullptr;
            nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

            if (result == NFD_OKAY)
            {
                free(outPath);

                //Copy outPath to m_InputWorksapceFolder
                strncpy(m_InputWorksapceImportFolder, outPath, sizeof(m_InputWorksapceImportFolder) - 1);
                m_InputWorksapceImportFolder[sizeof(m_InputWorksapceImportFolder) - 1] = 0;

            }
            else if (result == NFD_CANCEL)
            {

            }
            else
            {
               //printf("Error: %s\n", NFD_GetError() );
            }
        }

        ImGui::SameLine(wording_width + input_width - 60.f);
        if(ImGui::Button("Import##import_workspace", ImVec2(60, 0)))
        {

        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));



        ImGuiWindowFlags window_flag = ImGuiWindowFlags_None;
        window_flag |= ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        ImGui::BeginChild("##workspace_list", ImVec2(0.f, 0.f), true, window_flag);
        ImGui::Dummy(ImVec2(0.0f, 4.0f));


            for(const nlohmann::json& worksapce : m_ProjectManager->getWorkspaceTable())
            {
                if (ImGui::CollapsingHeader(worksapce["workspace_name"].get<std::string>().c_str()))
                {
                    std::string workspace_id            = ": " + worksapce["workspace_id"].get<std::string>();
                    std::string workspace_name          = ": " + worksapce["workspace_name"].get<std::string>();
                    std::string default_project_lead    = ": " + worksapce["default_project_lead"].get<std::string>();
                    std::string default_company_name    = ": " + worksapce["default_company_name"].get<std::string>();
                    std::string default_namespace       = ": " + worksapce["default_namespace"].get<std::string>();
                    std::string workspace_directory     = ": " + worksapce["workspace_directory"].get<std::string>();

                    ImGui::Text("Workspace Id");
                    ImGui::SameLine(wording_width);
                    ImGui::Text(workspace_id.c_str());
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::Text("Workspace Name");
                    ImGui::SameLine(wording_width);
                    ImGui::Text(workspace_name.c_str());
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::Text("Workspace Directory");
                    ImGui::SameLine(wording_width);
                    ImGui::Text(workspace_directory.c_str());
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::Text("Default Project Lead");
                    ImGui::SameLine(wording_width);
                    ImGui::Text(default_project_lead.c_str());
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::Text("Default Company Name");
                    ImGui::SameLine(wording_width);
                    ImGui::Text(default_company_name.c_str());
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::Text("Default Namespace");
                    ImGui::SameLine(wording_width);
                    ImGui::Text(default_namespace.c_str());
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                }

            }

        ImGui::EndChild();




        ImGui::SetNextWindowSize(ImVec2(300.f, 120.f));
		if(ImGui::BeginPopupModal(EditorConstant.ERROR_CREATING_WORKSPACE.c_str()))
        {
            ImGui::Text("%s", error_message.c_str());

            ImGui::Dummy(ImVec2(0.0f, 45.0f));

            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 95.f);

            if (ImGui::Button("Close##workspace_error_close_button", ImVec2(100, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
         }
    }

    void EditorInterface::createWorkspace(const nlohmann::json& workspaceJson)
    {
        m_ProjectManager->createWorkspace(workspaceJson);
    }

    int EditorInterface::createProject(const nlohmann::json& projectJson, int& status)
    {
        m_ProjectManager->createProject(projectJson, status);

        return status;
    }

    void EditorInterface::openProject(const std::string& project_name)
    {
         m_GameProject =  m_ProjectManager->openProject(project_name);
         m_AdvancedScene = m_GameProject->getAdvancedScene();
         m_UpdateWindowTile(project_name);
    }

    void EditorInterface::setUpdateWindowTitle(std::function<void (const std::string&)> fn)
    {
        m_UpdateWindowTile = fn;
    }


    void EditorInterface::compileProject()
    {
		m_CompileProjectFuture = std::async(std::launch::async, [this]()
		{
			if(m_GameProject)
			{
				m_GameProject->compileProject();

			}

			return 0;
		});

	}

    void EditorInterface::editProject()
    {
        if(m_GameProject)
        {
            m_GameProject->openEditor();

        }
    }

    void EditorInterface:: reloadProject()
    {

        if(m_GameProject)
        {
            nero_log("reloading project ...");

            m_GameProject->loadProjectLibrary();

        }
    }




    ////////////////////////Project and Workspace End////////////////////////

    /*void EditorInterface::addScene(const std::string& projectName, std::function<Scene::Ptr(Scene::Context)> factory)
    {
        //Engine SDK create and open a C++ project [Project_C++]

        if(m_ProjectManager->isProjectExist(projectName))
        {
            m_SceneManager->addSceneFacotry(formatString(projectName), factory);
        }
        else
        {
            m_ProjectManager->createProject(projectName, ProjectManager::CPP_PROJECT);
            m_SceneManager->addSceneFacotry(formatString(projectName), factory);

            //save project
            //load project
        }
    }

    void EditorInterface::addLuaScene(const std::string& projectName, std::function<LuaScene::Ptr(Scene::Context)> factory)
    {
        //Engine SDK create or open a C++/Lua project [Project_C++_LUA]

        if(m_ProjectManager->isProjectExist(projectName))
        {
            m_SceneManager->addLuaSceneFacotry(formatString(projectName), factory);
        }
        else
        {
            m_ProjectManager->createProject(projectName, ProjectManager::LUA_CPP_PROJECT);
            m_SceneManager->addLuaSceneFacotry(formatString(projectName), factory);
        }
    }*/

    void EditorInterface::setEditorSetting(const nlohmann::json& setting)
    {
        m_EditorSetting = setting;

        m_ProjectManager->setEditorSetting(m_EditorSetting);


        auto workspaceNameTable = m_ProjectManager->getWorkspaceNameTable();

        if(!workspaceNameTable.empty())
        {
            auto workspace = m_ProjectManager->findWorkspace(workspaceNameTable.front());

            fillCharArray(m_InputProjectCompany, IM_ARRAYSIZE(m_InputProjectCompany), workspace["default_company_name"].get<std::string>());
            fillCharArray(m_InputProjectLead, IM_ARRAYSIZE(m_InputProjectLead), workspace["default_project_lead"].get<std::string>());
            fillCharArray(m_InputProjectNamespace, IM_ARRAYSIZE(m_InputProjectNamespace), workspace["default_namespace"].get<std::string>());
            fillCharArray(m_InputProjectDescription, IM_ARRAYSIZE(m_InputProjectDescription), "My awsome Game Project !");
        }

    }

    void EditorInterface::loadAllProject()
    {
        m_ProjectManager->loadAllProject();
    }

    sf::Sprite EditorInterface::flipTexture(const sf::Texture& texture)
    {
        sf::Vector2u size = texture.getSize();
        sf::Sprite sprite(texture, sf::IntRect(0, size.y, size.x, -size.y));

        return sprite;
    }

    void EditorInterface::showLogWindow()
    {

        ImGui::Begin("Logging");

        // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
        m_LoggerApplication.Draw("Logging");
    }

    void EditorInterface::showResourceCategoryWindow()
    {
		ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::Begin("Resource", nullptr, flags);

		if(ImGui::Button("Sprite##open_sprite_resource", ImVec2(100.f, 100.f)))
        {
            open_sprite_browser = true;
        }

		ImGui::SameLine();


		if(ImGui::Button("Animation##open_sprite_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}

		ImGui::SameLine();


		if(ImGui::Button("Mesh##open_sprite_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}

		ImGui::SameLine();

		if(ImGui::Button("Shape##open_shape_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}

		ImGui::SameLine();

		if(ImGui::Button("Particle##open_shape_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}

		ImGui::SameLine();

		if(ImGui::Button("Light##open_shape_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}


		ImGui::SameLine();

		if(ImGui::Button("Font##open_shape_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}

		ImGui::SameLine();

		if(ImGui::Button("Custom##open_shape_resource", ImVec2(100.f, 100.f)))
		{
			open_sprite_browser = true;
		}


		ImGui::End();
    }

    void EditorInterface::showResourceWindow()
    {
        //project manager window
        if(open_sprite_browser)
        {
            ImGui::Begin("Resource Browser", nullptr, ImGuiWindowFlags());

                if(ImGui::Button("Import File##import_sprite_resource", ImVec2(100.f, 0.f)))
                {
                    //nfdchar_t *outPath = nullptr;
                    nfdpathset_t *outPaths = nullptr;
                    nfdresult_t result = NFD_OpenDialogMultiple( nullptr, nullptr, outPaths);


                    if ( result == NFD_OKAY ) {
                        //puts("Success!");
                        //puts(outPath);
                        free(outPaths);

                        nero_log(outPaths);

                    }
                    else if ( result == NFD_CANCEL ) {
                        puts("User pressed cancel.");
                    }
                    else {
                        //printf("Error: %s\n", NFD_GetError() );
                    }
                }

                ImGui::SameLine();

                if(ImGui::Button("Import Folder##import2_sprite_resource", ImVec2(100.f, 0.f)))
                {
                    nfdchar_t *outPath = nullptr;
                    nfdresult_t result = NFD_PickFolder(nullptr, &outPath); //NFD_OpenDialog( nullptr, nullptr, &outPath );


                    if ( result == NFD_OKAY ) {
                       //puts("Success!");
                       //puts(outPath);
                        free(outPath);

                        nero_log(outPath);

                    }
                    else if ( result == NFD_CANCEL ) {
                       //puts("User pressed cancel.");
                    }
                    else {
                       //printf("Error: %s\n", NFD_GetError() );
                    }
                }

                ImGui::SameLine();


                if(ImGui::Button("Close##close_sprite_resource", ImVec2(60.f, 0.f)))
                {
                    open_sprite_browser = false;
                }

                ImGui::Separator();


				auto& spriteTable = m_ResourceManager.texture.getSpriteTable();
				int sprite_count = spriteTable.size();
				ImGuiStyle& style = ImGui::GetStyle();
				float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

				for (int n = 0; n < sprite_count; n++)
				{
					sf::Vector2u boo = m_ResourceManager.texture.getSpriteTexture(spriteTable[n]).getSize();
					sf::Vector2u zoo = boo;
					if(n < sprite_count-1)
					{
						zoo = m_ResourceManager.texture.getSpriteTexture(spriteTable[n+1]).getSize();
					}

					std::function<sf::Vector2f(sf::Vector2f, float)> loo = [](sf::Vector2f koo, float size){

						if(koo.x > size)
						{
							float scale = size / koo.x;
							koo.x = size;
							koo.y = koo.y * scale;
						}
						if(koo.y > size)
						{
							float scale = size / koo.y;
							koo.y = size;
							koo.x = koo.x * scale;
						}

						return koo;
					};

					sf::Vector2f sprite_size(boo.x, boo.y);
					sprite_size = loo(sprite_size, 250);

					sf::Vector2f next_sprite_size(zoo.x, zoo.y);
					next_sprite_size = loo(next_sprite_size, 250);


					ImVec2 button_size(sprite_size.x, sprite_size.y);

					if(ImGui::ImageButton(m_ResourceManager.texture.getSpriteTexture(spriteTable[n]), button_size))
					{
					   ImGui::OpenPopup(EditorConstant.PROJECT_MANAGER.c_str());
					}

					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + next_sprite_size.x;
					if (n + 1 < sprite_count && next_button_x2 < window_visible_x2)
						ImGui::SameLine();
				}


				/*for(std::string sprite : spriteTable)
				{
					if(ImGui::ImageButton(m_ResourceManager.texture.getSpriteTexture(sprite)))
					{
					   ImGui::OpenPopup(EditorConstant.PROJECT_MANAGER.c_str());
					}
				}*/

			ImGui::End();
        }


    }

    void EditorInterface::showUtilityWindow()
    {
		ImGui::Begin(EditorConstant.UTILITY.c_str());

            ImVec2 size = ImGui::GetWindowSize();

            ImGui::BeginChild("scene_mode", ImVec2(0.f, 105.f), true);
                ImGui::Text("Choose Scene Mode");
                ImGui::Separator();

                static int e = 0;
                ImGui::RadioButton("Object", &e, 0);
                ImGui::RadioButton("Mesh", &e, 1);
                ImGui::RadioButton("Play", &e, 2);


            ImGui::EndChild();


            ImGui::BeginChild("save_load", ImVec2(0.f, 85.f), true);
                ImGui::Text("Save & Load");
                ImGui::Separator();

                ImGui::Dummy(ImVec2(0.f, 2.f));

                static bool auto_save = false;
                ImGui::Checkbox("Auto save", &auto_save);

                ImVec2 button_size = ImVec2(70.f, 0.f);

                 if(ImGui::Button("Save", button_size))
                 {

                 }

                ImGui::SameLine();

                 if(ImGui::Button("Load", button_size))
                 {

                 }

            ImGui::EndChild();



            ImGui::BeginChild("access_button", ImVec2(0.f, 90.f), true);
                ImGui::Text("Access Website");
                ImGui::Separator();

                ImGui::Dummy(ImVec2(0.f, 2.f));

                 if(ImGui::Button("Learn", button_size))
                 {

                 }

                ImGui::SameLine();

                 if(ImGui::Button("Forum", button_size))
                 {

                 }


                 if(ImGui::Button("Snippet", button_size))
                 {

                 }

                 ImGui::SameLine();

                 if(ImGui::Button("API", button_size))
                 {

                 }

            ImGui::EndChild();

        ImGui::End();
    }

	void EditorInterface::showMusicWindow()
	{
		ImGui::Begin(EditorConstant.Music.c_str());

		ImGui::End();
	}

    void EditorInterface::showSceneScreenWindow()
    {
		ImGui::Begin(EditorConstant.SCENE_SCREEN.c_str());

            ImGui::Text("Manage Scene Screens");
            ImGui::Separator();
			ImGui::Dummy(ImVec2(0.f, 2.f));

			float width = 82.f;
			static char new_screen_name[100] = "";
			ImGui::SetNextItemWidth(width*2.f + 8.f);
			ImGui::InputText("##screen_did", new_screen_name, IM_ARRAYSIZE(new_screen_name));

			ImGui::Dummy(ImVec2(0.f, 2.f));

			ImVec2 button_size = ImVec2(width, 0.f);
            //addcreen
			if(ImGui::Button("Add##add_screen", button_size))
            {
                //
            }

			ImGui::SameLine();

			if(ImGui::Button("Rename##add_screen", button_size))
			{
				//
			}


			if(ImGui::Button("Duplicate##add_screen", button_size))
			{
				//
			}

			ImGui::SameLine();

			if(ImGui::Button("Remove##add_screen", button_size))
			{
				//
			}




            //rename screen
            //copy

            ImGui::Dummy(ImVec2(0.f, 5.f));

            ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("scene_screen_list", ImVec2(), true);

                static bool check = false;
				ImGui::Checkbox("", &check);

                ImGui::SameLine();

                ImGuiInputTextFlags flags2 = ImGuiInputTextFlags_ReadOnly;
                static char screen_name[100] = "screen one";
				ImGui::SetNextItemWidth(130.f);
                ImGui::InputText("##screen_id", screen_name, IM_ARRAYSIZE(screen_name), flags2);               

           ImGui::EndChild();

        ImGui::End();
    }

	void EditorInterface::showSceneLayerWindow()
	{
		ImGui::Begin(EditorConstant.SCENE_LAYER.c_str());

		ImGui::Text("Manage Scene Level");
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.f, 2.f));

		float width = 82.f;
		static char input_level_name[100] = "";
		ImGui::SetNextItemWidth(width*2.f + 8.f);
		ImGui::InputText("##level_id", input_level_name, IM_ARRAYSIZE(input_level_name));

		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImVec2 button_size = ImVec2(width, 0.f);
		//addcreen
		if(ImGui::Button("Add##add_level", button_size))
		{
			//
		}

		ImGui::SameLine();

		if(ImGui::Button("Rename##rename_level", button_size))
		{
			//
		}


		if(ImGui::Button("Duplicate##duplicate_level", button_size))
		{
			//
		}

		ImGui::SameLine();

		if(ImGui::Button("Remove##remove_level", button_size))
		{
			//
		}

		//rename screen
		//copy

		ImGui::Dummy(ImVec2(0.f, 5.f));

		ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("scene_level_list", ImVec2(), true);

			static bool check = false;
			ImGui::Checkbox("", &check);

			ImGui::SameLine();

			ImGuiInputTextFlags flags2 = ImGuiInputTextFlags_ReadOnly;
			static char screen_name[100] = "screen one";
			ImGui::SetNextItemWidth(130.f);
			ImGui::InputText("##level_id", screen_name, IM_ARRAYSIZE(screen_name), flags2);

	   ImGui::EndChild();


		ImGui::End();
	}

	void EditorInterface::showSceneLevelWindow()
	{
		ImGui::Begin(EditorConstant.SCENE_LEVEL.c_str());

		ImGui::Text("Manage Scene Level");
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.f, 2.f));

		float width = 82.f;
		static char input_level_name[100] = "";
		ImGui::SetNextItemWidth(width*2.f + 8.f);
		ImGui::InputText("##level_id", input_level_name, IM_ARRAYSIZE(input_level_name));

		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImVec2 button_size = ImVec2(width, 0.f);
		//addcreen
		if(ImGui::Button("Add##add_level", button_size))
		{
			//
		}

		ImGui::SameLine();

		if(ImGui::Button("Rename##rename_level", button_size))
		{
			//
		}


		if(ImGui::Button("Duplicate##duplicate_level", button_size))
		{
			//
		}

		ImGui::SameLine();

		if(ImGui::Button("Remove##remove_level", button_size))
		{
			//
		}

		//rename screen
		//copy

		ImGui::Dummy(ImVec2(0.f, 5.f));

		ImGui::Separator();

		ImGui::Dummy(ImVec2(0.f, 5.f));

		//ImGui::BeginChild("scene_level_list", ImVec2());

			static int e = 0;

			ImGui::RadioButton("##bsdf", &e, 0);

			ImGui::SameLine();

			static bool sdfsfs = false;
			ImGui::Checkbox("##bsdf2", &sdfsfs);

			ImGui::SameLine();

			ImGuiInputTextFlags flags2 = ImGuiInputTextFlags_ReadOnly;
			static char screen_name[100] = "screen one";
			ImGui::SetNextItemWidth(118.f);
			ImGui::InputText("##level_id", screen_name, IM_ARRAYSIZE(screen_name), flags2);

	   //ImGui::EndChild();


		ImGui::End();
	};

    void EditorInterface::showSceneChunckWindow()
    {
		ImGui::Begin(EditorConstant.SCENE_CHUNCK.c_str());

        ImGui::Text("Manage Scene Chuncks");
        ImGui::Separator();
		ImGui::Dummy(ImVec2(0.f, 2.f));

		float width = 82.f;
		static char input_level_name[100] = "";
		ImGui::SetNextItemWidth(width*2.f + 8.f);
		ImGui::InputText("##level_id", input_level_name, IM_ARRAYSIZE(input_level_name));

		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImVec2 button_size = ImVec2(width, 0.f);
		//addcreen
		if(ImGui::Button("Add##add_level", button_size))
		{
			//
		}

		ImGui::SameLine();

		if(ImGui::Button("Rename##rename_level", button_size))
		{
			//
		}


		if(ImGui::Button("Duplicate##duplicate_level", button_size))
		{
			//
		}

		ImGui::SameLine();

		if(ImGui::Button("Remove##remove_level", button_size))
		{
			//
		}

		//rename screen
		//copy

		ImGui::Dummy(ImVec2(0.f, 5.f));

		ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("scene_level_list", ImVec2(), true);

			static bool check = false;
			ImGui::Checkbox("", &check);

			ImGui::SameLine();

			ImGuiInputTextFlags flags2 = ImGuiInputTextFlags_ReadOnly;
			static char screen_name[100] = "screen one";
			ImGui::SetNextItemWidth(130.f);
			ImGui::InputText("##level_id", screen_name, IM_ARRAYSIZE(screen_name), flags2);

	   ImGui::EndChild();

        ImGui::End();
    }

    void EditorInterface::showToggleButton(bool toggle, const std::string& label, std::function<void()> callback)
    {
        if(toggle)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_TabActive]);
            if(ImGui::Button(label.c_str()))
            {
                callback();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
        else
        {
            if(ImGui::Button(label.c_str()))
            {
                callback();
            }
        }
    }

    void EditorInterface::interfaceFirstDraw()
    {
        if(!m_InterfaceFirstDraw)
        {
            return;
        }

        auto right_node = ImGui::DockBuilderGetNode(dock_id_right);
        right_node->TabBar->NextSelectedTabId = right_node->TabBar->Tabs.front().ID;

		auto upper_left_node = ImGui::DockBuilderGetNode(dock_id_upper_left);
        upper_left_node->TabBar->NextSelectedTabId = right_node->TabBar->Tabs.front().ID;

		auto left_bottom_node = ImGui::DockBuilderGetNode(dock_id_left_bottom);
		left_bottom_node->TabBar->NextSelectedTabId = right_node->TabBar->Tabs.front().ID;


		ImGui::SetWindowFocus(EditorConstant.SCENE.c_str());

        //commit
        m_InterfaceFirstDraw = false;
    }

    void EditorInterface::showCurrentSceneWindow()
	{
		ImGui::Begin(EditorConstant.SCENE_EXPLORER.c_str());

		if (ImGui::CollapsingHeader("Scene Graph"))
		{

		}

		if (ImGui::CollapsingHeader("Current Level"))
		{

		}

		if (ImGui::CollapsingHeader("Current Chunk"))
		{

		}

		if (ImGui::CollapsingHeader("Current Layer"))
		{

		}

		if (ImGui::CollapsingHeader("Selected Object"))
		{

		}

		ImGui::End();
    }

	void EditorInterface::showBackgroundTaskWindow()
	{
		// FIXME-VIEWPORT: Select a default viewport
		const float DISTANCE = 10.0f;
		static int corner = 3;
		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 window_pos = ImVec2((corner & 1) ? (viewport->Pos.x + viewport->Size.x - DISTANCE) : (viewport->Pos.x + DISTANCE), (corner & 2) ? (viewport->Pos.y + viewport->Size.y - DISTANCE) : (viewport->Pos.y + DISTANCE));
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		//ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.000f, 1.000f, 1.000f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.911f, 0.499f, 0.146f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.000f, 0.000f, 0.000f, 1.00f));
		if (ImGui::Begin("##background_task_window", nullptr, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			auto taskTable = m_GameProject->getBackgroundTaskTable();

			for(BackgroundTask& task : taskTable)
			{
				if(!task.isCompleted())
				{
					ImGui::Text(task.printMessage().c_str());
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
	}

}