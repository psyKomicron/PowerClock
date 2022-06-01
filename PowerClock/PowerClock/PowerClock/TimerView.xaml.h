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

        inline winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
        {
            return e_propertyChanged.add(value);
        };
        inline void PropertyChanged(winrt::event_token const& token)
        {
            e_propertyChanged.remove(token);
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
        void Control_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UserControl_Unloaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NotifsToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ExitToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ForceToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NotifsToggleButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NotifsToggleButton_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SettingsButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ActionComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

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

        void Execute();
        void RestartTimer();
        void StartTimer();
        void StopTimer();
        void UpdateView(winrt::Windows::Foundation::TimeSpan timeSpan);
        void NotifyUser(winrt::hstring message);

        void DispatcherQueueTime_Tick(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);        
    };
}

namespace winrt::PowerClock::factory_implementation
{
    struct TimerView : TimerViewT<TimerView, implementation::TimerView>
    {
    };
}
