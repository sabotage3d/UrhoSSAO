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
#include "Font.h"
#include "FontFace.h"
#include "Log.h"
#include "Profiler.h"
#include "ResourceCache.h"
#include "Text.h"
#include "Texture2D.h"

#include "DebugNew.h"

namespace Urho3D
{

const char* textEffects[] =
{
    "None",
    "Shadow",
    "Stroke",
    0
};

static const float MIN_ROW_SPACING = 0.5f;

extern const char* horizontalAlignments[];
extern const char* UI_CATEGORY;

Text::Text(Context* context) :
    UIElement(context),
    usedInText3D_(false),
    fontSize_(DEFAULT_FONT_SIZE),
    textAlignment_(HA_LEFT),
    rowSpacing_(1.0f),
    wordWrap_(false),
    charLocationsDirty_(true),
    selectionStart_(0),
    selectionLength_(0),
    selectionColor_(Color::TRANSPARENT),
    hoverColor_(Color::TRANSPARENT),
    textEffect_(TE_NONE),
    effectColor_(Color::BLACK),
    effectDepthBias_(0.0f),
    rowHeight_(0)
{
    // By default Text does not derive opacity from parent elements
    useDerivedOpacity_ = false;
}

Text::~Text()
{
}

void Text::RegisterObject(Context* context)
{
    context->RegisterFactory<Text>(UI_CATEGORY);

    COPY_BASE_ATTRIBUTES(UIElement);
    UPDATE_ATTRIBUTE_DEFAULT_VALUE("Use Derived Opacity", false);
    MIXED_ACCESSOR_ATTRIBUTE("Font", GetFontAttr, SetFontAttr, ResourceRef, ResourceRef(Font::GetTypeStatic()), AM_FILE);
    ATTRIBUTE("Font Size", int, fontSize_, DEFAULT_FONT_SIZE, AM_FILE);
    ATTRIBUTE("Text", String, text_, String::EMPTY, AM_FILE);
    ENUM_ATTRIBUTE("Text Alignment", textAlignment_, horizontalAlignments, HA_LEFT, AM_FILE);
    ATTRIBUTE("Row Spacing", float, rowSpacing_, 1.0f, AM_FILE);
    ATTRIBUTE("Word Wrap", bool, wordWrap_, false, AM_FILE);
    ACCESSOR_ATTRIBUTE("Selection Color", GetSelectionColor, SetSelectionColor, Color, Color::TRANSPARENT, AM_FILE);
    ACCESSOR_ATTRIBUTE("Hover Color", GetHoverColor, SetHoverColor, Color, Color::TRANSPARENT, AM_FILE);
    ENUM_ATTRIBUTE("Text Effect", textEffect_, textEffects, TE_NONE, AM_FILE);
    ACCESSOR_ATTRIBUTE("Effect Color", GetEffectColor, SetEffectColor, Color, Color::BLACK, AM_FILE);

    // Change the default value for UseDerivedOpacity
    context->GetAttribute<Text>("Use Derived Opacity")->defaultValue_ = false;
}

void Text::ApplyAttributes()
{
    UIElement::ApplyAttributes();

    // Decode to Unicode now
    unicodeText_.Clear();
    for (unsigned i = 0; i < text_.Length();)
        unicodeText_.Push(text_.NextUTF8Char(i));

    fontSize_ = Max(fontSize_, 1);
    ValidateSelection();
    UpdateText();
}

void Text::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{
    FontFace* face = font_ ? font_->GetFace(fontSize_) : (FontFace*)0;
    if (!face)
    {
        hovering_ = false;
        return;
    }

    // If face has changed or char locations are not valid anymore, update before rendering
    if (charLocationsDirty_ || !fontFace_ || face != fontFace_)
        UpdateCharLocations();
    // If face uses mutable glyphs mechanism, reacquire glyphs before rendering to make sure they are in the texture
    else if (face->HasMutableGlyphs())
    {
        for (unsigned i = 0; i < printText_.Size(); ++i)
            face->GetGlyph(printText_[i]);
    }

    // Hovering and/or whole selection batch
    if ((hovering_ && hoverColor_.a_ > 0.0) || (selected_ && selectionColor_.a_ > 0.0f))
    {
        bool both = hovering_ && selected_ && hoverColor_.a_ > 0.0 && selectionColor_.a_ > 0.0f;
        UIBatch batch(this, BLEND_ALPHA, currentScissor, 0, &vertexData);
        batch.SetColor(both ? selectionColor_.Lerp(hoverColor_, 0.5f) : (selected_ && selectionColor_.a_ > 0.0f ?
            selectionColor_: hoverColor_));
        batch.AddQuad(0, 0, GetWidth(), GetHeight(), 0, 0);
        UIBatch::AddOrMerge(batch, batches);
    }

    // Partial selection batch
    if (!selected_ && selectionLength_ && charLocations_.Size() >= selectionStart_ + selectionLength_ && selectionColor_.a_ > 0.0f)
    {
        UIBatch batch(this, BLEND_ALPHA, currentScissor, 0, &vertexData);
        batch.SetColor(selectionColor_);

        IntVector2 currentStart = charLocations_[selectionStart_].position_;
        IntVector2 currentEnd = currentStart;
        for (unsigned i = selectionStart_; i < selectionStart_ + selectionLength_; ++i)
        {
            // Check if row changes, and start a new quad in that case
            if (charLocations_[i].size_ != IntVector2::ZERO)
            {
                if (charLocations_[i].position_.y_ != currentStart.y_)
                {
                    batch.AddQuad(currentStart.x_, currentStart.y_, currentEnd.x_ - currentStart.x_, currentEnd.y_ - currentStart.y_,
                        0, 0);
                    currentStart = charLocations_[i].position_;
                    currentEnd = currentStart + charLocations_[i].size_;
                }
                else
                {
                    currentEnd.x_ += charLocations_[i].size_.x_;
                    currentEnd.y_ = Max(currentStart.y_ + charLocations_[i].size_.y_, currentEnd.y_);
                }
            }
        }
        if (currentEnd != currentStart)
        {
            batch.AddQuad(currentStart.x_, currentStart.y_, currentEnd.x_ - currentStart.x_, currentEnd.y_ - currentStart.y_,
                0, 0);
        }

        UIBatch::AddOrMerge(batch, batches);
    }

    // Text batch
    TextEffect textEffect = font_->IsSDFFont() ? TE_NONE : textEffect_;
    const Vector<SharedPtr<Texture2D> >& textures = face->GetTextures();
    for (unsigned n = 0; n < textures.Size() && n < pageGlyphLocations_.Size(); ++n)
    {
        // One batch per texture/page
        UIBatch pageBatch(this, BLEND_ALPHA, currentScissor, textures[n], &vertexData);

        const PODVector<GlyphLocation>& pageGlyphLocation = pageGlyphLocations_[n];

        switch (textEffect)
        {
        case TE_NONE:
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 0);
            break;

        case TE_SHADOW:
            ConstructBatch(pageBatch, pageGlyphLocation, 1, 1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 0);
            break;

        case TE_STROKE:
            ConstructBatch(pageBatch, pageGlyphLocation, -1, -1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 0, -1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 1, -1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, -1, 0, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 1, 0, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, -1, 1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 1, 1, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 0);
            break;
        }

        UIBatch::AddOrMerge(pageBatch, batches);
    }

    // Reset hovering for next frame
    hovering_ = false;
}

void Text::OnResize()
{
    if (wordWrap_)
        UpdateText(true);
    else
        charLocationsDirty_ = true;
}

void Text::OnIndentSet()
{
    charLocationsDirty_ = true;
}

bool Text::SetFont(const String& fontName, int size)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    return SetFont(cache->GetResource<Font>(fontName), size);
}

bool Text::SetFont(Font* font, int size)
{
    if (!font)
    {
        LOGERROR("Null font for Text");
        return false;
    }

    if (font != font_ || size != fontSize_)
    {
        font_ = font;
        fontSize_ = Max(size, 1);
        UpdateText();
    }

    return true;
}

void Text::SetText(const String& text)
{
    text_ = text;

    // Decode to Unicode now
    unicodeText_.Clear();
    for (unsigned i = 0; i < text_.Length();)
        unicodeText_.Push(text_.NextUTF8Char(i));

    ValidateSelection();
    UpdateText();
}

void Text::SetTextAlignment(HorizontalAlignment align)
{
    if (align != textAlignment_)
    {
        textAlignment_ = align;
        charLocationsDirty_ = true;
    }
}

void Text::SetRowSpacing(float spacing)
{
    if (spacing != rowSpacing_)
    {
        rowSpacing_ = Max(spacing, MIN_ROW_SPACING);
        UpdateText();
    }
}

void Text::SetWordwrap(bool enable)
{
    if (enable != wordWrap_)
    {
        wordWrap_ = enable;
        UpdateText();
    }
}

void Text::SetSelection(unsigned start, unsigned length)
{
    selectionStart_ = start;
    selectionLength_ = length;
    ValidateSelection();
}

void Text::ClearSelection()
{
    selectionStart_ = 0;
    selectionLength_ = 0;
}

void Text::SetSelectionColor(const Color& color)
{
    selectionColor_ = color;
}

void Text::SetHoverColor(const Color& color)
{
    hoverColor_ = color;
}

void Text::SetTextEffect(TextEffect textEffect)
{
    textEffect_ = textEffect;
}

void Text::SetEffectColor(const Color& effectColor)
{
    effectColor_ = effectColor;
}

void Text::SetUsedInText3D(bool usedInText3D)
{
    usedInText3D_ = usedInText3D;
}

void Text::SetEffectDepthBias(float bias)
{
    effectDepthBias_ = bias;
}

int Text::GetRowWidth(unsigned index) const
{
    return index < rowWidths_.Size() ? rowWidths_[index] : 0;
}

IntVector2 Text::GetCharPosition(unsigned index)
{
    if (charLocationsDirty_)
        UpdateCharLocations();
    if (charLocations_.Empty())
        return IntVector2::ZERO;
    // For convenience, return the position of the text ending if index exceeded
    if (index > charLocations_.Size() - 1)
        index = charLocations_.Size() - 1;
    return charLocations_[index].position_;
}

IntVector2 Text::GetCharSize(unsigned index)
{
    if (charLocationsDirty_)
        UpdateCharLocations();
    if (charLocations_.Size() < 2)
        return IntVector2::ZERO;
    // For convenience, return the size of the last char if index exceeded (last size entry is zero)
    if (index > charLocations_.Size() - 2)
        index = charLocations_.Size() - 2;
    return charLocations_[index].size_;
}

void Text::SetFontAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    font_ = cache->GetResource<Font>(value.name_);
}

ResourceRef Text::GetFontAttr() const
{
    return GetResourceRef(font_, Font::GetTypeStatic());
}

bool Text::FilterImplicitAttributes(XMLElement& dest) const
{
    if (!UIElement::FilterImplicitAttributes(dest))
        return false;

    if (!IsFixedWidth())
    {
        if (!RemoveChildXML(dest, "Size"))
            return false;
        if (!RemoveChildXML(dest, "Min Size"))
            return false;
        if (!RemoveChildXML(dest, "Max Size"))
            return false;
    }

    return true;
}

void Text::UpdateText(bool onResize)
{
    rowWidths_.Clear();
    printText_.Clear();

    if (font_)
    {
        FontFace* face = font_->GetFace(fontSize_);
        if (!face)
            return;

        rowHeight_ = face->GetRowHeight();

        int width = 0;
        int height = 0;
        int rowWidth = 0;
        int rowHeight = (int)(rowSpacing_ * rowHeight_);

        // First see if the text must be split up
        if (!wordWrap_)
        {
            printText_ = unicodeText_;
            printToText_.Resize(printText_.Size());
            for (unsigned i = 0; i < printText_.Size(); ++i)
                printToText_[i] = i;
        }
        else
        {
            int maxWidth = GetWidth();
            unsigned nextBreak = 0;
            unsigned lineStart = 0;
            printToText_.Clear();

            for (unsigned i = 0; i < unicodeText_.Size(); ++i)
            {
                unsigned j;
                unsigned c = unicodeText_[i];

                if (c != '\n')
                {
                    bool ok = true;

                    if (nextBreak <= i)
                    {
                        int futureRowWidth = rowWidth;
                        for (j = i; j < unicodeText_.Size(); ++j)
                        {
                            unsigned d = unicodeText_[j];
                            if (d == ' ' || d == '\n')
                            {
                                nextBreak = j;
                                break;
                            }
                            const FontGlyph* glyph = face->GetGlyph(d);
                            if (glyph)
                            {
                                futureRowWidth += glyph->advanceX_;
                                if (j < unicodeText_.Size() - 1)
                                    futureRowWidth += face->GetKerning(d, unicodeText_[j + 1]);
                            }
                            if (d == '-' && futureRowWidth <= maxWidth)
                            {
                                nextBreak = j + 1;
                                break;
                            }
                            if (futureRowWidth > maxWidth)
                            {
                                ok = false;
                                break;
                            }
                        }
                    }

                    if (!ok)
                    {
                        // If did not find any breaks on the line, copy until j, or at least 1 char, to prevent infinite loop
                        if (nextBreak == lineStart)
                        {
                            while (i < j)
                            {
                                printText_.Push(unicodeText_[i]);
                                printToText_.Push(i);
                                ++i;
                            }
                        }
                        // Eliminate spaces that have been copied before the forced break
                        while (printText_.Size() && printText_.Back() == ' ')
                        {
                            printText_.Pop();
                            printToText_.Pop();
                        }
                        printText_.Push('\n');
                        printToText_.Push(Min((int)i, (int)unicodeText_.Size() - 1));
                        rowWidth = 0;
                        nextBreak = lineStart = i;
                    }

                    if (i < unicodeText_.Size())
                    {
                        // When copying a space, position is allowed to be over row width
                        c = unicodeText_[i];
                        const FontGlyph* glyph = face->GetGlyph(c);
                        if (glyph)
                        {
                            rowWidth += glyph->advanceX_;
                            if (i < unicodeText_.Size() - 1)
                                rowWidth += face->GetKerning(c, unicodeText_[i + 1]);
                        }
                        if (rowWidth <= maxWidth)
                        {
                            printText_.Push(c);
                            printToText_.Push(i);
                        }
                    }
                }
                else
                {
                    printText_.Push('\n');
                    printToText_.Push(Min((int)i, (int)unicodeText_.Size() - 1));
                    rowWidth = 0;
                    nextBreak = lineStart = i;
                }
            }
        }

        rowWidth = 0;

        for (unsigned i = 0; i < printText_.Size(); ++i)
        {
            unsigned c = printText_[i];

            if (c != '\n')
            {
                const FontGlyph* glyph = face->GetGlyph(c);
                if (glyph)
                {
                    rowWidth += glyph->advanceX_;
                    if (i < printText_.Size() - 1)
                        rowWidth += face->GetKerning(c, printText_[i + 1]);
                }
            }
            else
            {
                width = Max(width, rowWidth);
                height += rowHeight;
                rowWidths_.Push(rowWidth);
                rowWidth = 0;
            }
        }

        if (rowWidth)
        {
            width = Max(width, rowWidth);
            height += rowHeight;
            rowWidths_.Push(rowWidth);
        }

        // Set at least one row height even if text is empty
        if (!height)
            height = rowHeight;

        // Set minimum and current size according to the text size, but respect fixed width if set
        if (!IsFixedWidth())
        {
            SetMinWidth(wordWrap_ ? 0 : width);
            SetWidth(width);
        }
        SetFixedHeight(height);

        charLocationsDirty_ = true;
    }
    else
    {
        // No font, nothing to render
        pageGlyphLocations_.Clear();
    }

    // If wordwrap is on, parent may need layout update to correct for overshoot in size. However, do not do this when the
    // update is a response to resize, as that could cause infinite recursion
    if (wordWrap_ && !onResize)
    {
        UIElement* parent = GetParent();
        if (parent && parent->GetLayoutMode() != LM_FREE)
            parent->UpdateLayout();
    }
}

void Text::UpdateCharLocations()
{
    // Remember the font face to see if it's still valid when it's time to render
    FontFace* face = font_ ? font_->GetFace(fontSize_) : (FontFace*)0;
    if (!face)
        return;
    fontFace_ = face;

    int rowHeight = (int)(rowSpacing_ * rowHeight_);

    // Store position & size of each character, and locations per texture page
    unsigned numChars = unicodeText_.Size();
    charLocations_.Resize(numChars + 1);
    pageGlyphLocations_.Resize(face->GetTextures().Size());
    for (unsigned i = 0; i < pageGlyphLocations_.Size(); ++i)
        pageGlyphLocations_[i].Clear();

    IntVector2 offset = font_->GetTotalGlyphOffset(fontSize_);

    unsigned rowIndex = 0;
    unsigned lastFilled = 0;
    int x = GetRowStartPosition(rowIndex) + offset.x_;
    int y = offset.y_;

    for (unsigned i = 0; i < printText_.Size(); ++i)
    {
        CharLocation loc;
        loc.position_ = IntVector2(x, y);

        unsigned c = printText_[i];
        if (c != '\n')
        {
            const FontGlyph* glyph = face->GetGlyph(c);
            loc.size_ = IntVector2(glyph ? glyph->advanceX_ : 0, rowHeight_);
            if (glyph)
            {
                // Store glyph's location for rendering. Verify that glyph page is valid
                if (glyph->page_ < pageGlyphLocations_.Size())
                    pageGlyphLocations_[glyph->page_].Push(GlyphLocation(x, y, glyph));
                x += glyph->advanceX_;
                if (i < printText_.Size() - 1)
                    x += face->GetKerning(c, printText_[i + 1]);
            }
        }
        else
        {
            loc.size_ = IntVector2::ZERO;
            x = GetRowStartPosition(++rowIndex);
            y += rowHeight;
        }

        // Fill gaps in case characters were skipped from printing
        for (unsigned j = lastFilled; j <= printToText_[i]; ++j)
            charLocations_[j] = loc;
        lastFilled = printToText_[i] + 1;
    }
    // Store the ending position
    charLocations_[numChars].position_ = IntVector2(x, y);
    charLocations_[numChars].size_ = IntVector2::ZERO;

    charLocationsDirty_ = false;
}

void Text::ValidateSelection()
{
    unsigned textLength = unicodeText_.Size();

    if (textLength)
    {
        if (selectionStart_ >= textLength)
            selectionStart_ = textLength - 1;
        if (selectionStart_ + selectionLength_ > textLength)
            selectionLength_ = textLength - selectionStart_;
    }
    else
    {
        selectionStart_ = 0;
        selectionLength_ = 0;
    }
}

int Text::GetRowStartPosition(unsigned rowIndex) const
{
    int rowWidth = 0;

    if (rowIndex < rowWidths_.Size())
        rowWidth = rowWidths_[rowIndex];

    int ret = GetIndentWidth();

    switch (textAlignment_)
    {
    case HA_LEFT:
        break;
    case HA_CENTER:
        ret += (GetSize().x_ - rowWidth) / 2;
        break;
    case HA_RIGHT:
        ret += GetSize().x_ - rowWidth;
        break;
    }

    return ret;
}

void Text::ConstructBatch(UIBatch& pageBatch, const PODVector<GlyphLocation>& pageGlyphLocation, int dx, int dy, Color* color,
    float depthBias)
{
    unsigned startDataSize = pageBatch.vertexData_->Size();

    if (!color)
        pageBatch.SetDefaultColor();
    else
        pageBatch.SetColor(*color);

    for (unsigned i = 0; i < pageGlyphLocation.Size(); ++i)
    {
        const GlyphLocation& glyphLocation = pageGlyphLocation[i];
        const FontGlyph& glyph = *glyphLocation.glyph_;
        pageBatch.AddQuad(dx + glyphLocation.x_ + glyph.offsetX_, dy + glyphLocation.y_ + glyph.offsetY_, glyph.width_,
            glyph.height_, glyph.x_, glyph.y_);
    }

    if (depthBias != 0.0f)
    {
        unsigned dataSize = pageBatch.vertexData_->Size();
        for (unsigned i = startDataSize; i < dataSize; i += UI_VERTEX_SIZE)
            pageBatch.vertexData_->At(i + 2) += depthBias;
    }
}

}
