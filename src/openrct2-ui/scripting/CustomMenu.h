/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <openrct2/Context.h>
#include <openrct2/scripting/Duktape.hpp>
#include <openrct2/scripting/ScriptEngine.h>
#include <string>
#include <vector>

namespace OpenRCT2::Scripting
{
    class CustomToolbarMenuItem
    {
    public:
        std::shared_ptr<Plugin> Owner;
        std::string Text;
        DukValue Callback;

        CustomToolbarMenuItem(std::shared_ptr<Plugin> owner, std::string text, DukValue callback)
            : Owner(owner)
            , Text(text)
            , Callback(callback)
        {
        }

        void Invoke() const
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto& execInfo = scriptEngine.GetExecInfo();
            auto ctx = scriptEngine.GetContext();

            ScriptExecutionInfo::PluginScope scope(execInfo, Owner);
            Callback.push();
            duk_pcall(ctx, 0);
            duk_pop(ctx);
        }
    };

    extern std::vector<CustomToolbarMenuItem> CustomMenuItems;

    void InitialiseCustomMenuItems(ScriptEngine& scriptEngine);

} // namespace OpenRCT2::Scripting