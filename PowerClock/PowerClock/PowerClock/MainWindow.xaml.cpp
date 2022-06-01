#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Foundation::Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PowerClock::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        InitWindow();
    }

    void MainWindow::TitleBarGrid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        if (customTitleBar && appWindow)
        {
            SetDragRectangles();
        }
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

    void MainWindow::InitWindow()
    {
        auto nativeWindow{ this->try_as<::IWindowNative>() };
        check_bool(nativeWindow);
        HWND handle{ nullptr };
        nativeWindow->get_WindowHandle(&handle);
        if (nativeWindow == nullptr)
        {
            OutputDebugString(L"Failed to get window handle.");
            return;
        }

        WindowId windowID = GetWindowIdFromWindow(handle);
        appWindow = AppWindow::GetFromWindowId(windowID);
        if (appWindow != nullptr)
        {
            if (AppWindowTitleBar::IsCustomizationSupported())
            {
                customTitleBar = true;

                appWindow.TitleBar().ExtendsContentIntoTitleBar(true);

                appWindow.TitleBar().ButtonBackgroundColor(
                    Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonInactiveForegroundColor(Colors::Gray());
                appWindow.TitleBar().ButtonHoverBackgroundColor(
                    Application::Current().Resources().TryLookup(box_value(L"AppTitleBarHoverColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonPressedBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonPressedForegroundColor(Colors::White());
            }
            else
            {
                TitleBarGrid().Height(0);
                TitleBarRow().Height(GridLength{ 0, GridUnitType::Pixel });

                //ExtendsContentIntoTitleBar(true);
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
            ApplicationDataContainer settings = ApplicationData::Current().LocalSettings();
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
            if (windowSize != nullptr)
            {
                ApplicationDataCompositeValue composite = windowPosition.as<ApplicationDataCompositeValue>();
                IInspectable posX = composite.TryLookup(L"PositionX");
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
                PointInt32 position{ x, y };
                appWindow.Move(position);
            }
            else
            {
                RectInt32 rect{};
                rect.X = x;
                rect.Y = y;
                rect.Height = height;
                rect.Width = width;
                appWindow.MoveAndResize(rect);
            }
#pragma endregion


            appWindow.Title(L"Multitool");

            appWindow.Closing({ this, &MainWindow::AppWindow_Closing });
            appWindow.Changed({ this, &MainWindow::AppWindow_Changed });
        }
    }

    void MainWindow::SaveWindowState()
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

        setting = nullptr;
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

            uint8_t appTitleBar = 32 * scale;

            double leftPadding = appWindow.TitleBar().LeftInset() / scale;
            double rightPadding = appWindow.TitleBar().RightInset() / scale;

            RightPaddingColumn().Width(GridLength{ rightPadding });
            LeftPaddingColumn().Width(GridLength{ leftPadding });

            RectInt32 dragRectangleLeft{};
            dragRectangleLeft.X = leftPadding;
            dragRectangleLeft.Y = 0;
            dragRectangleLeft.Height = appTitleBar * scale;
            dragRectangleLeft.Width = 0;

            RectInt32 dragRectangleRight{};
            dragRectangleRight.X = (RightPaddingColumn().ActualWidth() + ContentColumn().ActualWidth()) * scale;
            dragRectangleRight.Y = 0;
            dragRectangleRight.Height = appTitleBar * scale;
            dragRectangleRight.Width = (DragColumn().ActualWidth() - rightPadding) * scale;

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

    void MainWindow::AppWindow_Closing(AppWindow const&, AppWindowClosingEventArgs const&)
    {
        SaveWindowState();
    }

    void MainWindow::AppWindow_Changed(AppWindow const&, AppWindowChangedEventArgs const& args)
    {
        if (appWindow)
        {
            if (args.DidSizeChange())
            {
                SaveWindowState();
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
                }
                else
                {
                    FontIcon icon{};
                    icon.FontSize(16);
                    icon.Glyph(Application::Current().Resources().TryLookup(box_value(L"MiniContract2Mirrored")).as<hstring>());
                    PipButton().Content(icon);
                }
            }
        }
    }
}

void winrt::PowerClock::implementation::MainWindow::KeepTopButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}
