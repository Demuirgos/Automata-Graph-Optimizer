#include "pch.h"
#include "GraphManaged.h"

using namespace Automata;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

IMap<int, IVector<int>^>^ Automata::GraphManaged::getUniquePairs()
{
	Map<int,IVector<int>^>^ result = ref new Map<int, IVector<int>^>();
	for (auto start : this->Edges) {
		if (!result->HasKey(start->Key)) result->Insert(start->Key, ref new Vector<int>());
		IVector<int>^ Neighbors = result->Lookup(start->Key);
		for (auto edge : start->Value) {
			for (int dest : edge->Value) {
				uint32 index;
				if (!result->HasKey(dest) || !result->Lookup(dest)->IndexOf(start->Key, &index)) {
					if (!Neighbors->IndexOf(dest, &index))
						Neighbors->Append(dest);
				}
			}
		}
	}
	return result;
}

IMap<int, IVector<int>^>^ Automata::GraphManaged::getPairs()
{
	Map<int, IVector<int>^>^ result = ref new Map<int, IVector<int>^>();
	for (auto start : this->Edges) {
		if (!result->HasKey(start->Key)) result->Insert(start->Key, ref new Vector<int>());
		IVector<int>^ Neighbors = result->Lookup(start->Key);
		for (auto edge : start->Value) {
			for (int dest : edge->Value) {
				if (!result->HasKey(dest)) result->Insert(dest, ref new Vector<int>());
				IVector<int>^ NeighborsReverese = result->Lookup(dest);
				uint32 index;
				if (!Neighbors->IndexOf(dest, &index))
					Neighbors->Append(dest);
				if (!NeighborsReverese->IndexOf(start->Key, &index))
					NeighborsReverese->Append(start->Key);
			}
		}
	}
	return result;
}

GraphManaged::GraphManaged(IMap<int, IMap<String^, IVector<int>^>^>^ e, IMap<int, int>^ b, IVector<int>^ n) {
	boundaries = b; nodes = n; edges = e; uniquepairs = getUniquePairs(); pairs = getPairs();
}