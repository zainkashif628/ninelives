#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define RAYGUI_IMPLEMENTATION
#define MYPURPLE Color {22,0,34, NULL}
#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 990.0f
#define G 1400.0f
#define PLAYER_HOR_SPEED 400.0f
#define PLAYER_JUMP_SPEED 850.0f
#define ENEMY_JUMP_SPEED 850.0f
#define BULLET_SPEED 800.0f

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <raygui.h>

Sound sounds[5];
int channels[5] = { 0, 0, 0, 0, 0 };
Music main_music;

struct Sardines {
	float x;
	float y;
	Texture img;
};

struct Object{
	Rectangle rect;
	bool broken;
	Color color;
};

struct Bullet {
	float x;
	float y;
	float radius;
	bool hit;
	bool fired;
	Color color;
	int direction;
};

struct Player {
	Texture img;
	float x;
	float y;
	Vector2 speed;
	bool can_jump;
	Rectangle bounds;
	Vector2 start;
	bool gun;
	int state;
	bool alive;
};

struct Enemy {
	Texture img;
	float x;
	float y;
	Vector2 speed;
	bool can_jump;
	bool alive;
	Rectangle bounds;
	int state;
	Vector2 start;
};

struct Button {
	Rectangle bounds;
	Texture img;
};


void InitPlayer(Player*, float, float, int, bool);
void InitEnemy(Enemy*, float, float, int);
void PlayDaSound(int);
void UpdatePlayer(Player*, Object*, int, Bullet*);
void UpdateEnemy(Enemy*, Player*, Object*, int, Bullet*);
void UpdateBullet(Bullet*, Player*, Object*, int);
void UpdatePauseButton(bool* pause);
void win_screen();
void InitButton(Button, int, int);
void cheat();
void LifeLost(Player*);
void DisplayLives();
void DrawBorderedText(const char*, int, int, int, Color, int, Color);
void menu(Music, Button, Button, Button, Button, Texture, Texture);
void level1(Player*, Object[], int, Bullet*);
void level2(Player*, Object[], Enemy*, int, Bullet*);
void level3(Player*, Object[], Enemy*, int, Bullet*);
void level4(Player*, Object[], Enemy*, Enemy*, Enemy*, Enemy*, int, Bullet*, Texture);
void level5(Player*, Object[], Enemy*, Enemy*, Enemy*, Enemy*, int, Bullet*);
void level6(Player*, Object[], Enemy*, Enemy*, Enemy*, int, Bullet*, Texture);
void level7(Player*, Object[], Enemy[], int, Bullet*, Texture);
//void level6(Player*, Object[], Enemy*, int, Bullet*);
//void level9(Player*, Object[], Enemy*, int, Bullet*);


bool is_time_pause = true;
int level = 0;
int lives = 9;
Texture cat_textures[6];
Texture enemy_textures[8];
float startTime = GetTime();
float elapsedTime = 0.0f;
float paused_time = 0.0f;
float randoms[100];
Texture platform;
Texture death_screen;
Texture pausebg;
Texture lava;
Texture bg;
Texture heart;
Image icon;
Sardines sardines;

int main() {
	
	for (int i = 0; i < 100; i++) randoms[i] = (float)((int)(rand()) % 900);

	srand(time(NULL));
	InitWindow(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2, "Nine Lives");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	MaximizeWindow();

	SetTargetFPS(60);
	InitAudioDevice();
	main_music = LoadMusicStream("assets/y2mate.com - The Cat from Ipanema.mp3");
	PlayMusicStream(main_music);
	GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0xAA5588FF);

	//menu assets
	Button playbutton, controlsbutton, exitbutton, backbutton;
	playbutton.img = LoadTexture("assets/playbutton.png");
	controlsbutton.img = LoadTexture("assets/controlsbutton.png");
	exitbutton.img = LoadTexture("assets/exitbutton.png");
	backbutton.img = LoadTexture("assets/backbutton.png");
	playbutton.bounds = { 280, SCREEN_HEIGHT / 2 - 30, (float)playbutton.img.width + 29, (float)playbutton.img.height + 29 };
	controlsbutton.bounds = { 275, SCREEN_HEIGHT / 2 + 110, (float)controlsbutton.img.width + 29, (float)controlsbutton.img.height + 29 };
	exitbutton.bounds = { 280, SCREEN_HEIGHT / 2 + 250, (float)exitbutton.img.width + 29, (float)exitbutton.img.height + 29 };
	backbutton.bounds = { (float)SCREEN_WIDTH - 100 - backbutton.img.width, 100, (float)backbutton.img.width + 29, (float)backbutton.img.height + 29 };
	Music music = LoadMusicStream("assets/MAIN MENU MUSIC.mp3");
	Texture menubg = LoadTexture("assets/menubg.png");
	Texture controlsbg = LoadTexture("assets/controlsbg.png");

	//game assets
	heart = LoadTexture("assets/heart.png");
	icon = LoadImage("assets/icon.png");
	platform = LoadTexture("assets/platform.png");
	bg = LoadTexture("assets/lvl1bg1.png");
	lava = LoadTexture("assets/lava.png");
	sardines.img = LoadTexture("assets/sardines4.png");
	pausebg = LoadTexture("assets/pausebg.png");
	death_screen = LoadTexture("assets/death.png");
    cat_textures[0] = LoadTexture("assets/cat idle left no gun.png");
    cat_textures[1] = LoadTexture("assets/cat idle right no gun.png");
    cat_textures[2] = LoadTexture("assets/cat idle left gun.png");
    cat_textures[3] = LoadTexture("assets/cat idle right gun.png");
    cat_textures[4] = LoadTexture("assets/flower cat left.png");
    cat_textures[5] = LoadTexture("assets/flower cat right.png");
    enemy_textures[0] = LoadTexture("assets/blue monster left.png");
    enemy_textures[1] = LoadTexture("assets/blue monster right.png");
    enemy_textures[2] = LoadTexture("assets/yellow monster left.png");
    enemy_textures[3] = LoadTexture("assets/yellow monster right.png");
    enemy_textures[4] = LoadTexture("assets/green monster left.png");
    enemy_textures[5] = LoadTexture("assets/green monster right.png");
    enemy_textures[6] = LoadTexture("assets/purple monster left.png");
    enemy_textures[7] = LoadTexture("assets/purple monster right.png");
	Player player;
	Bullet bullet;
	bullet.hit = false;
	bullet.fired = false;
	bullet.radius = 5;
	bullet.color = LIGHTGRAY;

	SetWindowIcon(icon);

	//level 1
	InitPlayer(&player, cat_textures[1].width, SCREEN_HEIGHT - cat_textures[1].height - 30, 1, false);
	Object lvl1objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 }, false, WHITE}, //ceiling
		{{0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30},  false, WHITE},//floor
		{{0, 0, 30, SCREEN_HEIGHT},  false, WHITE},//left wall
		{{SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT},  false, WHITE},//right wall
		{{SCREEN_WIDTH / 2.0 - 50, SCREEN_HEIGHT / 2.0 - 70, 30, SCREEN_HEIGHT / 2.0 + 70},  false, WHITE},//middle obstacle
		{{SCREEN_WIDTH / 2.0 - 300, SCREEN_HEIGHT - 200, 250, 30},  false, WHITE},//first platform
		{{30, 2 * SCREEN_HEIGHT / 3 - 40, 600, 30},  false, WHITE},//second platform
		{{SCREEN_WIDTH / 8 + 30, SCREEN_HEIGHT / 2.0 - 70,SCREEN_WIDTH / 4 + 180, 30}, false, WHITE},
		{{SCREEN_WIDTH / 2 + 220, 2 * SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2 - 250, 30}, false, WHITE}
	};
	int lvl1_num_objects = 9;
	
	//level 2
	Object lvl2objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 },  false, WHITE },//ceiling
		{{ 0, SCREEN_HEIGHT - 30, SCREEN_WIDTH - 430, 30 }, false, WHITE},//floor
		{{ 0, 0, 30, SCREEN_HEIGHT }, false, WHITE},//left wall
		{{ SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT }, false, WHITE},//right wall
		{{ 300, 780, 500, 30 }, false, WHITE},//object
		{{ SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0 - 200, 100, 30 }, false, WHITE},//object
		{{ 100, 380, 500, 30 }, false, WHITE},//object
		{{ 1000, 580, 400, 30 }, false, WHITE},//object
		{{ 1000, 610, 30, SCREEN_HEIGHT - 580 }, false, WHITE},
		{{ 1370, 300, 30, 280 }, false, WHITE}
	};
	int lvl2_num_objects = 10;
	Enemy lvl2enemy1;
	InitEnemy(&lvl2enemy1, SCREEN_WIDTH / 2.0 + enemy_textures[0].width / 2.0, SCREEN_HEIGHT / 2.0 - 220 - enemy_textures[0].height / 2.0, 0);

	//level 3
	Object lvl3objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 },  false, WHITE},//ceiling
		{{0, SCREEN_HEIGHT - 30, 200, 30},  false, WHITE},//floor
		{{0, 0, 30, SCREEN_HEIGHT},  false, WHITE},//left wall
		{{SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT}, false, WHITE}, //right wall
		{ {460, 780, 40, 30},  false, WHITE },//first
		{ {800, 580, 40, 30},  false, WHITE },//second
		{ {400, 380, 40, 30},  false, WHITE },//third
		{ {1154, 750, 40, 30}, false, WHITE },
		{ {1330, 310, 30, 30}, false, WHITE },
		{ {1450, 600, 40, 30}, false, WHITE },
		{ {SCREEN_WIDTH - 280, 500, 250, 30},  false, WHITE },//enemy plat
		{ {SCREEN_WIDTH - 200, 230, 170, 30}, false, WHITE },//sardines plat
	};
	int lvl3_num_objects = 12;
	Enemy lvl3enemy1;
	InitEnemy(&lvl3enemy1, SCREEN_WIDTH - 150, 480.0f - float(enemy_textures->height) / 2, 0);

	//level 4
	Texture gun = LoadTexture("assets/gun.png");
	Object lvl4objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 },  false, WHITE},//ceiling
		{{0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30},  false, WHITE},//floor
		{{0, 0, 30, SCREEN_HEIGHT},  false, WHITE},//left wall
		{{SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT}, false, WHITE}, //right wall
		{{30, 400, 200, 30},  false, WHITE},//first
		{{300, 580, 400, 30},  false, WHITE},//second
		{{SCREEN_WIDTH / 2, 3 * SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2 - 30, 30}, false, WHITE}, //long
		{{SCREEN_WIDTH / 2 - 300, 3 * SCREEN_HEIGHT / 4, 30, SCREEN_HEIGHT / 4 - 30}, false, WHITE}, //stopper
		{{3 * SCREEN_WIDTH / 4 - 200, SCREEN_HEIGHT / 3 + 50, 200, 40},  false, WHITE},//before gun
		{{SCREEN_WIDTH / 2 - 270, SCREEN_HEIGHT - 60, 50, 30}, false, WHITE},
		{{SCREEN_WIDTH - 280, 550, 250, 30},  false, WHITE},
		{{SCREEN_WIDTH - 200, 230, 170, 30}, false, WHITE},
		{{30, 3*SCREEN_HEIGHT/4 + 50, 100, 30}, false, WHITE},
		{{SCREEN_WIDTH / 2, 3 * SCREEN_HEIGHT / 4 + 30, 30, SCREEN_HEIGHT / 4 - 60}, false, PINK}
	};
	int lvl4_num_objects = 14;
	Enemy lvl4enemy1, lvl4enemy2, lvl4enemy3, lvl4enemy4;
	InitEnemy(&lvl4enemy1, SCREEN_WIDTH - 150, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
	InitEnemy(&lvl4enemy2, SCREEN_WIDTH - 180, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
	InitEnemy(&lvl4enemy3, SCREEN_WIDTH - 210, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
	InitEnemy(&lvl4enemy4, 1400, SCREEN_HEIGHT / 3 - float(enemy_textures->height), 3);

	//level 5
	Object lvl5objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 },  false, WHITE},//ceiling
		{{0, SCREEN_HEIGHT - 30, 200, 30},  false, WHITE},//floor
		{{0, 0, 30, SCREEN_HEIGHT},  false, WHITE},//left wall
		{{SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT}, false, WHITE}, //right wall
		{ {460, 780, 200, 30},  false, WHITE },//first
		{ {700, 580, 200, 30},  false, WHITE },//second
		{ {300, 380, 300, 30},  false, WHITE },//third
		{ {1000, 300, 100, 30}, false, WHITE },
		{ {800, SCREEN_HEIGHT-100, 400, 30}, false, WHITE },
		{ {1330, 310, 400, 30}, false, WHITE },
		{ {1200, 600, 200, 30}, false, WHITE },
		{ {SCREEN_WIDTH - 280, 700, 250, 30},  false, WHITE },//enemy plat
		{ {SCREEN_WIDTH - 200, 230, 170, 30}, false, WHITE },//sardines plat
	};
	int lvl5_num_objects = 13;
	Enemy lvl5enemy1, lvl5enemy2, lvl5enemy3, lvl5enemy4;
	InitEnemy(&lvl5enemy1, 320, 380 - float(enemy_textures->height)/2, 2);
	InitEnemy(&lvl5enemy2, 1050, SCREEN_HEIGHT - 120 - float(enemy_textures->height) / 2, 3);
	InitEnemy(&lvl5enemy3, 1000, 320 - float(enemy_textures->height)/2, 2);
	InitEnemy(&lvl5enemy4, 500, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 3);

	//level 6
	Texture dark = LoadTexture("assets/dark.png");
	Object lvl6objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 }, false, WHITE}, //ceiling
		{{0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30},  false, WHITE},//floor
		{{0, 0, 30, SCREEN_HEIGHT},  false, WHITE},//left wall
		{{SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT},  false, WHITE},//right wall
		{{SCREEN_WIDTH / 2.0 - 50, SCREEN_HEIGHT / 2.0 - 70, 30, SCREEN_HEIGHT / 2.0 + 70},  false, WHITE},//middle obstacle
		{{SCREEN_WIDTH / 2.0 - 300, SCREEN_HEIGHT - 200, 250, 30},  false, WHITE},//first platform
		{{30, 2 * SCREEN_HEIGHT / 3 - 40, 600, 30},  false, WHITE},//second platform
		{{SCREEN_WIDTH / 8 + 30, SCREEN_HEIGHT / 2.0 - 70,SCREEN_WIDTH / 4 + 480, 30}, false, WHITE},
		{{SCREEN_WIDTH / 2 + 220, 2 * SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2 - 250, 30}, false, WHITE}
	};
	int lvl6_num_objects = 9;
	Enemy lvl6enemy1, lvl6enemy2, lvl6enemy3;
	InitEnemy(&lvl6enemy1, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
	InitEnemy(&lvl6enemy2, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
	InitEnemy(&lvl6enemy3, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);


	//level 7
	Object lvl7objects[] = {
		{{0, 0, SCREEN_WIDTH, 30 },  false, WHITE},//ceiling
		{{0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30},  false, WHITE},//floor
		{{0, 0, 30, SCREEN_HEIGHT},  false, WHITE},//left wall
		{{SCREEN_WIDTH - 30, 0, 30, SCREEN_HEIGHT}, false, WHITE}, //right wall
	};
	int lvl7_num_objects = 4;
	Enemy enemies[20];

	//SetMusicVolume(music, 1.0);
	//SetMusicPitch(music, 1.0);

	while (!WindowShouldClose()) {
		cheat();
		UpdateMusicStream(main_music);
		if (level) { //starting time when level 1 starts
			
			static bool first = true;
			if (first) {
				first = false;
				startTime = GetTime();
			}
			elapsedTime = GetTime() - startTime;
		}

		BeginDrawing();
		DrawText(TextFormat("%d", level), 100, 190, 20, WHITE);
		switch (level)
		{
		case 0:	menu(music, playbutton, controlsbutton, exitbutton, backbutton, menubg, controlsbg); break; //emnu
		case 1: level1(&player, lvl1objects, lvl1_num_objects, &bullet); break; //tutorial
		case 2: level2(&player, lvl2objects, &lvl2enemy1, lvl2_num_objects, &bullet); break; //enemy intro
		case 3: level3(&player, lvl3objects, &lvl3enemy1, lvl3_num_objects, &bullet); break; //parkour-ish
		case 4: level4(&player, lvl4objects, &lvl4enemy1, &lvl4enemy2, &lvl4enemy3, &lvl4enemy4, lvl4_num_objects, &bullet, gun); break; //gun intro
		case 5: level5(&player, lvl5objects, &lvl5enemy1, &lvl5enemy2, &lvl5enemy3, &lvl5enemy4, lvl5_num_objects, &bullet); break; //yellow guys parkour?
		case 6: level6(&player, lvl6objects, &lvl6enemy1, &lvl6enemy2, &lvl6enemy3, lvl6_num_objects, &bullet, dark); break; //dark maze
		case 7: level7(&player, lvl7objects, enemies, lvl7_num_objects, &bullet, dark); break;//FINAL BATTLE
		//case 9: level9(&player, lvl9objects, &lvl9enemy1, lvl9_num_objects, &bullet); break; //BOSS BATTLE
			//void level7(Player* player, Object objects[], Enemy enemies[], Enemy* enemy2, Enemy* enemy3, int num_objects, Bullet* bullet, Texture dark) {

		default: win_screen(); break;
		}
		EndDrawing();
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}

void menu(Music music, Button playbutton, Button controlsbutton, Button exitbutton, Button backbutton, Texture menubg, Texture controlsbg) {
	static bool controls = false;
	if (controls) {
		DrawTexture(controlsbg, 0, 0, WHITE);
		InitButton(backbutton, SCREEN_WIDTH - 100 - backbutton.img.width, 100);
		if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { backbutton.bounds }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(LoadSound("assets/buttonclick.mp3"));
			controls = false;
		}
	}
	else {
		DrawTexture(menubg, 0, 0, WHITE);
		
		InitButton(playbutton, playbutton.bounds.x, playbutton.bounds.y);
		InitButton(controlsbutton, controlsbutton.bounds.x, controlsbutton.bounds.y);
		InitButton(exitbutton, exitbutton.bounds.x, exitbutton.bounds.y);
		if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { playbutton.bounds }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(LoadSound("assets/buttonclick.mp3"));
			is_time_pause = false;
			//UnloadTexture(playbutton.img);
			//UnloadTexture(controlsbutton.img);
			//UnloadTexture(backbutton.img);
			//UnloadTexture(exitbutton.img);
			//UnloadTexture(menubg);
			UnloadTexture(controlsbg);
			UnloadMusicStream(music);
			PlayMusicStream(main_music);
			level++;
		}
		else if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { controlsbutton.bounds }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(LoadSound("assets/buttonclick.mp3"));
			controls = true;
		}
		else if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { exitbutton.bounds }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(LoadSound("assets/buttonclick.mp3"));
			exit(0);
		}
	}
}

void level1(Player* player, Object objects[], int num_objects, Bullet* bullet) {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 1) {
			DrawTexture(pausebg, 0, 0, { 1, 1, 1, 20 });
			DrawText("LEVEL 1", (SCREEN_WIDTH - MeasureText("LEVEL 1", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, WHITE);
		}
		else if (elapsedTime - time < 3) {
			DrawTexture(pausebg, 0, 0, { 1, 1, 1, 20 });
		}
		else first = false;
		if (IsKeyPressed(KEY_ENTER)) first = false;
		if (!first)
			InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
	}
	else {
		is_time_pause = false;
		static bool pause = false;
		if (!pause && player->alive) {
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);
			DrawBorderedText("			OBJECTIVE:\nGET THE SARDINES", (SCREEN_WIDTH - MeasureText("			OBJECTIVE:\nGET THE SARDINES", 80)) / 2.0, 100, 80, ORANGE, 20, BLACK);

			UpdatePlayer(player, objects, num_objects, bullet);

			for (int i = 0; i < num_objects; i++) DrawTextureRec(platform, { 0, randoms[i], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);

			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);

			sardines.x = SCREEN_WIDTH - 140 - sardines.img.width;
			sardines.y = SCREEN_HEIGHT - sardines.img.height - 20;
			DrawTexture(sardines.img, sardines.x, sardines.y, WHITE);
			if (CheckCollisionRecs({ sardines.x, sardines.y, (float)sardines.img.width, (float)sardines.img.height }, { player->bounds })) {
				InitPlayer(player, 300, 300, 1, false);
				level++;
				return;
			}

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (!IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
			}
		}
	}
}
						
void level2(Player* player, Object objects[], Enemy* enemy1, int num_objects, Bullet* bullet) {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 2) {
			DrawTexture(pausebg, 0, 0, { 0,0,0, 20 });
			DrawText("LEVEL 2", (SCREEN_WIDTH - MeasureText("LEVEL 2", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, { 255, 255, 255, 100 });
		}
		else {
			DrawText("PRESS ENTER TO CONTINUE", (SCREEN_WIDTH - MeasureText("PRESS ENTER TO CONTINUE", 70)) / 2, SCREEN_HEIGHT / 2 + 150, 70, { 255, 255, 255, 20 });
		}
		if (IsKeyPressed(KEY_ENTER)) {
			first = false;
			InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
			InitEnemy(enemy1, SCREEN_WIDTH / 2.0 + enemy_textures[0].width / 2.0, SCREEN_HEIGHT / 2.0 - 220 - enemy_textures[0].height / 2.0, 0);
		}
	}
	else {
		static bool pause = false;
		if (!pause && player->alive) {
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);
			DrawBorderedText("	AVOID\nENEMIES", SCREEN_WIDTH / 4, 100, 80, ORANGE, 20, BLACK);

			UpdatePlayer(player, objects, num_objects, bullet);
			UpdateEnemy(enemy1, player, objects, num_objects, bullet);

			for (int i = 0; i < num_objects; i++) DrawTextureRec(platform, { 0, randoms[i], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);

			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);

			sardines.x = 1100;
			sardines.y = SCREEN_HEIGHT - 20 - sardines.img.height;
			DrawTexture(sardines.img, sardines.x, sardines.y, WHITE);
			if (CheckCollisionRecs({ sardines.x, sardines.y, (float)sardines.img.width, (float)sardines.img.height }, { player->bounds })) {
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 0, false);
				level++;
				return;
			}

			DrawTextureRec(lava, { (float((int)elapsedTime % 4)) * 90.0f, SCREEN_HEIGHT - 30, 400, 30 }, { SCREEN_WIDTH - 430, SCREEN_HEIGHT - 20 }, WHITE);

			if (player->y >= SCREEN_HEIGHT - 10 - player->img.height / 2) {
				lives--;
				player->x = player->start.x;
				player->y = player->start.y;
				enemy1->x = enemy1->start.x;
				enemy1->y = enemy1->start.y;
				enemy1->alive = true;
			}
			if (!lives) player->alive = false;

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
				level = 1;
				enemy1->alive = false;
			}
		}
	}
}
							
void level3(Player* player, Object objects[], Enemy* enemy1, int num_objects, Bullet* bullet)  {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 2) {
			DrawTexture(pausebg, 0, 0, { 0,0,0, 20 });
			DrawText("LEVEL 3", (SCREEN_WIDTH - MeasureText("LEVEL 3", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, { 255, 255, 255, 100 });
		}
		else {
			DrawText("PRESS ENTER TO CONTINUE", (SCREEN_WIDTH - MeasureText("PRESS ENTER TO CONTINUE", 70)) / 2, SCREEN_HEIGHT / 2 + 150, 70, { 255, 255, 255, 20 });
		}
		if (IsKeyPressed(KEY_ENTER)) {
			InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
			first = false;
			InitEnemy(enemy1, SCREEN_WIDTH - 150, 480.0f - float(enemy_textures->height)/2, 0);
			return;
		}
	}
	else {
		static bool pause = false;
		if (!pause && player->alive) {
			//background
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);
			//update entities
			UpdatePlayer(player, objects, num_objects, bullet);
			UpdateEnemy(enemy1, player, objects, num_objects, bullet);
			//draw platforms
			for (int i = 0; i < num_objects; i++) DrawTextureRec(platform, { 0, randoms[i + 20], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);
			//dev tools
			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);
			//sardines
			sardines.x = SCREEN_WIDTH - sardines.img.width - 50;
			sardines.y = 50 + sardines.img.height;
			DrawTexture(sardines.img, sardines.x, sardines.y, WHITE);
			if (CheckCollisionRecs({ sardines.x, sardines.y, (float)sardines.img.width, (float)sardines.img.height }, { player->bounds })){
				InitPlayer(player, cat_textures[0].width, 380 - cat_textures[0].height / 2, 1, false);
				level++;
				return ;
			}

			DrawTextureRec(lava, { 0, (float(int(elapsedTime) % 4)) * 300.0f, 1700, 30 }, { 200, SCREEN_HEIGHT - 20 }, WHITE);

			if (player->y >= SCREEN_HEIGHT - 10 - player->img.height/2) {
				lives--;
				player->x = player->start.x;
				player->y = player->start.y;
				enemy1->x = enemy1->start.x;
				enemy1->y = enemy1->start.y;
				enemy1->alive = true;
			}
			if (!lives) player->alive = false;

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
				level = 1;
				enemy1->alive = false;
			}
		}
	}
}

void level4(Player* player, Object objects[], Enemy* enemy1, Enemy* enemy2, Enemy* enemy3, Enemy* enemy4, int num_objects, Bullet* bullet, Texture gun) {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 2) {
			DrawTexture(pausebg, 0, 0, { 0,0,0, 20 });
			DrawText("LEVEL 4", (SCREEN_WIDTH - MeasureText("LEVEL 4", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, { 255, 255, 255, 100 });
		}
		else {
			DrawText("PRESS ENTER TO CONTINUE", (SCREEN_WIDTH - MeasureText("PRESS ENTER TO CONTINUE", 70)) / 2, SCREEN_HEIGHT / 2 + 150, 70, { 255, 255, 255, 20 });
		}
		if (IsKeyPressed(KEY_ENTER)) {
			first = false;
			InitPlayer(player, cat_textures[0].width, 380 - cat_textures[0].height / 2, 1, false);
			InitEnemy(enemy1, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
			InitEnemy(enemy2, SCREEN_WIDTH - 400, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
			InitEnemy(enemy3, SCREEN_WIDTH - 500, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
			InitEnemy(enemy4, 1400, SCREEN_HEIGHT / 3 - float(enemy_textures->height), 3);
			return;
		}
	}
	else {
		static bool pause = false;
		if (!pause && player->alive) {
			//background
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);
			DrawBorderedText("TINTED WALLS MAY\nBE BROKEN", SCREEN_WIDTH / 4, 100, 80, ORANGE, 20, BLACK);
			//sardines
			sardines.x = SCREEN_WIDTH - 200;
			sardines.y = SCREEN_HEIGHT - sardines.img.height - 20;
			DrawTexture(sardines.img, sardines.x, sardines.y, WHITE);
			if (CheckCollisionRecs({ sardines.x, sardines.y, (float)sardines.img.width, (float)sardines.img.height }, { player->bounds })) {
				//Next level initialisation
				InitPlayer(player, cat_textures[3].width, SCREEN_HEIGHT - cat_textures[3].height - 30, 3, true);
				level++;
				return;
			}
			//update entities
			UpdatePlayer(player, objects, num_objects, bullet);
			UpdateBullet(bullet, player, objects, num_objects);
			UpdateEnemy(enemy1, player, objects, num_objects, bullet);
			UpdateEnemy(enemy2, player, objects, num_objects, bullet);
			UpdateEnemy(enemy3, player, objects, num_objects, bullet);
			UpdateEnemy(enemy4, player, objects, num_objects, bullet);

			//draw platforms
			for (int i = 0; i < num_objects; i++)
				if (!(objects[i].broken))
					DrawTextureRec(platform, { 0, randoms[i], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);
			
			//dev tools
			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);
			DrawText(TextFormat("bullet.x = %.1f, bullet.y = %.1f", bullet->x, bullet->y), 100, 190, 20, WHITE);
			DrawText(TextFormat("%d", objects[12].broken), 100, 220, 20, WHITE);
			//bullet hit wall
			if (CheckCollisionCircleRec({ bullet->x, bullet->y }, bullet->radius, objects[13].rect) && objects[13].broken == false) {
				bullet->fired = false;
				bullet->hit = true;
				objects[13].broken = true;
			}

			//gun
			if (player->gun == false) DrawTextureEx(gun, { SCREEN_WIDTH - 250, 180 + 10*float(sin(elapsedTime))}, 0, 1.8, WHITE);
			if (CheckCollisionRecs({ SCREEN_WIDTH - 200, 100, (float)gun.width, (float)gun.height }, { player->bounds })) {
				player->gun = true;
				if (player->state % 2) player->state = 3;
				else player->state = 2;
			}

			if (player->y >= SCREEN_HEIGHT - 10 - player->img.height / 2) {
				lives--;
				player->x = player->start.x;
				player->y = player->start.y;
				enemy1->x = enemy1->start.x;
				enemy1->y = enemy1->start.y;
				enemy1->alive = true;
			}
			if (!lives) player->alive = false;

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
				level = 1;
				enemy1->alive = false;
			}
		}
	}
}

void level5(Player* player, Object objects[], Enemy* enemy1, Enemy* enemy2, Enemy* enemy3, Enemy* enemy4, int num_objects, Bullet* bullet) {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 2) {
			DrawTexture(pausebg, 0, 0, { 0,0,0, 20 });
			DrawText("LEVEL 5", (SCREEN_WIDTH - MeasureText("LEVEL 5", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, { 255, 255, 255, 100 });
		}
		else {
			DrawText("PRESS ENTER TO CONTINUE", (SCREEN_WIDTH - MeasureText("PRESS ENTER TO CONTINUE", 70)) / 2, SCREEN_HEIGHT / 2 + 150, 70, { 255, 255, 255, 20 });
		}
		if (IsKeyPressed(KEY_ENTER)) {
			InitPlayer(player, cat_textures[3].width, SCREEN_HEIGHT - cat_textures[3].height - 30, 3, true);
			first = false;
			InitEnemy(enemy1, 320, 380 - float(enemy_textures->height) / 2, 2);
			InitEnemy(enemy2, 1050, SCREEN_HEIGHT - 120 - float(enemy_textures->height) / 2, 3);
			InitEnemy(enemy3, 1000, 320 - float(enemy_textures->height) / 2, 2);
			InitEnemy(enemy3, 700, SCREEN_HEIGHT - 400 - float(enemy_textures->height) / 2, 3);
			return;
		}
	}
	else {
		static bool pause = false;
		if (!pause && player->alive) {
			//background
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);
			//draw platforms
			for (int i = 0; i < num_objects; i++) DrawTextureRec(platform, { 0, randoms[i + 20], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);
			//sardines
			sardines.x = SCREEN_WIDTH - sardines.img.width - 50;
			sardines.y = 50 + sardines.img.height;
			DrawTexture(sardines.img, sardines.x, sardines.y, WHITE);
			if (CheckCollisionRecs({ sardines.x, sardines.y, (float)sardines.img.width, (float)sardines.img.height }, { player->bounds })) {
				InitPlayer(player, cat_textures[0].width, 380 - cat_textures[0].height / 2, 3, true);
				level++;
				return;
			}
			//update entities
			UpdatePlayer(player, objects, num_objects, bullet);
			UpdateEnemy(enemy1, player, objects, num_objects, bullet);
			UpdateEnemy(enemy2, player, objects, num_objects, bullet);
			UpdateEnemy(enemy3, player, objects, num_objects, bullet);
			UpdateEnemy(enemy4, player, objects, num_objects, bullet);
			UpdateBullet(bullet, player, objects, num_objects);
			//dev tools
			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);
			
			DrawTextureRec(lava, { 0, (float(int(elapsedTime) % 4)) * 300.0f, 1700, 30 }, { 200, SCREEN_HEIGHT - 20 }, WHITE);


			if (player->y >= SCREEN_HEIGHT - 10 - player->img.height / 2) {
				lives--;
				player->x = player->start.x;
				player->y = player->start.y;
				enemy1->x = enemy1->start.x;
				enemy1->y = enemy1->start.y;
				enemy1->alive = true;
			}
			if (!lives) player->alive = false;

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
				level = 1;
				enemy1->alive = false;
			}
		}
	}
}

void level6(Player* player, Object objects[], Enemy* enemy1, Enemy* enemy2, Enemy* enemy3, int num_objects, Bullet* bullet, Texture dark) {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 2) {
			DrawTexture(pausebg, 0, 0, { 0,0,0, 20 });
			DrawText("LEVEL 6", (SCREEN_WIDTH - MeasureText("LEVEL 6", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, { 255, 255, 255, 100 });
		}
		else {
			DrawText("PRESS ENTER TO CONTINUE", (SCREEN_WIDTH - MeasureText("PRESS ENTER TO CONTINUE", 70)) / 2, SCREEN_HEIGHT / 2 + 150, 70, { 255, 255, 255, 20 });
		}
		if (IsKeyPressed(KEY_ENTER)) {
			first = false;
			InitPlayer(player, cat_textures[0].width, 380 - cat_textures[0].height / 2, 3, true);
			InitEnemy(enemy1, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
			InitEnemy(enemy2, SCREEN_WIDTH - 400, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
			InitEnemy(enemy3, SCREEN_WIDTH - 500, SCREEN_HEIGHT - 40 - float(enemy_textures->height) / 2, 0);
			return;
		}
	}
	else {
		static bool pause = false;
		if (!pause && player->alive) {
			//background
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);
			//sardines
			sardines.x = SCREEN_WIDTH - 200;
			sardines.y = SCREEN_HEIGHT - sardines.img.height - 20;
			DrawTexture(sardines.img, sardines.x, sardines.y, WHITE);
			if (CheckCollisionRecs({ sardines.x, sardines.y, (float)sardines.img.width, (float)sardines.img.height }, { player->bounds })) {
				InitPlayer(player, cat_textures[0].width, 380 - cat_textures[0].height / 2, 3, true);
				level++;
				return;
			}
			//update entities
			UpdatePlayer(player, objects, num_objects, bullet);
			UpdateBullet(bullet, player, objects, num_objects);
			UpdateEnemy(enemy1, player, objects, num_objects, bullet);
			UpdateEnemy(enemy2, player, objects, num_objects, bullet);
			UpdateEnemy(enemy3, player, objects, num_objects, bullet);

			//draw platforms
			for (int i = 0; i < num_objects; i++)
				if (!(objects[i].broken))
					DrawTextureRec(platform, { 0, randoms[i], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);

			//dev tools
			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);
			DrawText(TextFormat("bullet.x = %.1f, bullet.y = %.1f", bullet->x, bullet->y), 100, 190, 20, WHITE);
			DrawText(TextFormat("%d", objects[12].broken), 100, 220, 20, WHITE);
			//bullet hit wall

			if (player->y >= SCREEN_HEIGHT - 10 - player->img.height / 2) {
				lives--;
				player->x = player->start.x;
				player->y = player->start.y;
				enemy1->x = enemy1->start.x;
				enemy1->y = enemy1->start.y;
				enemy1->alive = true;
			}
			if (!lives) player->alive = false;
			

			DrawTexture(dark, player->x - dark.width / 2, player->y - dark.height / 2, Fade(WHITE, 0.99f));
			//DrawTexturePro(dark, {dark.width/2 - player->x)

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
				level = 1;
				enemy1->alive = false;
			}
		}
	}
}

void level7(Player* player, Object objects[], Enemy enemies[], int num_objects, Bullet* bullet, Texture dark) {
	static bool first = true;
	if (first) {
		static float time = elapsedTime;
		if (elapsedTime - time < 2) {
			DrawTexture(pausebg, 0, 0, { 0,0,0, 20 });
			DrawText("LEVEL 7", (SCREEN_WIDTH - MeasureText("LEVEL 7", 150)) / 2, SCREEN_HEIGHT / 2 - 100, 150, { 255, 255, 255, 100 });
		}
		else {
			DrawText("PRESS ENTER TO CONTINUE", (SCREEN_WIDTH - MeasureText("PRESS ENTER TO CONTINUE", 70)) / 2, SCREEN_HEIGHT / 2 + 150, 70, { 255, 255, 255, 20 });
		}
		if (IsKeyPressed(KEY_ENTER)) {
			first = false;
			#undef PLAYER_HOR_SPEED
			#define PLAYER_HOR_SPEED 500.0f
			InitPlayer(player, (SCREEN_WIDTH-cat_textures[0].width)/2, SCREEN_HEIGHT - cat_textures[0].height / 2-100, 3, true);
			return;
		}
	}
	else {
		static int enemyX = 0;
		static int enemySpawnSpeed = 2;
		static float secondTime = elapsedTime;
		static bool pause = false;
		if (!pause && player->alive) {
			if (elapsedTime - secondTime > enemySpawnSpeed && enemyX < 20) {
				enemyX++;
				secondTime = elapsedTime;
				InitEnemy(enemies + enemyX - 1, (rand() % 2 == 0 ? rand() % 300 + 10 : 1620 + rand() % 300), SCREEN_HEIGHT / 2, rand() % 6 + 2);
			}
			//background
			DrawTextureEx(bg, { 0, 0 }, 0, 1.2, SKYBLUE);

			switch (enemyX) {
				case 0:
				case 1:
				case 2:
				case 3:
					DrawBorderedText("SPEED BOOST ACQUIRED", (SCREEN_WIDTH-MeasureText("SPEED BOOST ACQUIRED", 80))/2, 100, 80, ORANGE, 20, BLACK);
					enemySpawnSpeed = 2;
					break;
				case 4:
				case 5:
				case 6:
					enemySpawnSpeed = 1.8;
					break;
				case 7:
				case 8:
				case 9:
					enemySpawnSpeed = 1.6;
					break;
				case 10:
				case 11:
				case 12:
					enemySpawnSpeed = 1.5;
					break;
				case 13:
				case 14:
				case 15:
					enemySpawnSpeed = 1.4;
					break;
				case 16:
				case 17:
				case 18:
					enemySpawnSpeed = 1.2;
					break;
				case 19:
				case 20:
					enemySpawnSpeed = 1.2;
					break;
				default:
					enemySpawnSpeed = 1.2;
					break;
			}

			//update entities
			UpdatePlayer(player, objects, num_objects, bullet);
			UpdateBullet(bullet, player, objects, num_objects);
			for (int i = 0; i < enemyX; i++) {
				UpdateEnemy(enemies + i, player, objects, num_objects, bullet);
			}
			
			//draw platforms
			for (int i = 0; i < num_objects; i++)
				if (!(objects[i].broken))
					DrawTextureRec(platform, { 0, randoms[i], objects[i].rect.width, objects[i].rect.height }, { objects[i].rect.x, objects[i].rect.y }, objects[i].color);
			bool all_dead = true;
			for (int i = 0; i < 20; i++) {
				if ((enemies + i)->x > SCREEN_WIDTH - 10 || (enemies + i)->x < 10 || (enemies + i)->y > SCREEN_HEIGHT - 10 || (enemies + i)->y < 10) {
					(enemies + i)->alive = false;
				}
				if ((enemies + i)->alive) {
					all_dead = false;
					break;
				}
			}
			if (all_dead && enemyX == 20) level++;
			//dev tools
			DrawText(TextFormat("player x: %.2f", player->x), 100, 100, 20, WHITE);
			DrawText(TextFormat("player y: %.2f", player->y), 100, 130, 20, WHITE);
			DrawText(TextFormat("time: %.1f", elapsedTime), 100, 160, 20, WHITE);
			//bullet hit wall

			if (player->y >= SCREEN_HEIGHT - 10 - player->img.height / 2) {
				lives--;
				player->x = player->start.x;
				player->y = player->start.y;

			}
			if (!lives) player->alive = false;

			//DrawTexture(dark, player->x - dark.width / 2, player->y - dark.height / 2, Fade(WHITE, 0.99f));
			//DrawTexturePro(dark, {dark.width/2 - player->x)

			DisplayLives();
			UpdatePauseButton(&pause);
		}
		else if (pause) {
			DrawTextureEx(pausebg, { 0, 0 }, 0, 1.0, { 255, 255, 255, 2 });
			DrawText("PAUSED", (SCREEN_WIDTH - MeasureText("PAUSED", 200)) / 2.0, 100, 200, BLACK);
			DrawText("PRESS ANY CONTROL KEY TO RESUME", (SCREEN_WIDTH - MeasureText("PRESS ANY CONTROL KEY TO RESUME", 70)) / 2.0, 600, 70, BLACK);
			UpdatePauseButton(&pause);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_SPACE))
				pause = false;
		}
		else {
			DrawTextureEx(death_screen, { 0, 0 }, 0, 2.0, { 255, 255, 255, 10 });
			DrawText("YOU DIED", (SCREEN_WIDTH - MeasureText("YOU DIED", 200)) / 2.0, 100, 200, RED);
			DrawText("PRESS SPACE TO PLAY AGAIN", (SCREEN_WIDTH - MeasureText("PRESS SPACE TO PLAY AGAIN", 50)) / 2.0, 400, 50, WHITE);
			if (IsKeyPressed(KEY_SPACE)) {
				player->alive = true;
				lives = 9;
				InitPlayer(player, cat_textures[0].width, SCREEN_HEIGHT - cat_textures[0].height - 30, 1, false);
				elapsedTime = 0.0f;
				level = 1;
			}
		}
	}
}

void win_screen() {
	static float final_time = GetTime();
	static bool credits = false;
	if (!credits) {
		ClearBackground({ (unsigned char)(128 + 128 * sin(elapsedTime)),(unsigned char)(128 + 128 * sin(elapsedTime * 2)),(unsigned char)(128 + 128 * sin(elapsedTime * 3)), 10 });
		//make cat go left/right
		if (int(GetTime() * 2) % 2) DrawTextureEx(cat_textures[4], { SCREEN_WIDTH / 2, 400 }, 0, 1, WHITE);
		else DrawTextureEx(cat_textures[5], { SCREEN_WIDTH / 2, 400 }, 0, 1, WHITE);
		//random circles go
		for (int i = 0; i < 10; i++) DrawCircle(rand() % 1920, rand() % 980, rand() % 100 + 10, { (unsigned char)(rand() % 255) ,(unsigned char)(rand() % 255) , (unsigned char)(rand() % 255), 255 });
		//YOU WIN text
		DrawBorderedText("YOU WIN!", (SCREEN_WIDTH - MeasureText("YOU WIN!", 300)) / 2.0, 100, 300, ORANGE, 30, BLACK); //text
		//display final stats
		DrawRectangleRounded({ SCREEN_WIDTH / 8 - 25, SCREEN_HEIGHT / 2 - 75, 650, 450 }, 0.5, 0, BLACK); //border
		DrawRectangleRounded({ SCREEN_WIDTH / 8, SCREEN_HEIGHT / 2 - 50, 600, 400 }, 0.5, 0, ORANGE);
		DrawText(TextFormat("Time: %.1fs", final_time), SCREEN_WIDTH / 8 + 50, SCREEN_HEIGHT / 2, 90, BLACK);
		DrawText(TextFormat("Lives: %d", lives), SCREEN_WIDTH / 8 + 50, SCREEN_HEIGHT / 2 + 120, 90, BLACK);
		for (int i = 0; i < lives; i++)
			DrawTextureEx(heart, { SCREEN_WIDTH / 8 + 40 + (float)i * 53, SCREEN_HEIGHT / 2 + 220 }, 0, 0.2, WHITE);
		//delay
		//WaitTime(0.1);
		if (IsKeyPressed(KEY_ENTER)) credits = true;
	}
	else {
		float time_diff = elapsedTime - final_time;
		//if (time_diff < )
		ClearBackground(Color{ 1, 46, 54, 255 });
		if (int(GetTime() * 2) % 2) DrawTextureEx(cat_textures[4], { (SCREEN_WIDTH - cat_textures[4].width)/2, 400 }, 0, 1, Color{ 255, 255, 255, 50 });
		else DrawTextureEx(cat_textures[5], { (SCREEN_WIDTH - cat_textures[4].width) / 2, 400 }, 0, 1, Color{ 255, 255, 255, 50 });
		DrawBorderedText("CREDITS", (SCREEN_WIDTH - MeasureText("CREDITS", 200)) / 2, SCREEN_HEIGHT - 100*(time_diff), 200, ORANGE, 20, BLACK);

		DrawBorderedText("Developed By", (SCREEN_WIDTH - MeasureText("Developed by", 50)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 300, 50, ORANGE, 20, BLACK);
		DrawBorderedText("Zain Kashif", (SCREEN_WIDTH - MeasureText("Zain Kashif", 80)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 400, 80, ORANGE, 20, BLACK);

		DrawBorderedText("Graphics Designed By", (SCREEN_WIDTH - MeasureText("Graphics Designed by", 50)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 600, 50, ORANGE, 20, BLACK);
		DrawBorderedText("Zain Kashif", (SCREEN_WIDTH - MeasureText("Zain Kashif", 80)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 700, 80, ORANGE, 20, BLACK);
		DrawBorderedText("Anoosheh Arshad", (SCREEN_WIDTH - MeasureText("Anoosheh Arshad", 80)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 780, 80, ORANGE, 20, BLACK);

		DrawBorderedText("Course Instructor", (SCREEN_WIDTH - MeasureText("Course Instructor", 50)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 980, 50, ORANGE, 20, BLACK);
		DrawBorderedText("Sir Jaudat Mamoon", (SCREEN_WIDTH - MeasureText("Sir Jaudat Mamoon", 80)) / 2, SCREEN_HEIGHT - 100 * (time_diff) + 1080, 80, ORANGE, 20, BLACK);

		DrawBorderedText("Thanks for\n   playing!", (SCREEN_WIDTH - MeasureText("Thanks for", 200)) / 2, SCREEN_HEIGHT - 100 * (time_diff)+1400 < SCREEN_HEIGHT/4 ? SCREEN_HEIGHT/4 : SCREEN_HEIGHT - 100*(time_diff) + 1400, 200, ORANGE, 20, BLACK);

	}
}

void InitButton(Button button, int x, int y) {
	if (CheckCollisionPointRec({ (float)GetMouseX(), (float)GetMouseY() }, { (float)x, (float)y, (float)button.img.width, (float)button.img.height})) {	
		for (int i = 0; i < 10; i++)
			DrawTexture(button.img, x - i, y + i, BLACK);
		DrawTexture(button.img, x, y, WHITE);
	}
	else {
		for (int i = 0; i < 5; i++)
			DrawTexture(button.img, x - 5 - i, y + 5 + i, BLACK);
		DrawTexture(button.img, x + 5, y - 5, WHITE);
	}
}

void PlayDaSound(int i) {
	if (!channels[i] || !IsSoundPlaying(sounds[i]))
	{
		channels[i] = 1;
		PlaySound(sounds[i]);
	}

}

void UpdatePlayer(Player* player, Object* objects, int num_objects, Bullet* bullet) {
	float time = GetFrameTime();
	bool hit_obstacle_y = false;
	bool hit_obstacle_x = false;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
		player->x -= PLAYER_HOR_SPEED * time;
		if (player->state % 2 == 1) player->state--;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
		player->x += PLAYER_HOR_SPEED * time;
		if (player->state % 2 == 0) player->state++;
	}
	if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) || (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))) player->speed.x = 0;

	if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE)) && player->can_jump) {
		player->speed.y = -PLAYER_JUMP_SPEED;
		player->can_jump = false;
	}
	//collision detection
	for (int i = 0; i < num_objects; i++) {
		if (!objects[i].broken) {
			if (CheckCollisionRecs({ player->x - float(player->img.width) / 2 , player->y - float(player->img.height) / 2, (float)player->img.width, (float)player->img.height }, (objects + i)->rect)) {
				Rectangle collision_rec = GetCollisionRec(player->bounds, objects[i].rect);

				if (collision_rec.width > collision_rec.height) { //player collides from above or below
					if (player->y >= (objects + i)->rect.y + (objects + i)->rect.height / 2.0) { //player collides from below
						player->y = (objects + i)->rect.y + (objects + i)->rect.height + player->img.height / 2;
						player->speed.y = 0;
						hit_obstacle_y = true;
					}
					else if (player->bounds.y <= (objects + i)->rect.y + (objects + i)->rect.height / 2.0) { //player collides from above
						player->can_jump = true;
						player->y = (objects + i)->rect.y - player->img.height / 2.0;
						player->speed.y = 0;
						hit_obstacle_y = true;
					}
				}
				else { //player collides from right or left
					if (player->bounds.x >= (objects + i)->rect.x + (objects + i)->rect.width / 2.0) { //player collides from right
						player->x = (objects + i)->rect.x + (objects + i)->rect.width + player->img.width / 2.0;
						player->speed.x = 0;
						hit_obstacle_x = true;
					}
					else if (player->bounds.x <= (objects + i)->rect.x + (objects + i)->rect.width / 2.0) { //player collides from left
						player->x = (objects + i)->rect.x - player->img.width / 2.0;
						player->speed.x = 0;
						hit_obstacle_x = true;
					}
				}
			}
		}
	}
	
	if (!hit_obstacle_y) {
		player->y += player->speed.y * time;
		player->speed.y += G * time;
		player->can_jump = false;
	}
	if (!hit_obstacle_x) {
		player->x += player->speed.x * time;
	}
	
	player->img = cat_textures[player->state];
	player->bounds = { player->x - float(player->img.width) / 2, player->y - float(player->img.height) / 2, (float)player->img.width, (float)player->img.height };
	DrawTextureEx(player->img, { (float)player->x - (float(player->img.width) / 2), (float)player->y - (float(player->img.height) / 2) }, 0, 1.0, WHITE);
}

void UpdateEnemy(Enemy* enemy, Player* player, Object* objects, int num_objects, Bullet* bullet) {
	float time = GetFrameTime();
	bool hit_obstacle_y = false;
	bool hit_obstacle_x = false;
	int range;
	float Xspeed;

	switch (enemy->state) {
	case 0:
	case 1:
	case 2:
	case 3: range = 600; Xspeed = 100.0f; break;
	case 4:
	case 5: range = 1000; Xspeed = 200.0f; break;
	case 6:
	case 7: range = 1500; Xspeed = 250.0f;
	}

	if (CheckCollisionCircleRec({ bullet->x, bullet->y }, bullet->radius, enemy->bounds)) {
		enemy->alive = false;
		bullet->hit = true;
		bullet->fired = false;
		enemy->bounds = { 0,0,0,0 };
	}
	if (enemy->alive) {
		if (abs((int)(enemy->x - player->x)) < range && abs((int)(enemy->y - player->y)) < range) {
			if (player->x > enemy->x + 1) {
				enemy->speed.x = Xspeed;
				if (enemy->state % 2 == 0) enemy->state++;
			}
			else if (player->x < enemy->x - 1) {
				enemy->speed.x = -Xspeed;
				if (enemy->state % 2 == 1) enemy->state--;
			}
			else {
				enemy->speed.x = 0;
				if (enemy->state % 2 == 0) enemy->state++;
			}
		}
		else enemy->speed.x = 0;

		//collision detection
		for (int i = 0; i < num_objects; i++) {
			if (!objects[i].broken) {
				if (CheckCollisionRecs({ enemy->x - float(enemy->img.width) / 2 , enemy->y - float(enemy->img.height) / 2, (float)enemy->img.width, (float)enemy->img.height }, objects[i].rect)) {
					Rectangle collision_rec = GetCollisionRec(enemy->bounds, objects[i].rect);

					if (collision_rec.width > collision_rec.height) { //enemy collides from above or below
						if (enemy->bounds.y >= (objects + i)->rect.y + (objects + i)->rect.height / 2.0) { //enemy collides from below
							enemy->y = (objects + i)->rect.y + (objects + i)->rect.height + enemy->img.height / 2.0;
							enemy->speed.y = 0;
							hit_obstacle_y = true;
						}
						else if (enemy->bounds.y <= (objects + i)->rect.y + (objects + i)->rect.height / 2.0) { //enemy collides from above
							if (enemy->state == 2 || enemy->state == 3) enemy->can_jump = true;
							enemy->y = (objects + i)->rect.y - enemy->img.height / 2.0;
							enemy->speed.y = 0;
							hit_obstacle_y = true;
						}
					}
					else { //enemy collides from right or left
						if (enemy->bounds.x >= (objects + i)->rect.x + (objects + i)->rect.width / 2.0) { //enemy collides from right
							enemy->x = (objects + i)->rect.x + (objects + i)->rect.width + enemy->img.width / 2.0;
							enemy->speed.x = 0;
							hit_obstacle_x = true;
						}
						else if (enemy->bounds.x <= (objects + i)->rect.x + (objects + i)->rect.width / 2.0) { //enemy collides from left
							enemy->x = (objects + i)->rect.x - enemy->img.height / 2.0;
							enemy->speed.x = 0;
							hit_obstacle_x = true;
						}
					}
				}
			}
		}
		if (enemy->can_jump && (rand() % 100 + 1 <= 50)) {
			enemy->speed.y = -ENEMY_JUMP_SPEED;
			enemy->can_jump = false;
		}
		if (!hit_obstacle_y) {
			enemy->speed.y += G * time;
			enemy->y += enemy->speed.y * time;
			enemy->can_jump = false;
		}
		if (!hit_obstacle_x) {
			enemy->x += enemy->speed.x * time;
		}

		enemy->img = enemy_textures[enemy->state];
		enemy->bounds = { enemy->x - float(enemy->img.width) / 2, enemy->y - float(enemy->img.height) / 2, float(enemy->img.width), float(enemy->img.height) };
		if (CheckCollisionRecs(enemy->bounds, player->bounds)) {
			lives--;
			player->x = player->start.x;
			player->y = player->start.y;
			enemy->x = enemy->start.x;
			enemy->y = enemy->start.y;
		}

		DrawTextureEx(enemy->img, { (float)(enemy->x - (enemy->img.width / 2.0)), (float)(enemy->y - (enemy->img.height / 2.0)) }, 0, 1.0, WHITE);
	}
	else {
		enemy->x = -200;
		enemy->y = -200;
	}
}

void DisplayLives() {
	int i = 0;
	for (; i < lives; i++) DrawTextureEx(heart, { 50 + (float)i * 40, 50 }, 0, 0.1, WHITE); //lives left
	for (; i < 9; i++) DrawTextureEx(heart, { 50 + (float)i * 40, 50 }, 0, 0.1, {40, 40, 40, 255}); //shadows
}

void UpdateBullet(Bullet* bullet, Player* player, Object* objects, int num_objects) {
	float time = GetFrameTime();
	if (IsKeyPressed(KEY_J) && player->gun && !(bullet->fired)) {
		bullet->fired = true;
		bullet->hit = false;
		bullet->y = player->y + 20;
		if (player->state % 2) {
			bullet->direction = 1;
			bullet->x = player->x + player->img.width/2;
		}
		else {
			bullet->direction = -1;
			bullet->x = player->x - player->img.width / 2.0;
		}
	}
	if (bullet->fired) {
		for (int i = 0; i < num_objects; i++) {
			if (!objects[i].broken && CheckCollisionCircleRec({ bullet->x, bullet->y }, bullet->radius, objects[i].rect)){
				//if (ColorToInt(objects[i].color) == ColorToInt(PINK))
				//	objects[i].broken = true;
				bullet->hit = true;
			}
		}
		if (!bullet->hit) {
			DrawCircle(bullet->x, bullet->y, bullet->radius, bullet->color);
			bullet->x += BULLET_SPEED * time * bullet->direction;
		}
		else {
			bullet->fired = false;
		}
	}
}

void UpdatePauseButton(bool *pause) {
	GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
	if (GuiButton({ SCREEN_WIDTH - 100, 50, 43, 50 }, "") || IsKeyPressed(KEY_P)) *pause = !*pause;
	DrawRectangle(1830, 58, 8, 35, BLACK);
	DrawRectangle(1845, 58, 8, 35, BLACK);
}

void InitPlayer(Player* player, float x, float y, int state, bool gun ) {
	player->state = state;
	player->img = cat_textures[player->state];
	player->start = { x, y };
	player->x = player->start.x;
	player->y = player->start.y;
	//player->x = player->img.width;
	//player->y = SCREEN_HEIGHT - player->img.height - 30;
	player->speed.x = 0;
	player->speed.y = 0;
	player->can_jump = false;
	player->gun = gun;
	player->alive = true;
}

void InitEnemy(Enemy* enemy, float x, float y, int state) {
	enemy->state = state;
	enemy->img = enemy_textures[enemy->state];
	enemy->start = { x, y };
	enemy->x = enemy->start.x;
	enemy->y = enemy->start.y;
	//enemy->x = SCREEN_WIDTH / 2.0 + enemy->img.width / 2.0;
	//enemy->y = SCREEN_HEIGHT / 2.0 - 200 - enemy->img.height / 2.0;
	enemy->speed.x = 0;
	enemy->speed.y = 0;
	enemy->alive = true;
}

void cheat() {
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_L)) level++;
	if (IsKeyDown(KEY_LEFT_CONTROL) && !IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_L)) level--;

}

void DrawBorderedText(const char* text, int posX, int posY, int fontSize, Color color, int border, Color borderColor) {
	for (int i = 0; i < border; i++)
		DrawText(text, posX - i, posY + i, fontSize, borderColor); //border
	DrawText(text, posX, posY, fontSize, color); //border
}

