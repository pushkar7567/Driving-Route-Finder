#include <iostream>
#include <fstream>
#include "wdigraph.h"
#include "heap.h"
#include "dijkstra.h"
using namespace std;

struct Point {
	long long lat;
	long long lon;
};

void Digraph::addVertex(int v) {
  // If it already exists, does nothing.
  // Otherwise, adds v with an empty adjacency set.
  nbrs[v];
}

void Digraph::addEdge(int u, int v) {
  addVertex(v);
  nbrs[u].insert(v); // will also add u to nbrs if it was not there already
}

long long manhatten(const Point& pt1, const Point& pt2) {
	long long mandist = abs((pt1.lat)-(pt2.lat)) + abs((pt1.lon)-(pt2.lon));
	return mandist;
}

unordered_set<int>::const_iterator Digraph::neighbours(int v) const {
  // this will crash the program if v is not a vertex
  return nbrs.find(v)->second.begin();
}

unordered_set<int>::const_iterator Digraph::endIterator(int v) const {
  // this will crash the program if v is not a vertex
  return nbrs.find(v)->second.end();
}

void readGraph (string filename, WDigraph& graph, unordered_map<int, Point>& points) {
	ifstream fin;
	fin.open(filename);
	string line;
	string ID_str;
	string lat_str, lon_str;
	string startEdge, stopEdge;
	int ID, l, m;
	double lat_d, lon_d;
	long long lat, lon;

	while (fin) {
		getline(fin, line);
		size_t foundV = line.find("V,");
		size_t foundE = line.find("E,");
		
		if (foundV != string::npos){
			startEdge = line.substr(2, line.size()-3);
			int posStart = startEdge.find(",");
			startEdge = startEdge.substr(0, posStart);
			ID_str = startEdge;
			stopEdge = line.substr(posStart+3, line.size()-posStart-2);
			int posStop = stopEdge.find(",");
			lat_str = stopEdge.substr(0,posStop);
			lon_str = stopEdge.substr(stopEdge.find(",")+1, stopEdge.size()-lat_str.size());

			ID = stoi(ID_str); 
			lat_d = stod(lat_str);
			lon_d = stod(lon_str);
			lat = static_cast <long long> (lat_d*100000); 
			lon = static_cast <long long> (lon_d*100000);

			Point temp_point;
			temp_point.lat = lat;
			temp_point.lon = lon;

			points.insert({ ID, temp_point});	
			graph.addVertex(ID);

		}

		else if (foundE != string::npos){
			startEdge = line.substr(2, line.size()-3);
			int posStart = startEdge.find(",");
			startEdge = startEdge.substr(0, posStart);
			stopEdge = line.substr(posStart+3, line.size()-posStart-2);
			int posStop = stopEdge.find(",");
			stopEdge = stopEdge.substr(0, stopEdge.size());
			l = stoi(startEdge);
			m = stoi(stopEdge);
			Point point1, point2;
			point1 = points[l];
			point2 = points[m];
			long long cost_of_edge = manhatten(point1, point2);
			graph.addEdge(m, l, cost_of_edge);
		}
	}
	
}


void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree) {
	BinaryHeap<pair<int, int>, long long> events;
	pair<int, int> startPair;
	startPair.first = startVertex;
	startPair.second = startVertex;

	events.insert(startPair, 0);
	while (events.size() > 0) {
		HeapItem<pair<int, int>, long long> minItem = events.min();
		events.popMin();
		unordered_map<int, PIL>::iterator it = tree.find(minItem.item.second);
		if (it == tree.end()) { 
			tree[minItem.item.second] = make_pair(minItem.item.first, minItem.key);
			//tree[minItem.item.second] = minItem.item.first;
			for (auto i= graph.neighbours(minItem.item.second); i!=graph.endIterator(minItem.item.second); i++){
				pair<int, int> newPair = make_pair(minItem.item.second, *i);
				events.insert(newPair, minItem.key + graph.getCost(minItem.item.second, *i));
			}
		}
	}
}	


int main() {
	WDigraph edmontonGraph;
	unordered_map<int, Point> points; 
	readGraph("edmonton-roads-2.0.1.txt", edmontonGraph, points);

	unordered_map<int, PIL> searchTree;

	char mode = 'T';
	
	long long lat1, lat2, lon1, lon2;
	long long current_dist1, current_dist2; 


	cin >> mode;
	if (mode == 'R') {
		cin >> lat1 >> lon1 >> lat2 >> lon2;
		unordered_map<int, Point>::iterator it1 = points.begin();
		Point required_vertex1, required_vertex2;
			
		required_vertex1.lat = lat1;
		required_vertex1.lon = lon1;

		required_vertex2.lat = lat2;
		required_vertex2.lon = lon2;

		pair<int, Point> min_pair1;
		pair<int, Point> min_pair2;

		long long min_dist1 = manhatten(required_vertex1, it1->second);
		long long min_dist2 = manhatten(required_vertex2, it1->second);

		while ( it1!= points.end()) {
			current_dist1 = manhatten(required_vertex1, it1->second);
			current_dist2 = manhatten(required_vertex2, it1->second);
			//cout << current_dist1 << endl;

			if (current_dist1 < min_dist1){
				min_dist1 = current_dist1;
				min_pair1.first = it1->first;
				min_pair1.second.lat = it1->second.lat;
				min_pair1.second.lon = it1->second.lon;
			}

			if (current_dist2 < min_dist2){
				min_dist2 = current_dist2;
				min_pair2.first = it1->first;
				min_pair2.second.lat = it1->second.lat;
				min_pair2.second.lon = it1->second.lon;
			}
			it1++;
		}
		

		dijkstra(edmontonGraph, min_pair2.first, searchTree);

		int prevertex = min_pair1.first;
		int counter = 1;
		while (prevertex != min_pair2.first) {	
			
			prevertex = (searchTree[prevertex].first);
		  	counter++;
		}
		
		cout << "N: " << counter << endl;

		prevertex = min_pair1.first;
		cout << "W " << points[prevertex].lat << " " << points[prevertex].lon << endl;
		while (prevertex != min_pair2.first) {	
			
			prevertex = (searchTree[prevertex].first);
		  	cout << "W " << points[prevertex].lat << " " << points[prevertex].lon << endl;
		}
		cout << "E" << endl;
	}
	
	return 0;
}

