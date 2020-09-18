//
// node.xaml.cpp
// Implementation of the node class
//

#include "pch.h"
#include "node.xaml.h"

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
using namespace Windows::UI::Xaml::Markup;
// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

Automata::node::node(String^ label, bool StartFlag, bool EndFlag, Point start)
{
	InitializeComponent();
	this->_Label = label;
	this->IsStartNode = StartFlag;
	this->IsEndNode = EndFlag;
	this->Location = start;
	Initialize();
}

Automata::node::node(String^ label, bool StartFlag, bool EndFlag, float size)
{
	InitializeComponent();
	this->_Label = label;
	this->IsStartNode = StartFlag;
	this->IsEndNode = EndFlag;
	this->Location = Point(0,0);
	this->size = size;
	Initialize();
}

void Automata::node::Initialize()
{
	this->Name = _Label;
	this->id->Text = this->_Label;
	this->shape->Height = this->size;
	this->shape->Width = this->size;
	this->Force = Point(0, 0);
	update();
}

void Automata::node::UnsibscribeDragEvents()
{
	this->ManipulationDelta -= this->DragTokenOnGoing;
	this->ManipulationCompleted -= this->DragTokenCompleteEventToken;
	this->Tapped -= this->LeftTapEventToken;
	this->RightTapped -= this->RightTapTokenToken;
}

void Automata::node::UnsibscribeDrawEvents()
{
	this->PointerEntered -= this->MouseEnterEventToken;
	this->PointerPressed -= this->PointerPressedEventToken;
	this->PointerReleased -= this->PointerPressedEventToken;
}

void Automata::node::update()
{
	switch (this->type) {
	case NodeType::StartingNode : 
		this->shape->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Green);
		break;
	case NodeType::EndingNode:
		this->shape->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Red);
		break;
	case NodeType::OrdinaryNode:
		this->shape->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
		break;
	case NodeType::StartingAndEndingNode:
		LinearGradientBrush^ linearStroke = ref new LinearGradientBrush();
		linearStroke->EndPoint = Point(0.5, 1); linearStroke->StartPoint = Point(0.5, 0);
		auto stop1 = ref new GradientStop(); stop1->Color = Windows::UI::Colors::Green; stop1->Offset = 0;
		auto stop2 = ref new GradientStop(); stop2->Color = Windows::UI::Colors::LightGray; stop2->Offset = 0.5;
		auto stop3 = ref new GradientStop(); stop3->Color = Windows::UI::Colors::Red; stop3->Offset = 1;
		linearStroke->GradientStops->Append(stop1);
		linearStroke->GradientStops->Append(stop2);
		linearStroke->GradientStops->Append(stop3);
		this->shape->Stroke = linearStroke;
		break;
	}
}
