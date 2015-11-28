#include <iostream>
#include <utility>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

using namespace std;

typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS> MyGraph;
typedef boost::graph_traits<MyGraph>::vertex_descriptor MyVertex;

//Used for start and end position
struct Position{
	int x_pos;
	int y_pos;
	int level;
};

//Each square in the maze, and it's properties.
struct Square{
	//1 = yellow, 0 = white
	char yellow_sq;
	//1 = wall, 0 = no wall
	char N;
	char S;
	char E;
	char W;
};

//Edge information, beg_vertex -> end_vertex
struct Edge{
	int beg_vertex;
	int end_vertex;
};

int main()
{
	ifstream inFile;
	inFile.open("input.txt", inFile.in);

	ofstream outFile;
	outFile.open("Bell.txt", outFile.out);

	int instances = 0, loop = 0;
	int levels = 0, width = 0, height = 0;
	int beg_level = 0, beg_x = 0, beg_y = 0;
	int end_level = 0, end_x = 0, end_y = 0;
	//char in_dir, out_dir;

	inFile >> instances;

	while (loop < instances){
		inFile >> levels >> width >> height;

		inFile >> beg_level >> beg_x >> beg_y;
		
		//Start position, x y z co-ords
		Position start;
		start.x_pos = beg_x;
		start.y_pos = beg_y;
		start.level = beg_level;

		inFile >> end_level >> end_x >> end_y;

		//Finish position, x y z co-ords
		Position finish;
		finish.x_pos = end_x;
		finish.y_pos = end_y;
		finish.level = end_level;

		int i = 0, j = 0;
		//Total height of the array, making 3D into 2D
		//Allowing double fors instead of triple
		//Increasing efficiency
		int total_height = height * levels;
		
		
		int exit = (finish.x_pos * finish.y_pos * finish.level);
		int entry = (start.x_pos * start.y_pos * start.level);

		//2D array, used to read into our graph
		Square** n = new Square*[total_height+1];
		for (i = 0; i < total_height+1; i++){
			n[i] = new Square[width+1];
		}

		// Read data into 2D Array of squares.
		// This will be used to look at the wall/ floor config. for each index.
		for (i = 1; i <= total_height; i++){
			for (j = 1; j <= width; j++){
				string(ch);
				inFile >> ch;
				n[i][j].yellow_sq = ch[0];
				n[i][j].N = ch[1];
				n[i][j].S = ch[2];
				n[i][j].E = ch[3];
				n[i][j].W = ch[4];
			}
		}

		//undirected graph to be passed to boost library
		MyGraph g(total_height*width+2);

		//These make it so the proper output is printed from the graph
		//example, 80 -> 81 or 20 -> 81
		boost::add_edge(entry, (total_height*width) + 1, g);

		//example, 1 -> 0, 21 -> 0
		boost::add_edge(finish.x_pos + ((finish.y_pos-1)*total_height), 0, g);

		//Iterates through Edge array
		for (i = 1; i <= total_height; i++){
			for (j = 1; j <= width; j++){
				//floor = 1, no floor = 0
				if (n[i][j].yellow_sq == '0'){
					boost::add_edge((i + ((j - 1)*total_height)), (i + ((j - 1)*total_height)) + height, g);
				}
				//wall = 1, no wall = 0
				if (n[i][j].N == '0'){
					if (i != 1){
						boost::add_edge(i + ((j - 1)*total_height), (i + ((j - 1)*total_height) - 1), g);
					}
				}
				//wall = 1, no wall = 0
				if (n[i][j].S == '0'){
					if (i != total_height){
						boost::add_edge(i + ((j - 1)*total_height), (i + ((j - 1)*total_height)) + 1, g);
					}
				}
				//wall = 1, no wall = 0
				if (n[i][j].W == '0'){
					if ((i + (j - 1)*total_height) >= total_height && j != 1){
						boost::add_edge(i + ((j - 1)*total_height), (i + ((j - 1)*total_height)) - total_height, g);
					}
				}

				//wall = 1, no wall = 0
				if (n[i][j].E == '0'){
					if (i < i + ((width - 1)*total_height) && j != width){
						boost::add_edge(i + ((j - 1)*total_height), (i + ((j - 1)*total_height)) + total_height, g);
					}
				}
			}
		}

		//Used for depth_first_search
		std::vector<MyVertex> predecessor;
		//Used to process output path, both numerical and directional
		std::vector<int> path;

		predecessor.resize(total_height*width+2);
		
		//Boost depth first search algorithm
		boost::depth_first_search(g, boost::visitor(boost::make_dfs_visitor(boost::record_predecessors(&predecessor[0], boost::on_tree_edge()))));

		int begin = (total_height*width) + 1;
		path.resize(predecessor.size());
		int next = 0;

		//Properly put the path into the path array
		while (begin != 0){
			path[next] = begin;
			begin = predecessor.at(begin);	
			next++;
		}

		//Ignore edge going into graph
		i = 1;

		//Put number co-ordinates into directional co-ordinates. 
		//Will stop once spider has reached location, for example
		//(1,1,1) will stop at (1,1,1)
	
		while (i != next){
			//Location reached, ignore edge going out
			if (path[i+1] == 0){
				break;
			}
			//West
			if (path[i] - total_height == path[i + 1]){
				outFile << "W ";
				i++;
			}
			//East
			else if (path[i] + total_height == path[i + 1]){
				outFile << "E ";
				i++;
			}
			//South
			else if (path[i] + 1 == path[i + 1]){
				outFile << "S ";
				i++;
			}
			//North
			else if (path[i] - 1 == path[i + 1]){
				outFile << "N ";
				i++;
			}
			//Down
			else if (path[i] + height == path[i + 1]){
				outFile << "D ";
				i++;
			}
			//Up
			else if (path[i] - height == path[i + 1]){
				outFile << "U ";
				i++;
			}
			else
				i++;
		}
		
		outFile << endl;

		//Free memory
		for (i = 0; i < total_height+1; i++){
			delete[] n[i];
		}
		delete[] n;

		loop++;
	}
	
	return 0;

}
