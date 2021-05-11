#ifndef DEMO_H
#define DEMO_H


#include <SOIL/SOIL.h>

#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>
#include <ctime>

#define FONTNAME "BigSpace.ttf"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

#define NUM_FRAMES 1
#define FRAME_DUR 80
#define Aliens_Num 30

using namespace glm;

#define FONTSIZE 40
#define NUM_BUTTON 3

struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	ivec2 Size; // Size of glyph
	ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	//Gameplay
	virtual void Update(float deltaTime);
	virtual void Render();
	//GUI
	virtual void UpdateGUI(float deltaTime);
	virtual void RenderGUI();
	//music
	void InitAudio();
	void AddInputs();
	int score=0, health = 3, ScoreArray[6];
	int random;
	float frame_width = 0, frame_height = 0, frame_width2 = 0, frame_height2 = 0, frame_width3 = 0, frame_height3 = 0,
		Aliens_width[Aliens_Num], Aliens_height[Aliens_Num], frame_width4 = 0, frame_height4 = 0;
private:
	void sfx();
	void sfx_dead1();
	void sfx_dead2();
	void InitText();
	void RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void InitButton();
	void RenderButton();
	map<GLchar, Character> Characters;
	float frame_dur = 0, oldxpos = 0, xpos = 0, ypos = 0, xpos2 = 0, ypos2 = 0, xpos3 = 0,
		ypos3 = 0, AlienXPOS[Aliens_Num], AlienYPOS[Aliens_Num], xposBulletAlien = 0, yposBulletAlien =0,
		gravity = 0, xVelocity = 0, yVelocity = 0, yposGround = 0;
	GLuint VBO, VAO, EBO, texture, program, 
		VBO2, VAO2, EBO2, texture2, program2,
		VBO3, VAO3, EBO3, texture3, program3,
		VBO4, VAO4, EBO4, texture_Alien[Aliens_Num], program4,
		VBO5, VAO5, EBO5, texture4, program5,
		//GUI
		textureGUI[NUM_BUTTON], hover_textureGUI[NUM_BUTTON], VBOGUI, VBO2GUI, VAOGUI, VAO2GUI, programGUI;
	float button_widthGUI[NUM_BUTTON], button_heightGUI[NUM_BUTTON], hover_button_widthGUI[NUM_BUTTON], hover_button_heightGUI[NUM_BUTTON];
	int activeButtonIndex = -2;
	bool walk_anim = false, onGround = true, dead[Aliens_Num], highscore = false, game_over= false;
	unsigned int frame_idx = 0, flip = 0;
	void RecalibrateAlien();
	void BuildBulletAlien();
	void BuildBullet();
	void BuildPlayerSprite();
	void BuildAlienSprite(int id);
	void DrawBulletAlien();
	void DrawBullet();
	void DrawAlienSprite(int id);
	void DrawPlayerSprite();
	void CollideAlienBullet();
	bool IsCollided(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);
	void UpdatePlayerSpriteAnim(float deltaTime);
	void ControlPlayerSprite(float deltaTime);
	void AlienMove(float deltaTime);
	void sort();
	void AlienShoot();
	void UpdateAlienSprite(float deltaTime);

	//music
	Mix_Chunk* sound = NULL;
	Mix_Chunk* sound_player_dies = NULL;
	Mix_Chunk* sound_alien_dies = NULL;
	Mix_Music* sound_game_over = NULL;
	Mix_Music* music = NULL;
	Mix_Music* gameplay_music = NULL;
	int sfx_channel = -1;
};
#endif

