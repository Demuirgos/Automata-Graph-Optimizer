//
// EditPage.xaml.cpp
// Implementation of the EditPage class
//

#include "pch.h"
#include "EditPage.xaml.h"
#include "CanvasRenderer.xaml.h"

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
	this->separator->RenderTransform = ref new TranslateTransform();
	this->Board->EdgeInserted += ref new Automata::InsertionEventHandler(this, &Automata::EditPage::OnEdgeInserted);
	this->Board->NodeStatusUpdated += ref new Automata::BoundarieEventHandler(this, &Automata::EditPage::OnNodeStatusUpdated);
	textChanging = this->TextInput->TextChanged += ref new Windows::UI::Xaml::Controls::TextChangedEventHandler(this, &Automata::EditPage::OnTextChanged);
	this->BoardsHolder->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &Automata::EditPage::OnSizeChanged);
	this->BoardsHolder->PaneOpened += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::SplitView^, Platform::Object^>(this, &Automata::EditPage::OnPaneOpened);
	InitGraph("");
}

void Automata::EditPage::OnModifiedEvent(Object^ sender)
{
	this->EdgeStart->Items->Clear();
	this->EdgeEnd->Items->Clear();
	for (int node : this->g->Nodes) {
		this->EdgeStart->Items->Append(node.ToString());
		this->EdgeEnd->Items->Append(node.ToString());
	}
	this->Board->start(5);
}

void Automata::EditPage::InitGraph(String^ data)
{
	if (data == "")
		g = ref new GraphManaged();
	else
		g = ref new GraphManaged(data);
	this->Board->Graph = this->g;
	g->ModifiedEvent += ref new Automata::Modified(this, &Automata::EditPage::OnModifiedEvent);
	g->UpdateCompleteEvent += ref new Automata::Modified(this, &Automata::EditPage::OnUpdateCompleteEvent);
}

void Automata::EditPage::SplitView_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args)
{
	this->ToolBox->Visibility = ::Visibility::Collapsed;
}

void Automata::EditPage::AddEdge_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->EdgeStart->SelectedIndex == -1 && this->EdgeEnd->SelectedIndex == -1 && this->EdgeLabel->Text == "") {
		Methods::MessageBoxInvoke("Missing Data", "Please Fill All the required Data!", "OK");
	}
	else {
		auto s = Methods::StringToInt(dynamic_cast<String^>(this->EdgeStart->SelectedItem));
		auto e = Methods::StringToInt(dynamic_cast<String^>(this->EdgeEnd->SelectedItem));
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
			int boundary = isStart + (isEnd << 1);
			g->Boundaries->Insert(s, boundary);
			g->insert(s, s, "0");
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

void Automata::EditPage::OnUpdateCompleteEvent(Platform::Object^ sender)
{
	this->TextInput->TextChanged -= textChanging;
	this->TextInput->Text = this->g->Data;
	textChanging = this->TextInput->TextChanged += ref new Windows::UI::Xaml::Controls::TextChangedEventHandler(this, &Automata::EditPage::OnTextChanged);
}

void Automata::EditPage::Stop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Board->stop();
}

void Automata::EditPage::Render_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Board->start();
}

void Automata::EditPage::Optimise_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->BoardsHolder->IsPaneOpen = true;
}

void Automata::EditPage::Draw_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

void Automata::EditPage::Clear_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->g->Clear();
	this->Board->start(5);
}

void Automata::EditPage::OnTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	this->data = this->TextInput->Text;
	InitGraph(data);
	this->Board->start(5);
}

void Automata::EditPage::Revert_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Board->EnableEditingMode(false);
}

void Automata::EditPage::Edit_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Board->EnableEditingMode(true);
}

void Automata::EditPage::OnEdgeInserted(int s, int f, Platform::String^ w)
{
	this->g->insert(s, f, w);
}

void Automata::EditPage::OnNodeStatusUpdated(int s, bool isEnd, bool isStart)
{
	this->g->NodeSetState(s, isStart+(isEnd<<1));
}

void Automata::EditPage::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	this->Board->Height = max(e->NewSize.Height, this->Board->Height); this->Result->Height = e->NewSize.Height;
	this->Board->Width = max(e->NewSize.Width, this->Board->Width); this->Result->Height = e->NewSize.Height;
}

void Automata::EditPage::OnPaneOpened(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args)
{
	bool p1 = this->optimisatioMode > 0; bool p2 = this->optimisatioMode > 1; bool p3 = this->optimisatioMode > 2;
	this->Result->Graph = ref new GraphManaged(this->g);
	this->Result->Graph->Optimise(p1, p2, p3);
	this->Result->start();
}

void Automata::EditPage::PrevUpdate_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	int rank = isStart->IsOn + (this->isEnd->IsOn << 1);
	switch (rank) {
	case 1:
		this->NodePrev->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Green);
		break;
	case 2:
		this->NodePrev->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Red);
		break;
	case 0:
		this->NodePrev->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
		break;
	case 3:
		LinearGradientBrush^ linearStroke = ref new LinearGradientBrush();
		linearStroke->EndPoint = Point(0.5, 1); linearStroke->StartPoint = Point(0.5, 0);
		auto stop1 = ref new GradientStop(); stop1->Color = Windows::UI::Colors::Green; stop1->Offset = 0;
		auto stop2 = ref new GradientStop(); stop2->Color = Windows::UI::Colors::LightGray; stop2->Offset = 0.5;
		auto stop3 = ref new GradientStop(); stop3->Color = Windows::UI::Colors::Red; stop3->Offset = 1;
		linearStroke->GradientStops->Append(stop1);
		linearStroke->GradientStops->Append(stop2);
		linearStroke->GradientStops->Append(stop3);
		this->NodePrev->Stroke = linearStroke;
		break;
	}
}
