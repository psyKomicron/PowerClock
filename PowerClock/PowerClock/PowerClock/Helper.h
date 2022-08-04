#pragma once
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Microsoft.UI.Xaml.h>

using namespace winrt::Microsoft::UI::Xaml;

namespace PowerClock::Common
{
    class Helper
    {
    public:
        template<typename T> 
        static T GetAppResource(winrt::hstring const& key)
        {

            return winrt::unbox_value<T>(Application::Current().Resources().Lookup(winrt::box_value(key)));
        };
    };
}

