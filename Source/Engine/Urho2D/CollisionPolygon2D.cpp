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
#include "Context.h"
#include "CollisionPolygon2D.h"
#include "PhysicsUtils2D.h"

#include "DebugNew.h"

namespace Urho3D
{

extern const char* URHO2D_CATEGORY;

CollisionPolygon2D::CollisionPolygon2D(Context* context) :
    CollisionShape2D(context)
{
    fixtureDef_.shape = &polygonShape_;
}

CollisionPolygon2D::~CollisionPolygon2D()
{
}

void CollisionPolygon2D::RegisterObject(Context* context)
{
    context->RegisterFactory<CollisionPolygon2D>(URHO2D_CATEGORY);
    COPY_BASE_ATTRIBUTES(CollisionShape2D);
}

void CollisionPolygon2D::SetVertexCount(unsigned count)
{
    vertices_.Resize(count);
}

void CollisionPolygon2D::SetVertex(unsigned index, const Vector2& vertex)
{
    if (index >= vertices_.Size())
        return;

    vertices_[index] = vertex;

    if (index == vertices_.Size() - 1)
    {
        MarkNetworkUpdate();
        RecreateFixture();
    }
}

void CollisionPolygon2D::SetVertices(const PODVector<Vector2>& vertices)
{
    vertices_ = vertices;

    MarkNetworkUpdate();
    RecreateFixture();
}

void CollisionPolygon2D::ApplyNodeWorldScale()
{
    RecreateFixture();
}

void CollisionPolygon2D::RecreateFixture()
{
    ReleaseFixture();
    
    if (vertices_.Size() < 3)
        return;

    PODVector<b2Vec2> b2Vertices;
    unsigned count = vertices_.Size();
    b2Vertices.Resize(count);

    Vector2 worldScale(cachedWorldScale_.x_, cachedWorldScale_.y_);
    for (unsigned i = 0; i < count; ++i)
        b2Vertices[i] = ToB2Vec2(vertices_[i] * worldScale);

    polygonShape_.Set(&b2Vertices[0], count);

    CreateFixture();
}

}
