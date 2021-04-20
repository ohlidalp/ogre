#pragma once

#include "SdkSample.h"
#include "OgreImGuiOverlay.h"
#include <OgreImGuiInputListener.h>

using namespace Ogre;
using namespace OgreBites;

class ResourceBrowserWindow
{
public:
    void draw()
    {
        if (ImGui::Begin("Resource Browser"))
        {
            for (auto mgr_pair: Ogre::ResourceGroupManager::getSingleton().getResourceManagers())
            {
                if (ImGui::CollapsingHeader(mgr_pair.first.c_str()))
                {
                    for (auto res_pair: mgr_pair.second->getResourceIterator())
                    {
                        ImGui::Text(res_pair.second->getName().c_str());
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            drawTooltipBody(res_pair.second);
                            ImGui::EndTooltip();
                        }
                    }
                }
            }

            ImGui::End(); // "Resource diag"
        }
    }

private:
    void drawTooltipBody(ResourcePtr& resource)
    {
        ImGui::TextDisabled("Group: ");
        ImGui::SameLine();
        ImGui::Text("%s", resource->getGroup().c_str());
        ImGui::TextDisabled("Origin: ");
        ImGui::SameLine();
        ImGui::Text("%s", resource->getOrigin().c_str());
        ImGui::TextDisabled("Loading state: ");
        ImGui::SameLine();
        switch (resource->getLoadingState())
        {
            /// Not loaded
            case Resource::LOADSTATE_UNLOADED: ImGui::Text("(%d) %s", (int)resource->getLoadingState(), "Unloaded"); break;
            /// Loading is in progress
            case Resource::LOADSTATE_LOADING: ImGui::Text("(%d) %s", (int)resource->getLoadingState(), "Loading"); break;
            /// Fully loaded
            case Resource::LOADSTATE_LOADED: ImGui::Text("(%d) %s", (int)resource->getLoadingState(), "Loaded"); break;
            /// Currently unloading
            case Resource::LOADSTATE_UNLOADING: ImGui::Text("(%d) %s", (int)resource->getLoadingState(), "Unloading"); break;
            /// Fully prepared
            case Resource::LOADSTATE_PREPARED: ImGui::Text("(%d) %s", (int)resource->getLoadingState(), "Prepared"); break;
            /// Preparing is in progress
            case Resource::LOADSTATE_PREPARING: ImGui::Text("(%d) %s", (int)resource->getLoadingState(), "Preparing"); break;
                                
            default: ImGui::TextDisabled("~invalid~");
        }
        ImGui::TextDisabled("Is reloadable: ");
        ImGui::SameLine();
        ImGui::Text("%s", resource->isReloadable()?"true":"false");
        ImGui::TextDisabled("Is manually loaded: ");
        ImGui::SameLine();
        ImGui::Text("%s", resource->isManuallyLoaded()?"true":"false");
    }
};

/*
  Extended Dear ImGui sample, created as test tool for Overlay/Resource integration.
  See https://github.com/OGRECave/ogre/issues/1335
*/
class _OgreSampleClassExport Sample_ImGuiResourceBrowser : public SdkSample, public RenderTargetListener
{
    std::unique_ptr<ImGuiInputListener> mImguiListener;
    InputListenerChain mListenerChain;
public:
    // Basic constructor
    Sample_ImGuiResourceBrowser()
    {
        mInfo["Title"] = "Resource Browser";
        mInfo["Description"] = "Resource system browser (using Dear ImGui)";
        mInfo["Category"] = "Unsorted";
        mInfo["Thumbnail"] = "thumb_imgui.png";
    }

    void preViewportUpdate(const RenderTargetViewportEvent& evt)
    {
        if(!evt.source->getOverlaysEnabled()) return;
        if(!mTrayMgr->getTraysLayer()->isVisible()) return;

        ImGuiOverlay::NewFrame();

        ImGui::ShowDemoWindow();

        ResourceBrowserWindow rb_win;
        rb_win.draw();
    }

    bool keyPressed(const KeyboardEvent& evt) { return mListenerChain.keyPressed(evt); }
    bool keyReleased(const KeyboardEvent& evt) { return mListenerChain.keyReleased(evt); }
    bool mouseMoved(const MouseMotionEvent& evt) { return mListenerChain.mouseMoved(evt); }
    bool mouseWheelRolled(const MouseWheelEvent& evt) { return mListenerChain.mouseWheelRolled(evt); }
    bool mousePressed(const MouseButtonEvent& evt) { return mListenerChain.mousePressed(evt); }
    bool mouseReleased(const MouseButtonEvent& evt) { return mListenerChain.mouseReleased(evt); }
    bool textInput (const TextInputEvent& evt) { return mListenerChain.textInput (evt); }

    void setupContent(void)
    {
        auto imguiOverlay = new ImGuiOverlay();
        imguiOverlay->setZOrder(300);
        imguiOverlay->show();
        OverlayManager::getSingleton().addOverlay(imguiOverlay); // now owned by overlaymgr

        /*
            NOTE:
            Custom apps will ASSERT on ImGuiOverlay::NewFrame() and not display any UI if they
            have not registered the overlay system by calling mSceneMgr->addRenderQueueListener(mOverlaySystem).
            OgreBites::SampleBrowser does this on behalf of the ImGuiDemo but custom applications will need to
            call this themselves.  See ApplicationContextBase::createDummyScene().
        */
        mWindow->addListener(this);

        mImguiListener.reset(new ImGuiInputListener());
        mListenerChain = InputListenerChain({mTrayMgr.get(), mImguiListener.get(), mCameraMan.get()});

        mTrayMgr->showCursor();
    }

    void cleanupContent()
    {
        OverlayManager::getSingleton().destroy("ImGuiOverlay");
        mWindow->removeListener(this);
    }
};
