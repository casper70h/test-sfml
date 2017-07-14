#include <SFML/Graphics.hpp>
//#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include "iostream"
#include "level.h"
#include <vector>
#include <list>
#include "TinyXML/tinyxml.h"

using namespace sf;


////////////////////////////////////����� ����� ��������//////////////////////////
class Entity {
public:
	std::vector<Object> obj;//������ �������� �����
	float dx, dy, x, y, speed, moveTimer;
	int w, h, health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;
	Entity(Image &image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {//�-��� ��������� ��������������. ��� �����,������� (���,�����).
		return FloatRect(x, y, w, h);//��� �-��� ����� ��� �������� ������������ 
	}

	virtual void update(float time) = 0; //��� ������� �������������� ��� �������
};
////////////////////////////////////////////////////����� ������////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;
	int playerScore;
	bool isShoot;
	Player(Image &image, String Name, Level &lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		playerScore = 0; state = stay; obj = lev.GetAllObjects();//��������������.�������� ��� ������� ��� �������������� ��������� � ������
		if (name == "Player1") {
			sprite.setTextureRect(IntRect(176, 0, w, h));
		}
	}

	float currentFrame = 0;
	void control(float time) {
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				state = left; speed = 0.1;
				currentFrame += 0.01*time;
				if (currentFrame > 6) currentFrame -= 6;
				if (int(currentFrame) == 6) sprite.setTextureRect(IntRect(199, 0, -w, h));
				else sprite.setTextureRect(IntRect(29 * int(currentFrame) + 29, 0, -w, h));
				
				
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				state = right; speed = 0.1;
				currentFrame += 0.01*time;
				if (currentFrame > 6) currentFrame -= 6;
				sprite.setTextureRect(IntRect(29 * int(currentFrame), 0, w, h));
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//���� ������ ������� ����� � �� �� �����, �� ����� �������
				state = jump; dy = -0.6; onGround = false;//�������� ������ ������
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down;
			}
			
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				isShoot = true;
			}

		}
		else {
			state = stay;
			sprite.setTextureRect(IntRect(172, 0, w, h));
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		/*������� �������������� �� ������ ������
		for (int i = y / 32; i < (y + h) / 32; i++)//���������� �� ��������� �����
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
		if (TileMap[i][j] == '0')
		{
		if (Dy>0){ y = i * 32 - h;  dy = 0; onGround = true; }
		if (Dy<0){ y = i * 32 + 32;  dy = 0; }
		if (Dx>0){ x = j * 32 - w; }
		if (Dx<0){ x = j * 32 + 32; }
		}
		}*/

		for (int i = 0; i<obj.size(); i++)//���������� �� ��������
			if (getRect().intersects(obj[i].rect))//��������� ����������� ������ � ��������
			{
				if (obj[i].name == "solid")//���� ��������� �����������
				{
					if (Dy>0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy<0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx>0) { x = obj[i].rect.left - w; }
					if (Dx<0) { x = obj[i].rect.left + obj[i].rect.width; }
				}
			}
	}


	float returnTime(float time) {
		return time;
	}

	void update(float time)
	{
		control(time);//������� ���������� ����������
		switch (state)//��� �������� ��������� �������� � ����������� �� ���������
		{
		case right:dx = speed; break;//��������� ���� ������
		case left:dx = -speed; break;//��������� ���� �����
		case up: break;//����� ��������� �������� ������ (�������� �� ��������)
		case down: dx = 0; break;//����� ��������� �� ����� ������ ��������� (�������� �� ��������)
		case stay: dx = 0; break;//� ����� ����		
		}
		x += dx*time;
		checkCollisionWithMap(dx, 0);//������������ ������������ �� �
		y += dy*time;
		checkCollisionWithMap(0, dy);//������������ ������������ �� Y
		sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
		if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//������� � ����� ������ ����������� � �����
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015*time;//��������� ������������� � �����
	}
};


class Enemy :public Entity {
public:
	Enemy(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetObjects("solid");//��������������.�������� ������ ������� ��� �������������� ����� � ������	if (name == "EasyEnemy") {
		sprite.setTextureRect(IntRect(0, 0, w, h));
		dx = 0.1;//���� ��������.���� ������ ������ ���������
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		/*������� �������������� �� ������ ������
		for (int i = y / 32; i < (y + h) / 32; i++)
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
		if (TileMap[i][j] == '0')
		{
		if (Dy>0){ y = i * 32 - h; }
		if (Dy<0){ y = i * 32 + 32; }
		if (Dx>0){ x = j * 32 - w; dx = -0.1; sprite.scale(-1, 1); }
		if (Dx<0){ x = j * 32 + 32; dx = 0.1; sprite.scale(-1, 1); }
		}
		}*/

		for (int i = 0; i<obj.size(); i++)//���������� �� ��������
			if (getRect().intersects(obj[i].rect))//��������� ����������� ������ � ��������
			{
				//if (obj[i].name == "solid"){//���� ��������� ����������� (������ � ������ solid)
				if (Dy>0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
				if (Dy<0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
				if (Dx>0) { x = obj[i].rect.left - w;  dx = -0.1; sprite.scale(-1, 1); }
				if (Dx<0) { x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; sprite.scale(-1, 1); }
				//}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy") {

			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//������ ����������� �������� ������ 3 ���
			checkCollisionWithMap(dx, 0);//������������ ������������ �� �
			x += dx*time;
			sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
			if (health <= 0) { life = false; }
		}
	}
};


/////////////////////// ����� ���������� ��������� //////////////////////////
class MovingPlatform : public Entity {//����� ���������� ���������
public:
	MovingPlatform(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		sprite.setTextureRect(IntRect(0, 34, W, H));//������������� 
		dx = 0.08;//����������� ��������� �� �
	}

	void update(float time)//������� ���������� ���������.
	{
		x += dx * time;//���������� �������� �� �����������
		moveTimer += time;//���������� ������
		if (moveTimer>2000) { dx *= -1; moveTimer = 0; }//���� ������ �������� 2 ���, �� �������� ����������� �������� ���������,� ������ ����������
		sprite.setPosition(x + w / 2, y + h / 2);//������ ������� �������
	}
};



//class MENU {
//public:
//	Sprite sprite;
//	MENU(Image &image, float X, float Y, int W, int H, String Name) {
//		sprite.setTextureRect(IntRect(0, 0, W, H));//�������������
//	}
//};



/////////////////////// ����� ����(��������) //////////////////////////
class Bullet :public Entity {//����� ����
public:
	int direction;//����������� ����

	Bullet(Image &image, String Name, Level &lvl, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H) {
		x = X;
		y = Y;
		direction = dir;
		speed = 0.8;
		w = h = 50;
		life = true;
	}


	void update(float time)
	{
		switch (direction)
		{
		case 0: dx = -speed; dy = 0;   break;
		case 1: dx = speed; dy = 0;   break;
		case 2: dx = 0; dy = -speed;   break;
		case 3: dx = 0; dy = -speed;   break;
		case 4: dx = 0; dy = -speed;   break;
		case 5: dx = 0; dy = -speed;   break;
		}

		x += dx*time;
		y += dy*time;

		//if (x <= 0) x = 1;
		//if (y <= 0) y = 1;

		sprite.setPosition(x + w / 2, y + h / 2);//�������� ������� ����
	}
};


int main()
{
	RenderWindow window(VideoMode(900, 400), "test");
	view.reset(FloatRect(0, 0, 800, 400));


	//����� �������� ������ �����
			/*
			Image map_image;
			map_image.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
			Texture map;
			map.loadFromImage(map_image);
			Sprite s_map;
			s_map.setTexture(map);
			*/
	
	Level lvl;//������� ��������� ������ �������
	lvl.LoadFromFile("newMap.tmx");//��������� � ���� �����, ������ ������ � ������� ������� �� �� ����������.

	Image heroImage;
	heroImage.loadFromFile("images\\player.png");
	heroImage.createMaskFromColor(Color(0, 0, 0));


	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images\\spider.png");
	easyEnemyImage.createMaskFromColor(Color(255, 255, 255));//����� �� �����


	Object player = lvl.GetObject("player");//������ ������ �� ����� �����.������ ���������� ������ � ������ ��� ������ ����
	//Object easyEnemyObject = lvl.GetObject("easyEnemy");//������ ������� ����� �� ����� �����.������ ���������� ������ � ������ ��� ������ ����

	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 30, 50);//�������� ���������� �������������� player �� ����� � ���������� ������ ������
	//Enemy easyEnemy(easyEnemyImage, "EasyEnemy", lvl, easyEnemyObject.rect.left, easyEnemyObject.rect.top, 80, 38);//�������� ���������� �������������� easyEnemy �� ����� � ���������� ������ �����

			//Player p(heroImage, 300, 200, 30, 50, "Player1");//������ ������ ������
			//Enemy easyEnemy(easyEnemyImage, 240, 386, 80, 38, "EasyEnemy");//������� ����, ������ ������ �����

	Image movePlatformImage;
	movePlatformImage.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
	//MovingPlatform movePlatform(movePlatformImage, "MovingPlatform", lvl, 650, 373, 50, 16);

	Image BulletImage;//����������� ��� ����
	BulletImage.loadFromFile("images\\bullet.png");//��������� �������� � ������ �����������
	BulletImage.createMaskFromColor(Color(0, 0, 0));


	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it; //���� ��� ����������� �� ��. ������
	std::list<Entity*>::iterator it2;//������ ��������.��� �������������� ����� ��������� ������

	std::vector<Object> e = lvl.GetObjects("EasyEnemy"); //��� ������� ����� EasyEnemy �� TMX �������� � ���� �������

	e = lvl.GetObjects("MovingPlatform"); // �������� ��� ��������� � ������

	for (int i(0); i < e.size(); i++) {
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 80, 38));
	}

	for (int i(0); i < e.size(); i++) {
		entities.push_back(new MovingPlatform(movePlatformImage, "MovingPlatform", lvl, e[i].rect.left, e[i].rect.top, 50, 16));
	}



	Clock clock;

	//Image menuButtonsImage;
	//menuButtonsImage.loadFromFile("images\\menuButtons.png");
	//menuButtonsImage.createMaskFromColor(Color(0, 0, 0));
	//MENU mainMenu(menuButtonsImage, 300, 200, 150, 50, "playButton");


	while (window.isOpen())
	{

		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;

		//����� ��������� ������� ��� �� � ������� � � ����� ���� ���� � ������� it
		/*if ((movePlatform.name== "MovingPlatform") && (movePlatform.getRect().intersects(p.getRect())))//���� ����� ���������� � �������� ������ � ��� ����� ������� movingplatform
		{
			if ((p.dy>0) || (p.onGround == false))//��� ���� ����� ��������� � ��������� ����� ������, �.� ������ ����
				if (p.y + p.h<movePlatform.y + movePlatform.h)//���� ����� ��������� ���� ���������, �.� ��� ��� ���� ������� (�� �� ��� ��������� ��� �� ���������� � ����������)
				{
					p.y = movePlatform.y - p.h + 3; p.x += movePlatform.dx*time; p.dy = 0; p.onGround = true; // �� ����������� ������ ���, ����� �� ��� �� ����� �� ���������
				}
		}
		*/

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();						
			//if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, "Bullet",lvl, p.x, p.y, 16, 16, p.state)); }//���� ����������, �� ���������� ����
		}
			

		p.update(time);// Player update function	
		

		/*
		for (it = entities.begin(); it != entities.end(); it++) {
			(*it)->update(time); // �������� ����. update ��� ���� ��. ������
		}
		*/
		
		for (it = entities.begin(); it != entities.end();)
		{
			Entity *b = *it;
			b->update(time);  //for (it = entities.begin(); it != entities.end(); it++) {(*it)->update(time); // �������� ����. update ��� ���� ��. ������
			if (b->life == false) { it = entities.erase(it); delete b; }
			else it++;
		}
		

		for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
		{
			if (((*it)->name == "MovingPlatform") && ((*it)->getRect().intersects(p.getRect())))//���� ����� ���������� � �������� ������ � ��� ����� ������� movingplatform
			{
				Entity *movPlat = *it;
				if ((p.dy>0) || (p.onGround == false))//��� ���� ����� ��������� � ��������� ����� ������, �.� ������ ����
					if (p.y + p.h<movPlat->y + movPlat->h)//���� ����� ��������� ���� ���������, �.� ��� ��� ���� ������� (�� �� ��� ��������� ��� �� ���������� � ����������)
					{
						p.y = movPlat->y - p.h + 3; p.x += movPlat->dx*time; p.dy = 0; p.onGround = true; // �� ����������� ������ ���, ����� �� ��� �� ����� �� ���������
					}
			}

			if ((*it)->getRect().intersects(p.getRect()))//���� ������������� ������� ������� ������������ � �������
			{
				if ((*it)->name == "EasyEnemy") {//� ��� ���� ��� ������� EasyEnemy,��..
					if ((*it)->dx>0)//���� ���� ���� ������
					{
						std::cout << "(*it)->x" << (*it)->x << "\n";//����� ������
						std::cout << "p.x" << p.x << "\n\n";//����� �����

						(*it)->x = p.x - (*it)->w; //����������� ��� �� ������ ����� (�������)
						(*it)->dx = 0;//�������������

						std::cout << "new (*it)->x" << (*it)->x << "\n";//����� ����� �����
						std::cout << "new p.x" << p.x << "\n\n";//����� ����� ������ (��������� �������)
					}
					if ((*it)->dx < 0)//���� ���� ���� �����
					{
						(*it)->x = p.x + p.w; //���������� - ����������� ������
						(*it)->dx = 0;//�������������
					}
					///////������������ ������
					if (p.dx < 0) { p.x = (*it)->x + (*it)->w; }//���� ����������� � ������ � ����� ���� ����� �� ����������� ������
					if (p.dx > 0) { p.x = (*it)->x - p.w; }//���� ����������� � ������ � ����� ���� ������ �� ����������� ������
				}
			}
			for (it2 = entities.begin(); it2 != entities.end(); it2++) {
				if ((*it)->getRect() != (*it2)->getRect())//��� ���� ��� ������ ���� ������ ��������������
					if (((*it)->getRect().intersects((*it2)->getRect())) && ((*it)->name == "EasyEnemy") && ((*it2)->name == "EasyEnemy"))//���� ����������� ��� ������� � ��� �����
					{
						(*it)->dx *= -1;//������ ����������� �������� �����
						(*it)->sprite.scale(-1, 1);//�������� ������ �� �����������
					}
			}
		}

						//   ^
						/*   |  
						for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
						{
							if ((*it)->getRect().intersects(p.getRect()))//���� ������������� ������� ������� ������������ � �������
							{
								if ((*it)->name == "easyEnemy") {//� ��� ���� ��� ������� EasyEnemy,��..

									if ((p.dy>0) && (p.onGround == false)) { (*it)->dx = 0; p.dy = -0.2; (*it)->health = 0; }//���� �������� �� �����,�� ���� ����� �������� 0,����������� �� ���� ���� �����,���� ��� �������� 0
									else {
										p.health -= 5;	//����� ���� ������� � ��� ����� � ����� ����
									}
								}
							}
						}
						*/

	

		//movePlatform.update(time);
		//easyEnemy.update(time);//easyEnemy update function
		window.setView(view);
		window.clear(Color(77, 83, 140));
		lvl.Draw(window);//������ ����� �����

		//�������� ����� ������ ����������
							/*for (int i = 0; i < HEIGHT_MAP; i++)
								for (int j = 0; j < WIDTH_MAP; j++)
								{
									if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 17, 17));
									//if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
									if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(576, 175, 17, 17));
									//if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
									//if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
									if ((TileMap[i][j] == 'd')) s_map.setTextureRect(IntRect(0, 17, 17, 17));
									//if ((TileMap[i][j] == 'p')) s_map.setTextureRect(IntRect(0, 34, 17, 17));
									s_map.setPosition(j * 17, i * 17);

									window.draw(s_map);
								}
							*/
			
		
			//window.draw(easyEnemy.sprite);

		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite); //������ ���� ������ ������� ��������� � ������
		}

		window.draw(p.sprite);
		//window.draw(movePlatform.sprite);
		window.display();
	}
	return 0;
}