#pragma once

#include "App.xaml.g.h"

namespace winrt::PowerClock::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::PowerClock::MainWindow window{ nullptr };
    };
}
