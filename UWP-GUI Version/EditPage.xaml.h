//
// EditPage.xaml.h
// Declaration of the EditPage class
//

#pragma once

#include "EditPage.g.h"
#include "GraphManaged.h"

namespace Automata
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class EditPage sealed
	{
	public:
		EditPage();
	private:
		GraphManaged^ g;
		Windows::Foundation::EventRegistrationToken textChanging;
		Platform::String^ data;
		void InitGraph(String^ data);
		void SplitView_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args);
		void AddEdge_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddNode_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OpenPane_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void editPage_PaneOpening(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args);
		void OnModifiedEvent(Object^ sender);
		void separator_ManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e);
		void separator_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void separator_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnUpdateCompleteEvent(Platform::Object^ sender);
		void Stop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Render_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Manage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Optimise_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Draw_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Clear_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Edit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void Revert_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
