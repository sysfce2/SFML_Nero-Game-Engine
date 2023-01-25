////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2023 Sanou A. K. Landry
////////////////////////////////////////////////////////////
#ifndef EDITORUI_H
#define EDITORUI_H
///////////////////////////HEADERS//////////////////////////
// Poco
#include <Poco/Logger.h>
// Nero
#include <Nero/editor/view/EditorDockspace.h>
#include <Nero/editor/view/EditorToolbar.h>
#include <Nero/editor/EditorSetup.h>
#include <Nero/editor/view/EditorSetupPopup.h>
#include <Nero/editor/view/ResourceSelectionWindow.h>
#include <Nero/editor/view/ResourceBrowserWindow.h>
#include <Nero/editor/view/SceneExplorerWindow.h>
#include <Nero/editor/view/EngineHelpWindow.h>
#include <Nero/editor/view/EditorUtilityWindow.h>
#include <Nero/editor/view/WorldChunkWindow.h>
#include <Nero/editor/view/ObjectLayerWindow.h>
#include <Nero/editor/view/GameLevelWindow.h>
#include <Nero/editor/view/ConsoleWindow.h>
#include <Nero/editor/view/LoggerWindow.h>
#include <Nero/editor/view/RenderCanvasWindow.h>
#include <Nero/editor/view/BackgroundTaskWindow.h>

#include <Nero/editor/project/ProjectManager.h>
#include <Nero/core/cpp/scene/Scene.h>
#include <Nero/core/cpp/engine/Parameter.h>
#include <Nero/core/lua/scene/LuaScene.h>
#include <Nero/editor/project/GameProject.h>
#include <Nero/core/cpp/utility/String.h>
#include <Nero/editor/project/AdvancedScene.h>
#include <Nero/core/cpp/resource/ResourceManager.h>
#include <Nero/editor/EditorCamera.h>
#include <Nero/editor/EditorUtility.h>
#include <Nero/editor/level/LevelBuilder.h>
#include <Nero/editor/screen/ScreenBuilder.h>
#include <Nero/editor/level/WorldBuilder.h>
// Json
#include <json/json.hpp>
// SFML
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
// File Diaglog
#include <nativefiledialog/include/nfd.h>
// Node Editor
#include <nodeeditor/imgui_node_editor.h>
// STD
#include <memory>
#include <functional>
#include <future>
#include <map>
////////////////////////////////////////////////////////////
namespace nero
{
    class EditorUI
    {

      public:
        typedef std::unique_ptr<EditorUI>          Ptr;
        typedef std::shared_ptr<sf::RenderTexture> RenderTexturePtr;

      public:
        EditorUI(sf::RenderWindow&   window,
                 AdvancedCamera::Ptr camera,
                 TextureHolder::Ptr  textureHolder,
                 FontHolder::Ptr     fontHolder,
                 SoundHolder::Ptr    soundHolder,
                 Setting::Ptr        setting);
        ~EditorUI();
        void destroy();

      private:
        // Initialization
        void        init();
        // Game loop
        void        handleEvent(const sf::Event& event);
        void        update(const sf::Time& timeStep);
        void        render();
        // Input
        void        handleKeyboardInput(const sf::Keyboard::Key& key, const bool& isPressed);
        // Core Engine callback
        void        updateFrameRate(const float& frameRate, const float& frameTime);
        void        setUpdateWindowTitleCallback(std::function<void(const std::string&)> callback);
        // Editor proxy
        void        setupEditorProxy();
        // Mouse Position
        bool        mouseOnCanvas();

        // signal handling
        void        registerSignalHandler();
        static void handleSignalAbnormalTermination(int signal);
        static void handleSignalArithmeticError(int signal);
        static void handleSignalIllegalInstruction(int signal);
        static void handleSignalInteractiveAttention(int signal);
        static void handleSignalInvalidStorageAccess(int signal);
        static void handleSignalTerminsationRequest(int signal);

      private:
        friend class GameEditor;
        // Constructor paramater
        sf::RenderWindow&                       m_RenderWindow;
        AdvancedCamera::Ptr                     m_EditorCamera;
        TextureHolder::Ptr                      m_EditorTextureHolder;
        FontHolder::Ptr                         m_EditorFontHolder;
        SoundHolder::Ptr                        m_EditorSoundHolder;
        Setting::Ptr                            m_EditorSetting;
        // Main paramater
        ProjectManager::Ptr                     m_ProjectManager;
        EditorProxy::Ptr                        m_EditorProxy;
        RenderTexturePtr                        m_RenderTexture;
        RenderContext::Ptr                      m_RenderContext;
        EditorContext::Ptr                      m_EditorContext;
        EditorSetup::Ptr                        m_EditorSetup;
        // UI
        EditorDockspace                         m_EditorDockspace;
        EditorToolbar                           m_EditorToolbar;
        EditorSetupPopup                        m_EditorSetupPopup;
        ResourceSelectionWindow                 m_ResourceSelectionWindow;
        ResourceBrowserWindow                   m_ResourceBrowserWindow;
        SceneExplorerWindow                     m_SceneExplorerWindow;
        EngineHelpWindow                        m_EngineHelpWindow;
        EditorUtilityWindow                     m_EditorUtilityWindow;
        WorldChunkWindow                        m_WorldChunkWindow;
        ObjectLayerWindow                       m_ObjectLayerWindow;
        GameLevelWindow                         m_GameLevelWindow;
        ConsoleWindow                           m_ConsoleWindow;
        LoggerWindow                            m_LoggerWindow;
        RenderCanvasWindow                      m_RenderCanvasWindow;
        BackgroundTaskWindow                    m_BackgroundTaskWindow;
        // Callback
        std::function<void(const std::string&)> m_UpdateWindowTitleCallback;

      private:
        //////////////docksapce
        bool                           m_InterfaceFirstDraw;
        void                           interfaceFirstDraw();
        //////////////Project
        //
        void                           showGameProjectWindow();
        void                           showGameSettingWindow();

        ax::NodeEditor::EditorContext* g_Context;
        void                           showVisualScriptWindow();

        TabSelectionHandler            m_BottomDockspaceTabBarSwitch;
        void                           showScriptCreationWindow();
        void                           clearScriptWizardInput();
        void                           switchBuilderMode();
        sf::Clock                      m_AutoSaveClock;
        // Canvas Script window
    };

} // namespace nero

#endif // EDITORUI_H
