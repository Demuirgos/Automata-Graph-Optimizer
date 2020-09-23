//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Graphs.h"
#include "CanvasRenderer.xaml.h"
#include "EditPage.xaml.h"
using namespace Platform::Collections;

using namespace Automata;
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
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
MainPage::MainPage()
{
	InitializeComponent();
}

void Automata::MainPage::openPath_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FileOpenPicker^ filePick = ref new FileOpenPicker();
	filePick->ViewMode = PickerViewMode::Thumbnail;
	filePick->SuggestedStartLocation = PickerLocationId::ComputerFolder;
	filePick->FileTypeFilter->Append(".dot");
	filePick->FileTypeFilter->Append(".gv");
	filePick->FileTypeFilter->Append(".txt");
	TextBox^ temp = pathText;
	pathText->Text = "";
	MainPage^ placeHolder = this;
	concurrency::create_task(filePick->PickSingleFileAsync()).then([placeHolder,temp](StorageFile^ sampleFile) {
		if (sampleFile != nullptr) {
			temp->Text = sampleFile->Path;
			Concurrency::create_task(FileIO::ReadBufferAsync(sampleFile)).then([placeHolder](IBuffer^ buffer) {
				auto dataReader = Windows::Storage::Streams::DataReader::FromBuffer(buffer);
				placeHolder->data = dataReader->ReadString(buffer->Length);
				placeHolder->parseText();
				});
		}
		});
}

void Automata::MainPage::E_NFA_to_NFA_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (E_NFA_to_NFA->IsOn==true) {
		NFA_to_DFA->IsEnabled = true;
		this->EditBoard->OptimisationMode += 1;
	}
	else {
		NFA_to_DFA->IsOn = false;
		NFA_to_DFA->IsEnabled = false;
		this->EditBoard->OptimisationMode -= 1;
	}
}

void Automata::MainPage::NFA_to_DFA_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (NFA_to_DFA->IsOn == true) {
		DFA_to_minDFA->IsEnabled = true;
		this->EditBoard->OptimisationMode += 1;
	}
	else {
		DFA_to_minDFA->IsEnabled = false;
		DFA_to_minDFA->IsOn = false;
		this->EditBoard->OptimisationMode -= 1;
	}
}

void Automata::MainPage::DFA_to_minDFA_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (NFA_to_DFA->IsOn == true) {
		this->EditBoard->OptimisationMode += 1;
	}
	else {
		this->EditBoard->OptimisationMode -= 1;
	}
}

void Automata::MainPage::getRender()
{
	if (!this->g->isValid()) {
		ContentDialog^ Warning = ref new ContentDialog();
		String^ MSGContent = ref new String();
		MSGContent = "Please chose Starting and Ending Nodes";
		Warning->Title = "Missing Arguments";
		Warning->PrimaryButtonText = "OK";
		Warning->Content = MSGContent;
		Concurrency::create_task(Warning->ShowAsync());
	}
	else {
		optimizeGraph();
		this->ResultBoard->clear();
		this->ResultBoard->Graph = ref new GraphManaged(this->r);
		this->ResultBoard->start();
	}
	optimizeGraph();
}

void Automata::MainPage::parseText()
{
	this->TextInput->Text = this->data;
	FlyoutBase::ShowAttachedFlyout(dynamic_cast<FrameworkElement^>(this->InsertText));
	fillUnderlayingData();
}

void Automata::MainPage::fillUnderlayingData()
{
	this->g = ref new GraphManaged(this->data);
	Start_slect_grid->Items->Clear();
	End_slect_grid->Items->Clear();
	for (int i:this->g->Nodes) {
		CheckBox^ c = ref new CheckBox();
		c->Content = "Node :" + i.ToString();
		c->Name = i.ToString();
		Start_slect_grid->Items->Append(c);
		if (this->g->isStartNode(i)) c->IsChecked = true;
		c->Checked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Automata::MainPage::OnStartChecked);
		c->Unchecked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Automata::MainPage::OnStartUnchecked);
	}
	for (int i : this->g->Nodes) {
		CheckBox^ c = ref new CheckBox();
		c->Content = "Node :" + i.ToString();
		c->Name = i.ToString();
		End_slect_grid->Items->Append(c);
		if (this->g->isEndNode(i)) c->IsChecked = true;
		c->Checked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Automata::MainPage::OnEndChecked);
		c->Unchecked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Automata::MainPage::OnEndUnchecked);
	}
	this->Button->IsEnabled = true;
}

void Automata::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!this->g->isValid()) {
		ContentDialog^ Warning = ref new ContentDialog();
		String^ MSGContent = ref new String();
		MSGContent = "Please chose Starting and Ending Nodes";
		Warning->Title = "Missing Arguments";
		Warning->PrimaryButtonText = "OK";
		Warning->Content = MSGContent;
		Concurrency::create_task(Warning->ShowAsync());
	}
	else {
		optimizeGraph();
		this->ResultText->Text = this->r->Data;
	}
}

void Automata::MainPage::TextInput_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	this->data = dynamic_cast<TextBox^>(sender)->Text;
	fillUnderlayingData();
}

void Automata::MainPage::InsertText_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FlyoutBase::ShowAttachedFlyout(dynamic_cast<FrameworkElement^>(sender));
}

void Automata::MainPage::RenderButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getRender();
}

void Automata::MainPage::Holder_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	auto castedSender = dynamic_cast<Grid^>(sender);
	this->ResultBoard->Width = castedSender->ActualWidth; this->ResultBoard->Height = castedSender->ActualHeight;
}

void Automata::MainPage::OnStartChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto nodeId = Methods::StringToInt(((CheckBox^)sender)->Name);
	int rank = this->g->Boundaries->Lookup(nodeId);
	this->g->NodeSetState(nodeId, rank + 1);
}

void Automata::MainPage::OnStartUnchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto nodeId = Methods::StringToInt(((CheckBox^)sender)->Name);
	int rank = this->g->Boundaries->Lookup(nodeId);
	this->g->NodeSetState(nodeId, rank - 1);
}

void Automata::MainPage::OnEndChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto nodeId = Methods::StringToInt(((CheckBox^)sender)->Name);
	int rank = this->g->Boundaries->Lookup(nodeId);
	this->g->NodeSetState(nodeId, rank + 2);
}

void Automata::MainPage::OnEndUnchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto nodeId = Methods::StringToInt(((CheckBox^)sender)->Name);
	int rank = this->g->Boundaries->Lookup(nodeId);
	this->g->NodeSetState(nodeId, rank - 2);

}

void Automata::MainPage::optimizeGraph()
{
	bool p1 = E_NFA_to_NFA->IsOn, p2 = NFA_to_DFA->IsOn, p3 = DFA_to_minDFA->IsOn;
	this->r = ref new GraphManaged(this->g);
	if (p1 || p2 || p3) {
		this->r->Optimise(p1, p2, p3);
	}
}

void Automata::MainPage::BoardHolder_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto Sender = dynamic_cast<Pivot^>(sender);
	if (Sender->SelectedIndex == 1) {
		Start_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		End_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Collapsed; 
		Button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	else {
		Start_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Visible;
		End_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Visible;
		Button->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
}



void Automata::MainPage::InnerTextOpener_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FlyoutBase::ShowAttachedFlyout(dynamic_cast<FrameworkElement^>(sender));
}
