// [main.c]
// this template is provided for the 2D shooter game.

#define _CRT_SECURE_NO_DEPRECATE
#include<iostream>
using namespace std;
#include<ctime>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <sstream>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED
clock_t lastTime;
bool timeUp = false;

/* Constants. */
const float MAX_COOLDOWN = 0.7f;
double last_shoot_timestamp = 0;
const float gravity = 15;
// Frame rate (frame per second)
const int FPS = 25;
// Display (screen) width.
const int SCREEN_W = 1500;
// Display (screen) height.
const int SCREEN_H = 1000;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;

const float movespeed = 20;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
    SCENE_MENU = 1,
    SCENE_START = 2,
    // [HACKATHON 3-7]
    // TODO: Declare a new scene id.
    SCENE_SETTINGS = 3,
    SCENE_GAME_END = 4
};

/* Input states */
// The active scene id.
int active_scene;/********************/
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...

/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_64;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_FONT* font_opensans_regular_64;
ALLEGRO_FONT* font_arial_64;
ALLEGRO_FONT* font_roboto_regular_64;
// TODO: More shared resources or data that needed to be accessed
// across different scenes.

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
// [HACKATHON 3-1]
// TODO: Declare 2 variables for storing settings images.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_principle;
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;

/* Game end Scene resources*/
ALLEGRO_SAMPLE* death_bgm;
ALLEGRO_SAMPLE_ID death_bgm_id;

/*Shooting resource*/
ALLEGRO_SAMPLE* shoot_bgm;
ALLEGRO_SAMPLE_ID shoot_bgm_id;
#define MAX_CAT_PIC 24
#define MAX_INVENTORY 3
#define MAX_BULLET 100
#define MAX_ENEMY 2
float BOX_RESIZED_RATIO = 0.3;

class Inventory {
public:
    float x, y;

    float w, h;

    bool hidden;

    ALLEGRO_BITMAP* img;
};
void draw_inventory_weapon(int weapon);

Inventory inventory[MAX_INVENTORY];
ALLEGRO_BITMAP* inventory_img[MAX_INVENTORY]; /************/

// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_bullet;

class MovableObject {
public:
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the objectís image.
    ALLEGRO_BITMAP* img;

};
void draw_movable_object(MovableObject obj);


// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
MovableObject bullets[MAX_BULLET];

class CouncilMan {
public:
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the objectís image.
    ALLEGRO_BITMAP* img[MAX_ENEMY];

    int picIndex;


};
void draw_councilMan(void);
CouncilMan councilMan;

class Cat {
public:
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the objectís image.
    ALLEGRO_BITMAP* img[MAX_CAT_PIC];

    bool active;
    bool isRight;
    bool canJump;
    int picIndex;
    int jumpSpeed;
    bool isAccelarating;

};
void draw_cat(void);
Cat cat;

class Trap
{
public:
    int x, y;

    int w, h;

    const int yMin = 950;

    const int yMax = 1500;

    int moveSpeed;

    int shooting_fort;

    bool canUse;

    ALLEGRO_BITMAP* img;
};
enum trapCondition { trapOut = 1, trapBack = 2 };
#define MAX_SPIKE 3
Trap spike[MAX_SPIKE];

// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.

// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.


/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
// [HACKATHON 3-2]
// TODO: Declare a function.
// Determines whether the point (px, py) is in rect (x, y, w, h).
// Uncomment the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);
bool checkCollisionMovObj(MovableObject bullet, Cat enemy);
bool checkCollisionCatTrap(Cat, Trap);
bool checkCollisionTrapCat(Trap, Cat);
bool collision(int item_x, int item_y, int item_w, int item_h, int item2_x, int item2_y, int item2_w, int item2_h);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);



int main(int argc, char** argv) {
    // Set random seed for better random outcome.
//    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    // Initialize game variables.
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");

    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    if (!al_install_audio())
        game_abort("failed to initialize audio add-on");
    if (!al_init_acodec_addon())
        game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
        game_abort("failed to reserve samples");
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");
    if (!al_install_mouse())
        game_abort("failed to install mouse");
    // TODO: Initialize other addons such as video, ...

    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");
    al_set_window_title(game_display, "2019 PD Final Project <8888>"); /**********cc**********/

    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");

    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");

    // Malloc mouse buttons state according to button counts.
    const unsigned m_buttons = al_get_mouse_num_buttons();
    game_log("There are total %u supported mouse buttons", m_buttons);
    // mouse_state[0] will not be used.
    mouse_state = (bool*)malloc((m_buttons + 1) * sizeof(bool));
    memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

    // Register display, timer, keyboard, mouse events to the event queue.
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_mouse_event_source());
    // TODO: Register other event sources such as timer, video, ...
    //al_register_event_source(game_event_queue, al_get_video());
    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void) {
    /* Shared resources*/
    font_pirulen_64 = al_load_font("pirulen.ttf", 64, 0);
    if (!font_pirulen_64)
        game_abort("failed to load font: pirulen.ttf with size 32");

    font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
    if (!font_pirulen_24)
        game_abort("failed to load font: pirulen.ttf with size 24");
    font_opensans_regular_64 = al_load_font("opensans_regular.ttf", 64, 0);
    if (!font_opensans_regular_64)
        game_abort("failed to load font: opensans_regular.ttf with size 64");

    font_arial_64 = al_load_font("arial.ttf", 64, 0);
    if (!font_arial_64)
        game_abort("failed to load font: .ttf with size 64");

    font_roboto_regular_64 = al_load_font("arial.ttf", 64, 0);
    if (!font_roboto_regular_64)
        game_abort("failed to load font: roboto_regular.ttf with size 64");

    /* Menu Scene resources*/
    main_img_background = load_bitmap_resized("main-bg.jpg", SCREEN_W, SCREEN_H);

    main_bgm = al_load_sample("beginning.wav");
    if (!main_bgm)
        game_abort("failed to load audio: beginning.wav");

    start_bgm = al_load_sample("theme.wav");
    if (!start_bgm)
        game_abort("failed to load audio: theme.wav");

    death_bgm = al_load_sample("death.wav");
    if (!death_bgm)
        game_abort("failed to load audio: death.wav");

    shoot_bgm = al_load_sample("shoot.wav");
    if (!shoot_bgm)
        game_abort("failed to load audio: shoot.wav");

    // [HACKATHON 3-4]
    // TODO: Load settings images.
    // Don't forget to check their return values.
    // Uncomment and fill in the code below.
    img_settings = al_load_bitmap("lightbulb.png");
    if (!img_settings)
        game_abort("failed to load image: lightbulb.png");
    img_principle = load_bitmap_resized("principle.png", SCREEN_W, SCREEN_H);
    if (!img_principle)
        game_abort("failed to load image: principle.png");

    /* Start Scene resources*/
    start_img_background = load_bitmap_resized("start-bg.jpg", SCREEN_W, SCREEN_H);

    //start_img_plane = al_load_bitmap("1.png");
//    if (!start_img_plane)
//        game_abort("failed to load image: 1.png");
    for (int i = 0; i < MAX_INVENTORY; i++) {
        switch (i)
        {
        case 0:
            inventory_img[i] = load_bitmap_resized("car.png", 90, 90);
            break;
        case 1:
            inventory_img[i] = load_bitmap_resized("trap2.png", 180, 90);
            break;
        case 2:
            inventory_img[i] = load_bitmap_resized("net.png", 90, 90);
            break;
        }
    }

    for (int i = 0; i < MAX_CAT_PIC; i++) {
        std::stringstream str;
        str << i + 1 << ".png";
        cat.img[i] = al_load_bitmap(str.str().c_str());
    }
    for (int i = 0; i < MAX_ENEMY; i++) {
        std::stringstream str;
        str << "councilMan" << i + 1 << ".png";
        councilMan.img[i] = al_load_bitmap(str.str().c_str());
    }
    if (!councilMan.img)
        game_abort("failed to load image:councilMan.png");

    for (int i = 0; i < MAX_SPIKE; i++) {
        spike[i].img = al_load_bitmap("trap2.png");
        if (!spike[i].img)
            game_abort("failed to load image: trap2.png");
    }
    // [HACKATHON 2-5]
    // TODO: Initialize bullets.
    // 1) Search for a bullet image online and put it in your project.
    //    You can use the image we provided.
    // 2) Load it in by 'al_load_bitmap' or 'load_bitmap_resized'.
    // 3) If you use 'al_load_bitmap', don't forget to check its return value.
    // Uncomment and fill in the code below.
    img_bullet = load_bitmap_resized("car.png",50,50);
    if (!img_bullet)
        game_abort("failed to load image: car.png");
    // Change to first scene.
    game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    int redraws = 0;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Event for clicking the window close button.
            game_log("Window close button clicked");
            done = true;
        }
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer)
                // The redraw timer has ticked.
                redraws++;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Event for keyboard key down.
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
            on_key_down(event.keyboard.keycode);
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            // Event for keyboard key up.
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Event for mouse key down.
            game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = true;
            on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            // Event for mouse key up.
            game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (event.mouse.dx != 0 || event.mouse.dy != 0) {
                // Event for mouse move.
                game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
            }
            else if (event.mouse.dz != 0) {
                // Event for mouse scroll.
                game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
            }
        }
        // TODO: Process more events and call callbacks by adding more entries inside Scene.
        // Redraw
        if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
            // if (redraws > 1)
            //     game_log("%d frame(s) dropped", redraws - 1);
            // Update and draw the next frame.
            game_update();
            game_draw();
            redraws = 0;
        }
    }
}

void game_update(void) {
    if (active_scene == SCENE_START) {
        clock_t eTime = clock();
        if ((eTime - lastTime) / CLOCKS_PER_SEC >= 60) {
            game_change_scene(SCENE_GAME_END);
            timeUp = true;
        }

        // TRAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP block
        int i;
        if (key_state[ALLEGRO_KEY_B] && spike[0].canUse) {
            for (i = 0; i < MAX_SPIKE; i++)
            {
                spike[i].shooting_fort = trapOut;
            }

        }
        for (i = 0; i < MAX_SPIKE; i++)
        {
            switch (spike[i].shooting_fort) {
            case trapOut:
                spike[i].y -= spike[i].moveSpeed;
                spike[i].canUse = false;
                break;
            case trapBack:
                spike[i].y += spike[i].moveSpeed;
                spike[i].canUse = false;
                break;
            case -1:
                spike[i].canUse = true;
            }
        }

        for (i = 0; i < MAX_SPIKE; i++)
        {
            if (spike[i].shooting_fort == trapOut) {
                if (spike[i].y <= spike[i].yMin)
                    spike[i].shooting_fort = trapBack;
            }
            else if (spike[i].shooting_fort == trapBack) {
                if (spike[i].y >= spike[i].yMax)
                    spike[i].shooting_fort = -1;
            }
        }

        for (int i = 0; i < MAX_SPIKE; i++)
        {
            if (checkCollisionCatTrap(cat, spike[i]) || checkCollisionTrapCat(spike[i], cat)) {
                game_change_scene(SCENE_GAME_END);
            }
        }

        // TRAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP block end

        // CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT block
        cat.vx = cat.vy = 0;
        cat.active = true;
        councilMan.vx = councilMan.vy = 0;
        if (key_state[ALLEGRO_KEY_UP] && cat.canJump) {
            cat.isAccelarating = true;
        }

        if (key_state[ALLEGRO_KEY_LEFT]) {
            cat.isRight = false;
            cat.vx -= movespeed;
        }

        if (key_state[ALLEGRO_KEY_RIGHT]) {
            cat.isRight = true;
            cat.vx += movespeed;
        }

        if ((!key_state[ALLEGRO_KEY_RIGHT]) && (!key_state[ALLEGRO_KEY_LEFT]) && (!key_state[ALLEGRO_KEY_UP])) {
            cat.active = false;
        }

        if (cat.active == true && cat.isRight == true) {
            cat.picIndex++;
            if (cat.picIndex > 11)
                cat.picIndex = 0;
        }

        else if (cat.active == false && cat.isRight == true) {
            cat.picIndex = 7;
        }

        if (cat.active == true && cat.isRight == false) {
            if (cat.picIndex < 12) {
                cat.picIndex = 19;
            }
            cat.picIndex++;
            if (cat.picIndex > 23) {
                cat.picIndex = 12;
            }

        }

        else if (cat.active == false && cat.isRight == false) {
            cat.picIndex = 19;
        }

        if (!cat.isAccelarating) {
            cat.vy += gravity;
        //    game_log("notAccelarating\n");
        }
        else if (cat.isAccelarating) {
            cat.vy -= gravity * 1.8;
        //    game_log("Accelarating\n");
            if ((cat.y + cat.h) <= (SCREEN_H - 50 - cat.jumpSpeed))
                cat.isAccelarating = false;
        }

        cat.y += cat.vy * (cat.vx ? 0.71f : 1);
        cat.x += cat.vx * (cat.vy ? 0.71f : 1);

        cat.canJump = ((cat.y + cat.h) >= (SCREEN_H - 50));//the pixel for the height parameter of catWalks are 175, so we write (1200 -175) to let the cat touch the ground
        if (cat.canJump) {
            cat.y = SCREEN_H - 50 - cat.h;
            std::cout << "true";
        }
        if (cat.x <= cat.w / 2)          /*********c********/
            cat.x = cat.w / 2;
        if (cat.x >= (SCREEN_W - cat.w / 2))
            cat.x = (SCREEN_W - cat.w / 2);
        // CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT block

        if (key_state[ALLEGRO_KEY_A])
            councilMan.vx -= movespeed;
        if (key_state[ALLEGRO_KEY_D])
            councilMan.vx += movespeed;
        // 0.71 is (1/sqrt(2)).

        councilMan.x += councilMan.vx;

        // [HACKATHON 1-1]
        // TODO: Limit the plane's collision box inside the frame.
        //       (x, y axes can be separated.)
        // Uncomment and fill in the code below.

        if (councilMan.x <= councilMan.w / 2)
            councilMan.x = councilMan.w / 2;
        else if (councilMan.x >= SCREEN_W - councilMan.w / 2)
            councilMan.x = SCREEN_W - councilMan.w / 2;
        if (councilMan.y < 0)
            councilMan.y = 0;
        else if (councilMan.y > SCREEN_H)
            councilMan.y = SCREEN_H;
        if (!key_state[ALLEGRO_KEY_SPACE])
            councilMan.picIndex = 0;
        // [HACKATHON 2-7]
        // TODO: Update bullet coordinates.
        // 1) For each bullets, if it's not hidden, update x, y
        // according to vx, vy.
        // 2) If the bullet is out of the screen, hide it.
        // Uncomment and fill in the code below.

        for (i = 0; i < MAX_BULLET; i++) {
            if (bullets[i].hidden)
                continue;
            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;
            if (bullets[i].y < 0)
                bullets[i].hidden = true;
            for (int j = 0; j < MAX_ENEMY; j++)
            {
                if (checkCollisionMovObj(bullets[i], cat)) {
                    bullets[i].hidden = true;
                    game_change_scene(SCENE_GAME_END);
                }
            }
        }

        // [HACKATHON 2-8]
        double now = al_get_time();
        if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
            for (i = 0; i < MAX_BULLET; i++) {
                if (bullets[i].hidden)
                    break;
            }
            if (i < MAX_BULLET) {
                last_shoot_timestamp = now;
                bullets[i].hidden = false;
                bullets[i].x = councilMan.x;
                bullets[i].y = councilMan.y + councilMan.h / 2;
                if (!al_play_sample(shoot_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &shoot_bgm_id))
                    game_abort("failed to play audio (bgm)");
            }
            councilMan.picIndex = 1;
        }

        // set inventory item hidden
        for (i = 0; i < MAX_INVENTORY; i++)
        {
            if (inventory[i].hidden == false && (key_state[ALLEGRO_KEY_SPACE] || key_state[ALLEGRO_KEY_B] || key_state[ALLEGRO_KEY_V])) {
                inventory[i].hidden = true;
            }
        }
        if (key_state[ALLEGRO_KEY_SPACE])
            inventory[0].hidden = false;
        if (key_state[ALLEGRO_KEY_B])
            inventory[1].hidden = false;
        //    if (key_state[ALLEGRO_KEY_V])
        //        inventory[2].hidden = false;

    }
}

void draw_inventory_weapon(int weapon) {
    if (inventory[weapon].hidden)
        return;
    al_draw_bitmap(inventory[weapon].img, SCREEN_W - 180, 5, 0);
}

void draw_cat(void) {
    al_draw_bitmap(cat.img[cat.picIndex], cat.x - cat.w / 2, cat.y - cat.h / 2, NULL);
//    al_draw_rectangle(cat.x - cat.w / 2, cat.y - cat.h / 2, cat.x + cat.w / 2, cat.y + cat.h / 2, al_map_rgb(255, 0, 0), 3);
}
void draw_councilMan(void) {
    al_draw_bitmap(councilMan.img[councilMan.picIndex], councilMan.x - councilMan.w / 2, councilMan.y - councilMan.h / 2, NULL);
//    al_draw_rectangle(councilMan.x - councilMan.w / 2, councilMan.y - councilMan.h / 2, councilMan.x + councilMan.w / 2, councilMan.y + councilMan.h / 2, al_map_rgb(255, 0, 0), 3);
}

void draw_trap(void) {
    for (int i = 0; i < 3; i++) {
        al_draw_bitmap(spike[i].img, spike[i].x - spike[i].w / 2, spike[i].y - spike[i].h / 2, NULL);
//        al_draw_rectangle(spike[i].x - spike[i].w / 2, spike[i].y - spike[i].h / 2, spike[i].x + spike[i].w / 2, spike[i].y + spike[i].h / 2, al_map_rgb(255, 0, 0), 3);
    }
}

void game_draw(void) {
    if (active_scene == SCENE_MENU) {
        al_draw_bitmap(main_img_background, 0, 0, 0);
        al_draw_text(font_pirulen_64, al_map_rgb(255, 255, 255), SCREEN_W / 2, 60, ALLEGRO_ALIGN_CENTER, "Leopard cat v.s. Council Man");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to start  1.6");
        // [HACKATHON 3-5]
        // TODO: Draw settings images.
        // The settings icon should be located at (x, y, w, h) =
        // (SCREEN_W - 48, 10, 38, 38).
        // Change its image according to your mouse position.
        // Uncomment and fill in the code below.
        timeUp = false;
        al_draw_bitmap(img_settings, SCREEN_W - 48, 10, 0);
    }
    else if (active_scene == SCENE_START) {
        int i;
        al_draw_bitmap(start_img_background, 0, 0, 0);
        // [HACKATHON 2-9]
        // TODO: Draw all bullets in your bullet array.
        // Uncomment and fill in the code below.
        for (i = 0; i < MAX_BULLET; i++)            /******************************/
            draw_movable_object(bullets[i]);
        //al_draw_rectangle(1400, 0, 1500, 100, al_map_rgb(255, 0, 0), 5);

        draw_cat();
        draw_councilMan();
        // draw inventory weapon icon
        for (i = 0; i < MAX_INVENTORY; i++)
            draw_inventory_weapon(i);

        draw_trap();

    }
    // [HACKATHON 3-9]
    // TODO: If active_scene is SCENE_SETTINGS.
    // Draw anything you want, or simply clear the display.
    else if (active_scene == SCENE_SETTINGS) {
        al_draw_bitmap(img_principle, 0, 0, 0);
    }
    else if (active_scene == SCENE_GAME_END) {
        al_draw_bitmap(main_img_background, 0, 0, 0);
        if (timeUp) {
            al_draw_text(font_pirulen_64, al_map_rgb(255, 0, 0), SCREEN_W / 2, 120, ALLEGRO_ALIGN_CENTER, "Time up!");
            al_draw_text(font_opensans_regular_64, al_map_rgb(255, 0, 0), SCREEN_W / 2, 180, ALLEGRO_ALIGN_CENTER, "Winner: Leopard cat!!");
        }
        else {
            al_draw_text(font_pirulen_64, al_map_rgb(255, 0, 0), SCREEN_W / 2, 120, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
            al_draw_text(font_opensans_regular_64, al_map_rgb(255, 0, 0), SCREEN_W / 2, 180, ALLEGRO_ALIGN_CENTER, "Winner: Council Man!!");

        }
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 60, ALLEGRO_ALIGN_CENTER, "Press enter key to restart");


    }
    al_flip_display();
}

void game_destroy(void) {
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // Destroy shared resources.
    al_destroy_font(font_pirulen_64);
    al_destroy_font(font_pirulen_24);
    al_destroy_font(font_arial_64);
    al_destroy_font(font_opensans_regular_64);
    al_destroy_font(font_roboto_regular_64);

    /* Menu Scene resources*/
    al_destroy_bitmap(main_img_background);
    al_destroy_sample(main_bgm);
    // [HACKATHON 3-6]
    // TODO: Destroy the 2 settings images.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_settings);
    al_destroy_bitmap(img_principle);

    /* Start Scene resources*/
    al_destroy_bitmap(start_img_background);
    al_destroy_bitmap(councilMan.img[0]);
    al_destroy_sample(start_bgm);
    al_destroy_sample(death_bgm);
    /* Game End Scene resources*/
    al_destroy_sample(shoot_bgm);
    // [HACKATHON 2-10]
    // TODO: Destroy your bullet image.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_bullet);

    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
    free(mouse_state);
}

void game_change_scene(int next_scene) {
    game_log("Change scene from %d to %d", active_scene, next_scene);
    // TODO: Destroy resources initialized when creating scene.
    if (active_scene == SCENE_MENU) {
        al_stop_sample(&main_bgm_id);
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_START) {
        al_stop_sample(&start_bgm_id);
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_SETTINGS) {
        al_stop_sample(&main_bgm_id);
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_GAME_END) {
        al_stop_sample(&death_bgm_id);
        game_log("stop audio (bgm)");
    }

    active_scene = next_scene;                           /********************/
    // TODO: Allocate resources before entering scene.
    if (active_scene == SCENE_MENU) {
        if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
    else if (active_scene == SCENE_SETTINGS) {          /**********c**********/
        if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
    else if (active_scene == SCENE_GAME_END) {          /**********c**********/
        if (!al_play_sample(death_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &death_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
    else if (active_scene == SCENE_START) {
        lastTime = clock();
        int i;
        cat.x = 400;
        cat.y = SCREEN_H - 10;
        cat.w = al_get_bitmap_width(cat.img[0]);
        cat.h = al_get_bitmap_height(cat.img[0]);
        cat.active = false;
        cat.isRight = true;
        cat.canJump = false;
        cat.picIndex = 0;
        cat.jumpSpeed = 100;
        cat.isAccelarating = false;
        councilMan.w = al_get_bitmap_width(councilMan.img[0]);
        councilMan.h = al_get_bitmap_height(councilMan.img[0]);
        councilMan.x = councilMan.w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - councilMan.w);  /*************/
        councilMan.y = 80;
        councilMan.picIndex = 0;
        // [HACKATHON 2-6]
        // TODO: Initialize bullets.
        // For each bullets in array, set their w and h to the size of
        // the image, and set their img to bullet image, hidden to true,
        // (vx, vy) to (0, -3).
        // Uncomment and fill in the code below.
        for (i = 0; i < MAX_BULLET; i++) {
            bullets[i].w = al_get_bitmap_width(img_bullet);
            bullets[i].h = al_get_bitmap_height(img_bullet);
            bullets[i].img = img_bullet;
            bullets[i].vx = 0;
            bullets[i].vy = 10;
            bullets[i].hidden = true;
        }



        for (i = 0; i < MAX_INVENTORY; i++) {             /******************/
            inventory[i].img = inventory_img[i];
            inventory[i].w = 90;
            inventory[i].h = 90;
            inventory[i].hidden = true;
        }

        int spikeX[] = { 258, 672, 1086 };
        for (i = 0; i < MAX_SPIKE; i++)
        {
            spike[i].x = spikeX[i];
            spike[i].y = spike[i].yMax;
            spike[i].moveSpeed = 15;
            spike[i].shooting_fort = -1;
            spike[i].w = al_get_bitmap_width(spike[i].img);
            spike[i].h = al_get_bitmap_height(spike[i].img);
            spike[i].canUse = true;
        }

        if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
}

void on_key_down(int keycode) {
    if (active_scene == SCENE_MENU) {
        if (keycode == ALLEGRO_KEY_ENTER)
            game_change_scene(SCENE_START);
    }
    if (active_scene == SCENE_GAME_END) {
        if (keycode == ALLEGRO_KEY_ENTER)
            game_change_scene(SCENE_MENU);
    }

}

void on_mouse_down(int btn, int x, int y) {                //*************failed//
    // [HACKATHON 3-8]
    // TODO: When settings clicked, switch to settings scene.
    // Uncomment and fill in the code below.
    if (active_scene == SCENE_MENU) {                             /******** c********/
        if (btn > 0) {
            if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38)) {
                game_change_scene(SCENE_SETTINGS);
                btn = 0;
            }
        }
    }
    if (active_scene == SCENE_SETTINGS) { /******** c********/
        if (btn > 0) {
            if (pnt_in_rect(x, y, SCREEN_W - 108, 10, SCREEN_W, 78))
                game_change_scene(SCENE_START);
            btn = 0;
        }
    }
}

void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
//    al_draw_rectangle(obj.x - obj.w / 2, obj.y - obj.h / 2, obj.x + obj.w / 2, obj.y + obj.h / 2, al_map_rgb(255, 0, 0), 3);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
    ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
        game_abort("failed to load image: %s", filename);
    ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
    ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();

    if (!resized_bmp)
        game_abort("failed to create bitmap when creating resized image: %s", filename);
    al_set_target_bitmap(resized_bmp);
    al_draw_scaled_bitmap(loaded_bmp, 0, 0,
        al_get_bitmap_width(loaded_bmp),
        al_get_bitmap_height(loaded_bmp),
        0, 0, w, h, 0);
    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);

    game_log("resized image: %s", filename);

    return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
    return ((px > x && px < (x + w)) && (py > y && py < (y + h)));
}

bool checkCollisionMovObj(MovableObject bullet, Cat enemy) {
    return collision(bullet.x - bullet.w / 2, bullet.y - bullet.h / 2, bullet.w, bullet.h, enemy.x - enemy.w / 2, enemy.y - enemy.h / 2, enemy.w, enemy.h);
}
bool checkCollisionCatTrap(Cat trap, Trap enemy) {
    return collision(trap.x - trap.w / 2, trap.y - trap.h / 2, trap.w, trap.h, enemy.x - enemy.w / 2, enemy.y - enemy.h / 2, enemy.w, enemy.h);
}

bool checkCollisionTrapCat(Trap trap, Cat enemy) {
    return collision(trap.x - trap.w / 2, trap.y - trap.h / 2, trap.w, trap.h, enemy.x - enemy.w / 2, enemy.y - enemy.h / 2, enemy.w, enemy.h);
}

bool collision(int item_x, int item_y, int item_w, int item_h, int item2_x, int item2_y, int item2_w, int item2_h) {

    item2_x = item2_x + BOX_RESIZED_RATIO * item2_w;
    item2_y = item2_y + BOX_RESIZED_RATIO * item2_h;
    item2_w *= (1 - 2 * BOX_RESIZED_RATIO);
    item2_h *= (1 - 2 * BOX_RESIZED_RATIO);
    int leftX = item_x;
    int rightX = item_x + item_w;
    int upY = item_y;
    int downY = item_y + item_h;

    // up-left corner
    if ((leftX >= item2_x && leftX <= (item2_x + item2_w)) && (upY >= item2_y && upY <= (item2_y + item2_h)))
        return true;
    // up-right corner
    if ((rightX >= item2_x && rightX <= (item2_x + item2_w)) && (upY >= item2_y && upY <= (item2_y + item2_h)))
        return true;
    // down-left corner
    if ((leftX >= item2_x && leftX <= (item2_x + item2_w)) && (downY >= item2_y && downY <= (item2_y + item2_h)))
        return true;
    // down-right corner
    if ((rightX >= item2_x && rightX <= (item2_x + item2_w)) && (downY >= item2_y && downY <= (item2_y + item2_h)))
        return true;

    return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}
