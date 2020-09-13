//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Graphs.h"
#include "node.xaml.h"
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
string ToCppString(String^ s) {
	string result;
	for (auto i : s) {
		result += i==L'\r'?'\n':i;
	}
	return result;
}
String^ FromCppString(std::string s) {;
	std::wstring ws;
	ws.assign(s.begin(), s.end());
	String^ result = ref new String(ws.c_str());
	return result;
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


void Automata::MainPage::TextBlock_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void Automata::MainPage::pathText_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{

}

int StringToInt(String^ s) {
	string new_s = ToCppString(s).c_str();
	int n = 0;
	for (int i = 0; i < new_s.size(); i++) {
		n = n * 10 + new_s[i] - '0';
	}
	return n;
}

void  Automata::MainPage::linkCanvs(Canvas^ c1, Canvas^ c2) {
	Shapes::Polyline^ Line = ref new Shapes::Polyline();
	Line->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
	Line->StrokeThickness = 4;
	PointCollection^ vertices = ref new PointCollection();
	float c1X = this->Board->GetLeft(c1) + 50/2;
	float c1Y = this->Board->GetTop(c1) + 60 / 2;
	float c2X = this->Board->GetLeft(c2) + 50 / 2;
	float c2Y = this->Board->GetTop(c2) + 60 / 2;
	Point*  p1 = new Point(c2X, c2Y);
	Point* p2 = new Point(c1X, c1Y);
	vertices->Append(*p1);
	vertices->Append(*p2);
	Line->Points = vertices;
	this->Board->Children->Append(Line);
	this->Board->SetZIndex(Line, -1);
}

void Automata::MainPage::getRender()
{
	DispatcherTimer^ timer = ref new DispatcherTimer();
	int timeDelay = 1;
	TimeSpan ts = TimeSpan();
	ts.Duration = timeDelay;
	timer->Interval = TimeSpan(ts);
	timer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &Automata::MainPage::OnTick);
	timer->Start();
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
	g = graph(ToCppString(this->data), 0);
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

void  Automata::MainPage::write(graph* g, int i, String^& accumulated) {
	if (!g->Processed[i]) {
		g->Processed[i] = true;
		for (map<string, set<int>>::iterator j = g->node[i].begin(); j != g->node[i].end(); j++) {
			if(j->first!="0")
				for (auto d : j->second) {
					if (g->nodes.find(d) != g->nodes.end()) {
						accumulated += i.ToString() + "->" + d.ToString() + " [label=\"" + FromCppString(j->first) + "\"];\n";
						if (g->end[i]) {
							accumulated += i.ToString() + "->" + "e;\n";
							g->end[i] = false;
						}
						write(g, d, accumulated);
					}
				}
		}
	}
}

void Automata::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	isRendered = false;
	if (!g.start_in_file) {
		g.start.clear();
		for (int i = 0; i < g.countNodes; i++) {
			CheckBox^ check1 = (CheckBox^)this->Start_slect_grid->Items->GetAt(i);
			if (check1!=nullptr && check1->IsChecked->Value == true) {
				g.start.insert(StringToInt(check1->Name));
			}
		}
	}
	if (!g.end_in_file) {
		g.end.clear();
		for (int i = 0; i < g.countNodes; i++) {
			CheckBox^ check2 = (CheckBox^)this->End_slect_grid->Items->GetAt(i);
			if (check2 != nullptr && check2->IsChecked->Value == true) {
				g.end[StringToInt(check2->Name)] = true;
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
		this->Board->Children->Clear();
		bool p1 = E_NFA_to_NFA->IsOn, p2 = NFA_to_DFA->IsOn, p3 = DFA_to_minDFA->IsOn;
		this->r.copyOf(&this->g);
		this->r.optimize(p1, p2, p3);
		graph* r = &this->r;
		StorageFolder^ storageFolder = ApplicationData::Current->LocalFolder;
		MainPage^ placeHolder = this;
		concurrency::create_task(storageFolder->CreateFileAsync("result.dot", CreationCollisionOption::ReplaceExisting));
		Concurrency::create_task(storageFolder->GetFileAsync("result.dot")).then([placeHolder,r](StorageFile^ sampleFile)
			{
				Concurrency::create_task(sampleFile->OpenAsync(FileAccessMode::ReadWrite)).then([placeHolder,sampleFile, r](IRandomAccessStream^ stream)
					{
						IOutputStream^ outputStream = stream->GetOutputStreamAt(0);
						DataWriter^ dataWriter = ref new DataWriter(outputStream);
						String^ accumulated = "";
						accumulated+="digraph result{\n";
						for (auto i : r->start) {
							accumulated += "s" + "->" + i.ToString() + ";\n";
							placeHolder->write(r, i, accumulated);
						}
						accumulated+="}";
						placeHolder->ResultText->Text = accumulated;
					});
			});
	}
}

void Automata::MainPage::render_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

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


void Automata::MainPage::OnTick(Platform::Object^ sender, Platform::Object^ args)
{
	auto L = 69; // spring rest length
	auto Kr = 6543; // repulsive force constant
	auto Ks = 1.23; // spring constant
	auto dt = (double)dynamic_cast<DispatcherTimer^>(sender)->Interval.Duration;
	bool rendered = this->isRendered;
	MainPage^ placeHolder = this;
	struct args {
		args(graph* _g, Canvas^ _c, IMap<String^, node^>^ _m) {
			g = _g; c = _c; m = _m;
		}
		graph* g;
		Canvas^ c;
		IMap<String^, node^>^ m;
	};
	auto getLayoutMap = [=](graph& Graph, Canvas^ board) {
		Map<String^, node^>^ mappingLayout = ref new Map<String^, node^>();
		if (!rendered) {
			board->Children->Clear();
			int j = 0;
			for (auto nodePair : Graph.nodes) {
				mappingLayout->Insert(nodePair.ToString(), ref new node(nodePair.ToString(), Graph.start.find(nodePair) != Graph.start.end(), Graph.end[nodePair], Point((float)(++j % (int)placeHolder->ActualWidth), (j / placeHolder->ActualWidth))));
			}
		}
		else
			for (Object^ child : board->Children) {
				auto castedChild = dynamic_cast<node^>(child);
				mappingLayout->Insert(castedChild->Name, castedChild);
			}
		return args(&Graph, board, mappingLayout);
	};
	auto process = [=](args arguments) {
		auto Graph = arguments.g;
		auto mappingLayout = arguments.m;
		auto board = arguments.c;
		auto calculateForces = [=](node^& Node1,node^& Node2,float forceCoeff,bool isRepuslive) {
			Point dv = Point(Node2->Position.X - Node1->Position.X, Node2->Position.Y - Node1->Position.Y);
			if (dv != Point(0, 0)) {
				double distance = sqrt(dv.X * dv.X + dv.Y * dv.Y);
				float f = isRepuslive ? (forceCoeff/(distance* distance)) : (forceCoeff*(distance-L));
				Point F = Point((float)f * dv.X / distance, (float)f * dv.Y / distance);
				Node1->Force = Point(Node1->Force.X + isRepuslive ? (-1) : 1 * F.X, Node1->Force.Y + isRepuslive ? (-1) : 1 * F.Y);
				Node2->Force = Point(Node1->Force.X + isRepuslive ? 1 : (-1) * F.X, Node1->Force.Y + isRepuslive ? 1 : (-1) * F.Y);
			}
		};
		for (auto Node1PaIR: mappingLayout) {
			for (auto Node2PaIR : mappingLayout) {
				if (Node1PaIR->Key != Node2PaIR->Key) {
					auto Node1 = Node1PaIR->Value;
					auto Node2 = Node2PaIR->Value;
					calculateForces(Node1, Node2,Kr,true);
				}
			}
		}
		for (auto GraphNode : Graph->node) {
			auto Node1 = mappingLayout->Lookup(GraphNode.first.ToString());
			for (auto edgeLiaison : GraphNode.second) {
				for (auto neighbor : edgeLiaison.second) {
					auto Node2 = mappingLayout->Lookup(neighbor.ToString());
					calculateForces(Node1, Node2,Ks,false);
				}
			}
		}
		if (placeHolder->isRendered) {
			for (auto NodeLayout : mappingLayout) {
				auto Node = NodeLayout->Value;
				Point dv = Point(Node->Force.X * dt + Node->Force.X * dt * dt, Node->Force.Y * dt + Node->Force.Y * dt * dt);
				Node->Position = Point(Node->Position.X + dv.X, Node->Position.Y + dv.Y);
			}
		}
		else {
			placeHolder->isRendered = true;
			for (auto Node : mappingLayout) {
				board->Children->Append(Node->Value);
			}
		}
		for (auto Node : mappingLayout) {
			board->SetLeft(Node->Value, Node->Value->Position.X);
			board->SetTop(Node->Value, Node->Value->Position.Y);
		}
	};
	process(getLayoutMap(this->g, this->Board));
	/*Render result graph*/{
		
	}
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
