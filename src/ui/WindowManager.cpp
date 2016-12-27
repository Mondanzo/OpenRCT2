#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include <vector>
#include "../drawing/DrawingContext.h"
#include "MouseEventArgs.h"
#include "Window.h"
#include "WindowManager.h"

extern "C"
{
    #include "../localisation/language.h"
}

namespace OpenRCT2::Ui
{
    class WindowManager : public IWindowManager
    {
    private:
        rect32 _bounds = { 0 };
        std::vector<Window *> _windows;

        Window * _cursorWindow = nullptr;
        Window * _focusWindow = nullptr;
        Window * _holdWindow = nullptr;

        sint32 _lastLanguage = -1;

    public:
        WindowManager()
        {
        }

        virtual ~WindowManager()
        {
        }

        rect32 GetBounds() const override
        {
            return _bounds;
        }

        void SetBounds(rect32 bounds) override
        {
            _bounds = bounds;
        }

        sint32 GetLineHeight(sint32 fontSize) const override
        {
            sint32 spriteBase = font_get_sprite_base_from_size(fontSize);
            return font_get_line_height(spriteBase);
        }

        float GetUiScale() const override
        {
            return 2;
        }

        void Invalidate(rect32 bounds) override
        {
            bounds.X += _bounds.X;
            bounds.Y += _bounds.Y;
            gfx_set_dirty_blocks(bounds.X, bounds.Y, bounds.GetRight(), bounds.GetBottom());
        }

        void RefreshAllWindows() override
        {
            // Force all windows to re-layout
            for (Window * w : _windows)
            {
                w->SetFlag(WINDOW_FLAGS::LAYOUT_DIRTY, true);
            }
        }

        void Update() override
        {
            // HACK Check if language has changed and force a refresh.
            //      Eventually this should be called from the set language code.
            if (gCurrentLanguage != _lastLanguage)
            {
                _lastLanguage = gCurrentLanguage;
                RefreshAllWindows();
            }

            for (Window * w : _windows)
            {
                w->Update();
            }
        }

        void Draw(IDrawingContext * dc) override
        {
            for (auto it = _windows.rbegin(); it != _windows.rend(); it++)
            {
                Window * w = *it;
                rect32 bounds = w->GetBounds();
                IDrawingContext * nestedDC = dc->Nest(bounds.X, bounds.Y, bounds.Width, bounds.Height);
                if (nestedDC != nullptr)
                {
                    w->Draw(nestedDC);
                }
            }
        }

        void MouseDown(const MouseEventArgs * e) override
        {
            Window * w = GetWindowAt(e->X, e->Y);
            SetWindowFocus(w);
            _holdWindow = w;
            if (w != nullptr)
            {
                xy32 wpos = w->GetLocation();
                MouseEventArgs e2 = e->CopyAndOffset(-wpos.X, -wpos.Y);
                w->MouseDown(&e2);
            }
        }

        void MouseMove(const MouseEventArgs * e) override
        {
            Window * w = GetWindowAt(e->X, e->Y);
            Window * cursorW = w;

            // If we first pressed down on another window, continue giving events to it
            if (_holdWindow != nullptr)
            {
                if (cursorW != _holdWindow)
                {
                    cursorW = nullptr;
                }
                w = _holdWindow;
            }

            SetWindowCursor(cursorW);

            if (w != nullptr)
            {
                xy32 wpos = w->GetLocation();
                MouseEventArgs e2 = e->CopyAndOffset(-wpos.X, -wpos.Y);
                w->MouseMove(&e2);
            }
        }

        void MouseUp(const MouseEventArgs * e) override
        {
            // If we first pressed down on another window, continue giving events to it
            Window * w = _holdWindow;
            if (w == nullptr)
            {
                w = GetWindowAt(e->X, e->Y);
            }
            if (w != nullptr)
            {
                xy32 wpos = w->GetLocation();
                MouseEventArgs e2 = e->CopyAndOffset(-wpos.X, -wpos.Y);
                w->MouseUp(&e2);
            }
        }

        void MouseWheel(const MouseEventArgs * e) override
        {
            Window * w = GetWindowAt(e->X, e->Y);
            if (w != nullptr)
            {
                xy32 wpos = w->GetLocation();
                MouseEventArgs e2 = e->CopyAndOffset(-wpos.X, -wpos.Y);
                w->MouseWheel(&e2);
            }
        }

        void ShowWindow(Window * window) override
        {
            AddWindow(window);
            SetWindowFocus(window);

            window->Initialise();
        }

    private:
        void AddWindow(Window * window)
        {
            window->SetWindowManager(this);
            _windows.push_back(window);
        }

        Window * GetWindowAt(sint32 x, sint32 y)
        {
            for (Window * w : _windows)
            {
                rect32 windowBounds = w->GetBounds();
                if (windowBounds.Contains(x, y))
                {
                    sint32 relX = x - windowBounds.X;
                    sint32 relY = y - windowBounds.Y;
                    if (w->HitTest(relX, relY))
                    {
                        return w;
                    }
                }
            }
            return nullptr;
        }

        void SetWindowCursor(Window * w)
        {
            // Unset, set cursor flag
            if (_cursorWindow != nullptr)
            {
                _cursorWindow->SetFlag(WINDOW_FLAGS::CURSOR, false);
            }
            _cursorWindow = w;
            if (w != nullptr)
            {
                w->SetFlag(WINDOW_FLAGS::CURSOR, true);
            }
        }

        void SetWindowFocus(Window * w)
        {
            // Unset, set focus flag
            if (_focusWindow != nullptr)
            {
                _focusWindow->SetFlag(WINDOW_FLAGS::FOCUS, false);
            }
            _focusWindow = w;
            if (w != nullptr)
            {
                w->SetFlag(WINDOW_FLAGS::FOCUS, true);

                // Move window to top of window list
                auto pos = std::find(_windows.begin(), _windows.end(), w);
                if (pos != _windows.end())
                {
                    _windows.erase(pos);
                    _windows.insert(_windows.begin(), w);
                }
            }
        }
    };

    IWindowManager * CreateWindowManager()
    {
        return new WindowManager();
    }
}
