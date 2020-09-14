//
// edge.xaml.cpp
// Implementation of the edge class
//

#include "pch.h"
#include "edge.xaml.h"

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

edge::edge(node^ start, node^ end, String^ weight)
{
	InitializeComponent();
	this->start = start;
	this->end = end;
	this->weight = weight;
	//this->InnerLine->X1 = start.X; this->InnerLine->Y1 = start.Y;
	this->start->moved += ref new Automata::positionChanged(this, &Automata::edge::Onmoved);
	this->end->moved += ref new Automata::positionChanged(this, &Automata::edge::Onmoved);
}


void Automata::edge::update()
{
	this->InnerLine->X2 = end->Position.X - start->Position.X;
	this->InnerLine->Y2 = end->Position.Y - start->Position.Y;
}

void Automata::edge::InnerLine_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	FlyoutBase::ShowAttachedFlyout(dynamic_cast<FrameworkElement^>(sender));
}


void Automata::edge::Onmoved(node^ sender,Windows::Foundation::Point newPos)
{
	update();
}
