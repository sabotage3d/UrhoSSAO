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
#include "Deserializer.h"
#include "FileSystem.h"
#include "Log.h"
#include "LuaFile.h"
#include "ProcessUtils.h"
#include "Serializer.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "DebugNew.h"

namespace Urho3D
{

LuaFile::LuaFile(Context* context) :
    Resource(context),
    size_(0),
    hasLoaded_(false),
    hasExecuted_(false)
{

}

LuaFile::~LuaFile()
{

}

void LuaFile::RegisterObject(Context* context)
{
    context->RegisterFactory<LuaFile>();
}

bool LuaFile::BeginLoad(Deserializer& source)
{
    size_ = source.GetSize();

    if (size_ == 0)
        return false;

    // Read all data.
    data_ = new char[size_];
    if (source.Read(data_, size_) != size_)
        return false;

    SetMemoryUse(size_);

    return true;
}

bool LuaFile::Save(Serializer& dest) const
{
    if (size_ == 0)
        return false;

    dest.Write(data_, size_);

    return true;
}


bool LuaFile::LoadChunk(lua_State* luaState)
{
    if (hasLoaded_)
        return true;

    if (size_ == 0)
        return false;

    if (!luaState)
        return false;

    int top = lua_gettop(luaState);

    // Get file base name
    String name = GetName();
    unsigned extPos = name.FindLast('.');
    if (extPos != String::NPOS)
    {
        name = name.Substring(0, extPos);
    }

    int error = luaL_loadbuffer(luaState, data_, size_, name.CString());
    if (error)
    {
        const char* message = lua_tostring(luaState, -1);
        LOGERROR("Load Buffer failed for " + GetName() + ": " + String(message));
        lua_settop(luaState, top);
        return false;
    }

    LOGINFO("Loaded Lua script " + GetName());
    hasLoaded_ = true;

    return true;
}

bool LuaFile::LoadAndExecute(lua_State* luaState)
{
    if (hasExecuted_)
        return true;

    if (!LoadChunk(luaState))
        return false;

    int top = lua_gettop(luaState);

    if (lua_pcall(luaState, 0, 0, 0))
    {
        const char* message = lua_tostring(luaState, -1);
        LOGERROR("Lua Execute failed for " + GetName() + ": " + String(message));
        lua_settop(luaState, top);
        return false;
    }

    hasExecuted_ = true;

    return true;
}

}
