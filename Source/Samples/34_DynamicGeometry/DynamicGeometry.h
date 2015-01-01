//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Sample.h"

namespace Urho3D
{
    
    class Node;
    class Scene;
    
}

/// Dynamic geometry example.
/// This sample demonstrates:
///     - Cloning a Model resource
///     - Modifying the vertex buffer data of the cloned models at runtime to efficiently animate them
///     - Creating a Model resource and its buffer data from scratch
class DynamicGeometry : public Sample
{
    OBJECT(DynamicGeometry);
    
public:
    /// Construct.
    DynamicGeometry(Context* context);
    
    /// Setup after engine initialization and before running the main loop.
    virtual void Start();
    
protected:
    /// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
    virtual String GetScreenJoystickPatchString() const { return
        "<patch>"
        "    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Is Visible']\" />"
        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Animation</replace>"
        "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]\">"
        "        <element type=\"Text\">"
        "            <attribute name=\"Name\" value=\"KeyBinding\" />"
        "            <attribute name=\"Text\" value=\"SPACE\" />"
        "        </element>"
        "    </add>"
        "</patch>";
    }
    
private:
    /// Construct the scene content.
    void CreateScene();
    /// Construct an instruction text to the UI.
    void CreateInstructions();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Read input and move the camera.
    void MoveCamera(float timeStep);
    /// Animate the vertex data of the objects.
    void AnimateObjects(float timeStep);
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    
    /// Cloned models' vertex buffers that we will animate.
    Vector<SharedPtr<VertexBuffer> > animatingBuffers_;
    /// Original vertex positions for the sphere model.
    PODVector<Vector3> originalVertices_;
    /// If the vertices are duplicates, indices to the original vertices (to allow seamless animation.)
    PODVector<unsigned> vertexDuplicates_;
    /// Animation flag.
    bool animate_;
    /// Animation's elapsed time.
    float time_;
    
    //custom
    ResourceCache* cache;
    
    //SAO
    
    bool VarChange(int keyDown, int keyUp, Urho3D::Vector3& var);
    // Enable ambient occlusion effect
    bool ao_enable;
    // Enable a simple Gaussian blur
    bool ao_blur;
    // Enable a depth aware Gaussian blur
    bool ao_blurdepth;
    // Show ambient occlusion raw values
    bool ao_only;
    
    //
    float var_steps;
    // AO radius in scene units
    Urho3D::Vector3 ao_radius;
    // AO intensity (in the paper/demo is divided by radius^6, this is commented out)
    Urho3D::Vector3 ao_intensity;
    // Radius scale adjust (not present in the paper/demo, TODO to be fixed)
    Urho3D::Vector3 ao_projscale;
    // Self occlusion margin
    Urho3D::Vector3 ao_bias;
    // Aux vars
    Urho3D::Vector3 ao_var1;
    Urho3D::Vector3 ao_var2;
    
    bool var_changed;
    bool define_changed;
    uint commandIndexSaoMain;
    uint commandIndexSaoCopy;
    
    SharedPtr<RenderPath> RenderPath;
    Camera* camera;
    Graphics* graphics;
    
    void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
    
    Material* sao;
    
    bool useMaterial;

};
