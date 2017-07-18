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

#pragma once

#include <string>
#include "../common.h"
#include "Bindings.hpp"

namespace OpenRCT2 { namespace Scripting
{
    template<typename TWrapper>
    class BindingObject
    {
    public:
        void PushNewESO()
        {
            PushBindingObject(Context, this);
            duk_push_c_function(Context, [](duk_context * ctx) -> int
            {
                auto wrapper = GetNativeReference<TWrapper>(ctx, 0);
                if (wrapper != nullptr)
                {
                    delete wrapper;
                }
                return 0;
            }, 2);
            duk_set_finalizer(Context, -2);
            AddProperties();
        }

    protected:
        duk_context * const Context;

        BindingObject(duk_context * context)
            : Context(context)
        {
        }

        virtual void AddProperties() abstract;

        template<class TType>
        void AddPropertyInt32()
        {
            duk_push_string(Context, TType{}.GetKey());
            duk_push_c_function(Context, [](duk_context * ctx) -> int
            {
                auto wrapper = GetNativeReference<TWrapper>(ctx);
                if (wrapper != nullptr)
                {
                    sint32 result = wrapper->Get(TType{});
                    duk_push_int(ctx, result);
                }
                else
                {
                    duk_push_int(ctx, 0);
                }
                return 1;
            }, 0);
            duk_push_c_function(Context, [](duk_context * ctx) -> int
            {
                auto wrapper = GetNativeReference<TWrapper>(ctx);
                if (wrapper != nullptr)
                {
                    auto numArgs = duk_get_top(ctx);
                    if (numArgs == 0) return DUK_RET_TYPE_ERROR;
                    wrapper->Set(TType{}, duk_to_int32(ctx, 0));
                }
                return 0;
            }, 1);
            duk_def_prop(Context, -4, DUK_DEFPROP_HAVE_GETTER |
                                      DUK_DEFPROP_HAVE_SETTER |
                                      DUK_DEFPROP_SET_ENUMERABLE);
        }

        template<class TType>
        void AddPropertyString()
        {
            duk_push_string(Context, TType{}.GetKey());
            duk_push_c_function(Context, [](duk_context * ctx) -> int
            {
                auto wrapper = GetNativeReference<TWrapper>(ctx);
                if (wrapper != nullptr)
                {
                    std::string result = wrapper->Get(TType{});
                    duk_push_string(ctx, result.c_str());
                }
                else
                {
                    duk_push_string(ctx, "");
                }
                return 1;
            }, 0);
            duk_push_c_function(Context, [](duk_context * ctx) -> int
            {
                auto wrapper = GetNativeReference<TWrapper>(ctx);
                if (wrapper != nullptr)
                {
                    auto numArgs = duk_get_top(ctx);
                    if (numArgs == 0) return DUK_RET_TYPE_ERROR;
                    auto cstr = duk_to_string(ctx, 0);
                    auto str = cstr == nullptr ? "" : std::string(cstr);
                    wrapper->Set(TType{}, str);
                }
                return 0;
            }, 1);
            duk_def_prop(Context, -4, DUK_DEFPROP_HAVE_GETTER |
                                      DUK_DEFPROP_HAVE_SETTER |
                                      DUK_DEFPROP_SET_ENUMERABLE);
        }
    };
} }

#define DefineProperty(TType, TKey, TName)              \
    struct TName                                        \
    {                                                   \
        static TType GetType() { return TType { }; }    \
        static const char * GetKey() { return TKey; }   \
    }
