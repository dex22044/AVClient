#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "GUIBaseElements.h"
#include "ScreenCapture.h"
#include "MediaEncoder.h"
#include <thread>
#include "MsgBoxes.h"

SDL_Window* mainWindow;
SDL_Renderer* mainRenderer;
TTF_Font* font;
GUIButton* connectBtn;
GUITextBox* otherClientIDBox;
GUIImage* remoteImageBox;
GUIContainer* mainTab;
GUIContainer* imageTab;
GUITabBox* tabBox;
GUIButton* tabBoxB1;
GUIButton* tabBoxB2;
int mouseX;
int mouseY;
bool mousePressed;
SDL_KeyboardEvent key;

SDL_Texture* displayImage;

ScreenCapture* screenCaptureModule;
MediaEncoder* encoderModule;

void DrawGUILayer(SDL_Renderer* renderer) {
	tabBox->update(mouseX, mouseY, mousePressed, key);
	tabBox->draw(renderer, 0);
	if (connectBtn->isPressedDelta) {
		tabBox->currentSelectedIndex = 1;
		connectBtn->isPressedDelta = false;
	}
}

void ScreenCaptureThread() {
	AVFrame* frame = av_frame_alloc();
	frame->width = screenCaptureModule->mode.Width;
	frame->height = screenCaptureModule->mode.Height;
	frame->format = AV_PIX_FMT_YUV444P;
	int size = avpicture_get_size(AV_PIX_FMT_YUV444P, frame->width, frame->height);
	uint8_t* frameData = (uint8_t*)av_malloc(size);
	avpicture_fill((AVPicture*)frame, frameData, AV_PIX_FMT_YUV444P, frame->width, frame->height);
	uint64_t len = frame->width * frame->height;

	while (true) {
		screenCaptureModule->CaptureFrame();

		for (uint64_t i = 0; i < len; i++) {
			frame->data[0][i] = screenCaptureModule->pixels[i * 4 + 0];
			frame->data[1][i] = screenCaptureModule->pixels[i * 4 + 1];
			frame->data[2][i] = screenCaptureModule->pixels[i * 4 + 2];
		}

		AVPacket pkt = *encoderModule->EncodeVideoFrame(frame);
		printf("Frame encoded, size: %d bytes;\n", pkt.size);

		av_free_packet(&pkt);
	}
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	av_register_all();

	screenCaptureModule = new ScreenCapture();

	screenCaptureModule->InitCapture();

	encoderModule = new MediaEncoder(screenCaptureModule->mode.Width, screenCaptureModule->mode.Height);

	mainWindow = SDL_CreateWindow("AVClient", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
	mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_SOFTWARE);

	displayImage = SDL_CreateTexture(mainRenderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, screenCaptureModule->mode.Width, screenCaptureModule->mode.Height);

	std::thread capThr(ScreenCaptureThread);

#pragma region GUI initialization
	mainTab = new GUIContainer();
	imageTab = new GUIContainer();

	tabBox = new GUITabBox();
	tabBox->buttonsHeight = 30;

	tabBoxB1 = new GUIButton();
	tabBoxB2 = new GUIButton();

	tabBox->addTab(mainTab, tabBoxB1);
	tabBox->addTab(imageTab, tabBoxB2);

	tabBoxB1->rect.x = 0;
	tabBoxB1->rect.y = 0;
	tabBoxB1->rect.w = 70;
	tabBoxB1->rect.h = 50;
	tabBoxB1->bgColor = CreateColor(127, 127, 127, 255);
	tabBoxB1->hoverbgColor = CreateColor(192, 192, 192, 255);
	tabBoxB1->activebgColor = CreateColor(255, 255, 255, 255);
	tabBoxB1->borderColor = CreateColor(63, 63, 63, 255);
	tabBoxB1->fgColor = CreateColor(20, 20, 20, 255);
	tabBoxB1->borderThickness = 5;
	tabBoxB1->text = (char*)"Tab 1";

	tabBoxB2->rect.x = 0;
	tabBoxB2->rect.y = 0;
	tabBoxB2->rect.w = 70;
	tabBoxB2->rect.h = 50;
	tabBoxB2->bgColor = CreateColor(127, 127, 127, 255);
	tabBoxB2->hoverbgColor = CreateColor(192, 192, 192, 255);
	tabBoxB2->activebgColor = CreateColor(255, 255, 255, 255);
	tabBoxB2->borderColor = CreateColor(63, 63, 63, 255);
	tabBoxB2->fgColor = CreateColor(20, 20, 20, 255);
	tabBoxB2->borderThickness = 5;
	tabBoxB2->text = (char*)"Tab 2";

	mainTab->rect.x = 0;
	mainTab->rect.y = 0;
	mainTab->rect.w = 1280;
	mainTab->rect.h = 720;

	imageTab->rect.x = 0;
	imageTab->rect.y = 0;
	imageTab->rect.w = 1280;
	imageTab->rect.h = 720;

	connectBtn = new GUIButton();
	otherClientIDBox = new GUITextBox();
	remoteImageBox = new GUIImage();

	otherClientIDBox->rect.x = 50;
	otherClientIDBox->rect.y = 100;
	otherClientIDBox->rect.w = 400;
	otherClientIDBox->rect.h = 50;
	otherClientIDBox->bgColor = CreateColor(127, 127, 127, 255);
	otherClientIDBox->hoverbgColor = CreateColor(192, 192, 192, 255);
	otherClientIDBox->activebgColor = CreateColor(255, 255, 255, 255);
	otherClientIDBox->borderColor = CreateColor(63, 63, 63, 255);
	otherClientIDBox->fgColor = CreateColor(20, 20, 20, 255);
	otherClientIDBox->borderThickness = 5;
	otherClientIDBox->text = (char*)"";

	connectBtn->rect.x = 500;
	connectBtn->rect.y = 100;
	connectBtn->rect.w = 100;
	connectBtn->rect.h = 50;
	connectBtn->bgColor = CreateColor(127, 127, 127, 255);
	connectBtn->hoverbgColor = CreateColor(192, 192, 192, 255);
	connectBtn->activebgColor = CreateColor(255, 255, 255, 255);
	connectBtn->borderColor = CreateColor(63, 63, 63, 255);
	connectBtn->fgColor = CreateColor(20, 20, 20, 255);
	connectBtn->borderThickness = 5;
	connectBtn->text = (char*)"Connect";

	remoteImageBox->rect.x = 550;
	remoteImageBox->rect.y = 100;
	remoteImageBox->rect.w = 500;
	remoteImageBox->rect.h = 500;
	remoteImageBox->bgColor = CreateColor(127, 127, 127, 255);
	remoteImageBox->hoverbgColor = CreateColor(192, 192, 192, 255);
	remoteImageBox->activebgColor = CreateColor(255, 255, 255, 255);
	remoteImageBox->borderColor = CreateColor(63, 63, 63, 255);
	remoteImageBox->borderThickness = 5;

	mainTab->addElement(connectBtn);
	mainTab->addElement(otherClientIDBox);
	imageTab->addElement(remoteImageBox);
#pragma endregion
	remoteImageBox->image = displayImage;

	SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);

	font = TTF_OpenFont("./arial.ttf", 24);

	connectBtn->font = font;
	otherClientIDBox->font = font;
	tabBoxB1->font = font;
	tabBoxB2->font = font;

	bool isRunning = true;

	int frame = 0;

	while (isRunning) {
		SDL_Event ev;
		key.state = SDL_RELEASED;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				isRunning = false;
			}
			else if (ev.type == SDL_MOUSEMOTION) {
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
			}
			else if (ev.type == SDL_MOUSEBUTTONDOWN) {
				mousePressed = true;
			}
			else if (ev.type == SDL_MOUSEBUTTONUP) {
				mousePressed = false;
			}
			else if (ev.type == SDL_KEYDOWN) {
				key = ev.key;
			}
		}
		frame++;

		int ww = 0, wh = 0;
		SDL_GetWindowSize(mainWindow, &ww, &wh);

		remoteImageBox->rect.x = 0;
		remoteImageBox->rect.y = 30;
		remoteImageBox->rect.w = ww;
		remoteImageBox->rect.h = wh - 30;

		SDL_UpdateTexture(displayImage, NULL, screenCaptureModule->pixels, screenCaptureModule->mode.Width * 4);

		SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 255);
		SDL_RenderClear(mainRenderer);
		DrawGUILayer(mainRenderer);
		SDL_RenderPresent(mainRenderer);
		SDL_Delay(5);
	}

	TTF_CloseFont(font);

	SDL_Quit();
	TTF_Quit();
	
	return 0;
}