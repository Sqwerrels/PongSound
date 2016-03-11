#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Matrix.h"
#include "ShaderProgram.h"


#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

struct Ball {
public:
	float x;
	float y;
	Ball(int currentX, int currentY)
	{
		x = currentX;
		y = currentY;
	}
	
private:

};

struct Paddle {
public:
	float x;
	float y;
	Paddle(int currentX,int currentY)
	{
		x = currentX;
		y = currentY;
	}
private:


};

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,	GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	SDL_FreeSurface(surface);

	return textureID;

}


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif




	SDL_Event event;
	bool done = false;

	glViewport(0, 0, 640, 360);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	GLuint bluePaddleTexture = LoadTexture("blueRectangle.png");
	GLuint greenPaddleTexture = LoadTexture("greenRectangle.png");
	GLuint emojiTexture = LoadTexture("emoji3.png");
	GLuint greenBallTexture = LoadTexture("greenCircle2.png");
	Ball greenBall(0.25, 0.25);
	Paddle greenPaddle(-3.35,0.5);
	Paddle bluePaddle(3.35,0.5);
	Matrix projectionMatrix;
	Matrix modelMatrixbluePaddle;
	Matrix modelMatrixgreenPaddle;
	Matrix modelMatrixgreenBall;
	Matrix viewMatrix;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk *someSound;
	someSound = Mix_LoadWAV("ballSound.wav");
	Mix_Music *music;
	music = Mix_LoadMUS("music.mp3");

	
	bool ballDown = true;
	bool ballLeft = true;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	float lastFrameTicks = 0.0f;

	Mix_PlayMusic(music, -1);
	
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		float vertices2[] = { 0, 0, 0.25, 0, 0.25, 0.25, 0, 0, 0.25, 0.25, 0, 0.25 };
		//GreenPaddle
		float vertices1[] = { -3.35f, -.5f, -3.2f, -.5f, -3.2f, .5f, -3.35f, -.5f, -3.2f, .5f, -3.35f, .5f };

		float shiftX = 0;
		float shiftY = 0;
		shiftX += 0.5f * elapsed;
		shiftY += 1.0f * elapsed;

		if (greenPaddle.y < 1.5){
			if (keys[SDL_SCANCODE_W]){
				modelMatrixgreenPaddle.Translate(0, shiftY, 0);
				greenPaddle.y += shiftY;
			}
		}
		if (greenPaddle.y > -1.5){
			if (keys[SDL_SCANCODE_S])
			{
				modelMatrixgreenPaddle.Translate(0, -shiftY, 0);
				greenPaddle.y -= shiftY;
			}
		}
		if (bluePaddle.y < 1.5){
			if (keys[SDL_SCANCODE_UP]){
				modelMatrixbluePaddle.Translate(0, shiftY, 0);
				bluePaddle.y += shiftY;
			}
		}
		if (bluePaddle.y > -1.5) {
			if (keys[SDL_SCANCODE_DOWN])
			{
				modelMatrixbluePaddle.Translate(0, -shiftY, 0);
				bluePaddle.y -= shiftY;
			}
		}
		if (keys[SDL_SCANCODE_R])
			modelMatrixgreenBall.setPosition(0.5, 0.5, 0);
		if (greenBall.y >= 1.5)
		{
			ballDown = true;
			Mix_PlayChannel(-1, someSound, 0);
		}
		if (greenBall.y <= -1.5)
		{
			ballDown = false;
			Mix_PlayChannel(-1, someSound, 0);
		}
		if (ballDown)
		{
			modelMatrixgreenBall.Translate(0, -.35*shiftY, 0);
			greenBall.y -= .35* shiftY;

		}
		else
		{
			modelMatrixgreenBall.Translate(0, .35*shiftY, 0);
			greenBall.y += .35*shiftY;
		}

		if (ballLeft && greenBall.x > -3.55)
		{
			modelMatrixgreenBall.Translate(-shiftX, 0, 0);
			greenBall.x -= shiftX;

		}
		
		
		if (greenBall.y > (greenPaddle.y - 1) && greenBall.y <= greenPaddle.y +.5 && greenBall.x <= -3.2)
		{
			Mix_PlayChannel(-1, someSound, 0);
			ballLeft = false;
		}
		if (!ballLeft && greenBall.x < 3.2)
		{
			modelMatrixgreenBall.Translate(shiftX, 0, 0);
			greenBall.x += shiftX;
		}
		if (greenBall.y >(bluePaddle.y - 1) && greenBall.y <= bluePaddle.y +.5  && greenBall.x >= 2.97)
		{
			Mix_PlayChannel(-1, someSound, 0);
			ballLeft = true;
		}
		if (greenBall.x >= 3.3 || greenBall.x <= -3.3)
		{
			modelMatrixgreenBall.setPosition(0, 0, 0);
			greenBall.x = 0;
			greenBall.y = 0;

		}
		

		glClear(GL_COLOR_BUFFER_BIT);

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glColor3f(0.0f, 0.0f, 0.0f);

		
		
		//GREEN PADDLE
		program.setModelMatrix(modelMatrixgreenPaddle);

		glBindTexture(GL_TEXTURE_2D, greenPaddleTexture);

		
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords1[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
		glEnableVertexAttribArray(program.texCoordAttribute);


		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		
		glBindTexture(GL_TEXTURE_2D, bluePaddleTexture);


		//BLUE PADDLE
		program.setModelMatrix(modelMatrixbluePaddle);

		float vertices3[] = { 3.35f, -.5f, 3.2f, -.5f, 3.2f, .5f, 3.35f, -.5f, 3.2f, .5f, 3.35f, .5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(program.texCoordAttribute);


		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		
		//GREEN BALL
		
		program.setModelMatrix(modelMatrixgreenBall);

		glBindTexture(GL_TEXTURE_2D, greenBallTexture);


		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);



		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
