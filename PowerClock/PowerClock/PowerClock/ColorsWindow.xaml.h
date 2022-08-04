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
        void TransparencyEffectToggleSwitch_Toggled(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void TransparencyTypeRadioButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        event_token BackdropChanged(winrt::Windows::Foundation::TypedEventHandler<PowerClock::ColorsWindow, BackdropControllerType> const& handler)
        {
            return e_backdropControllerChanged.add(handler);
        };
        void BackdropChanged(event_token const& token)
        {
            e_backdropControllerChanged.remove(token);
        };

    private:
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker{};
        winrt::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets backdropController = nullptr;

        winrt::event<winrt::Windows::Foundation::TypedEventHandler<PowerClock::ColorsWindow, BackdropControllerType>> e_backdropControllerChanged{};

        void SetBackground(bool const& useAcrylic);
        void InitWindow();
        void LoadSettings();
    };

    enum BackdropControllerType
    {
        None,
        AcrylicBackdrop,
        MicaBackdrop
    };
}

namespace winrt::PowerClock::factory_implementation
{
    struct ColorsWindow : ColorsWindowT<ColorsWindow, implementation::ColorsWindow>
    {
    };
}
