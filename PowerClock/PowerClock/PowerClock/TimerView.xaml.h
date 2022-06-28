#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "TimerView.g.h"

namespace winrt::PowerClock::implementation
{
    struct TimerView : TimerViewT<TimerView>
    {
    public:
        TimerView();
        ~TimerView();

        bool ButtonsEnabled() const;
        void ButtonsEnabled(bool const& value);
        bool IsReadOnly() const;
        void IsReadOnly(bool const& value);
        int32_t Hours() const;
        void Hours(int32_t const& value);
        int32_t Minutes() const;
        void Minutes(int32_t const& value);
        int64_t Seconds() const;
        void Seconds(int64_t const& value);

        winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
        {
            return e_propertyChanged.add(value);
        };
        void PropertyChanged(winrt::event_token const& token)
        {
            e_propertyChanged.remove(token);
        };
        winrt::event_token Changed(Windows::Foundation::TypedEventHandler<winrt::PowerClock::TimerView, TimerChangeStatus> const& handler)
        {
            return e_elapsed.add(handler);
        };
        void Changed(winrt::event_token const& token)
        {
            e_elapsed.remove(token);
        };

        void HoursUpButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void TextBox_BeforeTextChanging(winrt::Microsoft::UI::Xaml::Controls::TextBox const& sender, winrt::Microsoft::UI::Xaml::Controls::TextBoxBeforeTextChangingEventArgs const& args);
        void HoursDownButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MinutesUpButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MinutesDownButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SecondsUpButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SecondsDownButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void StartTimerButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RestartTimerButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Grid_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void HoursTextBox_PointerWheelChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void MinutesTextBox_PointerWheelChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void SecondsTextBox_PointerWheelChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void HoursTextBox_DoubleTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e);
        void MinutesTextBox_DoubleTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e);
        void SecondsTextBox_DoubleTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e);

    private:
        bool _buttonsEnabled = true;
        bool _isReadOnly = false;
        int32_t _hours = 0;
        int32_t _minutes = 0;
        int64_t _seconds = 0;
        winrt::event_token tickEventToken;
        winrt::event_token selectionChangedToken;
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer dispatcherTimer = nullptr;
        winrt::Windows::Foundation::TimeSpan originalTimeSpan;
        winrt::Windows::Foundation::TimeSpan currentTimeSpan;
        bool isTimerRunning = false;

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged;
        winrt::event<Windows::Foundation::TypedEventHandler<winrt::PowerClock::TimerView, TimerChangeStatus>> e_elapsed;

        void RestartTimer();
        void StartTimer();
        void StopTimer();
        void UpdateView(winrt::Windows::Foundation::TimeSpan timeSpan);

        void DispatcherQueueTime_Tick(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);
    };

    enum TimerChangeStatus
    {
        Started,
        Elapsed,
        Stopped,
        TripPoint
    };
}

namespace winrt::PowerClock::factory_implementation
{
    struct TimerView : TimerViewT<TimerView, implementation::TimerView>
    {
    };
}
