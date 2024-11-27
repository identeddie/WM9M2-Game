#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class node {
public:
	node* up = nullptr;
	node* down = nullptr;
	node* left = nullptr;
	node* right = nullptr;
	int type;

	node(int _type) {
		type = _type;
	}
};

class loopingTiles { //2-D Circular linked list 
public:
	node* start = nullptr;
	node* lineStart = nullptr;
	node* prev = nullptr;

	int width = 0;
	int height = 0;

	int prevPosX;
	int prevPosY;

	int mapHeight = 70;
	int mapWidth = 70;

	loopingTiles() {
		start = lineStart = prev = nullptr;

		width = 0;
		height = 0;

		prevPosX = 0;
		prevPosY = 0;

		mapHeight = 70;
		mapWidth = 70;
	}

	/*~loopingTiles() {

	}*/

	void generate(string filePath) {
		ifstream mapFile(filePath);

		while (mapFile.peek() != EOF) { //Check if next char end of file
			char c = mapFile.get(); //Get next char
			width++; 

			if (c != '\n') { //If not newline, continue appending to row
				node* n = new node(c - '0'); //Convert char to respecitve int

				if (start == nullptr) { //If start empty, assign to start
					start = lineStart = prev = n;
					height++;

				} else if (lineStart == start) { //Else if on first row, complete left and right
					n->left = prev;
					prev->right = n;

					prev = n;
				} else { //Otherwise comlplete all possible directions
					n->left = prev;
					prev->right = n;

					n->up = prev->up->right;
					prev->up->right->down = n;

					if (height == mapHeight) { //If at last row, join with top row
						if (width == mapWidth) {
							n->right = lineStart;
							lineStart->left = n;
						}

						prev = start;

						for (int i = 1; i < width; i++) {
							prev = prev->right;
						}

						prev->up = n;
						n->down = prev;
					}

					prev = n;
				}

			} else { //If newline
				prev->right = lineStart; //Join left and right ends of row
				lineStart->left = prev;

				char c = mapFile.get();
				node* n = new node(c - '0');
					
				height++;
				width = 1;

				n->up = lineStart;
				lineStart->down = n;
				lineStart = prev = n;

				if (height == mapHeight) { //If at last row, join with top row
					n->down = start;
					start->up = n;
				}
			}
		}
	}

	void display() {
		prev = start->down->down->right->down;

		for (int i = 1; i <= mapHeight * mapWidth; i++) {
			if (i % mapWidth == 0) {
				cout << prev->type << endl;
				prev = prev->right->down;
			}
			else {
				cout << prev->type;
				prev = prev->right;
			}
		}
	}
};