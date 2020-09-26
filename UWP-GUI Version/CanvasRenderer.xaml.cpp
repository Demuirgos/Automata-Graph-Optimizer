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
	SolidColorBrush^ b = ref new SolidColorBrush(Windows::UI::Colors::Black);
	drawingEdge->Stroke = b;
	drawingEdge->StrokeThickness = 5;
	this->NodeStatusUpdated += ref new Automata::BoundarieEventHandler(this, &Automata::CanvasRenderer::OnNodeStatusUpdated);
}

Automata::CanvasRenderer::CanvasRenderer(GraphManaged^ g)
{
	InitializeComponent();
	InitializeComponent2();
	this->Graph = g;
}

void Automata::CanvasRenderer::start()
{
	initialize();
	render();
	this->timer->Start();
}

void Automata::CanvasRenderer::start(int duration)
{
	this->isTimed = true;
	this->duration = duration;
	this->count = 0;
	initialize();
	render();
	this->timer->Start();
}

void Automata::CanvasRenderer::stop()
{
	this->isTimed = false;
	this->timer->Stop();
}

void Automata::CanvasRenderer::clear()
{
	this->isRendered = false;
	Linkers->Clear();
	Layout->Clear();
	this->Board->Children->Clear();
}

void Automata::CanvasRenderer::EnableEditingMode(bool b)
{
	this->editMode = b;
	for (auto Node : this->Layout) {
		Node->Value->Editing = b;
	}
}

void Automata::CanvasRenderer::initialize()
{
	this->clear();
	int j = 0;
	auto getColor = [=](int size, int i) {
		int step = 256 / size;
		int index = i + 1;
		SolidColorBrush^ brush = ref new SolidColorBrush();
		Windows::UI::Color* color = new Windows::UI::Color();
		color->R = (i % 16 * 16)/4;
		color->G = (i % 8 * 32)/4;
		color->B = (i % 4 * 64)/4;
		color->A = 255; brush->Color = *color;
		return brush;
	};
	for (int nodePair : g->Nodes) {
		bool isEnd = g->Boundaries->Lookup(nodePair) >= 2;
		bool isStart = (g->Boundaries->Lookup(nodePair) == 1 || g->Boundaries->Lookup(nodePair) == 3);
		auto Node = ref new node(nodePair.ToString(), isStart, isEnd, Point(this->Board->ActualWidth / 2 - 100 + (float)((++j * 50) % (int)this->Board->ActualWidth), this->Board->ActualHeight / 4 + (j * 50 / this->Board->ActualWidth)));
		Node->ManipulationMode = ::ManipulationModes::All;
		Node->DragTokenOnGoingEventToken = Node->ManipulationDelta += ref new Windows::UI::Xaml::Input::ManipulationDeltaEventHandler(this, &Automata::CanvasRenderer::OnManipulationDelta);
		Node->DragTokenCompleteEventToken = Node->ManipulationCompleted += ref new Windows::UI::Xaml::Input::ManipulationCompletedEventHandler(this, &Automata::CanvasRenderer::OnManipulationCompleted);
		Node->LeftTapEventToken = Node->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Automata::CanvasRenderer::OnTapped);
		Node->released += ref new Automata::MouseHookPressed(this, &Automata::CanvasRenderer::Onreleased);
		Node->RightTapTokenToken = Node->RightTapped += ref new Windows::UI::Xaml::Input::RightTappedEventHandler(this, &Automata::CanvasRenderer::OnRightTapped);
		Node->hooked += ref new Automata::MouseHookPressed(this, &Automata::CanvasRenderer::Onhooked);
		Node->locked += ref new Automata::MouseHookPressed(this, &Automata::CanvasRenderer::Onlocked);
		Node->deleted += ref new Automata::DeleteRequested(this, &Automata::CanvasRenderer::Ondeleted);
		this->Layout->Insert(nodePair.ToString(), Node);
		Node->Editing = this->editMode;
		Node->moved += ref new Automata::positionChanged(this, &Automata::CanvasRenderer::OnNodemoved);
	}
	for (auto start : g->Matrix) {
		if(g->Matrix->HasKey(start->Key))
			for (auto dest : start->Value) {
				Automata::edge^ edg = ref new Automata::edge(Layout->Lookup(start->Key.ToString()), Layout->Lookup(dest->Key.ToString()), dest->Value);
				Linkers->Append(edg);
				edg->moved += ref new Automata::translatedEvent(this, &Automata::CanvasRenderer::OnEdgemoved);
			}
	}
	int i = 0;
	for (edge^ edg : Linkers) {
		edg->Color = getColor(Linkers->Size, i++);
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
			Node1->setPoint(Node1->Force.X + (isRepuslive ? (-10000) : ( 1)) * F.X, Node1->Force.Y + (isRepuslive ? (-10000) : ( 1)) * F.Y, true);
			Node2->setPoint(Node2->Force.X + (isRepuslive ? ( 10000) : (-1)) * F.X, Node2->Force.Y + (isRepuslive ? ( 10000) : (-1)) * F.Y, true);
		}
	};

	for (auto&& NodeLayout : Layout) { 
		NodeLayout->Value->setPoint(0, 0, true);
	}

	for (int GraphNode : g->Nodes) {
		for (int neighbor : g->Nodes) {
			if (GraphNode != neighbor) {
				auto Node1 = Layout->Lookup(GraphNode.ToString());
				auto Node2 = Layout->Lookup(neighbor.ToString());
				calculateForces(Node1, Node2, _kr, true);
			}
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
		this->Board->SetTop(Edge, Edge->Start.Y + Edge->Size / 2);
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
	if(!suspended && !dragging && !drawing)
		for (Object^ element : castedSender->Children) {
			auto castedElement = dynamic_cast<node^>(element);
			if (castedElement != nullptr) {
				castedElement->Position = Point(castedElement->Position.X + e->Delta.Translation.X/this->ScaleFactor, castedElement->Position.Y + e->Delta.Translation.Y/this->ScaleFactor);
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
		this->Board->SetTop(sender, sender->Start.Y + sender->Size / 2);
		this->Board->SetZIndex(sender, -1);
	}
}

void Automata::CanvasRenderer::Rslider_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	_r = dynamic_cast<Slider^>(sender)->Value;// this->Rslider->Value;
	_ks = _kr / (_r * _l * _l *_l);
}

void Automata::CanvasRenderer::OnTick(Platform::Object^ sender, Platform::Object^ args)
{
	if (isRendered && !drawing) {
		process();
		if (this->isTimed) this->count++;
		if (this->isTimed && this->duration == this->count) this->stop();
	}
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

void Automata::CanvasRenderer::OnManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e)
{
	if(editMode)
		if (!drawing) {
			dragging = true;
			auto senderCasted = dynamic_cast<node^>(sender);
			senderCasted->Position = Point(senderCasted->Position.X + e->Delta.Translation.X * ZoomController->ScaleX, senderCasted->Position.Y + e->Delta.Translation.Y * ZoomController->ScaleY);
		} else {
			Windows::UI::Input::PointerPoint^ ptrPt = this->ptrID->GetCurrentPoint(this->Board);
			if (ptrPt->Properties->IsLeftButtonPressed)
			{
				Point ptrEPosition = this->ptrID->GetCurrentPoint(this->Board)->Position;
				Point ptrSPosition = this->Layout->Lookup(this->StartNode)->Position;
				float size = this->Layout->Lookup(this->StartNode)->Size;
				drawingEdge->X2 = (ptrEPosition.X - ptrSPosition.X) - size/2; drawingEdge->Y2 = (ptrEPosition.Y - ptrSPosition.Y) - size / 2;
			}
			else {
				this->drawing = false;
				uint32 index;
				if (this->Board->Children->IndexOf(this->drawingEdge, &index)) {
					this->Board->Children->RemoveAt(index);
				}
			}
		}
}

void Automata::CanvasRenderer::OnManipulationCompleted(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs^ e)
{
	if(editMode)
		dragging = false;
}

void Automata::CanvasRenderer::OnTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	if (editMode) {
		auto Node = dynamic_cast<node^>(sender);
		int NodeID = Methods::StringToInt(Node->Label);
		bool before = this->g->isStartNode(NodeID);
		Node->IsStartNode = !before;
		this->NodeStatusUpdated(NodeID, Node->IsEndNode, Node->IsStartNode);
	}
}

void Automata::CanvasRenderer::OnRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e)
{
	if (editMode) {
		auto Node = dynamic_cast<node^>(sender);
		int NodeID = Methods::StringToInt(Node->Label);
		bool before = this->g->isEndNode(NodeID);
		Node->IsEndNode = !before;
		this->NodeStatusUpdated(NodeID, Node->IsEndNode, Node->IsStartNode);
	}
}

void Automata::CanvasRenderer::Onhooked(Automata::node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (editMode) {
		uint32 index;
		if (this->Board->Children->IndexOf(this->drawingEdge, &index)) {
			this->Board->Children->RemoveAt(index);
		}
		if (!this->drawing) {
			this->StartNode = sender->Label;
			this->drawing = true;
			Point ptrPosition = sender->Position;
			this->Board->Children->Append(this->drawingEdge);
			this->Board->SetLeft(this->drawingEdge, ptrPosition.X + sender->Size / 2);
			this->Board->SetTop(this->drawingEdge, ptrPosition.Y + sender->Size / 2);
			this->Board->SetZIndex(this->drawingEdge, -1);
		}
	}
}

void Automata::CanvasRenderer::Onreleased(Automata::node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (editMode) {
		this->drawing = false;
		uint32 index;
		if (this->Board->Children->IndexOf(this->drawingEdge, &index)) {
			this->Board->Children->RemoveAt(index);
		}
	}
}

void Automata::CanvasRenderer::Board_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if(editMode)
		this->ptrID = e;
}

void Automata::CanvasRenderer::Onlocked(Automata::node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (drawing && editMode) {
		this->EndNode = sender->Label;
		this->stop();
		Point ptrEPosition = this->Layout->Lookup(this->EndNode)->Position;
		Point ptrSPosition = this->Layout->Lookup(this->StartNode)->Position;
		float size = this->Layout->Lookup(this->StartNode)->Size;
		drawingEdge->X2 = (ptrEPosition.X - ptrSPosition.X) * this->ZoomController->ScaleX; drawingEdge->Y2 = (ptrEPosition.Y - ptrSPosition.Y) * this->ZoomController->ScaleY;
		ContentDialog^ weightRequest = ref new ContentDialog();
		TextBox^ LabelEdge = ref new TextBox();
		LabelEdge->Width = 175;
		LabelEdge->TextAlignment = TextAlignment::Center;
		LabelEdge->Header = "Weight";
		LabelEdge->PlaceholderText = "Inser Edge Weight here";
		weightRequest->Content = LabelEdge;
		weightRequest->PrimaryButtonText = "OK";
		weightRequest->SecondaryButtonText = "Cancel";
		this->editMode = false;
		this->drawing = false;
		this->suspended = true;
		CanvasRenderer^ main = this;
		concurrency::create_task(weightRequest->ShowAsync()).then([LabelEdge, main](ContentDialogResult result) {
			if (result == ContentDialogResult::Primary) {
				main->EdgeInserted(Methods::StringToInt(main->StartNode), Methods::StringToInt(main->EndNode), LabelEdge->Text == "" ? "0" : LabelEdge->Text);
			}
			uint32 index;
			if (main->Board->Children->IndexOf(main->drawingEdge, &index))
				main->Board->Children->RemoveAt(index);
			main->editMode = true;
			main->suspended = false;
			});
	}
}

void Automata::CanvasRenderer::OnNodeStatusUpdated(int s, bool isEnd, bool isStart)
{
	this->Layout->Lookup(s.ToString())->update();
}

void Automata::CanvasRenderer::Ondeleted(Automata::node^ sender)
{
	if(this->editMode)
		this->g->removeNode(Methods::StringToInt(sender->Label));
}

void Automata::CanvasRenderer::UserControl_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	this->Board->Measure(e->NewSize);
}
