#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <conio.h> 
#include <cstring> 
#include <cstdlib> 
#include <string> 
#include <Windows.h>

const int directionToLeft = 0;
const int directionToRight = 1;

struct Screen;
struct Player;
struct Enemy;
struct Bullet;
struct Bullets;

struct Screen {
	int		size;
	char* canvas;
	Screen(unsigned int size)
	{
		if (size == 0) size = 80;
		this->size = size;
		canvas = new char[size + 1];
	}
	void clear()
	{
		memset(canvas, ' ', size);
	}
	void draw(int pos, const char* face)
	{
		strncpy(&(canvas[pos]), face, strlen(face));
	}
	void draw(int pos, char face)
	{
		if (pos < 0 || pos >= size) return;
		canvas[pos] = face;
	}
	void render()
	{
		canvas[size] = '\0';
		printf("%s\r", canvas);
	}
	bool isInRange(Bullet* bullet);
	int getSize()
	{
		return size;
	}
	~Screen()
	{
		delete[] canvas;
		canvas = nullptr;
		size = 0;
	}
};
struct Player {
	char	initFace[20];
	char	face[20];
	char	smileFace[20];
	char	deadFace[20];
	char	hpPrinter[8];
	int		pos;
	int		maxHp;
	int		hp;
	int		nHitRemaining;
	int		nHpRemaining;
	int		nFlickerInterval;

	Player(const char* face, int pos, int hp)
	{
		strcpy(initFace, face);
		strcpy(this->face, initFace);
		strcpy(smileFace, "(^_^)");
		strcpy(deadFace, "(X_X)");
		this->pos = pos;
		maxHp = hp;
		this->hp = maxHp;
		sprintf(hpPrinter, "%d", this->hp);
		nHitRemaining = 0;
		nHpRemaining = 0;
		nFlickerInterval = 0;
	}
	void update(bool* isLooping)
	{
		if (isDie() == true)
		{
			strcpy(face, deadFace);
			*isLooping = false;
			return;
		}
		checkFlicker();
		checkHit();
		if (nHpRemaining == 0) return;
		--nHpRemaining;
	}
	void draw(Screen* screen)
	{
		screen->draw(pos, face);
		if (nHpRemaining > 0)
		{
			snprintf(hpPrinter, 8, "%d", hp);
			screen->draw(pos - strlen(hpPrinter), hpPrinter);
		}
		if (isHpLack() == false || nFlickerInterval != 0) return;
		//isHpLack==true, nHpFlickerInterval==0 �϶��� ������
		for (int i = 0;i < strlen(face);i++)
			screen->draw(pos + i, ' ');
	}
	void move(int direction)
	{
		(direction == directionToLeft) ? --pos : ++pos;
	}
	void fire(Bullets* bullets, Enemy* enemy);
	void onEnemyHit()
	{
		strcpy(face, smileFace);
		nHitRemaining = 30;
	}
	bool isBeHit(Enemy* enemy);
	void beHit()
	{
		nHpRemaining = 5;//HPǥ�� �ð� : 0.5��
		if (hp == 0) return;
		hp -= 1;
	}
	void checkHit()
	{
		if (nHitRemaining == 0) return;
		--nHitRemaining;
		if (nHitRemaining == 0) strcpy(this->face, initFace);
	}
	bool isHpLack()
	{
		return ((hp <= 10) ? true : false);
	}
	void checkFlicker()
	{
		if (isHpLack() == false) return;
		if (nFlickerInterval == 0)
			nFlickerInterval = hp + 1;
		else
			--nFlickerInterval;
	}
	bool isDie()
	{
		return ((hp <= 0) ? true : false);
	}
	int  getPos()
	{
		return pos;
	}
	const char* getFace()
	{
		return face;
	}
	~Player()
	{}
};
struct Enemy {
	char	initFace[20];
	char	face[20];
	char	cryFace[20];
	char	deadFace[20];
	char	hpPrinter[8];
	int		initPos;//���� ��Ȱ ���� ó�� ��ġ ����
	int		pos;
	int		maxHp;//���� ��Ȱ ���� �ִ� ü�� ����
	int		hp;
	int		nBeHitRemaining;
	int		nHpRemaining;
	int		nDieRemaining;
	int     nMovementInterval;
	int		nReSpawnInterval;

	Enemy(const char* face, int pos, int hp)
	{
		strcpy(initFace, face);
		strcpy(this->face, initFace);
		strcpy(cryFace, "(T_#)");
		strcpy(deadFace, "(X_#)");
		initPos = pos;
		this->pos = initPos;
		maxHp = hp;
		this->hp = maxHp;
		snprintf(hpPrinter, 8, "%d", this->hp);
		nBeHitRemaining = 0;
		nHpRemaining = 0;
		nDieRemaining = 0;
		nMovementInterval = 0;
		nReSpawnInterval = 0;
	}
	void update(Player* player)
	{
		if (isDie() == true)
		{
			checkReSpawn();
			if (nDieRemaining == 0) return;
			--nDieRemaining;
			if (nDieRemaining == 0) pos = 0;
			return;
		}
		chase(player);
		checkBeHit();
		if (player->isBeHit(this) == true) player->beHit();
		if (nHpRemaining == 0) return;
		--nHpRemaining;
	}
	void draw(Screen* screen)
	{
		if (isDie() == true && nDieRemaining == 0) return;
		screen->draw(pos, face);
		if (nHpRemaining == 0) return;
		snprintf(hpPrinter, 8, "%d", this->hp);
		screen->draw(pos + strlen(face), hpPrinter);
	}
	void move(int direction)
	{
		direction == directionToLeft ? --pos : ++pos;
	}
	void chase(Player* player)
	{
		if (nMovementInterval > 0)
		{
			--nMovementInterval;
			return;
		}
		nMovementInterval = 10;
		int player_pos = player->getPos();
		const char* player_face = player->getFace();
		if (pos > player_pos + strlen(player_face)) move(directionToLeft); // �÷��̾� ��ġ + �÷��̾� ũ�� < ���� ��ġ�� ��
		else if (pos + strlen(face) < player_pos) move(directionToRight); // ���� ��ġ + ���� ũ�� < �÷��̾� ��ġ�� ��
		else return;//�� ���̿� ������, �� �÷��̾�� ���Ͱ� �������� �� �̵����� ����.
	}
	bool isBeHit(Bullet* bullet);
	void beHit(Bullet* bullet);
	void checkBeHit()
	{
		if (nBeHitRemaining == 0) return;
		--nBeHitRemaining;
		if (nBeHitRemaining == 0) strcpy(this->face, initFace);
	}
	bool isDie()//���� ��� �Ǻ�
	{
		return (hp <= 0 ? true : false);
	}
	void die()//���Ͱ� ���� ��� : ��� ��Ȱ �ð� �ο� �� ü�� �� �ʱ�ȭ
	{
		nReSpawnInterval = (rand() % 6 + 5) * 10; // ��� ��Ȱ �ð� : 5~10��
		nDieRemaining = 5; //��� ��� �ð� : 0.5��
		hp = 0;
		strcpy(face, deadFace);
	}
	void reSpawn()//���� ��Ȱ : ó�� �����ߴ� ��ġ,ü�� ���� �ٽ� ����
	{
		strcpy(this->face, initFace);
		pos = initPos;
		hp = maxHp;
		nHpRemaining = 0;
	}
	void checkReSpawn()
	{
		if (isDie() == false) return;
		//������
		--nReSpawnInterval;
		if (nReSpawnInterval == 0) reSpawn();

	}
	int  getPos()
	{
		return pos;
	}
	const char* getFace()
	{
		return face;
	}
	~Enemy()
	{}
};
struct Bullet {
	bool	isReady;
	bool	isFire;
	int		pos;
	int		direction;

	Bullet()
	{
		isReady = true;
		isFire = false;
		pos = 0;
		direction = directionToLeft;
	}
	void update(Player* player, Enemy* enemy, Screen* screen)
	{
		if (isFire == false) return;

		move();
		if (enemy->isBeHit(this) == false && screen->isInRange(this) == false) //�Ѿ��� �ȸ°�, ȭ�� �ۿ� ���� ���
		{
			isFire = false;//�߻���¸� �����ϰ� ����
			return;
		}
		if (enemy->isBeHit(this) == false) return; //�Ѿ��� �ȸ��� ��� ����
		//�Ѿ��� ������ ���� ���
		enemy->beHit(this);
		player->onEnemyHit();
		isFire = false;
		//���Ͱ� ������ �Ѿ��� isFire�� true�� ��� �ѹ��� �������� �԰� �ϱ� ���� (isFire�� ���� �ٲٸ� enemy->onHit���� ���� �浹�� �������� ����)
	}
	void draw(Screen* screen)
	{
		if (isFire == false) return;
		screen->draw(pos, '-');
	}
	void move()
	{
		(direction == directionToLeft) ? --pos : ++pos;
	}
	void setFire(Player* player, Enemy* enemy)
	{
		isReady = false;
		isFire = true;
		// direction ����
		direction = directionToRight;
		//if (enemy->isDie() == true) return;
		int enemy_pos = enemy->getPos();
		int player_pos = player->getPos();
		const char* player_face = player->getFace();
		if (player_pos > enemy_pos && enemy_pos!=0) direction = directionToLeft;
		// bullet position ����
		pos = player_pos;
		if (direction == directionToRight) pos += (strlen(player_face) - 1);
	}
	void reuse()
	{
		isReady = true;
	}
	int  getPos()
	{
		return pos;
	}
	int  getDirection()
	{
		return direction;
	}
	bool isAvailable()
	{
		return isReady;
	}
	bool isFiring()
	{
		return isFire;
	}
	~Bullet()
	{}
};
struct Bullets {
	int		nBullets; //�� źâ�� �Ѿ��� ����: 5��
	int		nReloadRemaining;
	int		nReloadInterval;
	Bullet* allBullets[3];//źâ�� ����:3��
	Bullet* myBullets;

	Bullets(unsigned int nBullets)
	{
		if (nBullets == 0) nBullets = 5;
		this->nBullets = nBullets;
		nReloadRemaining = 0;
		nReloadInterval = 100; //�Ѿ� ���� �ð� : 10��
		for (int i = 0;i < sizeof(allBullets) / sizeof(Bullet*);i++)
			allBullets[i] = new Bullet[nBullets];
		myBullets = new Bullet[nBullets];
		myBullets = allBullets[0];
	}
	void update(Player* player, Enemy* enemy, Screen* screen)
	{
		checkReload();
		for (int i = 0;i < sizeof(allBullets) / sizeof(Bullet*);i++)
		{
			for (int j = 0; j < nBullets; j++)
			{
				Bullet* bullet = &allBullets[i][j];
				bullet->update(player, enemy, screen);
			}
		}
		if (nReloadRemaining == 0) return;
		--nReloadRemaining;
	}
	void draw(Screen* screen)
	{
		for (int i = 0;i < sizeof(allBullets) / sizeof(Bullet*);i++)
		{
			for (int j = 0; j < nBullets; j++)
			{
				Bullet* bullet = &allBullets[i][j];
				bullet->draw(screen);
			}
		}
		if (nReloadRemaining == 0) return;
		int screen_size = screen->getSize();
		screen->draw(screen_size / 2, "źâ ��ü");
	}
	void reload()
	{
		if (find_unused_bullets() == nullptr) return;
		nReloadRemaining = 10;//źâ ��ü �޽��� �ð� : 1��
		nReloadInterval = 100;//źâ�� �غ�Ǵ� �ð� : 10��
		Bullet* spareBullets= find_unused_bullets();
		for (int i = 0; i < nBullets; i++)//���� �� �� źâ �ʱ�ȭ
		{
			Bullet* bullet = &myBullets[i];
			bullet->reuse();
		}
		myBullets = spareBullets;
	}
	void checkReload()
	{
		Bullet* firstBullet = &myBullets[0];
		if (firstBullet->isAvailable() == true) return;
		if (nReloadInterval > 0)
		{
			--nReloadInterval;
			return;
		}
		for (int i = 1; i < nBullets; i++)
		{
			Bullet* bullet = &myBullets[i];
			if (bullet->isAvailable() == true) return;
		}
		reload();
	}
	Bullet* find_unused_bullet()
	{
		if (myBullets == nullptr) return nullptr;
		for (int i = 0; i < nBullets; i++)
		{
			Bullet* bullet = &myBullets[i];
			if (bullet->isAvailable() == true && bullet->isFiring() == false)
				return bullet;
		}
		return nullptr;
	}
	Bullet* find_unused_bullets()
	{
		for (int i = 0; i < sizeof(allBullets) / sizeof(Bullet*); i++)
		{
			Bullet* firstBullet = &allBullets[i][0];//źâ�� ù��° �Ѿ� ���¸� Ȯ���ϸ� �̻�� źâ�� �� �� ����
			if (firstBullet->isFiring() == false && firstBullet->isAvailable() == true)
				return allBullets[i];//i��° źâ�� �ּҸ� ��ȯ (źâ�� �ּ�==ù��° �Ѿ��� �ּ��̹Ƿ�)
		}
		return nullptr;
	}
	~Bullets()
	{
		delete[] myBullets;
		myBullets = nullptr;
		for (int i = 0;i < sizeof(allBullets) / sizeof(Bullet*);i++)
		{
			delete[] allBullets[i];
			allBullets[i] = nullptr;
		}
	}
};
bool Screen::isInRange(Bullet* bullet)
{
	int bullet_pos = bullet->getPos();
	return bullet_pos >= 0 && bullet_pos < size;
}
void Player::fire(Bullets* bullets, Enemy* enemy)
{
	Bullet* bullet = bullets->find_unused_bullet();
	if (bullet == nullptr) return;
	bullet->setFire(this, enemy);
}
bool Player::isBeHit(Enemy* enemy)
{
	int enemy_pos = enemy->getPos();
	const char* enemy_face = enemy->getFace();
	return ((pos + strlen(face) >= enemy_pos) && (pos <= enemy_pos + strlen(enemy_face)));
}
bool Enemy::isBeHit(Bullet* bullet)
{
	if (isDie() == true) return false;//���Ͱ� �׾��� �� �Ѿ˿� ���� ����
	int bullet_direction = bullet->getDirection();
	int bullet_pos = bullet->getPos();
	return (
		(bullet_direction == directionToLeft && pos + strlen(face) - 1 >= bullet_pos) //==�� �� ���Ϳ� �Ѿ��� ��ġ�� ���ÿ� �ٲ�� �������� >=,<=�� �ٲ� 
		|| (bullet_direction == directionToRight && pos <= bullet_pos));
}
void Enemy::beHit(Bullet* bullet)
{
	if (bullet->isFiring() == false || isDie() == true) return;
	hp -= 5;//�Ѿ� 1���� ������ : 5
	strcpy(face, cryFace);
	nHpRemaining = 5;//���� Hp�� ǥ�� �ð� : 0.5��
	nBeHitRemaining = 10;
	if (isDie() == false) return;
	die();//���Ͱ� �Ѿ˿� �¾��� �� hp�� 0 ���ϰ� �Ǹ� ���
}

int main()
{
	int major;
	int minor;

	Screen  screen(80);
	Player	player("(-_-)", 0, 50);
	Enemy	enemy("(`_#)", 75, 50);
	Bullets bullets(5);

	// game loop
	bool isLooping = true;
	while (isLooping)
	{
		screen.clear();

		player.update(&isLooping);
		enemy.update(&player);
		bullets.update(&player, &enemy, &screen);

		player.draw(&screen);
		enemy.draw(&screen);
		bullets.draw(&screen);

		screen.render();
		Sleep(100);
		if (!_kbhit()) continue;
		major = _getch();
		switch (major) {
		case 'q':
			isLooping = false;
			break;

		case ' ':
			player.fire(&bullets, &enemy);
			break;
		case 224: // arrow key, function key pressed
			minor = _getch();
			switch (minor) {
			case 75: // left
				player.move(directionToLeft);
				break;
			case 77: // right
				player.move(directionToRight);
				break;
			}
			break;
		}
	}
	Sleep(500);
	screen.clear();
	screen.draw((screen.size + strlen("Game Over")) / 2, "Game Over");
	screen.render();
	return 0;
}