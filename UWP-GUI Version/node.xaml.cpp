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
		this->shape->Stroke = dynamic_cast<Brush^>(XamlBindingHelper::ConvertValue(Windows::UI::Xaml::Interop::TypeName(this->shape->Stroke->GetType()), L"<LinearGradientBrush EndPoint='0.5, 1' StartPoint='0.5, 0'>< GradientStop Color = 'Black' /><GradientStop Color = '#FFF51B1B' Offset = '1' /><GradientStop Color = '#FF2E7822' Offset = '0.5' /><GradientStop Color = '#FFAC3D1D' Offset = '0' />< / LinearGradientBrush>"));
		break;
	}
}
