#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "PowerController.h"
#include "ColorsWindow.xaml.h"
#include "Constants.h"

using namespace ::PowerClock::Common;

using namespace std;

using namespace winrt;
using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Composition;
using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;

using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PowerClock::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        InitWindow();
;
        if (unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"TransparencyAllowed"), true))
        {
            SetBackground(unbox_value_or(
                ApplicationData::Current().LocalSettings().Values().TryLookup(L"BackdropType"), ACRYLIC_BACKDROP) == 1? 
                winrt::PowerClock::BackdropControllerType::AcrylicBackdrop : winrt::PowerClock::BackdropControllerType::MicaBackdrop
            );
        }
        else
        {
            SetBackground(winrt::PowerClock::BackdropControllerType::None);
        }
    }

    Brush MainWindow::Background()
    {
        return _background;
    }

    void MainWindow::Background(winrt::Microsoft::UI::Xaml::Media::Brush const& value)
    {
        _background = value;
        e_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(L"Background"));
    }

    void MainWindow::SetPosition(winrt::Windows::Graphics::PointInt32 const& position)
    {
        if (appWindow)
        {
            appWindow.Move(position);
        }
    }


    void MainWindow::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        if (customTitleBar && appWindow)
        {
            SetDragRectangles();
        }
    }

    void MainWindow::Window_Activated(IInspectable const&, WindowActivatedEventArgs const& args)
    {
        systemBackdropConfiguration.IsInputActive(WindowActivationState::Deactivated != args.WindowActivationState());
    }

    void MainWindow::PipButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (appWindow.Presenter().Kind() == AppWindowPresenterKind::CompactOverlay)
        {
            appWindow.SetPresenter(AppWindowPresenterKind::Overlapped);
        }
        else
        {
            appWindow.SetPresenter(AppWindowPresenterKind::CompactOverlay);
        }
    }

    void MainWindow::RootGrid_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        ForceToggleButton().IsChecked(unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ForceShutdown"), true));
        //ExitToggleButton().IsChecked(unbox_value_or<IReference<bool>>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ExitWhenTimerEnds"), false));
        NotifsToggleButton().IsChecked(unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"NotificationsEnabled"), true));

#ifdef USE_PRI_FOR_UI_ITEMS
        ResourceLoader resLoader{};
        ActionComboBox().Items().Clear();
        TextBlock text{};

        text.Text(resLoader.GetString(L"LockTextBlockText"));
        ActionComboBox().Items().Append(text);

        text = {};
        text.Text(resLoader.GetString(L"SleepTextBlockText"));
        ActionComboBox().Items().Append(text);

        text = {};
        text.Text(resLoader.GetString(L"ShutdownTextBlockText"));
        ActionComboBox().Items().Append(text);

        text = {};
        text.Text(resLoader.GetString(L"HibernateTextBlockText"));
        ActionComboBox().Items().Append(text);
#endif
        auto index = unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"SelectedAction"), 0);
        ActionComboBox().SelectedIndex(index);

        ApplicationDataContainer container = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"Timer");
        if (container)
        {
            Timer().Hours(unbox_value<int32_t>(container.Values().Lookup(L"Hours")));
            Timer().Minutes(unbox_value<int32_t>(container.Values().Lookup(L"Minutes")));
            Timer().Seconds(unbox_value<int64_t>(container.Values().Lookup(L"Seconds")));
        }
    }

    void MainWindow::ActionComboBox_SelectionChanged(IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&)
    {
    }

    void MainWindow::NotifsToggleButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"NotificationsEnabled", NotifsToggleButton().IsChecked());
    }

    void MainWindow::ExitToggleButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        //ApplicationData::Current().LocalSettings().Values().Insert(L"ExitWhenTimerEnds", ExitToggleButton().IsChecked());
    }

    void MainWindow::ForceToggleButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"ForceShutdown", ForceToggleButton().IsChecked());
    }

    void MainWindow::NotifsToggleButton_Checked(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        RingerOff().Visibility(Visibility::Collapsed);
        RingerOn().Visibility(Visibility::Visible);
    }

    void MainWindow::NotifsToggleButton_Unchecked(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        RingerOff().Visibility(Visibility::Visible);
        RingerOn().Visibility(Visibility::Collapsed);
    }

    void MainWindow::SettingsButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        if (LeftColumn().ActualWidth() == 0)
        {
            LeftColumn().Width(GridLength(1, GridUnitType::Star));
            RightColumn().Width(GridLength(0, GridUnitType::Pixel));
        }
        else
        {
            LeftColumn().Width(GridLength(0, GridUnitType::Pixel));
            RightColumn().Width(GridLength(1, GridUnitType::Star));
        }
    }

    void MainWindow::KeepOnTopToggleButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"KeepOnTop", KeepOnTopToggleButton().IsChecked());
        auto&& presenter = appWindow.Presenter().try_as<OverlappedPresenter>();
        if (presenter)
        {
            presenter.IsAlwaysOnTop(KeepOnTopToggleButton().IsChecked().Value());
        }
    }

    void MainWindow::TimerView_Elapsed(IInspectable const&, winrt::PowerClock::TimerChangeStatus const& e)
    {
        if (e == TimerChangeStatus::Elapsed)
        {
            PowerController controller{ ForceToggleButton().IsChecked().GetBoolean() };
            int index = ActionComboBox().SelectedIndex();

            switch (index)
            {
                case 0: // Lock
                    controller.Lock();
                    break;
                case 1: // Sleep
                    controller.Sleep(false);
                    break;
                case 2: // Shutdown
                    controller.Shutdown();
                    break;
                case 3: // Hibenate
                    controller.Sleep(true);
                    break;
            }
        }
        else if (e == TimerChangeStatus::TripPoint)
        {
            hstring action;
            int index = ActionComboBox().SelectedIndex();

            ResourceLoader resLoader{};
            switch (index)
            {
                case 0: // Lock
                    action = resLoader.GetString(L"NotificationLock30");;
                    break;
                case 1: // Sleep
                    action = resLoader.GetString(L"NotificationSleep30");
                    break;
                case 2: // Shutdown
                    action = resLoader.GetString(L"NotificationShutdown30");
                    break;
                case 3: // Hibenate
                    action = resLoader.GetString(L"NotificationHibernate30");
                    break;
            }

            // notify user (with tile) if notifications enabled
            NotifyUser(action);
        }
        else if (e == TimerChangeStatus::Started)
        {
            SecondRow().Height(GridLength(0, GridUnitType::Pixel));
            BottomGrid().Visibility(Visibility::Collapsed);
        }

        if (e == TimerChangeStatus::Elapsed || e == TimerChangeStatus::Stopped)
        {
            SecondRow().Height(GridLength(1, GridUnitType::Auto));
            BottomGrid().Visibility(Visibility::Visible);
        }
    }

    void MainWindow::DarkModeToggleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Go dark mode
        if (DarkModeToggleButton().IsChecked().GetBoolean())
        {
            IInspectable o = ApplicationData::Current().LocalSettings().Values().TryLookup(L"NightModeFontColor");
            if (o)
            {
                ApplicationDataCompositeValue composite = o.as<ApplicationDataCompositeValue>();
                uint8_t a = composite.Lookup(L"A").as<uint8_t>();
                uint8_t r = composite.Lookup(L"R").as<uint8_t>();
                uint8_t g = composite.Lookup(L"G").as<uint8_t>();
                uint8_t b = composite.Lookup(L"B").as<uint8_t>();
                Timer().CountdownForeground(SolidColorBrush(ColorHelper::FromArgb(a, r, g, b)));
            }
            else
            {
                Timer().CountdownForeground(Application::Current().Resources().Lookup(box_value(L"NightModeFontBrush")).as<Brush>());
            }

            
            SetBackground(winrt::PowerClock::BackdropControllerType::None);
        }
        else
        {
            Timer().CountdownForeground(Application::Current().Resources().Lookup(box_value(L"ApplicationForegroundThemeBrush")).as<Brush>());
            SetBackground(unbox_value_or(
                ApplicationData::Current().LocalSettings().Values().TryLookup(L"BackdropType"), ACRYLIC_BACKDROP) == 1 ?
                winrt::PowerClock::BackdropControllerType::AcrylicBackdrop : winrt::PowerClock::BackdropControllerType::MicaBackdrop
            );
        }
    }

    void MainWindow::MoreSettingsButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!colorsWindow)
        {
            colorsWindow = make<ColorsWindow>();
            colorsWindowClosedRevoker = colorsWindow.Closed(winrt::auto_revoke, [&](auto&&, auto&&)
            {
                colorsWindow = nullptr;
                colorsWindowClosedRevoker.revoke();
                colorsWindowBackdropChangedRevoker.revoke();
            });
            colorsWindowBackdropChangedRevoker = colorsWindow.BackdropChanged(winrt::auto_revoke, [&](auto&&, winrt::PowerClock::BackdropControllerType const& type)
            {
                SetBackground(type);
            });
        }
        colorsWindow.Activate();
    }


    void MainWindow::InitWindow()
    {
        ApplicationDataContainer settings = ApplicationData::Current().LocalSettings();

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
                customTitleBar = true;

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
                TitleBarRow().Height(GridLength{ 0, GridUnitType::Pixel });
            }

            FontIcon icon{};
            icon.FontSize(16);
            icon.Glyph(Application::Current().Resources().TryLookup(box_value(L"MiniContract2Mirrored")).as<hstring>());
            PipButton().Content(icon);

#pragma region Settings
            int width = 340;
            int height = 270;
            int y = 50;
            int x = 50;
            
            IInspectable windowSize = settings.Values().TryLookup(L"WindowSize");
            if (windowSize != nullptr)
            {
                ApplicationDataCompositeValue composite = windowSize.as<ApplicationDataCompositeValue>();
                IInspectable widthBoxed = composite.Lookup(L"Width");
                IInspectable heightBoxed = composite.Lookup(L"Height");
                if (widthBoxed)
                {
                    width = unbox_value<int>(widthBoxed);
                }
                if (heightBoxed)
                {
                    height = unbox_value<int>(heightBoxed);
                }
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

            //mapChangedRevoker = settings.Values().MapChanged({ this, &MainWindow::Settings_MapChanged });
            mapChangedEventToken = settings.Values().MapChanged({ this, &MainWindow::Settings_MapChanged });
#pragma endregion

            appWindow.Title(L"PwrCLK");
            appWindow.Closing({ this, &MainWindow::AppWindow_Closing });
            appWindow.Changed({ this, &MainWindow::AppWindow_Changed });
        }
    }

    void MainWindow::SaveSettings()
    {
        if (!isWindowFullscreen)
        {
            SavePosition();
        }

        ApplicationData::Current().LocalSettings().Values().Insert(L"SelectedAction", box_value(ActionComboBox().SelectedIndex()));

        auto&& container = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"Timer");
        if (!container)
        {
            container = ApplicationData::Current().LocalSettings().CreateContainer(L"Timer", ApplicationDataCreateDisposition::Always);
        }
        container.Values().Insert(L"Hours", box_value(Timer().Hours()));
        container.Values().Insert(L"Minutes", box_value(Timer().Minutes()));
        container.Values().Insert(L"Seconds", box_value(Timer().Seconds()));
    }

    void MainWindow::SavePosition()
    {
        IPropertySet settings = ApplicationData::Current().LocalSettings().Values();
        ApplicationDataCompositeValue setting = nullptr;

        if (settings.HasKey(L"WindowSize"))
        {
            setting = settings.Lookup(L"WindowSize").as<ApplicationDataCompositeValue>();
        }
        else
        {
            setting = ApplicationDataCompositeValue{};
        }
        setting.Insert(L"Width", box_value(appWindow.Size().Width));
        setting.Insert(L"Height", box_value(appWindow.Size().Height));
        settings.Insert(L"WindowSize", setting);

        if (settings.HasKey(L"WindowPosition"))
        {
            setting = settings.Lookup(L"WindowPosition").as<ApplicationDataCompositeValue>();
        }
        else
        {
            setting = ApplicationDataCompositeValue{};
        }
        setting.Insert(L"PositionX", box_value(appWindow.Position().X));
        setting.Insert(L"PositionY", box_value(appWindow.Position().Y));
        settings.Insert(L"WindowPosition", setting);
    }

    void MainWindow::SetDragRectangles()
    {
        HWND windowHandle = GetWindowFromWindowId(appWindow.Id());
        if (windowHandle != INVALID_HANDLE_VALUE)
        {
            UINT windowDpi = GetDpiForWindow(windowHandle);
            double scale = (windowDpi * static_cast<double>(100) + (static_cast<double>(96) / 2)) / 96;
            scale /= static_cast<double>(100);

            uint8_t appTitleBar = static_cast<uint8_t>(32 * scale);

            double leftPadding = appWindow.TitleBar().LeftInset() / scale;
            double rightPadding = appWindow.TitleBar().RightInset() / scale;

            RightPaddingColumn().Width(GridLength{ rightPadding });
            LeftPaddingColumn().Width(GridLength{ leftPadding });

            RectInt32 dragRectangleLeft{};
            dragRectangleLeft.X = static_cast<int32_t>(leftPadding);
            dragRectangleLeft.Y = 0;
            dragRectangleLeft.Height = static_cast<int32_t>(appTitleBar * scale);
            dragRectangleLeft.Width = 0;

            RectInt32 dragRectangleRight{};
            dragRectangleRight.X = static_cast<int32_t>((RightPaddingColumn().ActualWidth() + ContentColumn().ActualWidth()) * scale);
            dragRectangleRight.Y = 0;
            dragRectangleRight.Height = static_cast<int32_t>(appTitleBar * scale);
            dragRectangleRight.Width = static_cast<int32_t>((DragColumn().ActualWidth() - rightPadding) * scale);

            if (dragRectangleLeft.Width < 0)
            {
                dragRectangleLeft.Width = 0;
            }
            if (dragRectangleRight.Width < 0)
            {
                dragRectangleRight.Width = 0;
            }

            RectInt32 dragRectangles[2]{ dragRectangleLeft, dragRectangleRight };
            appWindow.TitleBar().SetDragRectangles(dragRectangles);
        }
    }

    void MainWindow::SetBackground(winrt::PowerClock::BackdropControllerType const& backdropType)
    {
        if (backdropType == winrt::PowerClock::BackdropControllerType::None)
        {
            backdropController.RemoveAllSystemBackdropTargets();
            // TODO: Set window background
            IInspectable value = ApplicationData::Current().LocalSettings().Values().TryLookup(L"NightModeBackgroundColor");
            if (value)
            {
                ApplicationDataCompositeValue composite = value.as<ApplicationDataCompositeValue>();
                uint8_t a = composite.Lookup(L"A").as<uint8_t>();
                uint8_t r = composite.Lookup(L"R").as<uint8_t>();
                r = r == 0 ? 40 : r;
                uint8_t g = composite.Lookup(L"G").as<uint8_t>();
                uint8_t b = composite.Lookup(L"B").as<uint8_t>();
                b = b == 0 ? 20 : b;
                RootGrid().Background(SolidColorBrush(ColorHelper::FromArgb(a, r, g, b)));
            }
        }
        else
        {
            if (backdropType == winrt::PowerClock::BackdropControllerType::AcrylicBackdrop && !DesktopAcrylicController::IsSupported())
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
                if (backdropType == winrt::PowerClock::BackdropControllerType::AcrylicBackdrop)
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

    void MainWindow::NotifyUser(hstring const& message)
    {
        if (unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"NotificationsEnabled"), true))
        {
            XmlDocument toastContent{};
            XmlElement root = toastContent.CreateElement(L"toast");
            toastContent.AppendChild(root);

            XmlElement visual = toastContent.CreateElement(L"visual");
            root.AppendChild(visual);

            XmlElement binding = toastContent.CreateElement(L"binding");
            binding.SetAttribute(L"template", L"ToastText01");
            visual.AppendChild(binding);

            XmlElement text = toastContent.CreateElement(L"text");
            text.SetAttribute(L"id", L"1");
            text.InnerText(message);
            binding.AppendChild(text);

            ToastNotification toastNotif{ toastContent };
            ToastNotificationManager::CreateToastNotifier().Show(toastNotif);
        }
    }

    void MainWindow::AppWindow_Closing(AppWindow const&, AppWindowClosingEventArgs const&)
    {
        SaveSettings();

        if (backdropController)
        {
            backdropController.Close();
        }
        if (dispatcherQueueController)
        {
            dispatcherQueueController.ShutdownQueueAsync();
        }
    }

    void MainWindow::AppWindow_Changed(AppWindow const&, AppWindowChangedEventArgs const& args)
    {
        if (appWindow)
        {
            if (args.DidSizeChange())
            {
                OverlappedPresenter presenter = appWindow.Presenter().try_as<OverlappedPresenter>();
                if (presenter)
                {
                    if (presenter.State() != OverlappedPresenterState::Maximized)
                    {
                        SavePosition();
                    }
                    isWindowFullscreen = presenter.State() == OverlappedPresenterState::Maximized;
                }

                SettingsButton().Visibility(RootGrid().ActualWidth() < 180 ? Visibility::Collapsed : Visibility::Visible);
            }

            if (args.DidPresenterChange())
            {
                if (appWindow.Presenter().Kind() == AppWindowPresenterKind::CompactOverlay)
                {
                    int32_t height = Application::Current().Resources().TryLookup(box_value(L"CompactModeHeight")).as<int32_t>();
                    int32_t width = Application::Current().Resources().TryLookup(box_value(L"CompactModeWidth")).as<int32_t>();
                    appWindow.Resize(SizeInt32{ width, height });

                    FontIcon icon{};
                    icon.FontSize(16);
                    icon.Glyph(Application::Current().Resources().TryLookup(box_value(L"MiniExpand2Mirrored")).as<hstring>());
                    PipButton().Content(icon);

                    KeepOnTopToggleButton().IsEnabled(false);
                }
                else
                {
                    FontIcon icon{};
                    icon.FontSize(16);
                    icon.Glyph(Application::Current().Resources().TryLookup(box_value(L"MiniContract2Mirrored")).as<hstring>());
                    PipButton().Content(icon);
                    KeepOnTopToggleButton().IsEnabled(true);
                }
            }
        }
    }

    void MainWindow::Settings_MapChanged(IObservableMap<hstring, IInspectable> const&, IMapChangedEventArgs<hstring> const& args)
    {
        /*if (!UseAcrylicToggleButton().IsChecked().GetBoolean() && args.Key() == L"NightModeBackgroundColor")
        {
            ApplicationDataCompositeValue composite = ApplicationData::Current().LocalSettings().Values().Lookup(L"NightModeBackgroundColor").as<ApplicationDataCompositeValue>();
            uint8_t a = composite.Lookup(L"A").as<uint8_t>();
            uint8_t r = composite.Lookup(L"R").as<uint8_t>();
            uint8_t g = composite.Lookup(L"G").as<uint8_t>();
            uint8_t b = composite.Lookup(L"B").as<uint8_t>();
            Background(SolidColorBrush(ColorHelper::FromArgb(a, r, g, b)));
        }*/
    }
}