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


////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
	std::vector<Object> obj;//вектор объектов карты
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

	FloatRect getRect() {//ф-ция получения прямоугольника. его коорд,размеры (шир,высот).
		return FloatRect(x, y, w, h);//эта ф-ция нужна для проверки столкновений 
	}

	virtual void update(float time) = 0; //все потомки переопределяют эту функцию
};
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;
	int playerScore;
	bool isShoot;
	Player(Image &image, String Name, Level &lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		playerScore = 0; state = stay; obj = lev.GetAllObjects();//инициализируем.получаем все объекты для взаимодействия персонажа с картой
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

			if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
				state = jump; dy = -0.6; onGround = false;//увеличил высоту прыжка
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
		/*вариант взаимодействия со старой картой
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
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

		for (int i = 0; i<obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if (obj[i].name == "solid")//если встретили препятствие
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
		control(time);//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
		case right:dx = speed; break;//состояние идти вправо
		case left:dx = -speed; break;//состояние идти влево
		case up: break;//будет состояние поднятия наверх (например по лестнице)
		case down: dx = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
		case stay: dx = 0; break;//и здесь тоже		
		}
		x += dx*time;
		checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		y += dy*time;
		checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
		if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//убираем и будем всегда притягивать к земле
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015*time;//постоянно притягиваемся к земле
	}
};


class Enemy :public Entity {
public:
	Enemy(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetObjects("solid");//инициализируем.получаем нужные объекты для взаимодействия врага с картой	if (name == "EasyEnemy") {
		sprite.setTextureRect(IntRect(0, 0, w, h));
		dx = 0.1;//даем скорость.этот объект всегда двигается
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		/*вариант взаимодействия со старой картой
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

		for (int i = 0; i<obj.size(); i++)//проходимся по объектам
			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				//if (obj[i].name == "solid"){//если встретили препятствие (объект с именем solid)
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

			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx*time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
			if (health <= 0) { life = false; }
		}
	}
};


/////////////////////// КЛАСС ДВИЖУЩЕЙСЯ ПЛАТФОРМЫ //////////////////////////
class MovingPlatform : public Entity {//класс движущейся платформы
public:
	MovingPlatform(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		sprite.setTextureRect(IntRect(0, 34, W, H));//прямоугольник 
		dx = 0.08;//изначальное ускорение по Х
	}

	void update(float time)//функция обновления платформы.
	{
		x += dx * time;//реализация движения по горизонтали
		moveTimer += time;//наращиваем таймер
		if (moveTimer>2000) { dx *= -1; moveTimer = 0; }//если прошло примерно 2 сек, то меняется направление движения платформы,а таймер обнуляется
		sprite.setPosition(x + w / 2, y + h / 2);//задаем позицию спрайту
	}
};

//class MENU {
//public:
//	Sprite sprite;
//	MENU(Image &image, float X, float Y, int W, int H, String Name) {
//		sprite.setTextureRect(IntRect(0, 0, W, H));//прямоугольник
//	}
//};



/////////////////////// КЛАСС ПУЛИ(СТРЕЛЬБЫ) //////////////////////////
class Bullet :public Entity {//класс пули
public:
	int direction;//направление пули

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

		sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
	}
};


int main()
{
	RenderWindow window(VideoMode(900, 400), "test");
	view.reset(FloatRect(0, 0, 800, 400));


	//метод загрузки старой карты
			/*
			Image map_image;
			map_image.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
			Texture map;
			map.loadFromImage(map_image);
			Sprite s_map;
			s_map.setTexture(map);
			*/
	
	Level lvl;//создали экземпляр класса уровень
	lvl.LoadFromFile("newMap.tmx");//загрузили в него карту, внутри класса с помощью методов он ее обработает.

	Image heroImage;
	heroImage.loadFromFile("images\\player.png");
	heroImage.createMaskFromColor(Color(0, 0, 0));


	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images\\spider.png");
	easyEnemyImage.createMaskFromColor(Color(255, 255, 255));//маску по цвету


	Object player = lvl.GetObject("player");//объект игрока на нашей карте.задаем координаты игроку в начале при помощи него
	//Object easyEnemyObject = lvl.GetObject("easyEnemy");//объект легкого врага на нашей карте.задаем координаты игроку в начале при помощи него

	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 30, 50);//передаем координаты прямоугольника player из карты в координаты нашего игрока
	//Enemy easyEnemy(easyEnemyImage, "EasyEnemy", lvl, easyEnemyObject.rect.left, easyEnemyObject.rect.top, 80, 38);//передаем координаты прямоугольника easyEnemy из карты в координаты нашего врага

			//Player p(heroImage, 300, 200, 30, 50, "Player1");//объект класса игрока
			//Enemy easyEnemy(easyEnemyImage, 240, 386, 80, 38, "EasyEnemy");//простой враг, объект класса врага

	Image movePlatformImage;
	movePlatformImage.loadFromFile("images\\Tiny_Chao_Garden_SonicAdv_3_Tile_Sheet.png");
	MovingPlatform movePlatform(movePlatformImage, "MovingPlatform", lvl, 650, 373, 50, 16);

	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images\\bullet.png");//загрузили картинку в объект изображения
	BulletImage.createMaskFromColor(Color(0, 0, 0));


	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it; //итер для прохождения по эл. списка
	std::list<Entity*>::iterator it2;//второй итератор.для взаимодействия между объектами списка

	std::vector<Object> e = lvl.GetObjects("easyEnemy"); //все объекты врака EasyEnemy на TMX хранятся в этом векторе
	for (int i(0); i < e.size(); i++) {
		entities.push_back(new Enemy(easyEnemyImage, "easyEnemy", lvl, e[i].rect.left, e[i].rect.top, 80, 38));
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


		if ((movePlatform.name== "MovingPlatform") && (movePlatform.getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
		{
			if ((p.dy>0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
				if (p.y + p.h<movePlatform.y + movePlatform.h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
				{
					p.y = movePlatform.y - p.h + 3; p.x += movePlatform.dx*time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
				}
		}




		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();						
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, "Bullet",lvl, p.x, p.y, 16, 16, p.state)); }//если выстрелили, то появляется пуля
		}

		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite); //рисуем всех врагов которые находятся в списке
		}

		for (it = entities.begin(); it != entities.end(); it++) {
			(*it)->update(time); // вызываем функ. update для всех эл. списка
		}

		for (it = entities.begin(); it != entities.end();)
		{
			Entity *b = *it;
			b->update(time);
			if (b->life == false) { it = entities.erase(it); delete b; }
			else it++;
		}

		p.update(time);// Player update function	
		movePlatform.update(time);
		//easyEnemy.update(time);//easyEnemy update function
		window.setView(view);
		window.clear(Color(77, 83, 140));
		lvl.Draw(window);//рисуем новую карту

		//создание карты старая реализация
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
		window.draw(p.sprite);
		window.draw(movePlatform.sprite);
		window.display();
	}
	return 0;
}