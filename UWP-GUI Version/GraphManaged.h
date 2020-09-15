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
		IMap<int, IVector<int>^>^ getUniquePairs();
		IMap<int, IVector<int>^>^ getPairs();
		IMap<int, IVector<int>^>^ uniquepairs;
		IMap<int, IVector<int>^>^ pairs;
	public:
		GraphManaged(IMap<int, IMap<String^, IVector<int>^>^>^ e, IMap<int, int>^ b, IVector<int>^ n);
		property IMap<int, IMap<String^, IVector<int>^>^>^ Edges {
			IMap<int, IMap<String^, IVector<int>^>^>^ get() { return edges; }
		}
		property IMap<int, IVector<int>^>^ UniquePairs {
			IMap<int, IVector<int>^>^ get() { return uniquepairs; }
		}
		property IMap<int, IVector<int>^>^ Pairs {
			IMap<int, IVector<int>^>^ get() { return pairs; }
		}
		property IMap<int, int>^ Boundaries {
			IMap<int, int>^ get() { return boundaries; }
		}
		property IVector<int>^ Nodes {
			IVector<int>^ get() { return nodes; }
		}
	};
}

