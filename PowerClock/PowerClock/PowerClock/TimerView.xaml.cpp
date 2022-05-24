#include "pch.h"
#include "TimerView.xaml.h"
#if __has_include("TimerView.g.cpp")
#include "TimerView.g.cpp"
#endif

#include <chrono>
#include <PowerController.h>
using namespace winrt::Windows::Storage;

using namespace ::PowerClock::Common;
using namespace std::chrono_literals;
using namespace std::chrono;
using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Notifications;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PowerClock::implementation
{
    TimerView::TimerView()
    {
        InitializeComponent();
        dispatcherTimer = DispatcherQueue().CreateTimer();
        dispatcherTimer.Interval(duration(1s));
        tickEventToken = dispatcherTimer.Tick({ this, &TimerView::DispatcherQueueTime_Tick });
    }

    TimerView::~TimerView()
    {
        dispatcherTimer.Tick(tickEventToken);
    }

    bool TimerView::ButtonsEnabled() const
    {
        return _buttonsEnabled;
    }

    void TimerView::ButtonsEnabled(bool const& value)
    {
        _buttonsEnabled = value;
        e_propertyChanged(*this, PropertyChangedEventArgs{ L"ButtonsEnabled" });
    }

    bool TimerView::IsReadOnly() const
    {
        return _isReadOnly;
    }

    void TimerView::IsReadOnly(bool const& value)
    {
        _isReadOnly = value;
        e_propertyChanged(*this, PropertyChangedEventArgs{ L"IsReadOnly" });
    }

    int32_t TimerView::Hours() const
    {
        return _hours;
    }

    void TimerView::Hours(int32_t const& value)
    {
        if (value < 0)
        {
            return;
        }

        _hours = value;
        e_propertyChanged(*this, PropertyChangedEventArgs{ L"Hours" });
    }

    int32_t TimerView::Minutes() const
    {
        return _minutes;
    }

    void TimerView::Minutes(int32_t const& value)
    {
        if (value < 0)
        {
            return;
        }

        _minutes = value;
        e_propertyChanged(*this, PropertyChangedEventArgs{ L"Minutes" });
    }

    int64_t TimerView::Seconds() const
    {
        return _seconds;
    }

    void TimerView::Seconds(int64_t const& value)
    {
        if (value < 0)
        {
            return;
        }

        _seconds = value;
        e_propertyChanged(*this, PropertyChangedEventArgs{ L"Seconds" });
    }


    void TimerView::Control_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        ForceToggleButton().IsChecked(unbox_value_or<IReference<bool>>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ForceShutdown"), true));
        ExitToggleButton().IsChecked(unbox_value_or<IReference<bool>>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ExitWhenTimerEnds"), false));
        NotifsToggleButton().IsChecked(unbox_value_or<IReference<bool>>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"NotificationsEnabled"), true));
    }

    void TimerView::UserControl_Unloaded(IInspectable const&, RoutedEventArgs const&)
    {
    }

    void TimerView::TextBox_BeforeTextChanging(TextBox const&, TextBoxBeforeTextChangingEventArgs const& args)
    {
        args.Cancel(!(args.NewText().size() > 0 && args.NewText()[0] != '-'));
    }

    void TimerView::HoursUpButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Hours(Hours() + 1);
    }

    void TimerView::HoursDownButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Hours(Hours() - 1);
    }

    void TimerView::MinutesUpButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Minutes(Minutes() + 1);
    }

    void TimerView::MinutesDownButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Minutes(Minutes() - 1);
    }

    void TimerView::SecondsUpButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Seconds(Seconds() + 1);
    }

    void TimerView::SecondsDownButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Seconds(Seconds() - 1);
    }

    void TimerView::StartTimerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (isTimerRunning)
        {
            StopTimer();
        }
        else
        {
            StartTimer();
        }
    }

    void TimerView::RestartTimerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        RestartTimer();
    }

    void TimerView::NotifsToggleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"NotificationsEnabled", NotifsToggleButton().IsChecked());
    }

    void TimerView::ExitToggleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"ExitWhenTimerEnds", ExitToggleButton().IsChecked());
    }

    void TimerView::ForceToggleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"ForceShutdown", ForceToggleButton().IsChecked());
    }

    void TimerView::NotifsToggleButton_Checked(IInspectable const&, RoutedEventArgs const&)
    {
        RingerOff().Visibility(Visibility::Collapsed);
        RingerOn().Visibility(Visibility::Visible);
    }

    void TimerView::NotifsToggleButton_Unchecked(IInspectable const&, RoutedEventArgs const&)
    {
        RingerOff().Visibility(Visibility::Visible);
        RingerOn().Visibility(Visibility::Collapsed);
    }

    void TimerView::SettingsButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        GridLength width{};
        if (LeftColumn().ActualWidth() == 0)
        {
            width.GridUnitType = GridUnitType::Star;
            width.Value = 1;
            LeftColumn().Width(width);
        }
        else
        {
            width.GridUnitType = GridUnitType::Pixel;
            width.Value = 0;
            LeftColumn().Width(width);
        }

        width = GridLength{};
        if (RightColumn().ActualWidth() == 0)
        {
            width.GridUnitType = GridUnitType::Star;
            width.Value = 1;
            RightColumn().Width(width);
        }
        else
        {
            width.GridUnitType = GridUnitType::Pixel;
            width.Value = 0;
            RightColumn().Width(width);
        }
    }

    void TimerView::Execute()
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
            case 2: // Restart
                // not yet done.
                break;
            case 3: // Shutdown
                controller.Shutdown();
                break;
            case 4: // Hibenate
                controller.Sleep(true);
                break;
        }
        
        if (unbox_value_or<bool>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ExitWhenTimerEnds"), false))
        {
            Application::Current().Exit();
        }
    }

    void TimerView::RestartTimer()
    {
        RestartTimerButton().IsEnabled(false);
        StartTimerButton().IsEnabled(false);

        dispatcherTimer.Stop();
        currentTimeSpan = TimeSpan{ originalTimeSpan };
        UpdateView(currentTimeSpan);
        if (isTimerRunning)
        {
            dispatcherTimer.Start();
        }

        RestartTimerButton().IsEnabled(true);
        StartTimerButton().IsEnabled(true);
    }

    void TimerView::StartTimer()
    {
        originalTimeSpan = hours(Hours());
        originalTimeSpan += minutes(Minutes());
        originalTimeSpan += seconds(Seconds());
        currentTimeSpan = TimeSpan{ originalTimeSpan };

        dispatcherTimer.Start();
        isTimerRunning = true;

        StartTimerButtonIcon().Symbol(Symbol::Pause);
        ButtonsEnabled(false);
        IsReadOnly(true);
    }

    void TimerView::StopTimer()
    {
        dispatcherTimer.Stop();
        isTimerRunning = false;

        ButtonsEnabled(true);
        IsReadOnly(false);
        StartTimerButtonIcon().Symbol(Symbol::Play);
    }

    void TimerView::UpdateView(winrt::Windows::Foundation::TimeSpan timeSpan)
    {
        hours hours = duration_cast<std::chrono::hours>(timeSpan);
        timeSpan -= hours;
        minutes minutes = duration_cast<std::chrono::minutes>(timeSpan);
        timeSpan -= minutes;
        seconds seconds = duration_cast<std::chrono::seconds>(timeSpan);

        // Update text boxes
        Hours(hours.count());
        Minutes(minutes.count());
        Seconds(seconds.count());
    }

    void TimerView::NotifyUser(hstring message)
    {
        if (unbox_value_or<bool>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"NotificationsEnabled"), true))
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

    void TimerView::DispatcherQueueTime_Tick(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&)
    {
        currentTimeSpan -= seconds(1);
        UpdateView(currentTimeSpan);

        if ((currentTimeSpan / seconds(1)) == 0)
        {
            StopTimer();
            currentTimeSpan = TimeSpan{ originalTimeSpan };
            UpdateView(currentTimeSpan);
            Execute();
        }
        else if (currentTimeSpan == seconds(30))
        {
            hstring action;
            int index = ActionComboBox().SelectedIndex();
            switch (index)
            {
                case 0: // Lock
                    action = L"Lock in 30 seconds";
                    break;
                case 1: // Sleep
                    action = L"Sleep in 30 seconds";
                    break;
                case 2: // Restart
                    // not yet done. 
                    break;
                case 3: // Shutdown
                    action = L"Shutdown in 30 seconds";
                    break;
                case 4: // Hibenate
                    action = L"Hibernation in 30 seconds";
                    break;
            }
            
            // notify user (with tile) if notifications enabled
            NotifyUser(action);
        }
    }
}