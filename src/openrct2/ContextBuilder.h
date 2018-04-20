#include <memory>
#include <string>
#include <string_view>
#include "common.h"
#include "OpenRCT2.h"

namespace OpenRCT2
{
    interface IContext;
}

namespace OpenRCT2::Audio
{
    interface IAudioContext;
}

namespace OpenRCT2::Ui
{
    interface IUiContext;
}

namespace OpenRCT2
{
    using namespace OpenRCT2::Audio;
    using namespace OpenRCT2::Ui;

    /**
     * Class for building an OpenRCT2 context.
     */
    class ContextBuilder
    {
    private:
        std::string _dataPath;
        std::string _rct2Path;
        std::string _userPath;
        std::shared_ptr<IAudioContext> _audioContext;
        std::shared_ptr<IUiContext> _uiContext;
        sint32 _startupAction;
        std::string _uri;
        std::string _host;

    public:
        ContextBuilder& UseDataPath(const std::string_view& path) { _dataPath = path; }
        ContextBuilder& UseRCT2Path(const std::string_view& path) { _rct2Path = path; }
        ContextBuilder& UseUserPath(const std::string_view& path) { _userPath = path; }
        ContextBuilder& UseAudioContext(std::shared_ptr<IAudioContext> audioContext) { _audioContext = audioContext; }
        ContextBuilder& UseUiContext(std::shared_ptr<IUiContext> uiContext) { _uiContext = uiContext; }

        ContextBuilder& StartIntro() { _startupAction = STARTUP_ACTION_INTRO; }
        ContextBuilder& EditPark(const std::string_view& uri)
        {
            _startupAction = STARTUP_ACTION_EDIT;
            _uri = uri;
        }

        ContextBuilder& OpenPark(const std::string_view& uri)
        {
            _startupAction = STARTUP_ACTION_OPEN;
            _uri = uri;
        }

        /**
         * @param listen The host name or IP address and port to listen on, e.g. `*:11753`.
         */
        ContextBuilder& StartServer(const std::string_view& uri, const std::string_view& listen = "")
        {
            _startupAction = STARTUP_ACTION_HOST;
            _uri = uri;
            _host = listen;
        }

        /**
         * @param host The host name or IP address and port to join, e.g. `localhost:11753`.
         */
        ContextBuilder& JoinServer(const std::string_view& host)
        {
            _startupAction = STARTUP_ACTION_JOIN;
            _host = host;
        }

        std::unique_ptr<IContext> Build() const;
    };
}
