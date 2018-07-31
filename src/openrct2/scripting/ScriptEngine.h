/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/FileWatcher.h"
#include "HookEngine.h"
#include "Plugin.h"

#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

struct duk_hthread;
typedef struct duk_hthread duk_context;

class FileWatcher;
class InteractiveConsole;

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

namespace OpenRCT2::Scripting
{
    class ScriptExecutionInfo
    {
    private:
        std::shared_ptr<Plugin> _plugin;

    public:
        class PluginScope
        {
        private:
            ScriptExecutionInfo& _execInfo;
            std::shared_ptr<Plugin> _plugin;

        public:
            PluginScope(ScriptExecutionInfo& execInfo, std::shared_ptr<Plugin> plugin)
                : _execInfo(execInfo)
                , _plugin(plugin)
            {
                _execInfo._plugin = plugin;
            }
            PluginScope(const PluginScope&) = delete;
            ~PluginScope()
            {
                _execInfo._plugin = nullptr;
            }
        };

        std::shared_ptr<Plugin> GetCurrentPlugin()
        {
            return _plugin;
        }
    };

    class DukContext
    {
    private:
        duk_context* _context{};

    public:
        DukContext();
        DukContext(DukContext&) = delete;
        DukContext(DukContext&& src)
            : _context(std::move(src._context))
        {
        }
        ~DukContext();

        operator duk_context*()
        {
            return _context;
        }
    };

    class ScriptEngine
    {
    private:
        InteractiveConsole& _console;
        IPlatformEnvironment& _env;
        DukContext _context;
        bool _initialised{};
        std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
        std::vector<std::shared_ptr<Plugin>> _plugins;
        uint32_t _lastHotReloadCheckTick{};
        HookEngine _hookEngine;
        ScriptExecutionInfo _execInfo;

        std::unique_ptr<FileWatcher> _pluginFileWatcher;
        std::vector<std::string> _changedPluginFiles;
        std::mutex _changedPluginFilesMutex;

    public:
        ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env);
        ScriptEngine(ScriptEngine&) = delete;

        duk_context* GetContext()
        {
            return _context;
        }
        HookEngine& GetHookEngine()
        {
            return _hookEngine;
        }
        ScriptExecutionInfo& GetExecInfo()
        {
            return _execInfo;
        }

        void Update();
        std::future<void> Eval(const std::string& s);

    private:
        void Initialise();
        void LoadPlugins();
        void StartPlugins();
        void AutoReloadPlugins();
    };
} // namespace OpenRCT2::Scripting
