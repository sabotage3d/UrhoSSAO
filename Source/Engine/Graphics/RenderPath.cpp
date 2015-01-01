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
#include "Graphics.h"
#include "Log.h"
#include "Material.h"
#include "RenderPath.h"
#include "StringUtils.h"
#include "XMLFile.h"

#include "DebugNew.h"

namespace Urho3D
{

static const char* commandTypeNames[] =
{
    "none",
    "clear",
    "scenepass",
    "quad",
    "forwardlights",
    "lightvolumes",
    0
};

static const char* sortModeNames[] =
{
    "fronttoback",
    "backtofront",
    0
};

TextureUnit ParseTextureUnitName(String name);

void RenderTargetInfo::Load(const XMLElement& element)
{
    name_ = element.GetAttribute("name");
    tag_ = element.GetAttribute("tag");
    if (element.HasAttribute("enabled"))
        enabled_ = element.GetBool("enabled");
    
    String formatName = element.GetAttribute("format");
    format_ = Graphics::GetFormat(formatName);
    
    if (element.HasAttribute("filter"))
        filtered_ = element.GetBool("filter");
    
    if (element.HasAttribute("srgb"))
        sRGB_ = element.GetBool("srgb");
    
    if (element.HasAttribute("persistent"))
        persistent_ = element.GetBool("persistent");
    
    if (element.HasAttribute("size"))
        size_ = element.GetVector2("size");
    if (element.HasAttribute("sizedivisor"))
    {
        size_ = element.GetVector2("sizedivisor");
        sizeMode_ = SIZE_VIEWPORTDIVISOR;
    }
    else if (element.HasAttribute("rtsizedivisor"))
    {
        // Deprecated rtsizedivisor mode, acts the same as sizedivisor mode now
        LOGWARNING("Deprecated rtsizedivisor mode used in rendertarget definition");
        size_ = element.GetVector2("rtsizedivisor");
        sizeMode_ = SIZE_VIEWPORTDIVISOR;
    }
    else if (element.HasAttribute("sizemultiplier"))
    {
        size_ = element.GetVector2("sizemultiplier");
        sizeMode_ = SIZE_VIEWPORTMULTIPLIER;
    }
    
    if (element.HasAttribute("width"))
        size_.x_ = element.GetFloat("width");
    if (element.HasAttribute("height"))
        size_.y_ = element.GetFloat("height");
}

void RenderPathCommand::Load(const XMLElement& element)
{
    type_ = (RenderCommandType)GetStringListIndex(element.GetAttributeLower("type").CString(), commandTypeNames, CMD_NONE);
    tag_ = element.GetAttribute("tag");
    if (element.HasAttribute("enabled"))
        enabled_ = element.GetBool("enabled");
    if (element.HasAttribute("metadata"))
        metadata_ = element.GetAttribute("metadata");
    
    switch (type_)
    {
    case CMD_CLEAR:
        if (element.HasAttribute("color"))
        {
            clearFlags_ |= CLEAR_COLOR;
            if (element.GetAttributeLower("color") == "fog")
                useFogColor_ = true;
            else
                clearColor_ = element.GetColor("color");
        }
        if (element.HasAttribute("depth"))
        {
            clearFlags_ |= CLEAR_DEPTH;
            clearDepth_ = element.GetFloat("depth");
        }
        if (element.HasAttribute("stencil"))
        {
            clearFlags_ |= CLEAR_STENCIL;
            clearStencil_ = element.GetInt("stencil");
        }
        break;
        
    case CMD_SCENEPASS:
        pass_ = element.GetAttribute("pass");
        sortMode_ = (RenderCommandSortMode)GetStringListIndex(element.GetAttributeLower("sort").CString(), sortModeNames, SORT_FRONTTOBACK);
        if (element.HasAttribute("marktostencil"))
            markToStencil_ = element.GetBool("marktostencil");
        if (element.HasAttribute("vertexlights"))
            vertexLights_ = element.GetBool("vertexlights");
        break;
        
    case CMD_FORWARDLIGHTS:
        pass_ = element.GetAttribute("pass");
        if (element.HasAttribute("uselitbase"))
            useLitBase_ = element.GetBool("uselitbase");
        break;
        
    case CMD_LIGHTVOLUMES:
    case CMD_QUAD:
        vertexShaderName_ = element.GetAttribute("vs");
        pixelShaderName_ = element.GetAttribute("ps");
        vertexShaderDefines_ = element.GetAttribute("vsdefines");
        pixelShaderDefines_ = element.GetAttribute("psdefines");
        
        if (type_ == CMD_QUAD)
        {
            XMLElement parameterElem = element.GetChild("parameter");
            while (parameterElem)
            {
                String name = parameterElem.GetAttribute("name");
                shaderParameters_[name] = Material::ParseShaderParameterValue(parameterElem.GetAttribute("value"));
                parameterElem = parameterElem.GetNext("parameter");
            }
        }
        break;
    
    default:
        break;
    }
    
    // By default use 1 output, which is the viewport
    outputNames_.Push("viewport");
    if (element.HasAttribute("output"))
        outputNames_[0] = element.GetAttribute("output");
    // Check for defining multiple outputs
    XMLElement outputElem = element.GetChild("output");
    while (outputElem)
    {
        unsigned index = outputElem.GetInt("index");
        if (index < MAX_RENDERTARGETS)
        {
            if (index >= outputNames_.Size())
                outputNames_.Resize(index + 1);
            outputNames_[index] = outputElem.GetAttribute("name");
        }
        outputElem = outputElem.GetNext("output");
    }
    
    XMLElement textureElem = element.GetChild("texture");
    while (textureElem)
    {
        TextureUnit unit = TU_DIFFUSE;
        if (textureElem.HasAttribute("unit"))
            unit = ParseTextureUnitName(textureElem.GetAttribute("unit"));
        if (unit < MAX_TEXTURE_UNITS)
        {
            String name = textureElem.GetAttribute("name");
            textureNames_[unit] = name;
        }
        
        textureElem = textureElem.GetNext("texture");
    }
}

void RenderPathCommand::SetTextureName(TextureUnit unit, const String& name)
{
    if (unit < MAX_TEXTURE_UNITS)
        textureNames_[unit] = name;
}

void RenderPathCommand::SetShaderParameter(const String& name, const Variant& value)
{
    shaderParameters_[name] = value;
}

void RenderPathCommand::RemoveShaderParameter(const String& name)
{
    shaderParameters_.Erase(name);
}

void RenderPathCommand::SetNumOutputs(unsigned num)
{
    num = Clamp((int)num, 1, MAX_RENDERTARGETS);
    outputNames_.Resize(num);
}

void RenderPathCommand::SetOutputName(unsigned index, const String& name)
{
    if (index < outputNames_.Size())
        outputNames_[index] = name;
    else if (index == outputNames_.Size() && index < MAX_RENDERTARGETS)
        outputNames_.Push(name);
}

const String& RenderPathCommand::GetTextureName(TextureUnit unit) const
{
    return unit < MAX_TEXTURE_UNITS ? textureNames_[unit] : String::EMPTY;
}

const Variant& RenderPathCommand::GetShaderParameter(const String& name) const
{
    HashMap<StringHash, Variant>::ConstIterator i = shaderParameters_.Find(name);
    return i != shaderParameters_.End() ? i->second_ : Variant::EMPTY;
}

const String& RenderPathCommand::GetOutputName(unsigned index) const
{
    return index < outputNames_.Size() ? outputNames_[index] : String::EMPTY;
}

RenderPath::RenderPath()
{
}

RenderPath::~RenderPath()
{
}

SharedPtr<RenderPath> RenderPath::Clone()
{
    SharedPtr<RenderPath> newRenderPath(new RenderPath());
    newRenderPath->renderTargets_ = renderTargets_;
    newRenderPath->commands_ = commands_;
    return newRenderPath;
}

bool RenderPath::Load(XMLFile* file)
{
    renderTargets_.Clear();
    commands_.Clear();
    
    return Append(file);
}

bool RenderPath::Append(XMLFile* file)
{
    if (!file)
        return false;
    
    XMLElement rootElem = file->GetRoot();
    if (!rootElem)
        return false;
    
    XMLElement rtElem = rootElem.GetChild("rendertarget");
    while (rtElem)
    {
        RenderTargetInfo info;
        info.Load(rtElem);
        if (!info.name_.Trimmed().Empty())
            renderTargets_.Push(info);
        
        rtElem = rtElem.GetNext("rendertarget");
    }
    
    XMLElement cmdElem = rootElem.GetChild("command");
    while (cmdElem)
    {
        RenderPathCommand cmd;
        cmd.Load(cmdElem);
        if (cmd.type_ != CMD_NONE)
            commands_.Push(cmd);
        
        cmdElem = cmdElem.GetNext("command");
    }
    
    return true;
}

void RenderPath::SetEnabled(const String& tag, bool active)
{
    for (unsigned i = 0; i < renderTargets_.Size(); ++i)
    {
        if (!renderTargets_[i].tag_.Compare(tag, false))
            renderTargets_[i].enabled_ = active;
    }
    
    for (unsigned i = 0; i < commands_.Size(); ++i)
    {
        if (!commands_[i].tag_.Compare(tag, false))
            commands_[i].enabled_ = active;
    }
}

void RenderPath::ToggleEnabled(const String& tag)
{
    for (unsigned i = 0; i < renderTargets_.Size(); ++i)
    {
        if (!renderTargets_[i].tag_.Compare(tag, false))
            renderTargets_[i].enabled_ = !renderTargets_[i].enabled_;
    }
    
    for (unsigned i = 0; i < commands_.Size(); ++i)
    {
        if (!commands_[i].tag_.Compare(tag, false))
            commands_[i].enabled_ = !commands_[i].enabled_;
    }
}

void RenderPath::SetRenderTarget(unsigned index, const RenderTargetInfo& info)
{
    if (index < renderTargets_.Size())
        renderTargets_[index] = info;
    else if (index == renderTargets_.Size())
        AddRenderTarget(info);
}

void RenderPath::AddRenderTarget(const RenderTargetInfo& info)
{
    renderTargets_.Push(info);
}

void RenderPath::RemoveRenderTarget(unsigned index)
{
    renderTargets_.Erase(index);
}

void RenderPath::RemoveRenderTarget(const String& name)
{
    for (unsigned i = 0; i < renderTargets_.Size(); ++i)
    {
        if (!renderTargets_[i].name_.Compare(name, false))
        {
            renderTargets_.Erase(i);
            return;
        }
    }
}

void RenderPath::RemoveRenderTargets(const String& tag)
{
    for (unsigned i = renderTargets_.Size() - 1; i < renderTargets_.Size(); --i)
    {
        if (!renderTargets_[i].tag_.Compare(tag, false))
            renderTargets_.Erase(i);
    }
}

void RenderPath::SetCommand(unsigned index, const RenderPathCommand& command)
{
    if (index < commands_.Size())
        commands_[index] = command;
    else if (index == commands_.Size())
        AddCommand(command);
}

void RenderPath::AddCommand(const RenderPathCommand& command)
{
    commands_.Push(command);
}

void RenderPath::InsertCommand(unsigned index, const RenderPathCommand& command)
{
    commands_.Insert(index, command);
}

void RenderPath::RemoveCommand(unsigned index)
{
    commands_.Erase(index);
}

void RenderPath::RemoveCommands(const String& tag)
{
    for (unsigned i = commands_.Size() - 1; i < commands_.Size(); --i)
    {
        if (!commands_[i].tag_.Compare(tag, false))
            commands_.Erase(i);
    }
}

void RenderPath::SetShaderParameter(const String& name, const Variant& value)
{
    StringHash nameHash(name);
    
    for (unsigned i = 0; i < commands_.Size(); ++i)
    {
        HashMap<StringHash, Variant>::Iterator j = commands_[i].shaderParameters_.Find(nameHash);
        if (j != commands_[i].shaderParameters_.End())
            j->second_ = value;
    }
}

const Variant& RenderPath::GetShaderParameter(const String& name) const
{
    StringHash nameHash(name);
    
    for (unsigned i = 0; i < commands_.Size(); ++i)
    {
        HashMap<StringHash, Variant>::ConstIterator j = commands_[i].shaderParameters_.Find(nameHash);
        if (j != commands_[i].shaderParameters_.End())
            return j->second_;
    }
    
    return Variant::EMPTY;
}

}
