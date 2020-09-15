//
// CanvasRenderer.xaml.cpp
// Implementation of the CanvasRenderer class
//

#include "pch.h"
#include "CanvasRenderer.xaml.h"

using namespace Automata;


using namespace Platform;
using namespace Platform::Collections;
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

CanvasRenderer::CanvasRenderer()
{
	InitializeComponent();
	InitializeComponent2();
}

void Automata::CanvasRenderer::InitializeComponent2()
{
	this->Board->Width = this->Width; this->Board->Height = this->Height;
	ZoomController = ref new ScaleTransform();
	this->Board->RenderTransform = ZoomController;
	ZoomController->CenterX = 0; ZoomController->CenterY = 0;
	ZoomController->ScaleX = 1; ZoomController->ScaleY = 1;
	this->timer = ref new DispatcherTimer();
	int timeDelay = 1;
	TimeSpan ts = TimeSpan();
	ts.Duration = timeDelay;
	timer->Interval = TimeSpan(ts);
	timer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &Automata::CanvasRenderer::OnTick);
	this->Layout = ref new Map<String^, node^>();
	this->Linkers = ref new Vector<edge^>();
}

Automata::CanvasRenderer::CanvasRenderer(GraphManaged^ g)
{
	InitializeComponent();
	InitializeComponent2();
	this->Graph = g;
}

void Automata::CanvasRenderer::start()
{
	render();
	this->timer->Start();
}

void Automata::CanvasRenderer::stop()
{
	this->timer->Stop();
}

void Automata::CanvasRenderer::clear()
{
	this->isRendered = false;
	this->Board->Children->Clear();
	Layout->Clear();
	Linkers->Clear();
}

void Automata::CanvasRenderer::initialize()
{
	int j = 0;
	for (int nodePair : g->Nodes) {
		bool isEnd = g->Boundaries->Lookup(nodePair) >= 2;
		bool isStart = (g->Boundaries->Lookup(nodePair) == 1 || g->Boundaries->Lookup(nodePair) == 3);
		auto Node = ref new node(nodePair.ToString(), isStart, isEnd, Point(this->ActualWidth / 2 + (float)((++j * 50) % (int)this->ActualWidth), this->ActualHeight / 2 + (j * 50 / this->ActualWidth)));
		this->Layout->Insert(nodePair.ToString(), Node);
		Node->moved += ref new Automata::positionChanged(this, &Automata::CanvasRenderer::OnNodemoved);
	}
	for (auto start : g->Edges) {
		for (auto edge : start->Value) {
			for (int dest : edge->Value) {
				Automata::edge^ edg = ref new Automata::edge(Layout->Lookup(start->Key.ToString()), Layout->Lookup(dest.ToString()), edge->Key);
				Linkers->Append(edg);
				edg->moved += ref new Automata::translatedEvent(this, &Automata::CanvasRenderer::OnEdgemoved);
			}
		}
	}
}

void Automata::CanvasRenderer::process()
{
	float L = _l;
	auto calculateForces = [L](node^& Node1, node^& Node2, float forceCoeff, bool isRepuslive) {
		Point dv = Point(Node2->Position.X - Node1->Position.X, Node2->Position.Y - Node1->Position.Y);
		if (dv.X != 0 || dv.Y != 0) {
			double distance = sqrt(dv.X * dv.X + dv.Y * dv.Y);
			float f = (isRepuslive ? (forceCoeff / (pow(L * distance, 2))) : (forceCoeff * (distance - L) / L));
			Point F = Point((float)f * dv.X / distance, (float)f * dv.Y / distance);
			Node1->setPoint(Node1->Force.X + (isRepuslive ? (-1) : ( 1)) * F.X, Node1->Force.Y + (isRepuslive ? (-1) : ( 1)) * F.Y, true);
			Node2->setPoint(Node2->Force.X + (isRepuslive ? ( 1) : (-1)) * F.X, Node2->Force.Y + (isRepuslive ? ( 1) : (-1)) * F.Y, true);
		}
	};

	for (auto&& NodeLayout : Layout) NodeLayout->Value->setPoint(0, 0, true);

	for (auto&& GraphNode : g->UniquePairs) {
		auto Node1 = Layout->Lookup(GraphNode->Key.ToString());
		for (int neighbor : GraphNode->Value) {
			auto Node2 = Layout->Lookup(neighbor.ToString());
			calculateForces(Node1, Node2, _kr, true);
		}
	}

	for (auto&& GraphNode : g->Pairs) {
		auto Node1 = Layout->Lookup(GraphNode->Key.ToString());
		for (int neighbor : GraphNode->Value) {
			auto Node2 = Layout->Lookup(neighbor.ToString());
			calculateForces(Node1, Node2, _ks, false);
		}
	}

	for (auto&& NodeLayout : Layout) {
		auto Node = NodeLayout->Value;
		Point dv = Point(Node->Force.X * _dt, Node->Force.Y * _dt);
		float dp = dv.X * dv.X + dv.Y * dv.Y;
		if (dp > _maxdp) dv = Point(dv.X * sqrt(_maxdp / dp), dv.Y * sqrt(_maxdp / dp));
		Node->Position = Point(Node->Position.X + dv.X, Node->Position.Y + dv.Y);
	}
}

void Automata::CanvasRenderer::update()
{
	for (auto nodePair : this->Layout) {
		node^ Node = nodePair->Value;
		this->Board->SetLeft(Node, Node->Position.X);
		this->Board->SetTop(Node, Node->Position.Y);
	}
	for (edge^ Edge : this->Linkers) {
		this->Board->SetLeft(Edge, Edge->Start.X + Edge->Size / 2);
		this->Board->SetTop(Edge, Edge->Start.Y + +Edge->Size / 2);
		this->Board->SetZIndex(Edge, -1);
	}
}

void Automata::CanvasRenderer::render()
{
	if (!this->isRendered) {
		for (auto nodePair : this->Layout) {
			node^ Node = nodePair->Value;
			this->Board->Children->Append(Node);
		}
		for (edge^ Edge : this->Linkers) {
			this->Board->Children->Append(Edge);
		}
		this->isRendered = true;
	}
}

void Automata::CanvasRenderer::Board_ManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e)
{
	Canvas^ castedSender = dynamic_cast<Canvas^>(sender);
	for (Object^ element : castedSender->Children) {
		auto castedElement = dynamic_cast<node^>(element);
		if (castedElement != nullptr) {
			castedElement->Position = Point(castedElement->Position.X + e->Delta.Translation.X, castedElement->Position.Y + e->Delta.Translation.Y);
		}
	}
}

void Automata::CanvasRenderer::OnNodemoved(Automata::node^ sender, Windows::Foundation::Point newPos)
{
	if (this->isRendered) {
		this->Board->SetLeft(sender, sender->Position.X);
		this->Board->SetTop(sender, sender->Position.Y);
	}
}

void Automata::CanvasRenderer::OnEdgemoved(Automata::edge^ sender, Windows::Foundation::Point newPos)
{
	if (this->isRendered) {
		this->Board->SetLeft(sender, sender->Start.X + sender->Size / 2);
		this->Board->SetTop(sender, sender->Start.Y + +sender->Size / 2);
		this->Board->SetZIndex(sender, -1);
	}
}

void Automata::CanvasRenderer::Rslider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	_r = dynamic_cast<Slider^>(sender)->Value;// this->Rslider->Value;
}

void Automata::CanvasRenderer::OnTick(Platform::Object^ sender, Platform::Object^ args)
{
	process();
}


void Automata::CanvasRenderer::Board_PointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	bool isZoom = e->GetCurrentPoint(this)->Properties->MouseWheelDelta > 0;
	float scaleRatio = 0.023;
	float w = this->Width, h = this->Height;
	float r = max(0.01,ZoomController->ScaleX + (isZoom ? (-1.0) : 1.0) * scaleRatio);
	ZoomController->ScaleX = r; ZoomController->ScaleY = r;
	this->Board->Width = w / min(r,1);
	this->Board->Height = h / min(r, 1);
	e->Handled = true;
}
