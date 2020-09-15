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
using namespace Windows::UI::Xaml::Markup;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

edge::edge(node^ start, node^ end, String^ weight)
{
	InitializeComponent();
	this->start = start;
	this->end = end;
	this->weight = weight;
	this->start->moved += ref new Automata::positionChanged(this, &Automata::edge::Onmoved);
	this->end->moved += ref new Automata::positionChanged(this, &Automata::edge::Onmoved);
	this->Label->Content = weight;
	//        <Path Stroke="Black" Data="M 5,10 A 15,15 0 1 1 20,10" StrokeThickness="5" />

}


void Automata::edge::update()
{
	String^ path = "";
	if(start->Label!=end->Label)
		path = "M  0, 0 A 15, 15 0 1 1" +  (end->Position.X - start->Position.X).ToString() + "," + (end->Position.Y - start->Position.Y).ToString() ;
	else
		path = "M 5,10 A 15,15 0 1 1 20,10";
	Geometry^ geo = ref new PathGeometry();
	geo = dynamic_cast<Geometry^>(XamlBindingHelper::ConvertValue(Windows::UI::Xaml::Interop::TypeName(geo->GetType()), path));
	this->InnerLine->Data = geo;
	this->Label->Margin = Thickness(((this->End.X + this->Start.X)/2)- this->Start.X + 10, ((this->End.Y + this->Start.Y) / 2) - this->Start.Y,0,0);
}

void Automata::edge::Onmoved(node^ sender,Windows::Foundation::Point newPos)
{
	update();
	moved(this, this->Start);
}

