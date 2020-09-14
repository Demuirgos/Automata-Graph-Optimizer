//
// CanvasRenderer.xaml.h
// Declaration of the CanvasRenderer class
//

#pragma once

#include "CanvasRenderer.g.h"
#include "node.xaml.h"
#include "edge.xaml.h"
#include "Graphs.h"

namespace Automata
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CanvasRenderer sealed
	{
	public:
		CanvasRenderer();
		void InitializeComponent2();
		CanvasRenderer(GraphManaged ^ g);
		property GraphManaged^ Graph {
			GraphManaged^ get() { return this->g; }
			void set(GraphManaged^ newG) {
				this->g = newG;
				initialize();
			}
		}
		void start();
		void stop();
		void clear();
		void update();
	private:
		DispatcherTimer^ timer;
		GraphManaged^ g;
		Platform::Collections::Map<String^, node^>^ Layout;
		IVector<edge^>^ Linkers;
		float _dt = 1, _l = 50, _r = 0.005, _kr = 6900, _ks = _kr / (_r * pow(_l, 3));
		bool isRendered = false;
		void initialize();
		void process();
		void render();
		void Board_ManipulationDelta(Platform::Object ^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs ^ e);
		void OnNodemoved(Automata::node ^ sender, Windows::Foundation::Point newPos);
		void OnEdgemoved(Automata::edge ^ sender, Windows::Foundation::Point newPos);
		void Rslider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e);
		void OnTick(Platform::Object ^ sender, Platform::Object ^ args);
	};
}
