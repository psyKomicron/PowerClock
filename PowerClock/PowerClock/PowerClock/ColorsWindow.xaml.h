#pragma once

#include "ColorsWindow.g.h"

namespace winrt::PowerClock::implementation
{
    struct ColorsWindow : ColorsWindowT<ColorsWindow>
    {
    public:
        ColorsWindow();

        winrt::Windows::Foundation::IAsyncAction OpenDialogForBackground_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction OpenDialogForFontColor_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void InitWindow();

    private:
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
    };
}

namespace winrt::PowerClock::factory_implementation
{
    struct ColorsWindow : ColorsWindowT<ColorsWindow, implementation::ColorsWindow>
    {
    };
}
