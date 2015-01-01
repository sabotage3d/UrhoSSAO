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

#include "Precompiled.h"
#include "Camera.h"
#include "Context.h"
#include "Drawable2D.h"
#include "Geometry.h"
#include "GraphicsEvents.h"
#include "IndexBuffer.h"
#include "Log.h"
#include "Material.h"
#include "Node.h"
#include "Profiler.h"
#include "Renderer2D.h"
#include "Scene.h"
#include "Sort.h"
#include "Technique.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "WorkQueue.h"

#include "DebugNew.h"

namespace Urho3D
{

extern const char* blendModeNames[];

Renderer2D::Renderer2D(Context* context) :
    Drawable(context, DRAWABLE_GEOMETRY),
    indexBuffer_(new IndexBuffer(context_)),
    vertexBuffer_(new VertexBuffer(context_)),
    orderDirty_(true),
    frustum_(0),
    indexCount_(0),
    vertexCount_(0)
{
    SubscribeToEvent(E_BEGINVIEWUPDATE, HANDLER(Renderer2D, HandleBeginViewUpdate));
}

Renderer2D::~Renderer2D()
{
}

void Renderer2D::RegisterObject(Context* context)
{
    context->RegisterFactory<Renderer2D>();
}

static inline bool CompareDrawable2Ds(Drawable2D* lhs, Drawable2D* rhs)
{
    if (lhs->GetLayer() != rhs->GetLayer())
        return lhs->GetLayer() < rhs->GetLayer();

    if (lhs->GetOrderInLayer() != rhs->GetOrderInLayer())
        return lhs->GetOrderInLayer() < rhs->GetOrderInLayer();

    Material* lhsUsedMaterial = lhs->GetMaterial();
    Material* rhsUsedMaterial = rhs->GetMaterial();
    if (lhsUsedMaterial != rhsUsedMaterial)
        return lhsUsedMaterial->GetNameHash() < rhsUsedMaterial->GetNameHash();

    return lhs->GetID() < rhs->GetID();
}

void Renderer2D::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
    if (orderDirty_)
    {
        Sort(drawables_.Begin(), drawables_.End(), CompareDrawable2Ds);
        orderDirty_ = false;
    }

    unsigned resultSize = results.Size();
    for (unsigned i = drawables_.Size() - 1; i < drawables_.Size(); --i)
    {
        drawables_[i]->ProcessRayQuery(query, results);
        if (results.Size() != resultSize)
            return;
    }
}

void Renderer2D::UpdateBatches(const FrameInfo& frame)
{
    unsigned count = batches_.Size();

    // Update non-thread critical parts of the source batches
    for (unsigned i = 0; i < count; ++i)
    {
        batches_[i].distance_ = 10.0f + (count - i) * 0.001f;
        batches_[i].worldTransform_ = &Matrix3x4::IDENTITY;
    }
}

void Renderer2D::UpdateGeometry(const FrameInfo& frame)
{
    // Fill index buffer
    if (indexBuffer_->GetIndexCount() < indexCount_ || indexBuffer_->IsDataLost())
    {
        bool largeIndices = vertexCount_ > 0xffff;
        indexBuffer_->SetSize(indexCount_, largeIndices);
        void* buffer = indexBuffer_->Lock(0, indexCount_, true);
        if (buffer)
        {
            unsigned quadCount = indexCount_ / 6;
            if (largeIndices)
            {
                unsigned* dest = reinterpret_cast<unsigned*>(buffer);
                for (unsigned i = 0; i < quadCount; ++i)
                {
                    unsigned base = i * 4;
                    dest[0] = base;
                    dest[1] = base + 1;
                    dest[2] = base + 2;
                    dest[3] = base;
                    dest[4] = base + 2;
                    dest[5] = base + 3;
                    dest += 6;
                }
            }
            else
            {
                unsigned short* dest = reinterpret_cast<unsigned short*>(buffer);
                for (unsigned i = 0; i < quadCount; ++i)
                {
                    unsigned base = i * 4;
                    dest[0] = (unsigned short)(base);
                    dest[1] = (unsigned short)(base + 1);
                    dest[2] = (unsigned short)(base + 2);
                    dest[3] = (unsigned short)(base);
                    dest[4] = (unsigned short)(base + 2);
                    dest[5] = (unsigned short)(base + 3);
                    dest += 6;
                }
            }

            indexBuffer_->Unlock();
        }
        else
        {
            LOGERROR("Failed to lock index buffer");
            return;
        }
    }

    if (vertexBuffer_->GetVertexCount() < vertexCount_)
        vertexBuffer_->SetSize(vertexCount_, MASK_VERTEX2D);

    if (vertexCount_)
    {
        Vertex2D* dest = reinterpret_cast<Vertex2D*>(vertexBuffer_->Lock(0, vertexCount_, true));
        if (dest)
        {
            for (unsigned d = 0; d < drawables_.Size(); ++d)
            {
                if (!drawables_[d]->GetVisibility() || drawables_[d]->GetVertices().Empty())
                    continue;

                const Vector<Vertex2D>& vertices = drawables_[d]->GetVertices();
                for (unsigned i = 0; i < vertices.Size(); ++i)
                    dest[i] = vertices[i];
                dest += vertices.Size();
            }

            vertexBuffer_->Unlock();
        }
        else
            LOGERROR("Failed to lock vertex buffer");
    }
}

UpdateGeometryType Renderer2D::GetUpdateGeometryType()
{
    return UPDATE_MAIN_THREAD;
}

void Renderer2D::AddDrawable(Drawable2D* drawable)
{
    if (!drawable)
        return;

    drawables_.Push(drawable);
    materialDirtyDrawables_.Push(drawable);

    orderDirty_ = true;
}

void Renderer2D::RemoveDrawable(Drawable2D* drawable)
{
    if (!drawable)
        return;

    drawables_.Remove(drawable);
    materialDirtyDrawables_.Remove(drawable);
    orderDirty_ = true;
}

void Renderer2D::MarkMaterialDirty(Drawable2D* drawable)
{
    materialDirtyDrawables_.Push(drawable);
}

bool Renderer2D::CheckVisibility(Drawable2D* drawable) const
{
    const BoundingBox& box = drawable->GetWorldBoundingBox();
    if (frustum_)
        return frustum_->IsInsideFast(box) != OUTSIDE;

    return frustumBoundingBox_.IsInsideFast(box) != OUTSIDE;
}

void Renderer2D::OnWorldBoundingBoxUpdate()
{
    // Set a large dummy bounding box to ensure the renderer is rendered
    boundingBox_.Define(-M_LARGE_VALUE, M_LARGE_VALUE);
    worldBoundingBox_ = boundingBox_;
}

static void CheckDrawableVisibility(const WorkItem* item, unsigned threadIndex)
{
    Renderer2D* renderer = reinterpret_cast<Renderer2D*>(item->aux_);
    Drawable2D** start = reinterpret_cast<Drawable2D**>(item->start_);
    Drawable2D** end = reinterpret_cast<Drawable2D**>(item->end_);

    while (start != end)
    {
        Drawable2D* drawable = *start++;
        if (renderer->CheckVisibility(drawable) && drawable->GetVertices().Size())
            drawable->SetVisibility(true);
        else
            drawable->SetVisibility(false);
    }
}

void Renderer2D::HandleBeginViewUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace BeginViewUpdate;

    Scene* scene = GetScene();
    // Check that we are updating the correct scene
    if (scene != eventData[P_SCENE].GetPtr())
        return;

    PROFILE(UpdateRenderer2D);

    if (!materialDirtyDrawables_.Empty())
    {
        for (unsigned i = 0; i < materialDirtyDrawables_.Size(); ++i)
        {
            Drawable2D* drawable = materialDirtyDrawables_[i];
            if (!drawable->GetMaterial())
                drawable->SetMaterial(GetMaterial(drawable->GetTexture(), drawable->GetBlendMode()));
        }
        materialDirtyDrawables_.Clear();
    }

    if (orderDirty_)
    {
        Sort(drawables_.Begin(), drawables_.End(), CompareDrawable2Ds);
        orderDirty_ = false;
    }

    Camera* camera = static_cast<Camera*>(eventData[P_CAMERA].GetPtr());
    frustum_ = &camera->GetFrustum();
    if (camera->IsOrthographic() && camera->GetNode()->GetWorldDirection() == Vector3::FORWARD)
    {
        // Define bounding box with min and max points
        frustumBoundingBox_.Define(frustum_->vertices_[2], frustum_->vertices_[4]);
        frustum_ = 0;
    }

    // Check visibility
    {
        PROFILE(CheckDrawableVisibility);

        WorkQueue* queue = GetSubsystem<WorkQueue>();
        int numWorkItems = queue->GetNumThreads() + 1; // Worker threads + main thread
        int drawablesPerItem = drawables_.Size() / numWorkItems;
        
        PODVector<Drawable2D*>::Iterator start = drawables_.Begin();
        for (int i = 0; i < numWorkItems; ++i)
        {
            SharedPtr<WorkItem> item = queue->GetFreeItem();
            item->priority_ = M_MAX_UNSIGNED;
            item->workFunction_ = CheckDrawableVisibility;
            item->aux_ = this;

            PODVector<Drawable2D*>::Iterator end = drawables_.End();
            if (i < numWorkItems - 1 && end - start > drawablesPerItem)
                end = start + drawablesPerItem;
            
            item->start_ = &(*start);
            item->end_ = &(*end);
            queue->AddWorkItem(item);
            
            start = end;
        }

        queue->Complete(M_MAX_UNSIGNED);
    }

    vertexCount_ = 0;
    for (unsigned i = 0; i < drawables_.Size(); ++i)
    {
        if (drawables_[i]->GetVisibility())
            vertexCount_ += drawables_[i]->GetVertices().Size();
    }
    indexCount_ = vertexCount_ / 4 * 6;

    // Go through the drawables to form geometries & batches, but upload the actual vertex data later
    materials_.Clear();

    Material* material = 0;
    unsigned iStart = 0;
    unsigned iCount = 0;
    unsigned vStart = 0;
    unsigned vCount = 0;

    for (unsigned d = 0; d < drawables_.Size(); ++d)
    {
        if (!drawables_[d]->GetVisibility())
            continue;

        Material* usedMaterial = drawables_[d]->GetMaterial();
        const Vector<Vertex2D>& vertices = drawables_[d]->GetVertices();

        if (material != usedMaterial)
        {
            if (material)
            {
                AddBatch(material, iStart, iCount, vStart, vCount);
                iStart += iCount;
                iCount = 0;
                vStart += vCount;
                vCount = 0;
            }

            material = usedMaterial;
        }

        iCount += vertices.Size() / 4 * 6;
        vCount += vertices.Size();
    }

    if (material)
        AddBatch(material, iStart, iCount, vStart, vCount);

    // Now the amount of batches is known. Build the part of source batches that are sensitive to threading issues
    // (material & geometry pointers)
    unsigned count = materials_.Size();
    batches_.Resize(count);

    for (unsigned i = 0; i < count; ++i)
    {
        batches_[i].material_ = materials_[i];
        batches_[i].geometry_ = geometries_[i];
    }
}

void Renderer2D::GetDrawables(PODVector<Drawable2D*>& dest, Node* node)
{
    if (!node || !node->IsEnabled())
        return;

    const Vector<SharedPtr<Component> >& components = node->GetComponents();
    for (Vector<SharedPtr<Component> >::ConstIterator i = components.Begin(); i != components.End(); ++i)
    {
        Drawable2D* drawable = dynamic_cast<Drawable2D*>(i->Get());
        if (drawable && drawable->IsEnabled())
            dest.Push(drawable);
    }

    const Vector<SharedPtr<Node> >& children = node->GetChildren();
    for (Vector<SharedPtr<Node> >::ConstIterator i = children.Begin(); i != children.End(); ++i)
        GetDrawables(dest, i->Get());
}

Material* Renderer2D::GetMaterial(Texture2D* texture, BlendMode blendMode)
{
    HashMap<Texture2D*, HashMap<int, SharedPtr<Material> > >::Iterator t = cachedMaterials_.Find(texture);
    if (t == cachedMaterials_.End())
    {
        SharedPtr<Material> material(CreateMaterial(texture, blendMode));
        cachedMaterials_[texture][blendMode] = material;
        return material;
    }

    HashMap<int, SharedPtr<Material> >& materials = t->second_;
    HashMap<int, SharedPtr<Material> >::Iterator b = materials.Find(blendMode);
    if (b != materials.End())
        return b->second_;

    SharedPtr<Material> material(CreateMaterial(texture, blendMode));
    materials[blendMode] = material;

    return material;
}

Material* Renderer2D::CreateMaterial(Texture2D* texture, BlendMode blendMode)
{
    Material* material = new Material(context_);
    if (texture)
        material->SetName(texture->GetName() + "_" + blendModeNames[blendMode]);
    else
        material->SetName(blendModeNames[blendMode]);

    Technique* tech = new Technique(context_);
    Pass* pass = tech->CreatePass(PASS_ALPHA);
    pass->SetBlendMode(blendMode);

    pass->SetVertexShader("Urho2D");
    pass->SetPixelShader("Urho2D");

    pass->SetDepthWrite(false);

    material->SetTechnique(0, tech);
    material->SetCullMode(CULL_NONE);

    material->SetTexture(TU_DIFFUSE, texture);

    return material;
}

void Renderer2D::AddBatch(Material* material, unsigned indexStart, unsigned indexCount, unsigned vertexStart, unsigned vertexCount)
{
    if (!material || indexCount == 0 || vertexCount == 0)
        return;

    materials_.Push(SharedPtr<Material>(material));

    unsigned batchSize = materials_.Size();
    if (geometries_.Size() < batchSize)
    {
        SharedPtr<Geometry> geometry(new Geometry(context_));
        geometry->SetIndexBuffer(indexBuffer_);
        geometry->SetVertexBuffer(0, vertexBuffer_, MASK_VERTEX2D);
        geometries_.Push(geometry);
    }

    geometries_[batchSize - 1]->SetDrawRange(TRIANGLE_LIST, indexStart, indexCount, vertexStart, vertexCount, false);
}

}
