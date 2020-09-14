//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Graphs.h"
#include "CanvasRenderer.xaml.h"
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
	}
	else {
		NFA_to_DFA->IsOn = false;
		NFA_to_DFA->IsEnabled = false;
	}
}

void Automata::MainPage::NFA_to_DFA_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (NFA_to_DFA->IsOn == true) {
		DFA_to_minDFA->IsEnabled = true;
	}
	else {
		DFA_to_minDFA->IsEnabled = false;
		DFA_to_minDFA->IsOn = false;
	}
}


void Automata::MainPage::getRender()
{
	this->InputBoard->clear();
	this->InputBoard->Graph = this->g.ConvertFromNative();
	this->InputBoard->start();
}

void Automata::MainPage::parseText()
{
	this->TextInput->Text = this->data;
	this->InputText->Text = this->data;
	FlyoutBase::ShowAttachedFlyout(dynamic_cast<FrameworkElement^>(this->InsertText));
	fillUnderlayingData();
}

void Automata::MainPage::fillUnderlayingData()
{
	this->g = graph(Methods::ToCppString(this->data));
	Start_slect_grid->Items->Clear();
	End_slect_grid->Items->Clear();
	Start_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	End_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	if (!g.start_in_file)
		Start_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	for (map<int, map<string, set<int>>>::iterator i = g.node.begin(); i != g.node.end(); i++) {
		CheckBox^ c = ref new CheckBox();
		c->Content = "Node :" + i->first.ToString();
		c->Name = i->first.ToString();
		Start_slect_grid->Items->Append(c);
	}
	if (!g.end_in_file)
		End_slect_grid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	for (auto i = g.nodes.begin(); i != g.nodes.end(); i++) {
		CheckBox^ c = ref new CheckBox();
		c->Content = "Node :" + (*i).ToString();
		c->Name = (*i).ToString();
		End_slect_grid->Items->Append(c);
	}
	this->Button->IsEnabled = true;
}

void Automata::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!g.start_in_file) {
		g.start.clear();
		for (int i = 0; i < this->Start_slect_grid->Items->Size; i++) {
			CheckBox^ check1 = (CheckBox^)this->Start_slect_grid->Items->GetAt(i);
			if (check1 != nullptr && check1->IsChecked->Value == true) {
				g.start.insert(Methods::StringToInt(check1->Name));
			}
		}
	}
	if (!g.end_in_file) {
		g.end.clear();
		for (int i = 0; i < this->End_slect_grid->Items->Size; i++) {
			CheckBox^ check2 = (CheckBox^)this->End_slect_grid->Items->GetAt(i);
			if (check2 != nullptr && check2->IsChecked->Value == true) {
				g.end[Methods::StringToInt(check2->Name)] = true;
			}
		}
	}
	if (g.start.size() == 0 || g.end.size() == 0) {
		ContentDialog^ Warning = ref new ContentDialog();
		String^ MSGContent = ref new String();
		MSGContent = "Please chose Starting and Ending Nodes";
		Warning->Title = "Missing Arguments";
		Warning->PrimaryButtonText = "OK";
		Warning->Content = MSGContent;
		Concurrency::create_task(Warning->ShowAsync());
	}
	else {
		bool p1 = E_NFA_to_NFA->IsOn, p2 = NFA_to_DFA->IsOn, p3 = DFA_to_minDFA->IsOn;
		this->r = graph(this->g);
		this->r.optimize(p1, p2, p3);
		this->ResultText->Text = this->r.ToString();
	}
}

void Automata::MainPage::TextInput_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	this->InputText->Text = dynamic_cast<TextBox^>(sender)->Text;
	this->data = dynamic_cast<TextBox^>(sender)->Text;
	fillUnderlayingData();
}

void Automata::MainPage::InsertText_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FlyoutBase::ShowAttachedFlyout(dynamic_cast<FrameworkElement^>(sender));
}



void Automata::MainPage::InputText_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	this->TextInput->Text = dynamic_cast<TextBox^>(sender)->Text;
	this->data = dynamic_cast<TextBox^>(sender)->Text;
	fillUnderlayingData();
}


void Automata::MainPage::RenderButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getRender();
}


