//
// edge.xaml.h
// Declaration of the edge class
//

#pragma once

#include "edge.g.h"
#include "node.xaml.h"

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

namespace Automata
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class edge sealed 
	{
	public:
		edge(node^ start,node^ end, String^ weight);
		property Point Start {
			Point get() { return this->start->Position; }
			void set(Point v) { this->start->Position = v; }
		}
		property Point End {
			Point get() { return this->end->Position; }
			void set(Point v) { this->end->Position = v; }
		}
		property String^ Weight {
			String^ get() { return this->weight; }
			void set(String^ v) { this->weight = v; }
		}
		property float Size {
			float get() { return this->start->Size; }
		}
	private:
		node^ start;
		node^ end;
		String^ weight;
		void update();
		void InnerLine_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Onmoved(node^ sender,Windows::Foundation::Point newPos);
	};
}
