//
// EditPage.xaml.cpp
// Implementation of the EditPage class
//

#include "pch.h"
#include "EditPage.xaml.h"

using namespace Automata;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

EditPage::EditPage()
{
	InitializeComponent();
}


void Automata::EditPage::SplitView_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args)
{
	this->ToolBox->Visibility = ::Visibility::Collapsed;
}


void Automata::EditPage::AddEdge_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void Automata::EditPage::AddNode_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void Automata::EditPage::OpenPane_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->editPage->IsPaneOpen = !(this->editPage->IsPaneOpen);
}


void Automata::EditPage::editPage_PaneOpening(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args)
{
	this->ToolBox->Visibility = ::Visibility::Visible;
}
