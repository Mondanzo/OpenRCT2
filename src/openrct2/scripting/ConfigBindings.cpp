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

#include <cmath>
#include "../config/Config.h"
#include "../Context.h"
#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "Bindings.hpp"

namespace OpenRCT2
{
    namespace Scripting
    {
        namespace Bindings
        {
            static duk_int_t GetAll(duk_context * ctx)
            {
                duk_push_object(ctx);
                duk_push_number(ctx, gConfigGeneral.window_scale);
                duk_put_prop_string(ctx, -2, "interface.window_scale");
                return 1;
            }

            static duk_int_t Get(duk_context * ctx)
            {
                sint32 numArgs = duk_get_top(ctx);
                if (numArgs == 0)
                {
                    return DUK_RET_TYPE_ERROR;
                }

                const char * key = duk_get_string(ctx, 0);
                if (String::Equals(key, "general.window_scale"))
                {
                    duk_push_number(ctx, gConfigGeneral.window_scale);
                }
                else
                {
                    if (numArgs >= 2)
                    {
                        // Return the given default value
                        duk_dup(ctx, 1);
                    }
                    else
                    {
                        duk_push_undefined(ctx);
                    }
                }

                return 1;
            }

            static duk_int_t Set(duk_context * ctx)
            {
                sint32 numArgs = duk_get_top(ctx);
                if (numArgs == 0)
                {
                    return DUK_RET_TYPE_ERROR;
                }

                auto key = duk_get_string(ctx, 0);
                if (String::Equals(key, "general.window_scale"))
                {
                    auto value = duk_get_number(ctx, 1);
                    auto newScale = (float)(0.001 * std::trunc(1000 * value));
                    gConfigGeneral.window_scale = Math::Clamp(newScale, 0.5f, 5.0f);
                    config_save_default();
                    gfx_invalidate_screen();
                    context_trigger_resize();
                }

                return 0;
            }

            static duk_int_t Has(duk_context * ctx)
            {
                sint32 numArgs = duk_get_top(ctx);
                if (numArgs == 0)
                {
                    return DUK_RET_TYPE_ERROR;
                }

                auto result = false;
                auto key = duk_get_string(ctx, 0);
                if (String::Equals(key, "general.window_scale"))
                {
                    result = true;
                }

                duk_push_boolean(ctx, result);
                return 1;
            }

            void CreateConfiguration(duk_context * ctx)
            {
                auto objIdx = duk_push_object(ctx);
                RegisterFunction(ctx, objIdx, "getAll", GetAll);
                RegisterFunction(ctx, objIdx, "get", Get);
                RegisterFunction(ctx, objIdx, "set", Set);
                RegisterFunction(ctx, objIdx, "has", Has);
            }
        }
    }
}
