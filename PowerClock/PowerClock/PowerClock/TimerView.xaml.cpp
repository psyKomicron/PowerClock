﻿#include "pch.h"
#include "TimerView.xaml.h"
#if __has_include("TimerView.g.cpp")
#include "TimerView.g.cpp"
#endif

#include <chrono>
#include "Helper.h"

using namespace std::chrono_literals;

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media::Animation;
using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;

using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Input;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PowerClock::implementation
{
    TimerView::TimerView()
    {
        InitializeComponent();
        dispatcherTimer = DispatcherQueue().CreateTimer();
        dispatcherTimer.Interval(std::chrono::duration(1s));
        tickEventToken = dispatcherTimer.Tick({ this, &TimerView::DispatcherQueueTime_Tick });
    }

    TimerView::~TimerView()
    {
        dispatcherTimer.Tick(tickEventToken);
    }

    #pragma region Properties
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
            _minutes = 59;
        }
        else
        {
            _minutes = value % 60;
        }
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
            _seconds = 59;
        }
        else
        {
            _seconds = value % 60;
        }
        e_propertyChanged(*this, PropertyChangedEventArgs{ L"Seconds" });
    }

    winrt::Microsoft::UI::Xaml::Media::Brush TimerView::CountdownForeground()
    {
        return _countdownForeground;
    }

    void TimerView::CountdownForeground(winrt::Microsoft::UI::Xaml::Media::Brush const& value)
    {
        _countdownForeground = value;
        e_propertyChanged(*this, PropertyChangedEventArgs(L"CountdownForeground"));
    }
#pragma endregion


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

    void TimerView::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        double actualWidth = ActualWidth();
        double actualHeight = ActualHeight();
        constexpr double maxWidth = 1000.0;
        constexpr double maxHeight = 600.0;
        double width = actualWidth <= maxWidth ? ActualWidth() : maxWidth;
        double height = actualHeight <= maxHeight ? ActualHeight() : maxHeight;
        FontSize((height + width) * 0.147);

        if (actualHeight < 75)
        {
            FirstRow().Height(GridLength{ 0, GridUnitType::Pixel });
            LastRow().Height(GridLength{ 0, GridUnitType::Pixel });
            RestartTimerButton().Visibility(Visibility::Collapsed);
        }
        else
        {
            FirstRow().Height(GridLength{ 0, GridUnitType::Auto });
            LastRow().Height(GridLength{ 0, GridUnitType::Auto });
            RestartTimerButton().Visibility(Visibility::Visible);
        }
    }

    void TimerView::HoursTextBox_PointerWheelChanged(IInspectable const&, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
    {
        int32_t wheelData = e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
        if (wheelData > 0) // scroll up
        {
            Hours(Hours() + 1);
        }
        else // scroll down
        {
            Hours(Hours() - 1);
        }
    }

    void TimerView::MinutesTextBox_PointerWheelChanged(IInspectable const&, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
    {
        int32_t wheelData = e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
        if (wheelData > 0) // scroll up
        {
            Minutes(Minutes() + 1);
        }
        else // scroll down
        {
            Minutes(Minutes() - 1);
        }
    }

    void TimerView::SecondsTextBox_PointerWheelChanged(IInspectable const&, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
    {
        int32_t wheelData = e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
        if (wheelData > 0) // scroll up
        {
            Seconds(Seconds() + 1);
        }
        else // scroll down
        {
            Seconds(Seconds() - 1);
        }
    }

    void TimerView::HoursTextBox_DoubleTapped(IInspectable const&, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e)
    {
        Hours(0);
    }

    void TimerView::MinutesTextBox_DoubleTapped(IInspectable const&, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e)
    {
        Minutes(0);
    }

    void TimerView::SecondsTextBox_DoubleTapped(IInspectable const&, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e)
    {
        Seconds(0);
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
        originalTimeSpan = std::chrono::hours(Hours());
        originalTimeSpan += std::chrono::minutes(Minutes());
        originalTimeSpan += std::chrono::seconds(Seconds());
        currentTimeSpan = TimeSpan(originalTimeSpan);

        dispatcherTimer.Start();
        isTimerRunning = true;
        e_elapsed(*this, winrt::PowerClock::TimerChangeStatus::Started);

        Grid::SetRow(ControlButtonsPanel(), 2);
        Grid::SetColumnSpan(ControlButtonsPanel(), 3);
        Grid::SetColumn(ControlButtonsPanel(), 0);
        ControlButtonsPanel().Orientation(Orientation::Horizontal);

        StartTimerButtonIcon().Symbol(Symbol::Pause);
        ControlButtonsPanel().Margin(::PowerClock::Common::Helper::GetAppResource<Thickness>(L"TimerRunningMargin"));
        ButtonsEnabled(false);
        IsReadOnly(true);
    }

    void TimerView::StopTimer()
    {
        dispatcherTimer.Stop();
        isTimerRunning = false;

        e_elapsed(*this, PowerClock::TimerChangeStatus::Stopped);

        ButtonsEnabled(true);
        IsReadOnly(false);
        StartTimerButtonIcon().Symbol(Symbol::Play);

        Grid::SetRow(ControlButtonsPanel(), 0);
        Grid::SetRowSpan(ControlButtonsPanel(), 3);
        Grid::SetColumn(ControlButtonsPanel(), 4);
        ControlButtonsPanel().Orientation(Orientation::Vertical);
        ControlButtonsPanel().Margin(Thickness(0));
    }

    void TimerView::UpdateView(winrt::Windows::Foundation::TimeSpan timeSpan)
    {
        std::chrono::hours hours = duration_cast<std::chrono::hours>(timeSpan);
        timeSpan -= hours;
        std::chrono::minutes minutes = duration_cast<std::chrono::minutes>(timeSpan);
        timeSpan -= minutes;
        std::chrono::seconds seconds = duration_cast<std::chrono::seconds>(timeSpan);

        // Update text boxes & skip the checks
        int64_t hoursCount = hours.count();
        int64_t minutesCount = minutes.count();
        int64_t secondsCount = seconds.count();
        if (_hours != hoursCount)
        {
            _hours = hoursCount;
            e_propertyChanged(*this, PropertyChangedEventArgs{ L"Hours" });
        }
        if (_minutes != minutesCount)
        {
            _minutes = minutesCount;
            e_propertyChanged(*this, PropertyChangedEventArgs{ L"Minutes" });
        }
        if (_seconds != secondsCount)
        {
            _seconds = secondsCount;
            e_propertyChanged(*this, PropertyChangedEventArgs{ L"Seconds" });
        }
    }

    void TimerView::DispatcherQueueTime_Tick(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&)
    {
        currentTimeSpan -= std::chrono::seconds(1);
        UpdateView(currentTimeSpan);

        if ((currentTimeSpan / std::chrono::seconds(1)) == 0)
        {
            StopTimer();
            currentTimeSpan = TimeSpan{ originalTimeSpan };
            UpdateView(currentTimeSpan);
            e_elapsed(*this, winrt::PowerClock::TimerChangeStatus::Elapsed);
        }
        else if (currentTimeSpan == std::chrono::seconds(30))
        {
            e_elapsed(*this, winrt::PowerClock::TimerChangeStatus::TripPoint);
        }
    }
}