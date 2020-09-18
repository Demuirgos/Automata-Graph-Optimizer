//
// CanvasRenderer.xaml.h
// Declaration of the CanvasRenderer class
//

#pragma once

#include "CanvasRenderer.g.h"
#include "node.xaml.h"
#include "edge.xaml.h"
#include "GraphManaged.h"

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
		void start(int duration);
		void stop();
		void clear();
		void EnableManagementMode(bool b) {
			isEditModeOn = b;
			this->initialize();
			this->start(5);
		}
		void update();
	private:
		bool isEditModeOn = false;
		bool dragging = false;
		DispatcherTimer^ timer;
		GraphManaged^ g;
		Platform::Collections::Map<String^, node^>^ Layout;
		IVector<edge^>^ Linkers;
		ScaleTransform^ ZoomController;
		float _dt = 2, _l = 100, _r = 0.005, _kr = 10000, _maxdp= 10000, _ks = _kr / (_r * pow(_l, 3));
		bool isRendered = false;
		bool isTimed = false;
		int duration = 0,count = 0;
		void initialize();
		void process();
		void render();
		void Board_ManipulationDelta(Platform::Object ^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs ^ e);
		void OnNodemoved(Automata::node ^ sender, Windows::Foundation::Point newPos);
		void OnEdgemoved(Automata::edge ^ sender, Windows::Foundation::Point newPos);
		void Rslider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e);
		void OnTick(Platform::Object ^ sender, Platform::Object ^ args);
		void Board_PointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e);
		void OnManipulationCompleted(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs^ e);
		void OnTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);
	};
}
