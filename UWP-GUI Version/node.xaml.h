//
// node.xaml.h
// Declaration of the node class
//

#pragma once
#include "node.g.h"

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
namespace Automata
{
	public delegate void positionChanged(node^ sender, Point newPos);
	public delegate void MouseHookPressed(node^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
	public delegate void DeleteRequested(node^ sender);
	enum NodeType {
		StartingNode,
		EndingNode,
		OrdinaryNode,
		StartingAndEndingNode
	};
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class node sealed
	{
	public:
		event positionChanged^ moved;
		event MouseHookPressed^ hooked;
		event MouseHookPressed^ released;
		event MouseHookPressed^ locked;
		event DeleteRequested^ deleted;
		node(String^ label, bool StartFlag, bool EndFlag, Point start);
		node(String^ label, bool StartFlag, bool EndFlag, float size);
		void Initialize();
		property Point Position {
			Point get() { return this->Location; }
			void set(Point v) { 
				this->Location = v; 
				moved(this, this->Location);
			}
		}
		void setPoint(float x, float y, bool isForce) {
			if (!isForce) {
				this->Location.X = x;
				this->Location.Y = y;
				moved(this, this->Location);
			}
			else {
				this->force.X = x;
				this->force.Y = y;
			}
		}
		property String^ Label {
			String^ get() { return this->_Label; }
			void set(String^ v) { this->_Label = v; }
		}
		property float Size {
			float get() { return 60; }
		}
		property bool Editing {
			void set(bool v) { this->editing = v; }
		}
		property Point Force {
			Point get() { return this->force; }
			void set(Point v) { this->force = v; }
		}
		property Boolean IsEndNode {
			Boolean get() { return this->type == NodeType::EndingNode || this->type == NodeType::StartingAndEndingNode; }
			void set(Boolean v) { 
				if (v && type == NodeType::StartingNode) {
					this->type = NodeType::StartingAndEndingNode;
				}
				else if (v && type != NodeType::StartingNode) {
					this->type = NodeType::EndingNode;
				} else if (!v && type == NodeType::StartingNode) {
					this->type = NodeType::StartingNode;
				}
				else if (!v && type != NodeType::StartingNode) {
					this->type = NodeType::OrdinaryNode;
				};
			}
		}
		property Boolean IsStartNode {
			Boolean get() { return this->type == NodeType::StartingNode || this->type == NodeType::StartingAndEndingNode; }
			void set(Boolean v) {
				if (v && type == NodeType::EndingNode) {
					this->type = NodeType::StartingAndEndingNode;
				}
				else if (v && type != NodeType::EndingNode) {
					this->type = NodeType::StartingNode;
				}
				else if (!v && type == NodeType::EndingNode) {
					this->type = NodeType::EndingNode;
				}
				else if (!v && type != NodeType::EndingNode) {
					this->type = NodeType::OrdinaryNode;
				};;
			}
		}
		property EventRegistrationToken DragTokenOnGoingEventToken {
			EventRegistrationToken get() { return this->DragTokenOnGoing; }
			void set(EventRegistrationToken v) {this->DragTokenOnGoing = v; }
		}
		property EventRegistrationToken DragTokenCompleteEventToken {
			EventRegistrationToken get() { return this->DragTokenComplete; }
			void set(EventRegistrationToken v) { this->DragTokenComplete = v; }
		}
		property EventRegistrationToken LeftTapEventToken{
			EventRegistrationToken get() { return this->LeftTapToken; }
			void set(EventRegistrationToken v) { this->LeftTapToken = v; }
		}
		property EventRegistrationToken RightTapTokenToken {
			EventRegistrationToken get() { return this->RightTapToken; }
			void set(EventRegistrationToken v) { this->RightTapToken = v; }
		}
		property EventRegistrationToken MouseEnterEventToken {
			EventRegistrationToken get() { return this->MouseEnterToken; }
			void set(EventRegistrationToken v) { this->MouseEnterToken = v; }
		}
		property EventRegistrationToken PointerPressedEventToken {
			EventRegistrationToken get() { return this->PointerPressedToken; }
			void set(EventRegistrationToken v) { this->PointerPressedToken = v; }
		}
		property EventRegistrationToken MouseReleasedEventToken {
			EventRegistrationToken get() { return this->MouseReleasedToken; }
			void set(EventRegistrationToken v) { this->MouseReleasedToken = v; }
		}
		void UnsibscribeDragEvents();
		void UnsibscribeDrawEvents();
		void update();
	private:
		//event tokes
		EventRegistrationToken DragTokenOnGoing;
		EventRegistrationToken DragTokenComplete;
		EventRegistrationToken LeftTapToken;
		EventRegistrationToken RightTapToken;
		EventRegistrationToken MouseEnterToken;
		EventRegistrationToken PointerPressedToken;
		EventRegistrationToken MouseReleasedToken;
		bool editing = false;
		NodeType type = NodeType::OrdinaryNode;
		Point Location;
		Point force;
		String^ _Label = "";
		float size = 25;
		void Hook_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Hook_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Hook_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Hook_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Delete_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Delete_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Delete_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
};
}
