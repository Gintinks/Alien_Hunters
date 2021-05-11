#include "Demo.h"



Demo::Demo()
{
	if (Mix_Playing(sfx_channel) == 0) {
		Mix_FreeChunk(sound);
	}
	if (music != NULL) {
		Mix_FreeMusic(music);
	}
	Mix_CloseAudio();
	
}


bool bullet = false;
bool collide = false;
bool bullet2 = false;
bool collide2 = false;

string show_score = "Score = 0",show_health = "Health = 3",StrScoreArray[5],over_score;

float vel = 0.05;
float velcopy = 0.05;
float range = 0.0;

Demo::~Demo()
{
}

void Demo::Init()
{
	for (int i = 0; i < 6; ++i) {
		ScoreArray[i] = 0;
	}
	InitText();
	InitButton();
	this->programGUI = BuildShader("shader.vert", "shader.frag");
	InputMapping("SelectButton", SDLK_RETURN);
	InputMapping("NextButton", SDLK_DOWN);
	InputMapping("PrevButton", SDLK_UP);
	InitAudio();
	AddInputs();
	BuildPlayerSprite();
	RecalibrateAlien();
	BuildBulletAlien();
	
}

//Renderer & Updater
void Demo::Update(float deltaTime)
{
	if (Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(gameplay_music, -1);
		SDL_Delay(150);
	}
	
	if (IsKeyDown("BGM")) {
			//If the music is paused
			if (Mix_PausedMusic() == 1)
			{
				//Resume the music
				Mix_ResumeMusic();
				SDL_Delay(150);
			}
			//If the music is playing
			else
			{
				//Pause the music
				Mix_PauseMusic();
				SDL_Delay(150);
			}
		
	}

	if (IsKeyDown("Quit")) {
		Mix_HaltMusic();
		activity = 0;
	}
	if (bullet2==false)
	{
		AlienShoot();
	}
	if (yposBulletAlien > 600) {
		bullet2 = false;
		collide2 = true;
	}
	yposBulletAlien += deltaTime * yVelocity;
	CollideAlienBullet();
	//UpdatePlayerSpriteAnim(deltaTime);
	ControlPlayerSprite(deltaTime);
	UpdateAlienSprite(deltaTime);


}

void Demo::UpdateGUI(float deltaTime)
{
	if (IsKeyDown("Quit")) {
		highscore = false;
	}

	if (IsKeyDown("SelectButton")) {
		if (activeButtonIndex == 2) {
			SDL_Quit();
			exit(0);
		}
		else if(activeButtonIndex == 0)
		{
			activeButtonIndex--;
			activity = 1;
			Mix_HaltMusic();
		}
		else if (activeButtonIndex == 1) {
			highscore = true;
			for (size_t i = 0; i < 5; i++)
			{
				StrScoreArray[i] = to_string(ScoreArray[i]);

			}
		}

	}
	
	if (IsKeyUp("NextButton")) {
		if (activeButtonIndex < NUM_BUTTON - 1) {
			activeButtonIndex++;
			sfx();
			SDL_Delay(300);
		}
	}

	if (IsKeyUp("PrevButton")) {
		if (activeButtonIndex > 0) {
			activeButtonIndex--;
			sfx();
			SDL_Delay(300);
		}
	}

	//Music Update
	if (Mix_Playing(sfx_channel) == 0 && IsKeyDown("SFX")) {
		sfx_channel = Mix_PlayChannel(-1, sound, 0);
		if (sfx_channel == -1) {
			Err("Unable to play WAV file: " + string(Mix_GetError()));
		}
	}


	if (IsKeyDown("BGM")) {
		//If music is being played
		if (Mix_PlayingMusic() == 0)
		{
			//Play the music
			Mix_PlayMusic(music, -1);
			SDL_Delay(150);
		}
			//If the music is paused
			if (Mix_PausedMusic() == 1)
			{
				//Resume the music
				Mix_ResumeMusic();
				SDL_Delay(150);
			}
			//If the music is playing
			else
			{
				//Pause the music
				Mix_PauseMusic();
				SDL_Delay(150);
			}
		
	}
}

void Demo::RenderGUI()
{	
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programGUI, "projection"), 1, GL_FALSE, value_ptr(projection));
	if (highscore == true)
	{
		RenderText("<<<HIGH SCORE>>>", 200, 10, 1.5f, vec3(0, 219, 0));
		for (size_t i = 0; i < 5; i++)
		{
			RenderText("Player", 120, 150+(75*i), 1.0f, vec3(0, 219, 0));
			RenderText(StrScoreArray[i], 550, 150 + (75 * i), 1.0f, vec3(0, 219, 0));
		}
		return;
	}
	if (game_over == true)
	{
		RenderText("GAME OVER", 230, 200, 2.0f, vec3(255, 0, 0));
		RenderText(over_score, 280, 330, 1.0f, vec3(255, 0, 0));
		RenderText("Press Enter", 300, 400, 1.0f, vec3(255, 0, 0));
		if (IsKeyDown("SelectButton"))
		{
			game_over = false;
		}
		return;
	}
	RenderText("BGM ON/OF = Press M", 10, 570, 0.5f, vec3(255, 255, 255));
	RenderText("||| ALIEN HUNTER |||", 180, 20, 1.5f, vec3(0, 219, 0));
	RenderButton();
}

void Demo::Render()
{
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(51.0/255.0f, 51.0/255.0f, 255.0/255.0f, 1.0f);

	DrawPlayerSprite();

	//draw aliens
	for (size_t i = 0; i < Aliens_Num; i++)
	{
		if (dead[i] == true)
			continue;
		DrawAlienSprite(i);
	}
	if (bullet2 == true && collide2 != true) {
		DrawBulletAlien();
	}
	if (bullet == true && collide != true) {
		DrawBullet();
	}
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programGUI, "projection"), 1, GL_FALSE, value_ptr(projection));


	RenderText(show_score, 10, 10, 0.5f, vec3(255, 255, 255));
	RenderText(show_health, 700, 10, 0.5f, vec3(255, 255, 255));
}

//GUI
void Demo::InitText() {
	// Init Freetype
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		Err("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, FONTNAME, 0, &face)) {
		Err("ERROR::FREETYPE: Failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, FONTSIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAOGUI);
	glGenBuffers(1, &VBOGUI);
	glBindVertexArray(VAOGUI);
	glBindBuffer(GL_ARRAY_BUFFER, VBOGUI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL,
		GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Demo::RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	// Activate corresponding render state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UseShader(this->programGUI);

	glUniform3f(glGetUniformLocation(this->programGUI, "ourColor"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(this->programGUI, "text"), 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->programGUI, "ourTexture"), 0);
	mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(this->programGUI, "model"), 1, GL_FALSE, value_ptr(model));
	glBindVertexArray(VAOGUI);

	// Iterate through all characters
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character

		GLfloat vertices[] = {
			// Positions   // Texture Coords
			xpos + w,	ypos + h,	1.0f, 1.0f, // Bottom Right
			xpos + w,	ypos,		1.0f, 0.0f, // Top Right
			xpos,		ypos,		0.0f, 0.0f, // Top Left
			xpos,		ypos + h,	0.0f, 1.0f  // Bottom Left 
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBOGUI);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_QUADS, 0, 4);
		// Now advance cursors for next glyph (note that advance is number of 1 / 64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels(2 ^ 6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Demo::InitButton() {
	string buttons[NUM_BUTTON] = { "play-1.png", "score-1.png", "quit-2.png" };
	string hover_buttons[NUM_BUTTON] = { "play-2.png", "score-2.png", "quit-1.png" };

	glGenTextures(3, &textureGUI[0]);
	glGenTextures(3, &hover_textureGUI[0]);

	for (int i = 0; i < NUM_BUTTON; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, textureGUI[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		// Set up vertex data (and buffer(s)) and attribute pointers
		button_widthGUI[i] = (float)width / 3;
		button_heightGUI[i] = (float)height / 3;

		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, hover_textureGUI[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(hover_buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		 // Set up vertex data (and buffer(s)) and attribute pointers
		hover_button_widthGUI[i] = (float)width;
		hover_button_heightGUI[i] = (float)height;
	}

	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &VAO2GUI);
	glGenBuffers(1, &VBO2GUI);
	glBindVertexArray(VAO2GUI);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2GUI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Demo::RenderButton() {
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_BUTTON] = { GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3 };
	// Activate shader
	UseShader(this->programGUI);
	glUniform1i(glGetUniformLocation(this->programGUI, "text"), 0);

	glBindVertexArray(VAO2GUI);
	for (int i = 0; i < NUM_BUTTON; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (activeButtonIndex == i) ? hover_textureGUI[i] : textureGUI[i]);
		glUniform1i(glGetUniformLocation(this->programGUI, "ourTexture"), i + 1);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth() - button_widthGUI[i]) / 2, (i + 1) * 150, 0.0f));
		model = scale(model, vec3(button_widthGUI[i], button_heightGUI[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->programGUI, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

//Music Feature
void Demo::sfx() {
	//Music Update
	if (Mix_Playing(sfx_channel) == 0) {
		sfx_channel = Mix_PlayChannel(-1, sound, 0);
		if (sfx_channel == -1) {
			Err("Unable to play WAV file: " + string(Mix_GetError()));
		}
	}
}

void Demo::sfx_dead1() {
	//Music Update
	if (Mix_Playing(sfx_channel) == 0) {
		sfx_channel = Mix_PlayChannel(1,sound_alien_dies , 0);
		if (sfx_channel == -1) {
			Err("Unable to play WAV file: " + string(Mix_GetError()));
		}
	}
}

void Demo::sfx_dead2() {
	//Music Update
	if (Mix_Playing(sfx_channel) == 0) {
		sfx_channel = Mix_PlayChannel(2, sound_player_dies, 0);
		if (sfx_channel == -1) {
			Err("Unable to play WAV file: " + string(Mix_GetError()));
		}
	}
}

void Demo::AddInputs() {
	InputMapping("BGM", SDLK_m);
	InputMapping("SFX", SDLK_s);
}

void Demo::InitAudio() {
	int flags = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG;
	if (flags != Mix_Init(flags)) {
		Err("Unable to initialize mixer: " + string(Mix_GetError()));
	}

	int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		Err("Unable to initialize audio: " + string(Mix_GetError()));
	}


	music = Mix_LoadMUS("jkbomber.flac");
	if (music == NULL) {
		Err("Unable to load Music file: " + string(Mix_GetError()));
	}

	gameplay_music = Mix_LoadMUS("gameplay.ogg");
	if (gameplay_music == NULL) {
		Err("Unable to load Music file: " + string(Mix_GetError()));
	}

	sound = Mix_LoadWAV("laser.wav");
	if (sound == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}

	sound_alien_dies = Mix_LoadWAV("alien-dies.ogg");
	if (sound_alien_dies == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}

	sound_player_dies = Mix_LoadWAV("player-dies.ogg");
	if (sound_player_dies == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}
	sound_game_over = Mix_LoadMUS("gameover.wav");
	if (sound_game_over == NULL) {
		Err("Unable to load Music file: " + string(Mix_GetError()));
	}

}

//Gameplay

void Demo::RecalibrateAlien() {

	for (size_t yi = 0; yi < 5; ++yi)
	{
		for (size_t xi = 0; xi < 6; ++xi)
		{
			AlienXPOS[yi * 6 + xi] = 75 * xi + 300;
			AlienYPOS[yi * 6 + xi] = 60 * yi + 12;
		}
	}

	for (size_t i = 0; i < Aliens_Num; i++)
	{
		BuildAlienSprite(i);
		dead[i] = false;
	}
}

void Demo::UpdatePlayerSpriteAnim(float deltaTime)
{
	frame_dur += deltaTime;

	if (frame_dur > FRAME_DUR) {
		frame_dur = 0;
		if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

		// Pass frameIndex to shader
		GLint location = glGetUniformLocation(this->program, "frameIndex");
		UseShader(this->program);
		glUniform1i(location, frame_idx);
	}
}

void Demo::UpdateAlienSprite(float deltaTime) {
		AlienMove(deltaTime);
}

void Demo::AlienShoot() {
	srand((unsigned)time(0));
	random = rand() % 29;
	if (dead[random]==true)
	{
		return;
	}
	BuildBulletAlien();
	bullet2 = true;
	collide2 = false;
	xposBulletAlien = AlienXPOS[random];
	yposBulletAlien = AlienYPOS[random] + Aliens_height[random] + 10;
	yVelocity = 0.2f;
}

void Demo::AlienMove(float deltaTime) {
	
	if (AlienXPOS[0]<20) {
		vel *= -1;
	}
	if (AlienXPOS[4] > 680) {
		vel *= -1;
	}

	for (size_t i = 0; i < Aliens_Num; i++)
	{
		AlienXPOS[i] -= deltaTime * vel;
	}
	for (size_t i = 0; i < Aliens_Num; i++)
	{
		AlienYPOS[i] += deltaTime * 0.0040;
	}


}

void Demo::ControlPlayerSprite(float deltaTime)
{
	walk_anim = false;
	oldxpos = xpos;

	if (IsKeyDown("Move Right")&&xpos < 800-frame_width) {
		xpos += deltaTime * xVelocity;
		flip = 0;
		walk_anim = true;
	}

	if (ypos3 < 0) {
		bullet = false;
		collide = true;
	}

	if (IsKeyDown("Shoot") && bullet == false) {
		BuildBullet();
		bullet = true;
		collide = false;
	}

	if (IsKeyDown("Move Left") && xpos >0) {
		xpos -= deltaTime * xVelocity;
		flip = 1;
		walk_anim = true;
	}


	//bullet shot
	ypos3 -= deltaTime * yVelocity;

	// check collision between Player Bullet and Alien
	for (size_t i = 0; i < Aliens_Num; i++)
	{
		if (dead[i] == true)
		{
			continue;
		}
		if (IsCollided(AlienXPOS[i], AlienYPOS[i], Aliens_width[i], Aliens_height[i]
			, xpos3, ypos3, frame_width3, frame_height3) && bullet==true) {
			bullet = false;
			collide = true;
			dead[i] = true;
			score += 10;
			show_score = "Score = " + to_string(score);
			sfx_dead1();
			if ((score/10)%Aliens_Num==0)
			{
				RecalibrateAlien();
			}

		}
	}
}

void Demo::CollideAlienBullet() {
		
		if (IsCollided(xposBulletAlien, yposBulletAlien, frame_width4, frame_height4
			, xpos, ypos, frame_width, frame_height) && bullet2 == true) {
			bullet2 = false;
			collide2 = true;
			health -= 1;
			show_health = "Health = " + to_string(health);
			if (health == 0)
			{
				Mix_HaltMusic();
				if (Mix_PlayingMusic() == 0)
				{
					//Play the music
					Mix_PlayMusic(sound_game_over, 1);
					SDL_Delay(150);
				}
				RecalibrateAlien();
				ScoreArray[5] = score;
				sort();
				over_score = "Your Score =" + to_string(score);
				score = 0;
				health = 3;
				activity = 0;
				game_over = true;
				show_score = "Score = 0"; 
				show_health = "Health = 3";
				return;
			}
			sfx_dead2();
		}
	
}

void Demo::sort() {
	int n, i, j;
	int temp;
	n = 6;
	for (i = 0; i < n; i++)
	{
		for (j = i + 1; j < n; j++)
		{
			if (ScoreArray[i] < ScoreArray[j])
			{
				temp = ScoreArray[i];
				ScoreArray[i] = ScoreArray[j];
				ScoreArray[j] = temp;
			}
		}
	}
}

void Demo::DrawPlayerSprite() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos, ypos, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width, frame_height, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::BuildPlayerSprite()
{
	this->program = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->program);
	glUniform1f(glGetUniformLocation(this->program, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height ;
	unsigned char* image = SOIL_load_image("player.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width = ((float)width) / NUM_FRAMES;
	frame_height = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos = (GetScreenWidth() - frame_width) / 2;
	yposGround = GetScreenHeight() - frame_height;
	ypos = yposGround;
	xVelocity = 0.2f;

	// Add input mapping
	// to offer input change flexibility you can save the input mapping configuration in a configuration file (non-hard code) !
	InputMapping("Move Right", SDLK_RIGHT);
	InputMapping("Move Left", SDLK_LEFT);
	InputMapping("Move Right", SDLK_d);
	InputMapping("Move Left", SDLK_a);
	InputMapping("Move Right", SDL_BUTTON_RIGHT);
	InputMapping("Move Left", SDL_BUTTON_LEFT);
	InputMapping("Move Right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	InputMapping("Move Left", SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	InputMapping("Shoot", SDLK_SPACE);
	InputMapping("Quit", SDLK_ESCAPE);
}

void Demo::BuildAlienSprite(int id) {
	this->program4 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program4);

	// Load and create a texture 
	glGenTextures(num_aliens, &texture_Alien[id]);
	glBindTexture(GL_TEXTURE_2D, texture_Alien[id]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("alien_alive_1.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	Aliens_width[id] = (float)width ;
	Aliens_height[id] = (float)height ;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);

	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program4, "projection"), 1, GL_FALSE, value_ptr(projection));


}

void Demo::DrawAlienSprite(int id) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_Alien[id]);
	// Activate shader
	UseShader(this->program4);
	glUniform1i(glGetUniformLocation(this->program4, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(AlienXPOS[id], AlienYPOS[id], 0.0f));
	// Scale sprite 
	model = scale(model, vec3(Aliens_width[id], Aliens_height[id], 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program4, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO4);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void Demo::BuildBullet() {
	this->program3 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program3);

	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("bullet.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width3 = (float)width * 0.3;
	frame_height3 = (float)height * 0.3;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos3 = xpos + 12;
	ypos3 = ypos - frame_height+10;
	yVelocity = 0.2f;
}

void Demo::DrawBullet() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// Activate shader
	UseShader(this->program3);
	glUniform1i(glGetUniformLocation(this->program3, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos3, ypos3, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width3, frame_height3, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO3);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void Demo::DrawBulletAlien() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// Activate shader
	UseShader(this->program5);
	glUniform1i(glGetUniformLocation(this->program5, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposBulletAlien, yposBulletAlien, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_height4, frame_height4, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program5, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO5);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void Demo::BuildBulletAlien() {
	this->program5 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program5);

	// Load and create a texture 
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("alienBullet.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width4 = (float)width /2;
	frame_height4 = (float)height/2;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glGenBuffers(1, &EBO5);

	glBindVertexArray(VAO5);

	glBindBuffer(GL_ARRAY_BUFFER, VBO5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program5, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposBulletAlien = 300;
	yposBulletAlien = 300;
}

bool Demo::IsCollided(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2) {
	return (x1 < x2 + width2 && x1 + width1 > x2 && y1 < y2 + height2 && y1 + height1 > y2);
}


int main(int argc, char** argv) {

	Engine::Game &game = Demo();
	game.Start("Alien Hunter", 800, 600, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
