//
// EditPage.xaml.h
// Declaration of the EditPage class
//

#pragma once

#include "EditPage.g.h"

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
		Platform::String^ data;
		void SplitView_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args);
		void AddEdge_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddNode_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OpenPane_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void editPage_PaneOpening(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args);
	};
}
