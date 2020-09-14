#pragma once
#include<pch.h>
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
namespace Automata {
	public ref class GraphManaged sealed
	{
	private:
		IMap<int, int>^ boundaries;
		IVector<int>^ nodes;
		IMap<int, IMap<String^, IVector<int>^>^>^ edges;
	public:
		GraphManaged(IMap<int, IMap<String^, IVector<int>^>^>^ e, IMap<int, int>^ b, IVector<int>^ n) {
			boundaries = b; nodes = n; edges = e;
		}
		property IMap<int, IMap<String^, IVector<int>^>^>^ Edges {
			IMap<int, IMap<String^, IVector<int>^>^>^ get() { return edges; }
		}
		property IMap<int, int>^ Boundaries {
			IMap<int, int>^ get() { return boundaries; }
		}
		property IVector<int>^ Nodes {
			IVector<int>^ get() { return nodes; }
		}
	};
}

