#include <iostream>
#include <vector>
#include <cfloat>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

struct Node
{
	int x = 0;
	int y = 0;
	bool valid = 0;
	bool open = 0;
	bool closed = 0;

	char c = '.'; // Used For Printing Edited Map

	Node* parent = nullptr;
	double G = DBL_MAX;
	double H = DBL_MAX;

	double calcF()
	{
		return G + H;
	}

	double distance(Node* other)
	{
		/// Chose Euclidean for real life scenario map
		return sqrt((x - other->x) * (x - other->x) + (y - other->y) * (y - other->y)); // Euclidean Distance
		// return max(abs(x - other->x), abs(y - other->y)); //  Chebyshev Distance
		// return abs(x - other->x) + abs(y - other->y); //  Manhattan Distance
	}
};

class NodeHeap
{

private:
	vector<Node*> heap; // Vector for O(1) Random Access Time

	void heapify(int node) // For Insert Function
	{
		int p = (node - 1) / 2;
		if (heap[node]->calcF() < heap[p]->calcF() ||
			(heap[node]->calcF() == heap[p]->calcF() && heap[node]->H < heap[p]->H))
		{

			swap(heap[node], heap[p]);
			heapify(p);
		}
	}

	void heapifyDown(int node) // For Pop Function
	{
		int left = 2 * node + 1;
		int right = 2 * node + 2;
		int smallest = node;

		if (left < (int)heap.size() && (heap[left]->calcF() < heap[smallest]->calcF() ||
			(heap[left]->calcF() == heap[smallest]->calcF() && heap[left]->H < heap[smallest]->H)))
		{
			smallest = left;
		}

		if (right < (int)heap.size() && (heap[right]->calcF() < heap[smallest]->calcF() ||
			(heap[right]->calcF() == heap[smallest]->calcF() && heap[right]->H < heap[smallest]->H)))
		{
			smallest = right;
		}

		if (smallest != node)
		{
			swap(heap[node], heap[smallest]);
			heapifyDown(smallest);
		}
	}


public:
	void update(Node* node)
	{
		for (int i = 0; i < (int)heap.size(); ++i)
			if (heap[i] == node)
			{
				heapify(i);
				break;
			}
	}

	void insertNode(Node* n)
	{
		n->open = 1;
		heap.push_back(n);
		heapify(heap.size() - 1);
	}

	bool empty()
	{
		return heap.empty();
	}

	Node* pop()
	{
		if (heap.empty())
			return nullptr;
		Node* p = heap.front();
		heap[0] = heap.back();
		heap.pop_back();

		if (!heap.empty())
			heapifyDown(0);

		return p;
	}
};

struct Map
{

	// Actual Grid Size
	int xLength = 0;
	int yLength = 0;

	vector<vector<Node>> grid;
	Node* start;
	Node* end;

	void readGrid()
	{
		// Start and End Coordinates
		int eX;
		int sX;
		int eY;
		int sY;

		string filename = "easy.txt";
		cout << "Enter File Name (Same dir as the program) : ";
		cin >> filename;
		ifstream infile(filename);

		while (!infile)
		{
			cout << "Unable to open file try again..." << endl;
			cout << "Enter File Name (Same dir as the program) : ";
			cin >> filename;
			infile.open(filename);
		}

		string line;
		int y = 0;

		while (getline(infile, line))
		{
			int xIter = 0;
			vector<Node> row;

			int lineL = line.length();
			if (lineL == 1)
				continue;

			for (int i = 0; i < lineL; i++)
			{
				if (line[i] == '[' || line[i] == ',' || line[i] == ']' || line[i] == ' ')
					continue; // Skip characters

				Node node;
				node.x = xIter++;
				node.y = y;
				node.valid = (line[i] == '1' || line[i] == 'E' || line[i] == 'S');

				row.push_back(node);

				if (line[i] == 'E')
				{
					eX = xIter - 1;
					eY = y;
				}
				else if (line[i] == 'S')
				{
					sX = xIter - 1;
					sY = y;
				}
			}
			xLength = max(xLength, xIter);
			if (!row.empty())
			{
				grid.push_back(row);
				y++;
			}
		}

		start = &grid[sY][sX];
		end = &grid[eY][eX];

		yLength = y; // Set Y actual Lendgth

		// Setting Start and Ending Nodes
		start->G = 0;
		start->H = start->distance(end);
		end->H = 0;
		start->c = 'S';
		end->c = 'E';

		infile.close();
	}

	bool inGrid(int x, int y)
	{
		if (x < 0 || y < 0)
			return 0;
		if (x >= xLength || y >= yLength)
			return 0;

		return 1;
	}

	vector<Node*> findPath()
	{
		NodeHeap open; // Heap For Open Nodes
		open.insertNode(start);

		vector<Node*> path; // Solution Path

		while (!open.empty())
		{
			Node* current = open.pop();
			current->closed = 1;

			if (current == end)
			{
				path.push_back(end);
				current = end->parent;
				while (current != start)
				{
					grid[current->y][current->x].c = '#';
					path.push_back(current);
					current = current->parent;
				}
				path.push_back(start);
				return path;
			}

			// Loop to Check Neighbors
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
				{
					if ((i == 0 || j == 0) && !(i == 0 && j == 0) && inGrid(current->x + i, current->y + j)) // Check if inGrid and not Same Node // No Diagonal
					{
						Node* Neighbor = &grid[current->y + j][current->x + i];
						if (Neighbor->valid && !Neighbor->closed)
						{
							if (!Neighbor->open)
							{
								Neighbor->H = end->distance(Neighbor);
								open.insertNode(Neighbor);
							}
							double newG = current->G + current->distance(Neighbor);
							if (newG < Neighbor->G)
							{
								Neighbor->G = newG;
								Neighbor->parent = current;
								open.update(Neighbor);
							}
						}
					}
				}
		}

		// Path Blocked Case
		return path;
	}
};

void printPath(vector<Node*>& path, Map& M, bool RedPathing)
{
	cout << "\n\nOrigin is considered the top left cell \n";
	cout << "x increasing direction : Right \n";
	cout << "y increasing direction : Down \n\n";

	if (RedPathing)
		cout << "Path Length : \033[31m" << (int)path.size() << "\033[0m Node" << endl;
	else
		cout << "Path Length : " << (int)path.size() << " Node" << endl;

	cout << endl;

	cout << "Path Printed in steps";
	if (RedPathing)
		cout << "\033[31m (x,y)\033[0m :\n\n";
	else
		cout << " (x,y) :\n\n";

	reverse(path.begin(), path.end());
	for (auto a : path)
	{
		if (RedPathing)
			cout << " -> \033[31m(" << a->x << "," << a->y << ")\033[0m";
		else
			cout << " -> (" << a->x << "," << a->y << ")";
	}

	// Printing Map with Path indecated by '#'
	cout << "\n\nMap with Path indcatied with ";
	if (RedPathing)
		cout << "\033[31m#\033[0m :";
	else
		cout << "# :";

	cout << endl
		<< endl;
	for (auto r : M.grid)
	{
		cout << "[ ";
		for (auto j : r)
		{
			if (j.c == 'E')
			{
				if (RedPathing)
					cout << "\033[31mE\033[0m ,";
				else
					cout << "E ,";
			}
			else if (j.c == 'S')
			{
				if (RedPathing)
					cout << "\033[31mS\033[0m ,";
				else
					cout << "S ,";
			}
			else if (j.c == '#')
			{
				if (RedPathing)
					cout << "\033[31m#\033[0m ,";
				else
					cout << "# ,";
			}
			else if (j.valid == 1)
			{
				cout << "1 ,";
			}
			else
			{
				cout << "0 ,";
			}
		}
		cout << "]\n";
	}
}

int main()
{
	bool RedPathing = true; /// <- Disable this if your terminal doesn't Support Colors

	Map M;
	M.readGrid();
	vector<Node*> path = M.findPath();

	if (path.empty()) // Check if Destination is Blocked
	{
		cout << "Destination Is Blocked :(\n";
		return 0;
	}

	printPath(path, M, RedPathing); // Print Path
	return 0;
}