#pragma once

#include "GamesEngineeringBase.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "datastructs.h"

using namespace std;

class Vec2 {
public:
	float x, y;

	Vec2() {
		x = y = 0;
	}

	Vec2(float _x) {
		x = y = _x;
	}

	Vec2(float _x, float _y) {
		x = _x;
		y = _y;
	}

	Vec2 operator+(const Vec2 v) { //Elementwise addition
		Vec2 newVec;

		newVec.x = x + v.x;
		newVec.y = y + v.y;

		return newVec;
	}

	Vec2 operator-(const Vec2 v) { //Elementwise subtraction
		Vec2 newVec;

		newVec.x = x - v.x;
		newVec.y = y - v.y;

		return newVec;
	}

	Vec2 operator*(float k) { //Multiply each element by constant k
		Vec2 newVec;

		newVec.x = x * k;
		newVec.y = y * k;

		return newVec;
	}

	Vec2 operator*(const Vec2 v) { //Elementwise multiplication
		Vec2 newVec;

		newVec.x = x * v.x;
		newVec.y = y * v.y;

		return newVec;
	}

	Vec2 operator/(float k) { //Elementwise division by constant k
		Vec2 newVec;

		newVec.x = x / k;
		newVec.y = y / k;

		return newVec;
	}

	void setPos(float _x, float _y) {
		x = _x;
		y = _y;
	}

	float length() {
		return sqrtf(x * x + y * y);
	}

	Vec2 normalize() { //Normalize vector to unit length
		Vec2 newVec;

		newVec.x = x / length();
		newVec.y = y / length();

		return newVec;
	}

	void display() {
		std::cout << "X: " << x << " Y: " << y << endl;
	}
};

class camera {
public:
	int xPos, yPos, xDim, yDim;

	camera() {
		xPos = yPos = 0;
		xDim = 1024;
		yDim = 768;
	}

	camera(GamesEngineeringBase::Window& canvas, float _xPos, float _yPos) {
		xDim = (int)canvas.getWidth();
		yDim = (int)canvas.getHeight();
		xPos = (int)(_xPos + 0.5f) - xDim / 2;
		yPos = (int)(_yPos + 0.5f) - yDim / 2;
	}

	camera(float _xPos, float _yPos, int _xDim, int _yDim) {
		xDim = _xDim;
		yDim = _yDim;
		xPos = (int)(_xPos + 0.5f) - xDim / 2;
		yPos = (int)(_yPos + 0.5f) - yDim / 2;
	}

	void loadFromFile(string& fileLine) {
		string val;

		stringstream ss(fileLine);

		ss >> val;
		xDim = stoi(val);
		ss >> val;
		yDim = stoi(val);

		ss.clear();

		xPos = yPos = 0;
	}

	void save(ofstream& file) {
		file << xDim << " " << yDim << endl;
	}

	void focus(int worldSizeX, int worldSizeY, float _xPos, float _yPos) { //Focus camera on coordinates (for finite map)
		xPos = (int)(_xPos + 0.5f) - xDim / 2; //Round position coordinates to integers
		yPos = (int)(_yPos + 0.5f) - yDim / 2; //And set as center of viewbox

		if (xPos < 0) { //Limit viewbox to stop at map borders
			xPos = 0;
		} else if (xPos + xDim >= worldSizeX) {
			xPos = worldSizeX - xDim;
		}

		if (yPos < 0) {
			yPos = 0;
		} else if (yPos + yDim >= worldSizeY) {
			yPos = worldSizeY - yDim;
		}
	}

	void focus(float _xPos, float _yPos) { //Focus camera on coordinates (for infinite map)
		xPos = (int)(_xPos + 0.5f) - xDim / 2; //Round position coordinates to integers
		yPos = (int)(_yPos + 0.5f) - yDim / 2; //And set as center of viewbox
	}
};

class tileMap {
public:
	int xDim, yDim, tileDimX, tileDimY;
	int xTally, yTally;
	bool isInf;

	loopingTiles loop; //Data structure for map (2-D wrapping quadruply linked list?)

	GamesEngineeringBase::Image tilePack; //Image with all map tiles

	tileMap() {

		tilePack.load("Resources/tiles.png");

		xDim = yDim = 1500;
		tileDimX = tileDimY = 1500 / 32;
		xTally = yTally = 0;
		isInf = false;
	}

	tileMap(GamesEngineeringBase::Window& canvas) {
		tilePack.load("Resources/tiles.png");

		xDim = canvas.getWidth();
		yDim = canvas.getHeight();
		tileDimX = xDim / 32;
		tileDimY = yDim / 32;
		xTally = yTally = 0;
		isInf = false;
	}

	tileMap(int _tileDimX, int _tileDimY) {
		tilePack.load("Resources/tiles.png");

		tileDimX = _tileDimX;
		tileDimY = _tileDimY;

		xDim = tileDimX * 32;
		yDim = tileDimY * 32;
		xTally = yTally = 0;
		isInf = false;
	}

	tileMap(int _tileDimX, int _tileDimY, bool _isInf) {
		tilePack.load("Resources/tiles.png");
		loop.generate("Resources/mapnospace.txt");

		tileDimX = _tileDimX;
		tileDimY = _tileDimY;

		xDim = tileDimX * 32;
		yDim = tileDimY * 32;
		xTally = yTally = 0;
		isInf = _isInf;
	}

	void loadFromFile(string& fileLine) {
		tilePack.load("Resources/tiles.png");
		loop.generate("Resources/mapnospace.txt");

		string val;

		stringstream ss(fileLine);

		ss >> val;
		isInf = stoi(val);
		ss >> val;
		xDim = stoi(val);
		ss >> val;
		yDim = stoi(val);
		xTally = stoi(val);
		ss >> val;
		yTally = stoi(val);

		ss.clear();

		tileDimX = xDim / 32;
		tileDimY = yDim / 32;
	}

	void save(ofstream& file) {
		file << (int)isInf << " " << xDim << " " << yDim << " " << xTally << " " << yTally << endl;
	}

	void load(string filePath) { //Load file of map into the data structure for the map
		loop.generate(filePath);
	}

	void setStructStart(camera& cam) { //Set start pointer so that it keeps original node start points to at origin ((0, 0) on plane)
		if (cam.xPos > 0) { //If greater than zero, shift right by distance in tiles
			for (int i = 0; i < cam.xPos / 32; i++) {
				loop.start = loop.start->right;
			}
		} else {
			for (int i = 0; i > cam.xPos / 32; i--) { //Otherwise shift left
				loop.start = loop.start->left;
			}
		}

		if (cam.yPos > 0) {
			for (int i = 0; i < cam.yPos / 32; i++) { //Shift down
				loop.start = loop.start->down;
			}
		} else {
			for (int i = 0; i > cam.yPos / 32; i--) { //Otherwise shift up
				loop.start = loop.start->up;
			}
		}
	}

	void updateStructStart(camera& cam) { //Move the start pointer to match the camera's positon
		xTally += cam.xPos - loop.prevPosX;
		yTally += cam.yPos - loop.prevPosY;

		if (xTally > 32) { //When camera moved over 32 pixels, shift right to new tile
			loop.start = loop.start->right;
			xTally -= 32; //Maintain tally count of new position

		} else if (xTally < 1) { //Similarly, shift left when pixel tally is 0 or less
			loop.start = loop.start->left;
			xTally += 32;
		}

		//Identical logic for y-direction
		if (yTally > 32) {
			loop.start = loop.start->down;
			yTally -= 32;
		} else if (yTally < 1) {
			loop.start = loop.start->up;
			yTally += 32;
		}

		loop.prevPosX = cam.xPos;
		loop.prevPosY = cam.yPos;
	}

	void setPos(camera& cam) {
		xTally = cam.xPos % 32 + 1;
		yTally = cam.yPos % 32 + 1;

		loop.prevPosX = cam.xPos;
		loop.prevPosY = cam.yPos;
	}

	bool checkTile(node* n) { //Check if tile is impassablew (i.e. water)
		if (n->type == 1) {
			return true;
		} else {
			return false;
		}
	}

	void draw(GamesEngineeringBase::Window& canvas, camera& cam) { //Draws all tiles in camera's view
		loop.prev = loop.lineStart = loop.start;

		for (int x = cam.xPos; x < cam.xPos + cam.xDim; x++) {

			if (x % 32 == 0 && cam.xPos != x) {
				loop.prev = loop.prev->right;
				loop.lineStart = loop.lineStart->right;
			}

			for (int y = cam.yPos; y < cam.yPos + cam.yDim; y++) {

				if (y % 32 == 0 && y != cam.yPos) {
					loop.prev = loop.prev->down;
				}

				bool shiftX = 0;
				bool shiftY = 0;

				int xDraw = x;
				int yDraw = y;

				if (loop.prev->type & 1) {
					shiftX = 1;
				}

				if ((loop.prev->type >> 1) & 1) {
					shiftY = 1;
				}

				if (x < 0) {
					xDraw = 32 + abs(x);
				}

				if (y < 0) {
					yDraw = 32 + abs(y);
				}

				canvas.draw(x - cam.xPos, y - cam.yPos, tilePack.at(yDraw % 32 + (32 * shiftX), xDraw % 32 + (32 * shiftY)));
			}

			loop.prev = loop.lineStart;
		}
	}
};

class Bar { //Rectangular Object with adjustable width
public:
	float xPos;
	float yPos;
	int width;
	float percentage;
	unsigned int r, g, b;

	Bar() {
		xPos = 0.f;
		yPos = 0.f;
		width = 0;
		percentage = 1.f;
		r = 200;
		g = b = 0;
	}

	Bar(float _xPos, float _yPos) {
		xPos = _xPos;
		yPos = _yPos;
		percentage = 1.f;
		width = 0;
		r = 200;
		g = b = 0;
	}

	Bar(float _xPos, float _yPos, int _width) {
		xPos = _xPos;
		yPos = _yPos;
		width = _width;
		percentage = 1.f;
		r = 200;
		g = b = 0;
	}

	void update(float _xPos, float _yPos, float _percentage) {
		xPos = _xPos;
		yPos = _yPos;
		percentage = _percentage;
	}

	void setColor(unsigned int _r, unsigned int _g, unsigned int _b) {
		r = _r;
		g = _g;
		b = _b;
	}

	void draw(GamesEngineeringBase::Window& canvas, camera& cam) {
		for (int x = 0; x < (int)(width * percentage); x++) { //Limit width by percentage
			for (int y = 0; y < 7; y++) { //Fixed height of 7 pixels
				if ((int)(xPos + 0.5f) + x > cam.xPos && (int)(xPos + 0.5f) + x < cam.xPos + cam.xDim) { //Check if pixels are off screen
					if ((int)(yPos + 0.5f) + y > cam.yPos && (int)(yPos + 0.5f) + y < cam.yPos + cam.yDim) {
						canvas.draw(((int)(xPos + 0.5f) - cam.xPos) + x, ((int)(yPos + 0.5f) - cam.yPos) + y, r, g, b);
					}
				}
			}
		}
	}
};

class sprite {
public:
	float xPos, yPos;
	int xDim, yDim;
	float maxHP;
	float health;
	float speed;

	Bar healthBar;
	Vec2 dir;

	GamesEngineeringBase::Image* image = new GamesEngineeringBase::Image;

	sprite() {

		image->load("Resources/L.png");
		xPos = 512.f;
		yPos = 380.f;
		xDim = image->width;
		yDim = image->height;
		maxHP = 0.f;
		health = 0.f;
		speed = 0.f;

		dir.setPos(0, 0);
	}

	sprite(float _xPos, float _yPos) {
		image->load("Resources/L.png");
		xPos = _xPos;
		yPos = _yPos;
		xDim = healthBar.width = image->width;
		yDim = image->height;
		health = 100.f;
		maxHP = 100.f;
		speed = 5.f;
	}

	sprite(float _xPos, float _yPos, int _dim) {
		image->load("Resources/L.png");
		xPos = _xPos;
		yPos = _yPos;
		xDim = yDim = healthBar.width = _dim;
		health = 100.f;
		maxHP = 100.f;
		speed = 5.f;
	}

	sprite(float _xPos, float _yPos, int _xDim, int _yDim) {
		image->load("Resources/L.png");
		xPos = _xPos;
		yPos = _yPos;
		xDim = healthBar.width = _xDim;
		yDim = _yDim;
		health = 100.f;
		maxHP = 100.f;
		speed = 2.f;
	}

	sprite(float _xPos, float _yPos, int _xDim, int _yDim, string filePath) {
		image->load(filePath);
		xPos = _xPos;
		yPos = _yPos;
		xDim = healthBar.width = _xDim;
		yDim = _yDim;
		health = 100.f;
		maxHP = 100.f;
		speed = 2.f;
	}

	/*~sprite() {
		delete image;
	}*/

	virtual void save(ofstream& file) {}

	void setPos(float _xPos, float _yPos) {
		xPos = _xPos;
		yPos = _yPos;
	}

	void setSpriteImg(string filePath) { //Load image from file
		image->load(filePath);
		xDim = healthBar.width = image->width; //Set health bar width to sprite width
		yDim = image->height;
	}

	void setSpriteImg(string filePath, int _xDim, int _yDim) { //Load image from file, providing sprite dimensions (for images with multiple sprites_
		image->load(filePath);
		xDim = healthBar.width = _xDim; //Set health bar width to sprite width
		yDim = _yDim;
	}

	float getCenterX() {
		return xPos + (float)xDim / 2.f;
	}

	float getCenterY() {
		return yPos + (float)yDim / 2.f;
	}

	void draw(GamesEngineeringBase::Window& canvas, camera& cam) { //Draw sprite
		for (int x = 0; x < xDim; x++) {
			for (int y = 0; y < yDim; y++) {
				if ((int)(xPos + 0.5f) + x > cam.xPos && (int)(xPos + 0.5f) + x < cam.xPos + cam.xDim) { //Check if pixels are off screen
					if ((int)(yPos + 0.5f) + y > cam.yPos && (int)(yPos + 0.5f) + y < cam.yPos + cam.yDim) {
						if (image->alphaAt(x, y) > 200) {
							canvas.draw(((int)(xPos + 0.5f) - cam.xPos) + x, ((int)(yPos + 0.5f) - cam.yPos) + y, image->at(x, y));
						}
					}
				}
			}
		}

		healthBar.update(xPos, yPos - 20.f, health / maxHP); //Update position and percentage of health bar to match sprite data
		healthBar.draw(canvas, cam); //Draw health bar
	}

	bool touching(sprite& s) { //Checks if two sprites are touching (intersection of rectangular areas)
		if (xPos + xDim > s.xPos && xPos < s.xPos + s.xDim) {
			if (yPos + yDim > s.yPos && yPos < s.yPos + s.yDim) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	void resetVec() { //Resets movement vector
		dir.setPos(0.f, 0.f);
	}

	void move(float dt) { //Moves sprite, uncoupled with framerate
		if (dir.length() != 0.f) { //Only move if movement vector is non-zero 
			
			float k = (speed * dt) / dir.length(); 
			dir = dir * k; //Normalize movement vector, cap movement at sprite's speed value

			//Increment position in respective directions
			if (dir.x > 0) {
				xPos += dir.x;
			} else {
				xPos += dir.x;
			}

			if (dir.y > 0) {
				yPos += dir.y;
			} else {
				yPos += dir.y;
			}
		}
	}
};

class player : public sprite {
public:
	unsigned int aoeLvl;
	float atkLvl;

	float tempPosX;
	float tempPosY;

	float damage;

	float elapsed;

	player() {
		image->load("Resources/L.png");
		xPos = tempPosX = 512.f;
		yPos = tempPosY = 380.f;
		xDim = healthBar.width = image->width;
		yDim = image->height;
		health = 100.f;
		maxHP = 100.f;
		speed = 0.f;
		damage = 15.f;

		elapsed = 0.f;

		dir.setPos(0, 0);

		aoeLvl = 2;
		atkLvl = 3.f;
	}

	player(float _xPos, float _yPos) {
		image->load("Resources/L.png");
		xPos = tempPosX = _xPos;
		yPos = tempPosY = _yPos;
		xDim = healthBar.width = image->width;
		yDim = image->height;

		health = 100.f;
		maxHP = 100.f;
		speed = 200.f;
		damage = 15.f;

		elapsed = 0.f;

		aoeLvl = 2;
		atkLvl = 3.f;
	}

	void loadFromFile(string& fileLine) {
		string val;

		stringstream ss(fileLine);

		ss >> val;
		xPos = stof(val);
		ss >> val;
		yPos = stof(val);
		ss >> val;
		tempPosX = stof(val);
		ss >> val;
		tempPosY = stof(val);
		ss >> val;
		health = stof(val);
		ss >> val;
		atkLvl = stof(val);
		ss >> val;
		aoeLvl = stoi(val);
		ss >> val;
		elapsed = stof(val);

		ss.clear();

		maxHP = 100.f;
		speed = 200.f;
		damage = 15.f;

		image->load("Resources/L.png");
		xDim = healthBar.width = image->width;
		yDim = image->height;
	}

	void save(ofstream& file) {
		file << xPos << " " << yPos << " " << tempPosX << " " << tempPosY << " " << health << " " << atkLvl << " " << aoeLvl << " " << elapsed << endl;
	}

	void processInputs(GamesEngineeringBase::Window& canvas, float dt) {
		resetVec(); //Reset movement vector

		//Move player
		if (canvas.keyPressed('W')) { //up
			dir.y -= speed * dt;
		}

		if (canvas.keyPressed('S')) { //down
			dir.y += speed * dt;
		}

		if (canvas.keyPressed('A')) { //left
			dir.x -= speed * dt;
		}

		if (canvas.keyPressed('D')) { //right
			dir.x += speed * dt;
		}
	}

	void attack(sprite& mob) { //Direct attack, simply deducts health from targeted sprite (no projectile)
		mob.health -= damage;
	}

	void move(float dt) { //moves player, only difference to sprite method is saving temp position values (for collisions)
		if (dir.length() != 0.f) { //Only move if movement vector is non-zero 
			float k = (speed * dt) / dir.length();
			dir = dir * k; //Cap movment vector at player speed

			tempPosX = xPos; //save player position before movement (for collisions)
			tempPosY = yPos;

			if (dir.x > 0) {
				xPos += dir.x;
			} else {
				xPos += dir.x;
			}

			if (dir.y > 0) {
				yPos += dir.y;
			} else {
				yPos += dir.y;
			}
		}
	}

	void handleRestricted(tileMap& map, camera& cam) { //Reverts position for map borders and impassable terrain
		if (!map.isInf) { //For finite map, prevent passing world boundary
			if (xPos < 0.f || xPos + xDim > map.xDim) {
				xPos = tempPosX;
			}

			if (yPos < 0.f || yPos + yDim > map.yDim) {
				yPos = tempPosY;
			}
		}

		int xDiff = (int)((xPos - cam.xPos + map.xTally) / 32.f); //Get difference from camera corner to player, in tiles
		int yDiff = (int)((yPos - cam.yPos + map.yTally) / 32.f);

		map.loop.prev = map.loop.start;

		//Shift from start node to tile at top right of player
		for (int i = 0; i < xDiff; i++) {
			map.loop.prev = map.loop.prev->right;
		}
		for (int i = 0; i < yDiff; i++) {
			map.loop.prev = map.loop.prev->down;
		}

		//Loop through area of player
		map.loop.lineStart = map.loop.prev;
		for (int x = (int)xPos; x < (int)(xPos + 0.5f) + xDim; x++) {
			map.loop.prev = map.loop.lineStart;
			if (x % 32 == 0 && (int)xPos != x) { //If passed tile width, shift to next tile
				map.loop.lineStart = map.loop.lineStart->right;
			}

			for (int y = (int)yPos; y < (int)(yPos + 0.5f) + yDim; y++) {
				if (y % 32 == 0 && (int)yPos != y) { //If passed tile width, shift to next tile
					map.loop.prev = map.loop.prev->down;
				}

				if (map.checkTile(map.loop.prev)) { //If tile is impassible, revert to last acceptable position
					xPos = tempPosX;
					yPos = tempPosY;
					break;
				}
			}
		}
	}
};

class mob : public sprite {
public:
	short type;

	short shiftX;

	float farness;

	float dps;

	mob() {
		type = 0;

		//image->load("Resources/L.png");
		xPos = 512.f;
		yPos = 380.f;
		xDim = healthBar.width = 32;
		yDim = 32;
		health = 10;
		maxHP = 0;
		speed = 60.f;
		dps = 1.f;
		farness = 0.f;

		shiftX = 0;

		dir.setPos(0, 0);
	}

	void initType(short _type) {
		type = _type;

		switch (type) { //Cases for each mob type
		case 0:
			xDim = 90;
			yDim = 120;
			health = maxHP = 80.f;
			speed = 0.f;
			dps = 1.f;
			shiftX = 0;
			break;
		case 1:
			xDim = 55;
			yDim = 40;
			health = maxHP = 15.f;
			speed = 100.f;
			dps = 5.f;
			shiftX = 90;
			break;
		case 2:
			xDim = 80;
			yDim = 100;
			health = maxHP = 50.f;
			speed = 30.f;
			dps = 30.f;
			shiftX = 145;
			break;
		case 3:
			xDim = 50;
			yDim = 50;
			health = maxHP = 30.f;
			speed = 75.f;
			dps = 12.f;
			shiftX = 225;
			break;
		}
	}

	mob(short _type, GamesEngineeringBase::Image& img) {
		image = &img;
		xPos = 512.f;
		yPos = 380.f;
		dps = 1.f;
		shiftX = 0;

		initType(_type);

		healthBar.width = xDim;
		farness = 0.f;
		dir.setPos(0, 0);
	}

	mob(string fileLine, GamesEngineeringBase::Image& img) {
		image = &img;
		string val;

		stringstream ss(fileLine);

		ss >> val;
		initType((short)stoi(val));
		ss >> val;
		xPos = stof(val);
		ss >> val;
		yPos = stof(val);
		ss >> val;
		health = stof(val);
		ss >> val;
		farness = stof(val);
		ss >> val;
		dir.x = stof(val);
		ss >> val;
		dir.y = stof(val);
		
		ss.clear();

		healthBar.width = xDim;
	}

	void save(ofstream& file) {
		file << type << " " << xPos << " " << yPos << " " << health << " " << farness << " " << dir.x << " " << dir.y << endl;
	}

	void distanceTo(sprite& player) { //Calculate and save distance to player
		Vec2 vec = Vec2(getCenterX() - player.getCenterX(), getCenterY() - player.getCenterY());

		farness = vec.length();
	}

	void target(sprite& player) { //Direct movement towards a sprite and save distance to same sprite
		dir.setPos((float)(player.getCenterX() - getCenterX()), (float)(player.getCenterY() - getCenterY()));
		distanceTo(player);
	}

	void draw(GamesEngineeringBase::Window& canvas, camera& cam) { //Draw mob
		for (int x = 0; x < xDim; x++) {
			for (int y = 0; y < yDim; y++) {
				if ((int)(xPos + 0.5f) + x > cam.xPos && (int)(xPos + 0.5f) + x < cam.xPos + cam.xDim) { //Check if pixels are off screen
					if ((int)(yPos + 0.5f) + y > cam.yPos && (int)(yPos + 0.5f) + y < cam.yPos + cam.yDim) {
						if (image->alphaAt(x + shiftX, y) > 200) { //Do not draw transparent pixels
							canvas.draw(((int)(xPos + 0.5f) - cam.xPos) + x, ((int)(yPos + 0.5f) - cam.yPos) + y, image->at(x + shiftX, y));
						}
					}
				}
			}
		}

		healthBar.update(xPos, yPos - 20.f, health / maxHP);
		healthBar.draw(canvas, cam);
	}
};

template <class T>
class nodeLL {
public:
	nodeLL<T>* next = nullptr;
	nodeLL<T>* prev = nullptr;

	T sprite;

	nodeLL(T b) {
		sprite = b;
	}
};

template <class T>
class linkedList {
public:
	nodeLL<T>* head = nullptr;
	nodeLL<T>* tail = nullptr;

	linkedList() {
		head = nullptr;
	}

	/*~linkedList() { //Destructor for list
		for (nodeLL<T>* nptr = head; nptr != nullptr; nptr = nptr->next) {
			nodeLL<T>* next = nptr->next;
			delete nptr;
			nptr = next;
		}
	}*/

	virtual void save(ofstream& file) {}

	void add(T b) { //Adds node
		nodeLL<T>* n = new nodeLL<T>(b);

		if (head == nullptr) { //If empty, add as head and tail
			head = tail = n;
		} else { //Otherwise, append to end
			tail->next = n;
			n->prev = tail;
			tail = n; //Make node new tail
		}
	}

	void swap(nodeLL<T>* n) { //Swap a node and the node previous to it
		if (head != n) { //Don't swap if it is the head
			nodeLL<T>* nextNode = n->next; 
			nodeLL<T>* prevNode = n->prev->prev;

			if (nextNode == nullptr && prevNode == nullptr) { //Meaning node is tail and previous node is head
				head = n;
				tail = n->prev;

				n->next = n->prev;
				n->prev->prev = n;

				n->prev->next = nullptr;
				n->prev = nullptr;
			} else if (nextNode == nullptr) { //Meaning node is tail
				tail = n->prev;

				prevNode->next = n;
				n->next = tail;

				tail->prev = n;
				n->prev = prevNode;

				tail->next = nullptr;
			} else if (prevNode == nullptr) { //Meaning previous node is head
				n->next = head;
				head->next = nextNode;

				nextNode->prev = head;
				head->prev = n;

				head = n;
				head->prev = nullptr;
			} else { //For all general cases
				prevNode->next = n;
				n->next = n->prev;
				n->prev->next = nextNode;

				nextNode->prev = n->prev;
				n->prev->prev = n;
				n->prev = prevNode;
			}
		}
	}

	void display() {
		for (nodeLL<T>* n = head; n != nullptr; n = n->next) {
			cout << n->sprite << "\t";
		}
		cout << endl;
	}

	virtual void movecleandraw(GamesEngineeringBase::Window& canvas, camera& cam, player& guy, float dt) {}
};

class swarmLL : public linkedList<mob> {
public:
	GamesEngineeringBase::Image mobPack;
	float elapsed = 0.f;
	float spawnTime = 4.f;

	nodeLL<mob>* closest = nullptr;

	swarmLL() {
		mobPack.load("Resources/mobs2.png");
	}

	void loadFromFile(ifstream& save) {
		mobPack.load("Resources/mobs2.png");

		string val;
		string fileLine;

		getline(save, fileLine);

		stringstream ss(fileLine);

		ss >> val;
		elapsed = stof(val);
		ss >> val;
		spawnTime = stof(val);
		ss >> val;
		unsigned int num = stoi(val);

		ss.clear();

		for (unsigned int i = 0; i < num; i++) {
			getline(save, fileLine);
			add(mob(fileLine, mobPack));
		}
	}

	void save(ofstream& file) {
		unsigned int i = 0;
		for (nodeLL<mob>* nptr = head; nptr != nullptr; nptr = nptr->next) {
			i++;
		}

		file << elapsed << " " << spawnTime << " " << i << endl;

		for (nodeLL<mob>* nptr = head; nptr != nullptr; nptr = nptr->next) {
			nptr->sprite.save(file);
		}
	}

	void assertSort(nodeLL<mob>* n) { //Check if ordered by health correctly, otherwise swap and do recursion
		if (n != head) {
			if (n->prev->sprite.health > n->sprite.health) {
				swap(n);
				assertSort(n);
			}
		}
	}

	void addSorted (mob m) { //Add node adhering to sorted order
		nodeLL<mob>* n = new nodeLL<mob>(m);

		if (head == nullptr) { //If empty, just add
			head = tail = n;
		} else {
			for (nodeLL<mob>* nptr = head; nptr != nullptr; nptr = nptr->next) { //Loop through list until acceptable position found, then insert
				if (n->sprite.health <= nptr->sprite.health) {
					if (nptr == head) {
						n->next = head;
						head->prev = n;
						head = n;
						return;
					} else {
						nptr->prev->next = n;
						n->prev = nptr->prev;
						n->next = nptr;
						nptr->prev = n;
						return;
					}
				}
			}
			
			tail->next = n;
			n->prev = tail;
			tail = n;
		}
	}

	void movecleandraw(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Timer& time, unsigned int& score, float& elap, camera& cam, player& guy, float dt) {
		nodeLL<mob>* nptr = closest = head;
		nodeLL<mob>* next;
		nodeLL<mob>* prevClosest = closest;

		while (nptr != nullptr) {
			next = nptr->next;
			nptr->sprite.target(guy);

			if (nptr->sprite.health <= 0) {
				score++;
				if (nptr == head) {
					head = next;
					closest = prevClosest = head;
				} else if (next == nullptr) {
					nptr->prev->next = nullptr;
				} else {
					nptr->prev->next = next;
					next->prev = nptr->prev;
				}

				if (rand() % 100 < 45) { //When mob dies, 45% chance to get power-up
					elap += time.dt();
					short powerUp = 0;
					do {
						cout << "Input Power Up: 1. Attack Speed, 2. AoE Spread ";
						cin >> powerUp;

						if (powerUp == 1) {
							guy.atkLvl -= 0.15f * guy.atkLvl; //Decrease attack time by 15%
							elapsed -= time.dt();
						} else if (powerUp == 2) {
							guy.aoeLvl += 2;
							elap -= time.dt();
						} else {
							cout << "Invalid Input" << endl;
						}
					} while (powerUp != 1 && powerUp != 2);
				}

				delete nptr;
			} else if (nptr->sprite.touching(guy)) {
				if (nptr != closest) {
					if (nptr->sprite.farness < closest->sprite.farness) {
						prevClosest = closest;
						closest = nptr;
					}
				}

				guy.health -= nptr->sprite.dps * dt;
				nptr->sprite.draw(canvas, cam);
			} else {
				if (nptr != closest) {
					if (nptr->sprite.farness < closest->sprite.farness) {
						prevClosest = closest;
						closest = nptr;
					}
				}

				nptr->sprite.move(dt);
				nptr->sprite.draw(canvas, cam);
			}

			nptr = next;
		}
	}

	void update(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Timer& time, unsigned int& score, float& elap, tileMap& map, player& guy, camera& cam, float dt) {
		elapsed += dt;

		if (elapsed > spawnTime) { //Spawn new mob if time greater than current spawn time
			spawnTime -= 0.025f * spawnTime; //Decrease spawn time by 2.5%
			mob p(rand() % 4, mobPack);

			int spawnZone = rand() % 4;
			int spawnDist = rand() % 100;
			int spawnPosX = 0;
			int spawnPosY = 0;

			if (spawnZone == 0) { //Above view
				spawnPosX = rand() % (cam.xDim + 200) + (cam.xPos - 100);
				spawnPosY = cam.yPos - p.yDim - spawnDist;
			}
			else if (spawnZone == 1) { //Below view
				spawnPosX = rand() % (cam.xDim + 200) + (cam.xPos - 100);
				spawnPosY = cam.yPos + cam.yDim + spawnDist;
			}
			else if (spawnZone == 2) { //Left of view
				spawnPosY = rand() % (cam.yDim + 200) + (cam.yPos - 100);
				spawnPosX = cam.xPos - p.xDim - spawnDist;
			}
			else if (spawnZone == 3) { //Right of view
				spawnPosY = rand() % (cam.yDim + 200) + (cam.yPos - 100);
				spawnPosX = cam.xPos + cam.xDim + spawnDist;
			}

			p.setPos((float)spawnPosX, (float)spawnPosY);
			addSorted(p);
			elapsed = 0;
		}

		movecleandraw(canvas, time, score, elap, cam, guy, dt);
	}
};

template <class T>
class projectile : public sprite {
public:
	float damage;
	bool inView;

	T* targ;

	projectile() {
		image->load("Resources/L.png");
		xPos = 0.f;
		yPos = 0.f;
		xDim = 16;
		yDim = 16;
		maxHP = 0.f;
		health = 0.f;
		speed = 500.f;
		inView = false;
		targ = nullptr;

		damage = 5.f;

		dir.setPos(0, 0);
	}

	projectile(float _xPos, float _yPos) {
		image->load("Resources/bullet.png");
		xPos = _xPos;
		yPos = _yPos;
		xDim = 16;
		yDim = 16;
		maxHP = 0.f;
		health = 0.f;
		speed = 500.f;
		inView = false;
		targ = nullptr;

		damage = 5.f;

		dir.setPos(0, 0);
	}

	projectile(float _xPos, float _yPos, float _damage) {
		image->load("Resources/bullet.png");
		xPos = _xPos;
		yPos = _yPos;
		xDim = 16;
		yDim = 16;
		maxHP = 0.f;
		health = 0.f;
		speed = 500.f;
		inView = false;
		targ = nullptr;

		damage = _damage;

		dir.setPos(0, 0);
	}

	projectile(string fileLine, player& guy, T* mobs) {
		string val;
		stringstream ss(fileLine);

		ss >> val;
		xPos = stof(val);
		ss >> val;
		yPos = stof(val);
		ss >> val;
		damage = stof(val);
		ss >> val;
		inView = stoi(val);
		ss >> val;
		bool fromSwarm = stoi(val);
		ss >> val;
		dir.x = stof(val);
		ss >> val;
		dir.y = stof(val);

		ss.clear();

		targ = mobs;

		xDim = 16;
		yDim = 16;
		maxHP = 0.f;
		health = 0.f;
		speed = 500.f;
	}

	void save(ofstream& file, bool fromSwarm) {
		file << xPos << " " << yPos << " " << damage << " " << inView << " " << fromSwarm << " " << dir.x << " " << dir.y << endl;
	}

	void setInView(camera& cam) { //Only checks if projectiles out of view are now in view
		if (inView == false) {
			if (xPos >= (cam.xPos - xDim) && xPos <= (cam.xPos + cam.xDim) && yPos >= (cam.yPos - yDim) && yPos <= (cam.yPos + cam.yDim)) {
				inView = true;
			}
		}
	}

	bool checkInView(camera& cam) { //Checks if projectile is in view
		if (xPos < (cam.xPos - xDim) || xPos >(cam.xPos + cam.xDim) || yPos < (cam.yPos - yDim) || yPos >(cam.yPos + cam.yDim)) {
			return false;
		} else {
			return true;
		}
	}

	void target(T& _targ) { //Set the target
		targ = &_targ;
		dir.setPos((float)(_targ.getCenterX() - getCenterX()), (float)(_targ.getCenterY() - getCenterY()));
	}

	void target(nodeLL<mob>* _targ) { //Set the target, overloaded to allow node of type mob as target
		targ = _targ;
		dir.setPos((float)(targ->sprite.getCenterX() - getCenterX()), (float)(targ->sprite.getCenterY() - getCenterY()));
	}
};

template <class T>
class projectiles : public linkedList<projectile<T>> {
public:
	float elapsed = 0.f;

	projectiles() {}

	void loadFromFile(ifstream& save, player& guy, T* mobs, bool fromSwarm) {
		string val;
		string fileLine;
		getline(save, fileLine);
		stringstream ss(fileLine);

		ss >> val;
		elapsed = stof(val);
		ss >> val;
		unsigned int num = stoi(val);

		ss.clear();

		for (unsigned int i = 0; i < num; i++) {
			getline(save, fileLine);
			projectile<T> p(fileLine, guy, mobs);
			this->add(p);
		}
	}

	void save(ofstream& file, bool fromSwarm) {
		unsigned int i = 0;
		for (nodeLL<projectile<T>>* nptr = this->head; nptr != nullptr; nptr = nptr->next) {
			i++;
		}

		file << elapsed << " " << i << endl;

		for (nodeLL<projectile<T>>* nptr = this->head; nptr != nullptr; nptr = nptr->next) {
			nptr->sprite.save(file, fromSwarm);
		}
	}

	void movecleandraw(GamesEngineeringBase::Window& canvas, camera& cam, swarmLL& swarm, float dt) {
		nodeLL<projectile<T>>* nptr = this->head;
		nodeLL<projectile<T>>* next;

		while (nptr != nullptr) { //Loop through until end is found
			next = nptr->next;
			nptr->sprite.move(dt); //Move projectile (decoupled from framerate)
			nptr->sprite.setInView(cam); //Set if projectile is initially in view or not

			if (nptr->sprite.touching(nptr->sprite.targ->sprite)) { //If touching, damage and delete
				nptr->sprite.targ->sprite.health -= nptr->sprite.damage;
				
				if (nptr->sprite.targ->sprite.health > 0.f) { //No point sorting if mob is dead
					swarm.assertSort(nptr->sprite.targ); //If still alive, swap until in valid position
				}

				if (nptr == this->head) {
					this->head = next;
				} else if (next == nullptr) {
					nptr->prev->next = nullptr;
				} else {
					nptr->prev->next = next;
					next->prev = nptr->prev;
				}

				delete nptr;
			} else if (nptr->sprite.checkInView(cam) == false && nptr->sprite.inView) { //Delete if projectile comes into view then out of view
				if (nptr == this->head) {
					this->head = next;
				} else if (next == nullptr) {
					nptr->prev->next = nullptr;
				} else {
					nptr->prev->next = next;
					next->prev = nptr->prev;
				}

				delete nptr;
			} else { //if no deletion cases occur, draw
				nptr->sprite.draw(canvas, cam); 
			}

			nptr = next;
		}
	}

	//Same function, overloaded for player target
	void movecleandraw(GamesEngineeringBase::Window& canvas, camera& cam, player& guy, float dt) {
		nodeLL<projectile<T>>* nptr = this->head;
		nodeLL<projectile<T>>* next;

		while (nptr != nullptr) {
			next = nptr->next;
			nptr->sprite.move(dt);
			nptr->sprite.setInView(cam);

			if (nptr->sprite.touching(guy)) {
				guy.health -= nptr->sprite.damage;

				if (nptr == this->head) {
					this->head = next;
				}
				else if (next == nullptr) {
					nptr->prev->next = nullptr;
				}
				else {
					nptr->prev->next = next;
					next->prev = nptr->prev;
				}

				delete nptr;
			}
			else if (nptr->sprite.checkInView(cam) == false && nptr->sprite.inView) {
				if (nptr == this->head) {
					this->head = next;
				}
				else if (next == nullptr) {
					nptr->prev->next = nullptr;
				}
				else {
					nptr->prev->next = next;
					next->prev = nptr->prev;
				}

				delete nptr;
			}
			else {
				nptr->sprite.draw(canvas, cam);
			}

			nptr = next;
		}
	}

	void update(GamesEngineeringBase::Window& canvas, camera& cam, swarmLL& swarm, player& targ, float dt) { //Handle generation of swarm projectiles
		elapsed += dt;
		bool shot = false;

		for (nodeLL<mob>* nptr = swarm.head; nptr != nullptr; nptr = nptr->next) {
			if (nptr->sprite.type == 0) { //Check mob is stationary type
				if (nptr->sprite.farness < 1200.f) { //Ignore if mobs are far away
					if (rand() % 3 + 2 < elapsed) { //Shoot if elapsed greater than random time (2s to 4s)
						shot = true;
						projectile<T> p(nptr->sprite.getCenterX(), nptr->sprite.getCenterY()); //Create new projectile
						p.setInView(cam); //Set initial inView status
						p.target(targ); //Target direction towards player
						this->add(p); //Add projectile to list

					}
				}
			}
		}

		if (shot) {
			elapsed = 0; //Reset elapsed if at least one stationary mob shot projectile
		}

		movecleandraw(canvas, cam, targ, dt); //Do all maintenance tasks
	}

	void addProjectile(player& guy, T* targ) { //Manually create projectile (for targeting closest mob in swarm)
		projectile<T> p(guy.getCenterX(), guy.getCenterY(), guy.damage);
		p.target(targ);
		this->add(p);
	}
};
 
void saveGame(unsigned int& score, float& elapsed, float& cooldown, float& roundTimer, unsigned int& frameCnt, tileMap& world, camera& cam, player& guy, swarmLL& swarm, projectiles<player>& mbullets, projectiles<nodeLL<mob>>& pbullets) {
	ofstream save("savestate.txt");
	save << score << " " << elapsed << " " << cooldown << " " << roundTimer << " " << frameCnt << endl;
	cam.save(save);
	world.save(save);
	guy.save(save);
	swarm.save(save);
	mbullets.save(save, true);
	pbullets.save(save, false);

	save.close();
}

void loadGame(string saveFile, unsigned int& score, float& elapsed, float& cooldown, float& roundTimer, unsigned int& frameCnt, tileMap& world, camera& cam, player& guy, swarmLL& swarm, projectiles<player>& mbullets, projectiles<nodeLL<mob>>& pbullets) {
	ifstream save(saveFile);
	string fileLine;

	getline(save, fileLine);
	stringstream ss(fileLine);
	string val;

	ss >> val;
	score = stoi(val);
	ss >> val;
	elapsed = stof(val);
	ss >> val;
	cooldown = stof(val);
	ss >> val;
	roundTimer = stof(val);
	ss >> val;
	frameCnt = stoi(val);

	getline(save, fileLine);
	cam.loadFromFile(fileLine);

	getline(save, fileLine);
	world.loadFromFile(fileLine);

	getline(save, fileLine);
	guy.loadFromFile(fileLine);

	swarm.loadFromFile(save);
	mbullets.loadFromFile(save, guy, &guy, true);
	pbullets.loadFromFile(save, guy, swarm.closest, false);

	save.close();
}