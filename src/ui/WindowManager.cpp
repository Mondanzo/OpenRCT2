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
#include "../drawing/IDrawingContext.h"
#include "MouseEventArgs.h"
#include "Window.h"
#include "WindowManager.h"

namespace OpenRCT2::Ui
{
    Window * OpenParkWindow();

    class WindowManager : public IWindowManager
    {
    private:
        rect32 _bounds = { 0 };
        std::vector<Window *> _windows;

        Window * _cursorWindow = nullptr;
        Window * _focusWindow = nullptr;
        Window * _holdWindow = nullptr;

    public:
        WindowManager()
        {
            // Window * w = new Window();
            // w->Bounds = { 32, 32, 512, 386 };
            // _windows.push_back(w);
            // 
            // Window * w2 = new Window();
            // w2->Bounds = { 256, 88, 200, 100 };
            // _windows.push_back(w2);

            Window * w3 = OpenParkWindow();
            AddWindow(w3);
            SetWindowFocus(w3);
            Update();
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

        void Invalidate(rect32 bounds) override
        {
            bounds.X += _bounds.X;
            bounds.Y += _bounds.Y;
            gfx_set_dirty_blocks(bounds.X, bounds.Y, bounds.GetRight(), bounds.GetBottom());
        }

        void Update() override
        {
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
                IDrawingContext * nestedDC = dc->Nest(w->X, w->Y, w->Width, w->Height);
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
                MouseEventArgs e2 = e->CopyAndOffset(-w->X, -w->Y);
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
                MouseEventArgs e2 = e->CopyAndOffset(-w->X, -w->Y);
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
                MouseEventArgs e2 = e->CopyAndOffset(-w->X, -w->Y);
                w->MouseUp(&e2);
            }
        }

        void MouseWheel(const MouseEventArgs * e) override
        {
            Window * w = GetWindowAt(e->X, e->Y);
            if (w != nullptr)
            {
                MouseEventArgs e2 = e->CopyAndOffset(-w->X, -w->Y);
                w->MouseWheel(&e2);
            }
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
                rect32 windowBounds = w->Bounds;
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
                _cursorWindow->Flags &= ~WINDOW_FLAGS::CURSOR;
            }
            _cursorWindow = w;
            if (w != nullptr)
            {
                w->Flags |= WINDOW_FLAGS::CURSOR;
            }
        }

        void SetWindowFocus(Window * w)
        {
            // Unset, set focus flag
            if (_focusWindow != nullptr)
            {
                _focusWindow->Flags &= ~WINDOW_FLAGS::FOCUS;
            }
            _focusWindow = w;
            if (w != nullptr)
            {
                w->Flags |= WINDOW_FLAGS::FOCUS;

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
