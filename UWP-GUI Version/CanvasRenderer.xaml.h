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
	public delegate void InsertionEventHandler(int s, int f, String^ w);
	public delegate void BoundarieEventHandler(int s,bool isEnd,bool isStart);
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CanvasRenderer sealed
	{
	public:
		event InsertionEventHandler^ EdgeInserted;
		event BoundarieEventHandler^ NodeStatusUpdated;
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
		property float ScaleFactor {
			float get() { return this->ZoomController->ScaleX; }
		}
		void start();
		void start(int duration);
		void stop();
		void clear();
		void EnableEditingMode(bool b);
		void update();
	private:
		DispatcherTimer^ timer;
		bool editMode = false;
		Windows::UI::Xaml::Input::PointerRoutedEventArgs^ ptrID;
		bool drawing = false;
		bool dragging = false;
		bool suspended = false;
		String^ StartNode;
		String^ EndNode;
		Shapes::Line^ drawingEdge = ref new Shapes::Line();

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
		void Onhooked(Automata::node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Onreleased(Automata::node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Board_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Onlocked(Automata::node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnNodeStatusUpdated(int s, bool isEnd, bool isStart);
		void Ondeleted(Automata::node^ sender);
		void UserControl_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
	};
}
