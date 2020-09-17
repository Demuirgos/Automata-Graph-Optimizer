#include "pch.h"
#include "GraphManaged.h"

using namespace Automata;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

void Automata::GraphManaged::MakeCopy(GraphManaged^ source)
{
	this->boundaries = source->boundaries;
	this->edges = source->edges;
	this->nodes = source->nodes;
	this->pairs = source->pairs;
	this->uniquepairs = source->uniquepairs;
}

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

void Automata::GraphManaged::OnModifiedEvent(Object^ sender)
{
	pairs =this->getPairs();
	uniquepairs = this->getUniquePairs();
	UpdateUnderLayingNativeRepr();
}

Automata::GraphManaged::GraphManaged(String^ data)
{
	this->g = graph(Methods::ToCppString(data));
	this->MakeCopy(this->ConvertToManaged());
	this->ModifiedEvent += ref new Automata::Modified(this, &Automata::GraphManaged::OnModifiedEvent);
}

Automata::GraphManaged::GraphManaged(GraphManaged^ source)
{
	this->boundaries = source->boundaries;
	this->edges = source->edges;
	this->nodes = source->nodes;
	this->g = source->g;
	this->pairs = source->pairs;
	this->uniquepairs = source->uniquepairs;
	this->ModifiedEvent += ref new Automata::Modified(this, &Automata::GraphManaged::OnModifiedEvent);
}

GraphManaged::GraphManaged(IMap<int, IMap<String^, IVector<int>^>^>^ e, IMap<int, int>^ b, IVector<int>^ n) {
	boundaries = b; nodes = n; edges = e; uniquepairs = getUniquePairs(); pairs = getPairs();
	this->ModifiedEvent += ref new Automata::Modified(this, &Automata::GraphManaged::OnModifiedEvent);
}

void Automata::GraphManaged::insert(int s, int f, String^ w)
{
	if (!edges->HasKey(s)) edges->Insert(s, ref new Map<String^, IVector<int>^>());
	auto startNode = edges->Lookup(s);
	if (!startNode->HasKey(w)) startNode->Insert(w, ref new Vector<int>());
	auto destinations = startNode->Lookup(w);
	uint32 index;
	if (!destinations->IndexOf(f, &index)) {
		destinations->Append(f);
	}
	ModifiedEvent(this);
}

void Automata::GraphManaged::UpdateUnderLayingNativeRepr()
{
	graph r;
	for (auto start : this->Edges) {
		for (auto edge : start->Value) {
			for (int dest : edge->Value) {
				r.insert(start->Key,dest,Methods::ToCppString(edge->Key));
			}
		}
	}
	/*
	for (auto nodes : this->boundaries) {
		bool isStart = nodes->Value == 1 || nodes->Value == 3;
		bool isEnd = nodes->Value >= 2;
		if (isStart) r.start.insert(nodes->Key);
		if (isEnd) r.end[nodes->Key] = true;
	}
	*/
}

GraphManaged^ Automata::GraphManaged::ConvertToManaged()
{
	Map<int, IMap<String^, IVector<int>^>^>^ result = ref new Map<int, IMap<String^, IVector<int>^>^>();
	for (auto Node : this->g.node) {
		if (!result->HasKey(Node.first)) {
			result->Insert(Node.first, ref new Map<String^, IVector<int>^>());
		}
		for (auto weight : Node.second) {
			auto Weight = Methods::FromCppString(weight.first);
			if (!result->Lookup(Node.first)->HasKey(Weight)) {
				result->Lookup(Node.first)->Insert(Weight, ref new Vector<int>());
			}
			for (auto dest : weight.second) {
				uint32 index;
				if (!result->Lookup(Node.first)->Lookup(Weight)->IndexOf(dest, &index)) {
					result->Lookup(Node.first)->Lookup(Weight)->Append(dest);
				}
			}
		}
	}
	Vector<int>^ nodes = ref new Vector<int>();
	for (auto Node : this->g.nodes) {
		nodes->Append(Node);
	}
	Map<int, int>^ boundaries = ref new Map<int, int>();
	for (auto Node : this->g.nodes) {
		boundaries->Insert(Node, (this->g.start.find(Node) != this->g.start.end()) + (this->g.end[Node] << 1));
	}
	return ref new Automata::GraphManaged(result, boundaries, nodes);
}

void Automata::GraphManaged::Optimise(bool Phase1, bool Phase2, bool Phase3)
{
	this->g.optimize(Phase1, Phase2, Phase3);
	this->g.Clean();
	this->g.Compact();
	this->MakeCopy(this->ConvertToManaged());
}

GraphManaged::GraphManaged() {
	boundaries = ref new Map<int, int>(); nodes = ref new Vector<int>(); edges = ref new Map<int, IMap<String^, IVector<int>^>^>();
	this->ModifiedEvent += ref new Automata::Modified(this, &Automata::GraphManaged::OnModifiedEvent);
}