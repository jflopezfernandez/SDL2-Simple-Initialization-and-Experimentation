
#include "sdl2basic.h"

#define DEFAULT_SCREEN_WIDTH  800
#define DEFAULT_SCREEN_HEIGHT 600

static int screen_width  = DEFAULT_SCREEN_WIDTH;
static int screen_height = DEFAULT_SCREEN_HEIGHT;

int main(void)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Unable to initialize SDL2: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    // Disable the screen saver. It will automatically be re-enabled by SDL2
    // when the program quits, but it is explicitly re-enabled at the end of the
    // program, just in case.
    SDL_DisableScreenSaver();

    // Register 'SDL_EnableScreenSaver()' to exit callback function vector.
    if (atexit(SDL_EnableScreenSaver)) {
        // If this branch is triggered, it means the system failed to register
        // SDL_EnableScreenSaver to the exit function callback vector. It isn't
        // a huge deal, since SDL will re-enable it on exit anyways, but just
        // log it so we know.
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to register 'SDL_EnableScreenSaver' to atexit function vector.\n");
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        fprintf(stderr, "Unable to initialize hinting: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow(
        "Main Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 
        screen_width, 
        screen_height, 
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (window == NULL) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        fprintf(stderr, "Failed to initialize renderer: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    SDL_Texture* background = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA4444,
        SDL_TEXTUREACCESS_TARGET,
        screen_height,
        screen_width
    );

    if (background == NULL) {
        fprintf(stderr, "Failed to create background texture from image: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    SDL_Rect input_rect;

    if (SDL_GetDisplayBounds(0, &input_rect) < 0) {
        SDL_Log("SDL_GetDisplayBounds failed: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    static char text[4096];
    memset(text, 0, 4096);

    SDL_SetTextInputRect(&input_rect);

    while (TRUE) {
        SDL_Event event;

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_TEXTINPUT) {
                strcat(text, event.text.text);
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    break;
                } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    SDL_ShowSimpleMessageBox(
                        SDL_MESSAGEBOX_INFORMATION,
                        "Donald Trump is an idiot",
                        "This is obvious and requires no explanation.",
                        NULL
                    );
                } else if (event.key.keysym.scancode == SDL_SCANCODE_1) {
                    const SDL_MessageBoxButtonData buttons[] = {
                        { 0,                                       0, "No"     },
                        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"    },
                        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Cancel" }
                    };

                    const SDL_MessageBoxColorScheme color_scheme = {
                        {
                            { 255,   0,   0 },
                            {   0, 255,   0 },
                            { 255, 255,   0 },
                            {   0,   0, 255 },
                            { 255,   0, 255 }
                        }
                    };

                    const SDL_MessageBoxData message_box_data = {
                        SDL_MESSAGEBOX_INFORMATION,
                        NULL,
                        "Example Message Box",
                        "Select a Button",
                        SDL_arraysize(buttons),
                        buttons,
                        &color_scheme
                    };

                    int button_id = 0;

                    if (SDL_ShowMessageBox(&message_box_data, &button_id) < 0) {
                        SDL_Log("No selection was made.");
                    } else {
                        SDL_Log("User selected: %s\n", buttons[button_id].text);
                    }
                } else if (event.key.keysym.scancode == SDL_SCANCODE_KP_PLUS) {
                    SDL_MaximizeWindow(window);
                } else if (event.key.keysym.scancode == SDL_SCANCODE_KP_MINUS) {
                    SDL_MinimizeWindow(window);
                } else if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD) {
                    SDL_StartTextInput();
                } else if (event.key.keysym.scancode == SDL_SCANCODE_9) {
                    SDL_StopTextInput();
                    SDL_Log("%s\n", text);
                    SDL_zero(text);
                }
            }
            
            if (event.type == SDL_QUIT) {
                break;
            }
        }

        // Re-render the window.
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    // Clean up window resources.
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Terminate SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}

