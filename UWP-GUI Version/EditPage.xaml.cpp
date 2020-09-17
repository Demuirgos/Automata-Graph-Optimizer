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
	g = ref new GraphManaged();
	this->separator->RenderTransform = ref new TranslateTransform();
	g->ModifiedEvent += ref new Automata::Modified(this, &Automata::EditPage::OnModifiedEvent);
}

void Automata::EditPage::OnModifiedEvent(Object^ sender)
{
	this->EdgeStart->Items->Clear();
	this->EdgeEnd->Items->Clear();
	for (int node : this->g->Nodes) {
		this->EdgeStart->Items->Append(node.ToString());
		this->EdgeEnd->Items->Append(node.ToString());
	}
}


void Automata::EditPage::SplitView_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args)
{
	this->ToolBox->Visibility = ::Visibility::Collapsed;
}


void Automata::EditPage::AddEdge_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->EdgeStart->SelectedIndex != -1 && this->EdgeEnd->SelectedIndex != -1 && this->EdgeLabel->Text != "") {
		Methods::MessageBoxInvoke("Missing Data", "Please Fill All the required Data!", "OK");
	}
	else {
		auto s = Methods::StringToInt(dynamic_cast<String^>(this->EdgeStart->SelectedItem));
		auto e = Methods::StringToInt(dynamic_cast<String^>(this->EdgeStart->SelectedItem));
		this->g->insert(s, e, this->EdgeLabel->Text);
	}
}


void Automata::EditPage::AddNode_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ labl = this->NodeLabel->Text;
	bool isStart = this->isStart->IsOn;
	bool isEnd = this->isEnd->IsOn;
	if (labl == "") {
		Methods::MessageBoxInvoke("Missing Data", "Please Add a label to the Node!","OK");
	}
	else {
		int s = Methods::StringToInt(labl);
		uint32 index;
		if (this->g->Nodes->IndexOf(s,&index)) {
			Methods::MessageBoxInvoke("Duplciate Found", "A node with the same label already exists!","OK");
		}
		else {
			g->insert(s, s, "0");
			int boundary = isStart + (isEnd << 1);
			g->Boundaries->Insert(s, boundary);
		}
	}
}


void Automata::EditPage::OpenPane_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->editPage->IsPaneOpen = !(this->editPage->IsPaneOpen);
}


void Automata::EditPage::editPage_PaneOpening(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args)
{
	this->ToolBox->Visibility = ::Visibility::Visible;
}

void Automata::EditPage::separator_ManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e)
{
	float dy = e->Delta.Translation.Y;
	if (this->BoardsHolder->Height + dy > 0 && this->TextInput->Height - dy > 20) {
		this->BoardsHolder->Height += dy;
		this->TextInput->Height -= dy;
		dynamic_cast<TranslateTransform^>(this->separator->RenderTransform)->Y += dy;
	}
}


void Automata::EditPage::separator_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	Window::Current->CoreWindow->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::SizeNorthSouth, 0);
}


void Automata::EditPage::separator_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	Window::Current->CoreWindow->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
}
