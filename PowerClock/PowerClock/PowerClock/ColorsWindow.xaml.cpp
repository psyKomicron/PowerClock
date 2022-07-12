#include "pch.h"
#include "ColorsWindow.xaml.h"
#if __has_include("ColorsWindow.g.cpp")
#include "ColorsWindow.g.cpp"
#endif
using namespace winrt::Microsoft::UI::Xaml::Media;

using namespace winrt;
using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PowerClock::implementation
{
    ColorsWindow::ColorsWindow()
    {
        InitializeComponent();
        InitWindow();

        if (ApplicationData::Current().LocalSettings().Values().HasKey(L"NightModeFontColor"))
        {
            ApplicationDataCompositeValue composite = ApplicationData::Current().LocalSettings().Values().Lookup(L"NightModeFontColor").as<ApplicationDataCompositeValue>();
            uint8_t a = composite.Lookup(L"A").as<uint8_t>();
            uint8_t r = composite.Lookup(L"R").as<uint8_t>();
            uint8_t g = composite.Lookup(L"G").as<uint8_t>();
            uint8_t b = composite.Lookup(L"B").as<uint8_t>();

            hstring str = L"#" + to_hstring(a) + to_hstring(r) + to_hstring(g) + to_hstring(b);
            NightModeFontColorTextBox().Text(str);
            NightModeFontColorRectangle().Fill(SolidColorBrush(ColorHelper::FromArgb(a, r, g, b)));
        }

        if (ApplicationData::Current().LocalSettings().Values().HasKey(L"NightModeBackgroundColor"))
        {
            ApplicationDataCompositeValue composite = ApplicationData::Current().LocalSettings().Values().Lookup(L"NightModeBackgroundColor").as<ApplicationDataCompositeValue>();
            uint8_t a = composite.Lookup(L"A").as<uint8_t>();
            uint8_t r = composite.Lookup(L"R").as<uint8_t>();
            uint8_t g = composite.Lookup(L"G").as<uint8_t>();
            uint8_t b = composite.Lookup(L"B").as<uint8_t>();

            hstring str = L"#" + to_hstring(a) + to_hstring(r) + to_hstring(g) + to_hstring(b);
            NightModeBackgroundTextBox().Text(str);
            NightModeBackgroundRectangle().Fill(SolidColorBrush(ColorHelper::FromArgb(a, r, g, b)));
        }
    }

    IAsyncAction ColorsWindow::OpenDialogForBackground_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto&& result = co_await ColorPickerContentDialog().ShowAsync();
        if (result == ContentDialogResult::Primary)
        {
            ApplicationDataCompositeValue composite{};
            composite.Insert(L"A", box_value(Color_Picker().Color().A));
            composite.Insert(L"R", box_value(Color_Picker().Color().R));
            composite.Insert(L"G", box_value(Color_Picker().Color().G));
            composite.Insert(L"B", box_value(Color_Picker().Color().B));
            ApplicationData::Current().LocalSettings().Values().Insert(L"NightModeBackgroundColor", composite);
        }
    }

    IAsyncAction ColorsWindow::OpenDialogForFontColor_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto&& result = co_await ColorPickerContentDialog().ShowAsync();
        if (result == ContentDialogResult::Primary)
        {
            ApplicationDataCompositeValue composite{};
            composite.Insert(L"A", box_value(Color_Picker().Color().A));
            composite.Insert(L"R", box_value(Color_Picker().Color().R));
            composite.Insert(L"G", box_value(Color_Picker().Color().G));
            composite.Insert(L"B", box_value(Color_Picker().Color().B));
            ApplicationData::Current().LocalSettings().Values().Insert(L"NightModeFontColor", composite);
        }
    }

    void ColorsWindow::InitWindow()
    {
        auto nativeWindow{ this->try_as<::IWindowNative>() };
        check_bool(nativeWindow);
        HWND handle{ nullptr };
        nativeWindow->get_WindowHandle(&handle);
        WindowId windowID = GetWindowIdFromWindow(handle);
        appWindow = AppWindow::GetFromWindowId(windowID);
        if (appWindow != nullptr)
        {
            if (AppWindowTitleBar::IsCustomizationSupported())
            {
                appWindow.TitleBar().ExtendsContentIntoTitleBar(true);

                appWindow.TitleBar().ButtonBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonInactiveForegroundColor(Application::Current().Resources().TryLookup(box_value(L"AppTitleBarHoverColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverBackgroundColor(Application::Current().Resources().TryLookup(box_value(L"ButtonHoverBackgroundColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonPressedBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonPressedForegroundColor(Colors::White());
            }
            else
            {
                TitleBarGrid().Height(0);
            }

#pragma region Settings
            int width = 340;
            int height = 270;
            int y = 50;
            int x = 50;

            ApplicationDataContainer settings = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"ColorsWindow");
            if (!settings)
            {
                settings = ApplicationData::Current().LocalSettings().CreateContainer(L"ColorsWindow", ApplicationDataCreateDisposition::Always);
            }

            IInspectable windowSize = settings.Values().TryLookup(L"WindowSize");
            if (windowSize != nullptr)
            {
                ApplicationDataCompositeValue composite = windowSize.as<ApplicationDataCompositeValue>();
                width = unbox_value_or<int>(composite.Lookup(L"Width"), 340);
                height = unbox_value_or<int>(composite.Lookup(L"Height"), 270);
            }
            IInspectable windowPosition = settings.Values().TryLookup(L"WindowPosition");
            if (windowPosition != nullptr)
            {
                ApplicationDataCompositeValue composite = windowPosition.as<ApplicationDataCompositeValue>();
                IInspectable posX = composite.Lookup(L"PositionX");
                IInspectable posY = composite.Lookup(L"PositionY");
                if (posX)
                {
                    x = unbox_value<int>(posX);
                }
                if (posY)
                {
                    y = unbox_value<int>(posY);
                }
            }

            if (appWindow.Presenter().Kind() == AppWindowPresenterKind::CompactOverlay) //don't resize if the window is using compact overlay
            {
                appWindow.Move(PointInt32(x, y));
            }
            else
            {
                appWindow.MoveAndResize(RectInt32(x, y, width, height));
            }
#pragma endregion
            
            // I18N: Translate additional settings window title
            appWindow.Title(L"Additional settings");
        }
    }
}