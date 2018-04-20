#include "ContextBuilder.h"
#include "Context.h"
#include "PlatformEnvironment.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

std::unique_ptr<IContext> OpenRCT2::ContextBuilder::Build() const
{
    if (_uiContext == nullptr)
    {
        return std::unique_ptr<IContext>(CreateContext());
    }
    else
    {
        auto env = std::shared_ptr<IPlatformEnvironment>(CreatePlatformEnvironment());
        if (!_dataPath.empty())
        {
            env->SetBasePath(DIRBASE::OPENRCT2, _dataPath);
        }
        if (!_rct2Path.empty())
        {
            env->SetBasePath(DIRBASE::RCT2, _rct2Path);
        }
        if (!_userPath.empty())
        {
            env->SetBasePath(DIRBASE::CACHE, _userPath);
            env->SetBasePath(DIRBASE::CONFIG, _userPath);
            env->SetBasePath(DIRBASE::USER, _userPath);
        }
        return std::unique_ptr<IContext>(CreateContext(env, _audioContext, _uiContext));
    }
}

void TryOut()
{
    auto context = ContextBuilder()
        .UseDataPath("")
        .UseRCT2Path("")
        .Build();
    if (context->Initialise())
    {
        context->RunOpenRCT2(0, nullptr);
    }
}
