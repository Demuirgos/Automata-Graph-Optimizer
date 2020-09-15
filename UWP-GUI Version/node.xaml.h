//
// node.xaml.h
// Declaration of the node class
//

#pragma once
#include "node.g.h"

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
	public delegate void positionChanged(node^ sender, Point newPos);
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
			float get() { return this->size; }
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
				};
			}
		}
		property Boolean IsStartNode {
			Boolean get() { return this->type == NodeType::StartingNode || this->type == NodeType::StartingAndEndingNode; }
			void set(Boolean v) {
				if (v && type == NodeType::EndingNode) {
					this->type = NodeType::StartingAndEndingNode;
				}
				else if(v && type != NodeType::EndingNode) {
					this->type = NodeType::StartingNode;
				};
			}
		}
		void update();
	private:
		NodeType type = NodeType::OrdinaryNode;
		Point Location;
		Point force;
		String^ _Label = "";
		float size = 25;
		float AreaRange = .5f;
	};
}
