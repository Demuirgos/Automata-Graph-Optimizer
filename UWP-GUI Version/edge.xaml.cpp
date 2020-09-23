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

}


void Automata::edge::update()
{
	String^ path = "";
	if (start->Label != end->Label) {
		Point Tangent = Point();
		Point destination = Point((end->Position.X - start->Position.X), (end->Position.Y - start->Position.Y));
		float dist = sqrt(pow(destination.X, 2) + pow(destination.Y, 2));
		int xCoeff = destination.X < 0 ? -1 : 1;
		int yCoeff = destination.Y < 0 ? -1 : 1;
		float theta = atan(destination.X / destination.Y);
		destination = Point(destination.X*(1 - 7/dist), destination.Y * (1 - 7 / dist));
		Tangent = setRotate(Point(destination.X / 8,destination.Y / 8),Point(0,0),5);//Point(destination.X / 4 + 50* xCoeff * (cos(theta)-1/sqrt(2)), destination.Y / 4 + yCoeff * 50 * (cos(theta) - 1 / sqrt(2)));
		Point Arrow1 = setRotate(getCoordinates(Point(0, 0), destination, Tangent, 1 - 10 / dist), destination,  - 90 - 10);
		//Point Arrow2 = setRotate(getCoordinates(Point(0, 0), destination, Tangent, 1 - 10 / dist), destination,  90 - 10);
		path = "M  0, 0 Q " + Tangent.X.ToString() + "," + Tangent.Y.ToString() + " " + destination.X.ToString() + "," + destination.Y.ToString() + " " + "M " + destination.X.ToString() + "," + destination.Y.ToString() + " L " + Arrow1.X.ToString() + "," + Arrow1.Y.ToString();// +" " + "M " + destination.X.ToString() + "," + destination.Y.ToString() + " L " + Arrow2.X.ToString() + "," + Arrow2.Y.ToString();
	}
	else
		path = "M -3,1 A 11,11 0 1 1 1,1";
	Geometry^ geo = ref new PathGeometry();
	geo = dynamic_cast<Geometry^>(XamlBindingHelper::ConvertValue(Windows::UI::Xaml::Interop::TypeName(geo->GetType()), path));
	this->InnerLine->Data = geo;
	
}

void Automata::edge::Onmoved(node^ sender,Windows::Foundation::Point newPos)
{
	update();
	moved(this, this->Start);
}

Point Automata::edge::getCoordinates(Point s, Point f, Point c, float t)
{
	//(1-t) ((1-t)P0 + tP1) + t((1-t)P1 + tP2)
	return Point((1-t)*((1-t)*s.X+t*c.X)+t*((1-t)*c.X+t*f.X),(1 - t) * ((1 - t) * s.Y + t * c.Y) + t * ((1 - t) * c.Y + t * f.Y));
}

Point Automata::edge::setRotate(Point p, Point c ,float angle) {

	return Point(cos(angle) * (p.X - c.X) - sin(angle) * (p.Y - c.Y) + c.X,sin(angle) * (p.X - c.X) + cos(angle) * (p.Y - c.Y) + c.Y);
}