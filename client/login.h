#ifndef CLIENT_LOGIN_H
#define CLIENT_LOGIN_H

#include "mylib.h"
#include "socklib.h"
#include "character.h"
#include "request.pb-c.h"
#include "response.pb-c.h"

#define MSG_INPUT_HINT  "👋 Input your nickname: "
#define MSG_CHOOSE_CLASS_HINT  "⚔️ Choose your class"
#define MSG_EXIT_HINT  "Input ESC to exit."
#define MSG_BACK_HINT  "Input ESC to go back."
#define MSG_NAME_TOO_SHORT  "🤷 ‍Nickname must be longer than 5 characters."
#define MSG_NAME_TOO_LONG "🤷 Nickname must be shorter than 15 characters."
#define MSG_CANNOT_CONNECT "🤷 Cannot connect to server"
#define MSG_NICKNAME_LIMIT "🤷 Nickname must be number or english character."
#define MSG_WAITING_RESPONSE "⌛ Waiting for response..."

void login();

#endif //CLIENT_LOGIN_H
