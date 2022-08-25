#pragma once
#include "MainWindow.g.h"

namespace winrt::PowerClock::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();

        winrt::Microsoft::UI::Xaml::Media::Brush Background();
        void Background(winrt::Microsoft::UI::Xaml::Media::Brush const& value);

        winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
        {
            return e_propertyChanged.add(value);
        };
        void PropertyChanged(winrt::event_token const& token)
        {
            e_propertyChanged.remove(token);
        };

        void SetPosition(winrt::Windows::Graphics::PointInt32 const& position);

        void Grid_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void PipButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Window_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void RootGrid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ActionComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        void NotifsToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ExitToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ForceToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NotifsToggleButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NotifsToggleButton_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SettingsButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void KeepOnTopToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void TimerView_Elapsed(winrt::Windows::Foundation::IInspectable const& sender, winrt::PowerClock::TimerChangeStatus const& e);
        void DarkModeToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MoreSettingsButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        bool customTitleBar = false;
        bool isWindowFullscreen = false;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets backdropController = nullptr;
        winrt::Microsoft::UI::Xaml::Media::Brush _background{ nullptr };
        winrt::PowerClock::ColorsWindow colorsWindow{ nullptr };
        winrt::event_token mapChangedEventToken;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
        winrt::Windows::Foundation::Collections::IObservableMap<hstring, IInspectable>::MapChanged_revoker mapChangedRevoker;
        winrt::Microsoft::UI::Xaml::Window::Closed_revoker colorsWindowClosedRevoker;
        winrt::PowerClock::ColorsWindow::BackdropChanged_revoker colorsWindowBackdropChangedRevoker;

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged;

        void InitWindow();
        void SaveSettings();
        void SavePosition();
        void SetDragRectangles();
        void SetBackground(winrt::PowerClock::BackdropControllerType const& backdropType);
        void NotifyUser(winrt::hstring const& message);

        void AppWindow_Closing(Microsoft::UI::Windowing::AppWindow const&, Microsoft::UI::Windowing::AppWindowClosingEventArgs const& args);
        void AppWindow_Changed(Microsoft::UI::Windowing::AppWindow const&, Microsoft::UI::Windowing::AppWindowChangedEventArgs const& args);
        void Settings_MapChanged(winrt::Windows::Foundation::Collections::IObservableMap<winrt::hstring, winrt::Windows::Foundation::IInspectable> const& sender, winrt::Windows::Foundation::Collections::IMapChangedEventArgs<winrt::hstring> const& args);        
    };
}

namespace winrt::PowerClock::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
