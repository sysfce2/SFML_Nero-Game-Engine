////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2023 Sanou A. K. Landry
////////////////////////////////////////////////////////////
#ifndef EDITORSETUPCODEEDITORVIEW_H
#define EDITORSETUPCODEEDITORVIEW_H
///////////////////////////HEADERS//////////////////////////
//Nero
#include <Nero/editor/ui/UIComponent.h>
#include <Nero/editor/EditorSetup.h>
////////////////////////////////////////////////////////////
namespace nero
{
    class EditorSetupCodeEditorView : public UIComponent
    {
        public:
                                    EditorSetupCodeEditorView(EditorContext::Ptr editorContext,
                                                              EditorSetup::Ptr editorSetup);
            virtual                ~EditorSetupCodeEditorView() override;

            virtual void 			destroy() override;
            virtual void            render() override;

        private:
            EditorSetup::Ptr        m_EditorSetup;
	};
}
#endif // EDITORSETUPCODEEDITORVIEW_H
