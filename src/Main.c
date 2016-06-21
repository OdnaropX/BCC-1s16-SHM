//
//  main.c
//  PI6 - TowerDefense
//
//  Created by Danilo Makoto Ikuta on 10/09/15.
//  Copyright (c) 2015 Danilo Makoto Ikuta. All rights reserved.
//

//Main Menu (Loads SDL and main menu assets)

#include <SDL2/SDL.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <math.h>
	#include <string.h>
	
	#ifdef _WIN32 //Load lib for windows or mac.
		#include <windows.h>
		#include <Lmcons.h>
		#define windows 1
		#include <SDL2/SDL_image.h>
		#include <SDL2/SDL_ttf.h>
        #include <SDL2/SDL_mixer.h>
		#include <SDL2/SDL_net.h>
		#include <SDL2/SDL_thread.h>
	#else
		#define windows 0
		#include <SDL2_image/SDL_image.h>
		#include <SDL2_ttf/SDL_ttf.h>
        #include <SDL2_mixer/SDL_mixer.h>
		#include <SDL2_net/SDL_net.h>
		#include <SDL2/SDL_thread.h>
		#include <unistd.h>
	#endif
	

#define FRAMES_PER_SEC 60

//Global variable
///////////////////////////////////////////////////////////////////////
//SDL stuff
SDL_Window *main_Window;
SDL_Surface *main_Surface;
SDL_Renderer *renderer;

//Media
SDL_Texture *background_Texture;

//Constants
SDL_Color black = {0, 0, 0, 255};
SDL_Color white = {255, 255, 255, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {255, 0, 0, 255};

//Cabeçalhos
bool main_init();
void main_quit();

typedef enum _game_status{PLAYER_TYPPING, PLAYER_FINISH, NEXT_PLAYER, WAITING} STATUS;

int main(int argc, char * argv[]) {
	int sequencia_player[2][1000], i;
	int seq_pos = 0, reset_timer = 0, timer_count = 0, player_finished = 0;
    bool quit = false;
	
	//Game status
    STATUS game_status = NEXT_PLAYER;
	int player = 0, begin_sequencia_player = 0;
	
    //Main init
    if(!main_init()){
        quit = true;
    }
 
	//Game events
	SDL_Event event;

    int score[2] = {0,0};
	
	//FPS and timer
    int t1, t2;
    int delay = 17; //Aprox. de 1000ms/60
	int frame = 0;

	printf("Start game\n");
    //Main loop
    while(!quit){
        t1 = SDL_GetTicks();
        //Event Handler
        ///////////////////////////////////////////////////
		while(SDL_PollEvent(&event) != 0){
			switch (event.type) {
				//Quit
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYUP: //Handle keyboard key release event.
					switch(event.key.keysym.sym){
						case SDLK_UP: //Sequencia 0
							sequencia_player[player][seq_pos] = 0;
							printf("Digitado %d (Jogador: %d)\n", sequencia_player[player][seq_pos], player);
							seq_pos += 1;
							reset_timer = 1;
							break;
						case SDLK_LEFT: //Sequencia 1
							sequencia_player[player][seq_pos] = 1;
							printf("Digitado %d (Jogador: %d)\n", sequencia_player[player][seq_pos], player);
							seq_pos += 1;
							reset_timer = 1;
							break;
						case SDLK_RIGHT: //Sequencia 2
							sequencia_player[player][seq_pos] = 2;
							printf("Digitado %d (Jogador: %d)\n", sequencia_player[player][seq_pos], player);
							seq_pos += 1;
							reset_timer = 1;
							break;
						case SDLK_DOWN: //Sequencia 3
							sequencia_player[player][seq_pos] = 3;
							printf("Digitado %d (Jogador: %d)\n", sequencia_player[player][seq_pos], player);
							seq_pos += 1;
							reset_timer = 1;
							break;
						case SDLK_ESCAPE:
							quit = true;
							break;
					}
					break;
			}
		}
     
		
		
        //Timer handling
		////////////////////////////////////////////////////////
		frame++;
		if (frame == FRAMES_PER_SEC) {
			frame = 0;
			
			//One second timer
			if(reset_timer){
				timer_count = 0;
				reset_timer = 0;
				game_status = PLAYER_TYPPING;
			}
			else {
				timer_count++;
			}
		}
		
		//Action Performancer
		/////////////////////////////////////////////////////
		switch(game_status){
			case PLAYER_TYPPING:
				//Wait finish typping.
				if(timer_count > 3) {
					game_status = PLAYER_FINISH;
					player_finished++;
					printf("Jogada finalizada\n");
				}
				break;
			case PLAYER_FINISH:
				//Check if both players already typed.
				if(player_finished > 1){
					bool equal = true;
					//Compare game
					for(i = 0; i < seq_pos; i++){
						if(sequencia_player[0][i] != sequencia_player[1][i]){
							equal = false;
							break;
						}
					}
					
					if(equal){
						printf("(Servo) 0 (Move ambos)\n");
						printf("Draw\n");
					}
					else {
						printf("(Servo) %d (contrai folha do jogador %d)\n", (begin_sequencia_player + 1) % 2 , (begin_sequencia_player + 1) % 2);
						printf("Winner %d\n", begin_sequencia_player + 1);
						score[begin_sequencia_player] += 1;
					}
					printf("Pontuação:\nJogador 1: %d\nJogador 2: %d\n", score[0], score[1]);
					
					player_finished = 0;
					begin_sequencia_player = (begin_sequencia_player + 1 ) % 2;
					player = begin_sequencia_player;
				}

				player = (player + 1) % 2; 
				game_status = NEXT_PLAYER;
				seq_pos = 0;
				break;
			case NEXT_PLAYER:
				//Send message to arduino of player and LED
				printf("Player %d: Begin LED\n", player + 1);
				game_status = WAITING;
				break;
			case WAITING:
				//Do nothing.
				break;
		}
		
	
	
		// Select the color for drawing.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

        //Clear render
        SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, background_Texture, NULL, NULL);
		
		//Scene Renderer 
		/////////////////////////////////////////////////////
       
        //Update render and surfaces
        SDL_RenderPresent(renderer);
		
		//FPS Handling
        t2 = SDL_GetTicks() - t1;
        if(t2 < delay){
            SDL_Delay(delay - t2);
        }
    }
    
						
    //Quit
    main_quit();
    return 0;
}

//Init SDL, configs e menu principal
bool main_init(){    
    //Init SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL_Init error: %s\n", SDL_GetError());
        return false;
    }
    
    //Create window
    main_Window = SDL_CreateWindow("Grade Defender", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
    if(!main_Window){
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return false;
    }
    
    //Set main_Surface
    main_Surface = SDL_CreateRGBSurface(0, 1280, 720, 32, 0, 0, 0, 0);
    
    //Init renderer
    renderer = SDL_CreateRenderer(main_Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	SDL_Surface *background_Surface;
	//Selector
	if(windows){
		background_Surface = IMG_Load("images/Main Fundo.png");
	}
	else {
		background_Surface = IMG_Load("../images/Main Fundo.png");
	}
	
	background_Texture = SDL_CreateTextureFromSurface(renderer, background_Surface);
	
	SDL_FreeSurface(background_Surface);
	
    return true;
}

//Encerra SDL
void main_quit(){       
    //////free window
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(main_Window)
        SDL_DestroyWindow(main_Window);
    
    //Quit SDL, SDLNet, TTF, IMG, SDL_Mixer
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

