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

#include "Camera.h"
#include "CoreEvents.h"
#include "Engine.h"
#include "Font.h"
#include "Geometry.h"
#include "Graphics.h"
#include "IndexBuffer.h"
#include "Input.h"
#include "Light.h"
#include "Log.h"
#include "Model.h"
#include "Octree.h"
#include "Profiler.h"
#include "Renderer.h"
#include "ResourceCache.h"
#include "Scene.h"
#include "StaticModel.h"
#include "Text.h"
#include "VertexBuffer.h"
#include "UI.h"
#include "Zone.h"
#include "Material.h"
#include "RenderPath.h"

#include "DynamicGeometry.h"

#include "DebugNew.h"

#include <iostream>

DEFINE_APPLICATION_MAIN(DynamicGeometry)

DynamicGeometry::DynamicGeometry(Context* context) :
Sample(context),
animate_(true),
time_(0.0f)
{
    useMaterial = false;
    
    // Number of steps in each controller (high = precise+slow)
    //var_steps = 200.0f;
    var_steps = 10.0f;
    
    // Enable ambient occlusion effect
    ao_enable = true;
    // Enable a simple Gaussian blur
    ao_blur = true;
    // Enable a depth aware Gaussian blur
    ao_blurdepth = true;
    // Show ambient occlusion raw values
    ao_only = true;
    
    // uniforms format: Vector3[val, min, max]
    
    // AO radius in scene units
    ao_radius = Urho3D::Vector3(1.0f, 0.0f, 4.0f);
    // AO intensity (in the paper/demo is divided by radius^6, this is commented out)
    ao_intensity = Urho3D::Vector3(0.15f, 0.0f, 2.0f);
    // Radius scale adjust (not present in the paper/demo, TODO to be fixed)
    ao_projscale = Urho3D::Vector3(0.3f, 0.0f, 1.0f);
    // Self occlusion margin
    ao_bias = Urho3D::Vector3(0.01f, 0.0f, 0.1f);
    // Aux vars
    ao_var1 = Urho3D::Vector3(0.0f, -1.0f, 1.0f);
    ao_var2 = Urho3D::Vector3(1.0f, 0.0f, 1.0f);
    
    var_changed = true;
    define_changed = true;
    commandIndexSaoMain = -1;
    commandIndexSaoCopy = -1;

}

void DynamicGeometry::Start()
{
    // Execute base class startup
    Sample::Start();
    
    // Create the scene content
    CreateScene();
    
    // Create the UI content
    CreateInstructions();
    
    // Setup the viewport for displaying the scene
    SetupViewport();
    
    // Hook up to the frame update events
    SubscribeToEvents();
}

void DynamicGeometry::CreateScene()
{
    cache = GetSubsystem<ResourceCache>();
    
    
    scene_ = new Scene(context_);
    
    // Create the Octree component to the scene so that drawable objects can be rendered. Use default volume
    // (-1000, -1000, -1000) to (1000, 1000, 1000)
    scene_->CreateComponent<Octree>();
    
    // Create a Zone for ambient light & fog control
    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));
    zone->SetFogStart(200.0f);
    zone->SetFogEnd(300.0f);
    
    // Create a directional light
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    //lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
    //Light* light = lightNode->CreateComponent<Light>();
    //light->SetLightType(LIGHT_DIRECTIONAL);
    //light->SetColor(Color(0.4f, 1.0f, 0.4f));
    //light->SetSpecularIntensity(1.5f);
    
    //SSAO
    graphics = GetSubsystem<Graphics>();

    
    //Create Floor
    Node* floorNode = scene_->CreateChild("Floor");
    floorNode->SetPosition(Urho3D::Vector3(0.0f, -1.0f, 0.0f));
    floorNode->SetScale(Urho3D::Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
    floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    //floorObject->SetMaterial(cache->GetResource<Material>("Materials/simple.xml"));
    
    //sao = cache->GetResource<Material>("Materials/SimpleSAO.xml");
    //sao = cache->GetResource<Material>("Materials/simple.xml");
    //Color myCola = Color(Random(1.0f),Random(1.0f),Random(1.0f),1.0f);
    Color myCola = Color(1  , 0,0 ,1.0f);
    //sao->SetShaderParameter("MyColor", Variant(myCola));
    //sao->SetShaderParameter("ObjectColor", Variant(myCola));
    
    
    //sao->SetShaderParameter("Radius", ao_radius.x_);
    //sao->SetShaderParameter("ProjScale2", ao_projscale.x_);
    //sao->SetShaderParameter("IntensityDivR6", ao_intensity.x_);
    //sao->SetShaderParameter("Var1", ao_var1.x_);
    //sao->SetShaderParameter("Var2", ao_var2.x_);
    
    

    if(useMaterial)
        floorObject->SetMaterial(sao);
    
    // Get the original model and its unmodified vertices, which are used as source data for the animation
    Model* originalModel = cache->GetResource<Model>("Models/Box.mdl");
    if (!originalModel)
    {
        LOGERROR("Model not found, cannot initialize example scene");
        return;
    }
    // Get the vertex buffer from the first geometry's first LOD level
    VertexBuffer* buffer = originalModel->GetGeometry(0, 0)->GetVertexBuffer(0);
    const unsigned char* vertexData = (const unsigned char*)buffer->Lock(0, buffer->GetVertexCount());
    if (vertexData)
    {
        unsigned numVertices = buffer->GetVertexCount();
        unsigned vertexSize = buffer->GetVertexSize();
        // Copy the original vertex positions
        for (unsigned i = 0; i < numVertices; ++i)
        {
            const Vector3& src = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize);
            originalVertices_.Push(src);
        }
        buffer->Unlock();
        
        // Detect duplicate vertices to allow seamless animation
        vertexDuplicates_.Resize(originalVertices_.Size());
        for (unsigned i = 0; i < originalVertices_.Size(); ++i)
        {
            vertexDuplicates_[i] = i; // Assume not a duplicate
            for (unsigned j = 0; j < i; ++j)
            {
                if (originalVertices_[i].Equals(originalVertices_[j]))
                {
                    vertexDuplicates_[i] = j;
                    break;
                }
            }
        }
    }
    else
    {
        LOGERROR("Failed to lock the model vertex buffer to get original vertices");
        return;
    }
    
    // Create StaticModels in the scene. Clone the model for each so that we can modify the vertex data individually
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            Node* node = scene_->CreateChild("Object");
            node->SetPosition(Vector3(x * 2.0f, 0.0f, y * 2.0f));
            StaticModel* object = node->CreateComponent<StaticModel>();
            SharedPtr<Model> cloneModel = originalModel->Clone();
            object->SetModel(cloneModel);
            // Store the cloned vertex buffer that we will modify when animating
            animatingBuffers_.Push(SharedPtr<VertexBuffer>(cloneModel->GetGeometry(0, 0)->GetVertexBuffer(0)));
            
            if(useMaterial)
                object->SetMaterial(sao);
        }
    }
    
    // Finally create one model (pyramid shape) and a StaticModel to display it from scratch
    // Note: there are duplicated vertices to enable face normals. We will calculate normals programmatically
    {
        const unsigned numVertices = 18;
        
        float vertexData[] = {
            // Position             Normal
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
            
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
            
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
            
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
            
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
            
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f
        };
        
        const unsigned short indexData[] = {
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
            9, 10, 11,
            12, 13, 14,
            15, 16, 17
        };
        
        // Calculate face normals now
        for (unsigned i = 0; i < numVertices; i += 3)
        {
            Vector3& v1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i]));
            Vector3& v2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1)]));
            Vector3& v3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2)]));
            Vector3& n1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i + 3]));
            Vector3& n2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1) + 3]));
            Vector3& n3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2) + 3]));
            
            Vector3 edge1 = v1 - v2;
            Vector3 edge2 = v1 - v3;
            n1 = n2 = n3 = edge1.CrossProduct(edge2).Normalized();
        }
        
        SharedPtr<Model> fromScratchModel(new Model(context_));
        SharedPtr<VertexBuffer> vb(new VertexBuffer(context_));
        SharedPtr<IndexBuffer> ib(new IndexBuffer(context_));
        SharedPtr<Geometry> geom(new Geometry(context_));
        
        // Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
        vb->SetShadowed(true);
        vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL);
        vb->SetData(vertexData);
        
        ib->SetShadowed(true);
        ib->SetSize(numVertices, false);
        ib->SetData(indexData);
        
        geom->SetVertexBuffer(0, vb);
        geom->SetIndexBuffer(ib);
        geom->SetDrawRange(TRIANGLE_LIST, 0, numVertices);
        
        fromScratchModel->SetNumGeometries(1);
        fromScratchModel->SetGeometry(0, 0, geom);
        fromScratchModel->SetBoundingBox(BoundingBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f)));
        
        Node* node = scene_->CreateChild("FromScratchObject");
        node->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
        StaticModel* object = node->CreateComponent<StaticModel>();
        object->SetModel(fromScratchModel);
        
        //object->SetMaterial(bmata);
    }
    
    // Create the camera
    cameraNode_ = new Node(context_);
    cameraNode_->SetPosition(Vector3(0.0f, 2.0f, -20.0f));
    camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(300.0f);
}

void DynamicGeometry::CreateInstructions()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    
    // Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText(
                             "Use WASD keys and mouse/touch to move\n"
                             "Space to toggle animation"
                             );
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    // The text has multiple rows. Center them in relation to each other
    instructionText->SetTextAlignment(HA_CENTER);
    
    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 4);
}

void DynamicGeometry::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    
    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
    
    RenderPath = viewport->GetRenderPath()->Clone();
    //RenderPath->Load(cache->GetResource<XMLFile>("RenderPaths/DeferredSAO2.xml"));
    RenderPath->Load(cache->GetResource<XMLFile>("RenderPaths/ForwardDepthSAO3.xml"));
    //RenderPath->Load(cache->GetResource<XMLFile>("RenderPaths/ForwardDepthSAO4.xml"));
    
    viewport->SetRenderPath(RenderPath);
    
    
    // Search and save AO commands indices
    for (uint i = 0; i < RenderPath->GetNumCommands(); ++i)
    {
        const RenderPathCommand command = RenderPath->commands_[i];
        if (command.tag_ == "SAO_main")
            commandIndexSaoMain = i;
        if (command.tag_ == "SAO_copy")
            commandIndexSaoCopy = i;
    }

}

void DynamicGeometry::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, HANDLER(DynamicGeometry, HandleUpdate));
    
    //SSAO
    SubscribeToEvent(E_RENDERUPDATE, HANDLER(DynamicGeometry, HandleRenderUpdate));
}

void DynamicGeometry::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;
    
    Input* input = GetSubsystem<Input>();
    
    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;
    
    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    IntVector2 mouseMove = input->GetMouseMove();
    yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);
    
    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    
    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown('W'))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('S'))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('A'))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('D'))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void DynamicGeometry::AnimateObjects(float timeStep)
{
    PROFILE(AnimateObjects);
    
    time_ += timeStep * 100.0f;
    
    // Repeat for each of the cloned vertex buffers
    for (unsigned i = 0; i < animatingBuffers_.Size(); ++i)
    {
        float startPhase = time_ + i * 30.0f;
        VertexBuffer* buffer = animatingBuffers_[i];
        
        // Lock the vertex buffer for update and rewrite positions with sine wave modulated ones
        // Cannot use discard lock as there is other data (normals, UVs) that we are not overwriting
        unsigned char* vertexData = (unsigned char*)buffer->Lock(0, buffer->GetVertexCount());
        if (vertexData)
        {
            unsigned vertexSize = buffer->GetVertexSize();
            unsigned numVertices = buffer->GetVertexCount();
            for (unsigned j = 0; j < numVertices; ++j)
            {
                // If there are duplicate vertices, animate them in phase of the original
                float phase = startPhase + vertexDuplicates_[j] * 10.0f;
                Vector3& src = originalVertices_[j];
                Vector3& dest = *reinterpret_cast<Vector3*>(vertexData + j * vertexSize);
                dest.x_ = src.x_ * (1.0f + 0.1f * Sin(phase));
                dest.y_ = src.y_ * (1.0f + 0.1f * Sin(phase + 60.0f));
                dest.z_ = src.z_ * (1.0f + 0.1f * Sin(phase + 120.0f));
            }
            
            buffer->Unlock();
        }
    }
}

void DynamicGeometry::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;
    
    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    
    // Toggle animation with space
    Input* input = GetSubsystem<Input>();
    if (input->GetKeyPress(KEY_SPACE))
        animate_ = !animate_;
    
    // Move the camera, scale movement with time step
    MoveCamera(timeStep);
    
    // Animate objects' vertex data if enabled
    if (animate_)
        AnimateObjects(timeStep);
    
    //SAO
    // Ambient occlusion controllers
    if (input->GetKeyPress('V'))
    {
        ao_enable = !ao_enable;
        var_changed = true;
    }
    
    if (input->GetKeyPress('B'))
    {
        ao_blur = !ao_blur;
        ao_blurdepth = false;
        var_changed = true;
    }
    
    if (input->GetKeyPress('N'))
    {
        ao_blurdepth = !ao_blurdepth;
        ao_blur = false;
        var_changed = true;
    }
    
    if (input->GetKeyPress('M'))
    {
        ao_only = !ao_only;
        var_changed = true;
        define_changed = true;
    }
    
    var_changed = var_changed || VarChange('F', 'R', ao_radius);
    var_changed = var_changed || VarChange('G', 'T', ao_projscale);
    var_changed = var_changed || VarChange('H', 'Y', ao_intensity);
    var_changed = var_changed || VarChange('J', 'U', ao_bias);
    
    var_changed = var_changed || VarChange('K', 'I', ao_var1);
    var_changed = var_changed || VarChange('L', 'O', ao_var2);
    
    if (var_changed)
    {
        var_changed = false;
        
        
        RenderPath->SetEnabled("SAO_copy", ao_enable);
        RenderPath->SetEnabled("BlurGaussian", ao_blur);
        RenderPath->SetEnabled("BlurGaussianDepth", ao_blurdepth);
        
        if (define_changed && commandIndexSaoCopy != -1)
        {
            define_changed = false;
            // TODO: avoid replace?
            RenderPathCommand command = RenderPath->commands_[commandIndexSaoCopy];
            if (ao_only)
                command.pixelShaderDefines_ = "AO_ONLY";
            else
                command.pixelShaderDefines_ = "";
            RenderPath->RemoveCommand(commandIndexSaoCopy);
            RenderPath->InsertCommand(commandIndexSaoCopy, command);
        }
        
        RenderPath->SetShaderParameter("Radius", ao_radius.x_);
        RenderPath->SetShaderParameter("ProjScale2", ao_projscale.x_);
        RenderPath->SetShaderParameter("IntensityDivR6", ao_intensity.x_);
        RenderPath->SetShaderParameter("Var1", ao_var1.x_);
        RenderPath->SetShaderParameter("Var2", ao_var2.x_);
        
    }

}

void DynamicGeometry::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    //camera = cameraNode.GetComponent("Camera");
    //viewport = renderer.viewports[0];
    //RenderPath@ renderPath = viewport.renderPath;
    
    // Projection vector: used to reconstruct pixels positions
    Urho3D::Matrix4 p = camera->GetProjection();
    Urho3D::Vector4 projInfo;
    bool OpenGL = true;
    if (OpenGL)
    {
        // OpenGL
        projInfo = Urho3D::Vector4( 2.0f / p.m00_,
                                   2.0f / p.m11_,
                                   -(1.0f + p.m02_) / p.m00_,
                                   -(1.0f + p.m12_) / p.m11_ );
    }
    else
    {
        // DirectX
        projInfo = Vector4( 2.0f / p.m00_,
                           -2.0f / p.m11_,
                           -(1.0f + p.m02_ + 1.0f / graphics->GetWidth()) / p.m00_,
                           (1.0f - p.m12_ + 1.0f / graphics->GetHeight()) / p.m11_ );
    }
    
    
    RenderPath->SetShaderParameter("ProjInfo", Variant(projInfo));
    //sao->SetShaderParameter("ProjInfo", Variant(projInfo));
    
    // Projection scale: used to scale raidius (TODO: not correct and needs ProjScale2 to fix it)
    float viewSize = 2.0f *  camera->GetHalfViewSize();
    RenderPath->SetShaderParameter("ProjScale", Variant(graphics->GetHeight() / viewSize));
    //sao->SetShaderParameter("ProjScale", Variant(graphics->GetHeight() / viewSize));
    // TODO: viewport.get_rect is zero?
    //IntRect viewRect = viewport.get_rect();
    //renderPath.set_shaderParameters("ProjScale", Variant(viewRect.height / viewSize));
    
    // View matrix: used to rotate normals (we can also reconstruct normals in the shader)
    Urho3D::Matrix3 v = camera->GetView().ToMatrix3();
    
    RenderPath->SetShaderParameter("View", Variant(v));
    //sao->SetShaderParameter("View", Variant(v));
    
    //Urho3D::PrintLine("View: " + sao->GetShaderParameter("View").ToString());
    
    //Urho3D::PrintLine("Occlusion: " + sao->GetShaderParameter("Occlusion").ToString());
    
}

bool DynamicGeometry::VarChange(int keyDown, int keyUp, Urho3D::Vector3& var)
{
    // var = [value, min, max]
    Input* input = GetSubsystem<Input>();
    
    float step = (var.z_ - var.y_) / var_steps;
    if (input->GetKeyDown(keyDown))
    {
        var.x_ -= step;
        if (var.x_ < var.y_)
            var.x_ = var.y_;
        return true;
    }
    if (input->GetKeyDown(keyUp))
    {
        var.x_ += step;
        if (var.x_ > var.z_)
            var.x_ = var.z_;
        return true;
    }
    return false;
}
