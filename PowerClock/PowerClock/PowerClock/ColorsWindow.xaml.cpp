#include "pch.h"
#include "ColorsWindow.xaml.h"
#if __has_include("ColorsWindow.g.cpp")
#include "ColorsWindow.g.cpp"
#endif

#include "Helper.h"
#include "Constants.h"
using namespace winrt::Windows::Foundation::Collections;

using namespace winrt;
using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Composition;
using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PowerClock::implementation
{
    ColorsWindow::ColorsWindow()
    {
        InitializeComponent();
        InitWindow();
        LoadSettings();
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

    void ColorsWindow::TransparencyEffectToggleSwitch_Toggled(IInspectable const&, RoutedEventArgs const&)
    {
        bool enabled = TransparencyEffectToggleSwitch().IsOn();
        bool wasEnabled = MicaRadioButton().IsEnabled();
        MicaRadioButton().IsEnabled(enabled);
        AcrylicRadioButton().IsEnabled(enabled);

        if (!wasEnabled && enabled)
        {
            e_backdropControllerChanged(*this, AcrylicRadioButton().IsChecked().GetBoolean() ?
                winrt::PowerClock::BackdropControllerType::AcrylicBackdrop : winrt::PowerClock::BackdropControllerType::MicaBackdrop);
        }
        else if (!enabled)
        {
            e_backdropControllerChanged(*this, winrt::PowerClock::BackdropControllerType::None);
        }

        ApplicationData::Current().LocalSettings().Values().Insert(L"TransparencyAllowed", IReference(enabled));
    }

    void ColorsWindow::TransparencyTypeRadioButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"BackdropType", box_value(AcrylicRadioButton().IsChecked().GetBoolean() ? ACRYLIC_BACKDROP : MICA_BACKDROP));

        backdropController.RemoveAllSystemBackdropTargets();
        systemBackdropConfiguration = nullptr;
        SetBackground(AcrylicRadioButton().IsChecked().GetBoolean());

        e_backdropControllerChanged(*this, AcrylicRadioButton().IsChecked().GetBoolean() ?
            winrt::PowerClock::BackdropControllerType::AcrylicBackdrop : winrt::PowerClock::BackdropControllerType::MicaBackdrop);
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
            int32_t width = ::PowerClock::Common::Helper::GetAppResource<int32_t>(L"ColorsWindowWidth");
            int32_t height = ::PowerClock::Common::Helper::GetAppResource<int32_t>(L"ColorsWindowHeight");
            int32_t y = 50;
            int32_t x = 50;

            ApplicationDataContainer windowSettings = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"ColorsWindow");
            if (!windowSettings)
            {
                windowSettings = ApplicationData::Current().LocalSettings().CreateContainer(L"ColorsWindow", ApplicationDataCreateDisposition::Always);
            }

            IInspectable windowSize = windowSettings.Values().TryLookup(L"WindowSize");
            if (windowSize != nullptr)
            {
                ApplicationDataCompositeValue composite = windowSize.as<ApplicationDataCompositeValue>();
                width = unbox_value_or<int>(composite.Lookup(L"Width"), 340);
                height = unbox_value_or<int>(composite.Lookup(L"Height"), 270);
            }
            IInspectable windowPosition = windowSettings.Values().TryLookup(L"WindowPosition");
            if (windowPosition != nullptr)
            {
                ApplicationDataCompositeValue composite = windowPosition.as<ApplicationDataCompositeValue>();
                IInspectable posX = composite.Lookup(L"PositionX");
                IInspectable posY = composite.Lookup(L"PositionY");
                x = unbox_value<int>(posX);
                y = unbox_value<int>(posY);
            }

            // don't resize if the window is using compact overlay
            if (appWindow.Presenter().Kind() == AppWindowPresenterKind::CompactOverlay)
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
    
    void ColorsWindow::LoadSettings()
    {
        IPropertySet values = ApplicationData::Current().LocalSettings().Values();
        if (values.HasKey(L"NightModeFontColor"))
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

        if (values.HasKey(L"NightModeBackgroundColor"))
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

        bool transparencyAllowed = unbox_value_or(values.TryLookup(L"TransparencyAllowed"), true);
        TransparencyEffectToggleSwitch().IsOn(transparencyAllowed);
        MicaRadioButton().IsEnabled(transparencyAllowed);
        AcrylicRadioButton().IsEnabled(transparencyAllowed);

        short backdrop = unbox_value_or(values.TryLookup(L"BackdropType"), ACRYLIC_BACKDROP);
        MicaRadioButton().IsChecked(backdrop == MICA_BACKDROP);
        AcrylicRadioButton().IsChecked(backdrop == ACRYLIC_BACKDROP);

#ifdef _DEBUG
        SettingsListView().Items().Append(ListViewHeaderItem{});

        Grid grid{};
        ColumnDefinition col1{};
        col1.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        grid.ColumnDefinitions().Append(col1);
        ColumnDefinition col2{};
        col2.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Auto));
        grid.ColumnDefinitions().Append(col2);

        TextBlock textBlock{};
        textBlock.Text(L"DEBUG");
        Grid::SetColumn(textBlock, 0);

        TextBlock timestamp{};
        //timestamp.Text(to_hstring(__DATE__) + L" - " + to_hstring(__TIME__));
        timestamp.Text(to_hstring(__TIMESTAMP__));
        Grid::SetColumn(timestamp, 1);

        grid.Children().Append(textBlock);
        grid.Children().Append(timestamp);
        SettingsListView().Items().Append(grid);
#endif

        SetBackground(backdrop == ACRYLIC_BACKDROP);
    }

    void ColorsWindow::SetBackground(bool const& useAcrylic)
    {
        if (useAcrylic && !DesktopAcrylicController::IsSupported())
        {
            return;
        }
        else if (!MicaController::IsSupported())
        {
            return;
        }

        RootGrid().Background(SolidColorBrush(Colors::Transparent()));

        if (!DispatcherQueue::GetForCurrentThread() && !dispatcherQueueController)
        {
            DispatcherQueueOptions options
            {
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_CURRENT,
                DQTAT_COM_NONE
            };

            ABI::Windows::System::IDispatcherQueueController* ptr{ nullptr };
            check_hresult(CreateDispatcherQueueController(options, &ptr));
            dispatcherQueueController = { ptr, take_ownership_from_abi };
        }

        systemBackdropConfiguration = SystemBackdropConfiguration();
        systemBackdropConfiguration.IsInputActive(true);
        systemBackdropConfiguration.Theme((SystemBackdropTheme)RootGrid().ActualTheme());

        themeChangedRevoker = RootGrid().ActualThemeChanged(auto_revoke, [this](FrameworkElement const&, IInspectable const&)
        {
            systemBackdropConfiguration.Theme((SystemBackdropTheme)RootGrid().ActualTheme());
        });

        auto&& supportsBackdrop = try_as<ICompositionSupportsSystemBackdrop>();
        if (supportsBackdrop)
        {
            if (useAcrylic)
            {
                auto&& controller = DesktopAcrylicController();
#pragma warning(suppress : 4244)
                controller.TintOpacity(Application::Current().Resources().TryLookup(box_value(L"BackdropSecondaryTintOpacity")).as<double>());
#pragma warning(suppress : 4244)
                controller.LuminosityOpacity(Application::Current().Resources().TryLookup(box_value(L"BackdropSecondaryLuminosityOpacity")).as<double>());
                controller.FallbackColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                controller.TintColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorSecondary")).as<Windows::UI::Color>());

                backdropController = std::move(controller);
            }
            else
            {
                auto&& controller = MicaController();
#pragma warning(suppress : 4244)
                controller.TintOpacity(Application::Current().Resources().TryLookup(box_value(L"BackdropSecondaryTintOpacity")).as<double>());
#pragma warning(suppress : 4244)
                controller.LuminosityOpacity(Application::Current().Resources().TryLookup(box_value(L"BackdropSecondaryLuminosityOpacity")).as<double>());
                controller.FallbackColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                controller.TintColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorSecondary")).as<Windows::UI::Color>());

                backdropController = std::move(controller);
            }
            
            backdropController.SetSystemBackdropConfiguration(systemBackdropConfiguration);
            backdropController.AddSystemBackdropTarget(supportsBackdrop);
        }
    }
}