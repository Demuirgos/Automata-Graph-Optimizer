#pragma once
#include<pch.h>
#include"Graphs.h"
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;


namespace Automata {
	public delegate void Modified(Object^ sender);
	public ref class GraphManaged sealed
	{
	private:
		void MakeCopy(GraphManaged^ source);
		IMap<int, int>^ boundaries;
		IVector<int>^ nodes;
		IMap<int, IMap<String^, IVector<int>^>^>^ edges;
		IMap<int, IVector<int>^>^ getUniquePairs();
		IMap<int, IVector<int>^>^ getPairs();
		IMap<int, IVector<int>^>^ uniquepairs;
		IMap<int, IVector<int>^>^ pairs;
		graph g;
		void OnModifiedEvent(Object^ sender);
		void UpdateUnderLayingNativeRepr();
		GraphManaged^ ConvertToManaged();
	public:
		event Modified^ ModifiedEvent;
		GraphManaged(String^ data);
		GraphManaged(GraphManaged^ source);
		GraphManaged(IMap<int, IMap<String^, IVector<int>^>^>^ e, IMap<int, int>^ b, IVector<int>^ n);
		GraphManaged();
		void insert(int s, int f, String^ w);
		void Optimise(bool Phase1,bool Phase2, bool Phase3);
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
		property String^ Data {
			String^ get() { return this->g.ToString(); }
		}
		bool isStartNode(int i) { return this->boundaries->Lookup(i) == 1 || this->boundaries->Lookup(i) == 3; }
		bool isEndNode(int i) { return this->boundaries->Lookup(i) >= 2; }
		bool isValid(){
			bool startIn = false, endIn = false;
			for (auto node : this->boundaries) {
				if (node->Value == 1 || node->Value == 3) startIn = true;
				if (node->Value >= 2) endIn = true;
				if (startIn && endIn) return true;
			}
			return false;
		}
		void EndNodeSetState(int i, bool isEnd) {
			if (!this->Boundaries->HasKey(i)) this->Boundaries->Insert(i, 0);
			int rank = this->Boundaries->Lookup(i);
			if (isEnd) rank += 2;
			else rank -= 2;
			this->Boundaries->Insert(i, rank);
			if (!isEnd) this->g.end.erase(i);
			else this->g.end[i] = true;
		}
		void StartNodeSetState(int i, bool isStart) {
			if (!this->Boundaries->HasKey(i)) this->Boundaries->Insert(i, 0);
			int rank = this->Boundaries->Lookup(i);
			if (isStart) rank += 1;
			else rank -= 1;
			this->Boundaries->Insert(i, rank);
			if (!isStart) this->g.start.erase(i);
			else this->g.start.insert(i);
		}
	};
}

