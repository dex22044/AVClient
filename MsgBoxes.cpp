#include "MsgBoxes.h"

void ErrorMessageBox(const char* message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical error", message, NULL);
}