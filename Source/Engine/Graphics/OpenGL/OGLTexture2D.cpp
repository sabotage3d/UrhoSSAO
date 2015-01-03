//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rightsR
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
#include "FileSystem.h"
#include "Graphics.h"
#include "GraphicsEvents.h"
#include "GraphicsImpl.h"
#include "Image.h"
#include "Log.h"
#include "Profiler.h"
#include "Renderer.h"
#include "ResourceCache.h"
#include "Texture2D.h"
#include "XMLFile.h"

#include "DebugNew.h"

namespace Urho3D
{

Texture2D::Texture2D(Context* context) :
    Texture(context)
{
    target_ = GL_TEXTURE_2D;
}

Texture2D::~Texture2D()
{
    Release();
}

void Texture2D::RegisterObject(Context* context)
{
    context->RegisterFactory<Texture2D>();
}

bool Texture2D::BeginLoad(Deserializer& source)
{
    // In headless mode, do not actually load the texture, just return success
    if (!graphics_)
        return true;
    
    // If device is lost, retry later
    if (graphics_->IsDeviceLost())
    {
        LOGWARNING("Texture load while device is lost");
        dataPending_ = true;
        return true;
    }
    
    // Load the image data for EndLoad()
    loadImage_ = new Image(context_);
    if (!loadImage_->Load(source))
    {
        loadImage_.Reset();
        return false;
    }

    // Precalculate mip levels if async loading
    if (GetAsyncLoadState() == ASYNC_LOADING)
        loadImage_->PrecalculateLevels();
    
    // Load the optional parameters file
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    String xmlName = ReplaceExtension(GetName(), ".xml");
    loadParameters_ = cache->GetTempResource<XMLFile>(xmlName, false);
    
    return true;
}

bool Texture2D::EndLoad()
{
    // In headless mode, do not actually load the texture, just return success
     if (!graphics_ || graphics_->IsDeviceLost())
        return true;
    
    // If over the texture budget, see if materials can be freed to allow textures to be freed
    CheckTextureBudget(GetTypeStatic());

    SetParameters(loadParameters_);
    bool success = SetData(loadImage_);
    
    loadImage_.Reset();
    loadParameters_.Reset();
    
    return success;
}

void Texture2D::OnDeviceLost()
{
    GPUObject::OnDeviceLost();
    
    if (renderSurface_)
        renderSurface_->OnDeviceLost();
}

void Texture2D::OnDeviceReset()
{
    if (!object_ || dataPending_)
    {
        // If has a resource file, reload through the resource cache. Otherwise just recreate.
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        if (cache->Exists(GetName()))
            dataLost_ = !cache->ReloadResource(this);

        if (!object_)
        {
            Create();
            dataLost_ = true;
        }
    }
    
    dataPending_ = false;
}

void Texture2D::Release()
{
    if (object_)
    {
        if (!graphics_)
            return;
        
        if (!graphics_->IsDeviceLost())
        {
            for (unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
            {
                if (graphics_->GetTexture(i) == this)
                    graphics_->SetTexture(i, 0);
            }
            
            glDeleteTextures(1, &object_);
        }
        
        if (renderSurface_)
            renderSurface_->Release();
        
        object_ = 0;
    }
    else
    {
        if (renderSurface_)
            renderSurface_->Release();
    }
}

bool Texture2D::SetSize(int width, int height, unsigned format, TextureUsage usage)
{
    // Delete the old rendersurface if any
    renderSurface_.Reset();
    
    usage_ = usage;
    
    if (usage >= TEXTURE_RENDERTARGET)
    {
        renderSurface_ = new RenderSurface(this);
        
        // Clamp mode addressing by default, nearest filtering, and mipmaps disabled
        addressMode_[COORD_U] = ADDRESS_CLAMP;
        addressMode_[COORD_V] = ADDRESS_CLAMP;
        filterMode_ = FILTER_NEAREST;
        requestedLevels_ = 1;
    }
    
    if (usage == TEXTURE_RENDERTARGET)
        SubscribeToEvent(E_RENDERSURFACEUPDATE, HANDLER(Texture2D, HandleRenderSurfaceUpdate));
    else
        UnsubscribeFromEvent(E_RENDERSURFACEUPDATE);
    
    width_ = width;
    height_ = height;
    format_ = format;
    
    return Create();
}

bool Texture2D::SetData(unsigned level, int x, int y, int width, int height, const void* data)
{
    PROFILE(SetTextureData);

    if (!object_ || !graphics_)
    {
        LOGERROR("No texture created, can not set data");
        return false;
    }
    
    if (!data)
    {
        LOGERROR("Null source for setting data");
        return false;
    }
    
    if (level >= levels_)
    {
        LOGERROR("Illegal mip level for setting data");
        return false;
    }
    
    if (graphics_->IsDeviceLost())
    {
        LOGWARNING("Texture data assignment while device is lost");
        dataPending_ = true;
        return true;
    }
    
    if (IsCompressed())
    {
        x &= ~3;
        y &= ~3;
    }
    
    int levelWidth = GetLevelWidth(level);
    int levelHeight = GetLevelHeight(level);
    if (x < 0 || x + width > levelWidth || y < 0 || y + height > levelHeight || width <= 0 || height <= 0)
    {
        LOGERROR("Illegal dimensions for setting data");
        return false;
    }
    
    graphics_->SetTextureForUpdate(this);
    
    bool wholeLevel = x == 0 && y == 0 && width == levelWidth && height == levelHeight;
    unsigned format = GetSRGB() ? GetSRGBFormat(format_) : format_;
    
    if (!IsCompressed())
    {
        if (wholeLevel)
            glTexImage2D(target_, level, format, width, height, 0, GetExternalFormat(format_), GetDataType(format_), data);
        else
            glTexSubImage2D(target_, level, x, y, width, height, GetExternalFormat(format_), GetDataType(format_), data);
    }
    else
    {
        if (wholeLevel)
            glCompressedTexImage2D(target_, level, format, width, height, 0, GetDataSize(width, height), data);
        else
            glCompressedTexSubImage2D(target_, level, x, y, width, height, format, GetDataSize(width, height), data);
    }
    
    graphics_->SetTexture(0, 0);
    return true;
}

bool Texture2D::SetData(SharedPtr<Image> image, bool useAlpha)
{
    if (!image)
    {
        LOGERROR("Null image, can not set data");
        return false;
    }

    unsigned memoryUse = sizeof(Texture2D);
    
    int quality = QUALITY_HIGH;
    Renderer* renderer = GetSubsystem<Renderer>();
    if (renderer)
        quality = renderer->GetTextureQuality();
    
    if (!image->IsCompressed())
    {
        unsigned char* levelData = image->GetData();
        int levelWidth = image->GetWidth();
        int levelHeight = image->GetHeight();
        unsigned components = image->GetComponents();
        unsigned format = 0;
        
        // Discard unnecessary mip levels
        for (unsigned i = 0; i < mipsToSkip_[quality]; ++i)
        {
            image = image->GetNextLevel();
            levelData = image->GetData();
            levelWidth = image->GetWidth();
            levelHeight = image->GetHeight();
        }
        
        switch (components)
        {
        case 1:
            format = useAlpha ? Graphics::GetAlphaFormat() : Graphics::GetLuminanceFormat();
            break;
            
        case 2:
            format = Graphics::GetLuminanceAlphaFormat();
            break;
            
        case 3:
            format = Graphics::GetRGBFormat();
            break;
            
        case 4:
            format = Graphics::GetRGBAFormat();
            break;
        }
        
        // If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
        if (IsCompressed() && requestedLevels_ > 1)
            requestedLevels_ = 0;
        SetSize(levelWidth, levelHeight, format);
        if (!object_)
            return false;
        
        for (unsigned i = 0; i < levels_; ++i)
        {
            SetData(i, 0, 0, levelWidth, levelHeight, levelData);
            memoryUse += levelWidth * levelHeight * components;
            
            if (i < levels_ - 1)
            {
                image = image->GetNextLevel();
                levelData = image->GetData();
                levelWidth = image->GetWidth();
                levelHeight = image->GetHeight();
            }
        }
    }
    else
    {
        int width = image->GetWidth();
        int height = image->GetHeight();
        unsigned levels = image->GetNumCompressedLevels();
        unsigned format = graphics_->GetFormat(image->GetCompressedFormat());
        bool needDecompress = false;
        
        if (!format)
        {
            format = Graphics::GetRGBAFormat();
            needDecompress = true;
        }
        
        unsigned mipsToSkip = mipsToSkip_[quality];
        if (mipsToSkip >= levels)
            mipsToSkip = levels - 1;
        while (mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4))
            --mipsToSkip;
        width /= (1 << mipsToSkip);
        height /= (1 << mipsToSkip);
        
        SetNumLevels(Max((int)(levels - mipsToSkip), 1));
        SetSize(width, height, format);
        
        for (unsigned i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
        {
            CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
            if (!needDecompress)
            {
                SetData(i, 0, 0, level.width_, level.height_, level.data_);
                memoryUse += level.rows_ * level.rowSize_;
            }
            else
            {
                unsigned char* rgbaData = new unsigned char[level.width_ * level.height_ * 4];
                level.Decompress(rgbaData);
                SetData(i, 0, 0, level.width_, level.height_, rgbaData);
                memoryUse += level.width_ * level.height_ * 4;
                delete[] rgbaData;
            }
        }
    }
    
    SetMemoryUse(memoryUse);
    return true;
}

bool Texture2D::GetData(unsigned level, void* dest) const
{
    #ifndef GL_ES_VERSION_2_0
    if (!object_ || !graphics_)
    {
        LOGERROR("No texture created, can not get data");
        return false;
    }
    
    if (!dest)
    {
        LOGERROR("Null destination for getting data");
        return false;
    }
    
    if (level >= levels_)
    {
        LOGERROR("Illegal mip level for getting data");
        return false;
    }
    
    if (graphics_->IsDeviceLost())
    {
        LOGWARNING("Getting texture data while device is lost");
        return false;
    }
    
    graphics_->SetTextureForUpdate(const_cast<Texture2D*>(this));
    
    if (!IsCompressed())
        glGetTexImage(target_, level, GetExternalFormat(format_), GetDataType(format_), dest);
    else
        glGetCompressedTexImage(target_, level, dest);
    
    graphics_->SetTexture(0, 0);
    return true;
    #else
    LOGERROR("Getting texture data not supported");
    return false;
    #endif
}

bool Texture2D::Create()
{
    Release();
    
    if (!graphics_ || !width_ || !height_)
        return false;
    
    if (graphics_->IsDeviceLost())
    {
        LOGWARNING("Texture creation while device is lost");
        return true;
    }

    unsigned format = GetSRGB() ? GetSRGBFormat(format_) : format_;
    unsigned externalFormat = GetExternalFormat(format_);
    unsigned dataType = GetDataType(format_);
    
    // Create a renderbuffer instead of a texture if depth texture is not properly supported, or if this will be a packed
    // depth stencil texture
    #ifndef GL_ES_VERSION_2_0
    if (format == Graphics::GetDepthStencilFormat())
    #else
    if (format == GL_DEPTH_COMPONENT16 || format == GL_DEPTH_COMPONENT24_OES || format == GL_DEPTH24_STENCIL8_OES ||
        (format == GL_DEPTH_COMPONENT && !graphics_->GetShadowMapFormat()))
    #endif
    {
        if (renderSurface_)
        {
            renderSurface_->CreateRenderBuffer(width_, height_, format);
            return true;
        }
        else
            return false;
    }
    
    glGenTextures(1, &object_);
    
    // Ensure that our texture is bound to OpenGL texture unit 0
    graphics_->SetTextureForUpdate(this);
    
    // If not compressed, create the initial level 0 texture with null data
    bool success = true;
    
    if (!IsCompressed())
    {
        glGetError();
        glTexImage2D(target_, 0, format, width_, height_, 0, externalFormat, dataType, 0);
        if (glGetError())
        {
            LOGERROR("Failed to create texture");
            success = false;
        }
    }
    
    // Set mipmapping
    levels_ = requestedLevels_;
    if (!levels_)
    {
        unsigned maxSize = Max((int)width_, (int)height_);
        while (maxSize)
        {
            maxSize >>= 1;
            ++levels_;
        }
    }
    
    #ifndef GL_ES_VERSION_2_0
    glTexParameteri(target_, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(target_, GL_TEXTURE_MAX_LEVEL, levels_ - 1);
    #endif
    
    // Set initial parameters, then unbind the texture
    UpdateParameters();
    graphics_->SetTexture(0, 0);
    
    return success;
}

void Texture2D::HandleRenderSurfaceUpdate(StringHash eventType, VariantMap& eventData)
{
    if (renderSurface_ && renderSurface_->GetUpdateMode() == SURFACE_UPDATEALWAYS)
        renderSurface_->QueueUpdate();
}

}
