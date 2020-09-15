//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Graphs.h"
#include "node.xaml.h"
#include "edge.xaml.h"

using namespace std;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::Pickers;
using namespace Platform;
using namespace Windows::System;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

namespace Automata
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
	private:
		String^ data;
		graph g;
		graph r;
		DispatcherTimer^ timer;
		bool isNodesRendered = false,isEdgesRendered=false;
		void getRender();
		void parseText();
		void fillUnderlayingData();
		void openPath_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void E_NFA_to_NFA_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void NFA_to_DFA_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextInput_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void InsertText_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void InputText_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void RenderButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Holder_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnStartChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnEndChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnStartUnchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnEndUnchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void optimizeGraph();
	};
}
