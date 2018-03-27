// #include<stdio.h> //printf
// #include<string.h>    //strlen
// #include<sys/socket.h>    //socket
// #include<arpa/inet.h> //inet_addr

// #define HEIGHT 30
// #define WIDTH 30

// int 

// int main(int argc , char *argv[])
// {
//     // int sock;
//     // struct sockaddr_in server;
//     // char message[1000] , server_reply[2000];

//     // //Create socket
//     // sock = socket(AF_INET , SOCK_STREAM , 0);
//     // if (sock == -1)
//     // {
//     //     printf("Could not create socket");
//     // }
//     // puts("Socket created");

//     // server.sin_addr.s_addr = inet_addr("127.0.0.1");
//     // server.sin_family = AF_INET;
//     // server.sin_port = htons( 8888 );

//     // //Connect to remote server
//     // if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
//     // {
//     //     perror("connect failed. Error");
//     //     return 1;
//     // }

//     // puts("Connected\n");

//     // //keep communicating with server
//     // while(1)
//     // {
//     //     printf("Enter message : ");
//     //     scanf("%s" , message);

//     //     //Send some data
//     //     if( send(sock , message , strlen(message) , 0) < 0)
//     //     {
//     //         puts("Send failed");
//     //         return 1;
//     //     }

//     //     //Receive a reply from the server
//     //     if( recv(sock , server_reply , 2000 , 0) < 0)
//     //     {
//     //         puts("recv failed");
//     //         break;
//     //     }

//     //     puts("Server reply :");
//     //     puts(server_reply);
//     // }

//     // close(sock);
//     return 0;
// }

#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <time.h>

#define DESIRED_WIDTH  70
#define DESIRED_HEIGHT 25
#define BUFFLEN 2048

WINDOW * g_mainwin;
int g_width, g_height;
bool *spaces;
int sock;
char key = 'd';
char key2 = 'd';

int snake_length = 1, x, y;

typedef struct{
    int x;
    int y;
} node;

void snake_write_text(int y, int x, char *str){
    mvwaddstr( g_mainwin, y , x, str );
}

void snake_draw_board( ){
    int i;
    for( i=0; i<g_height; i++ )
    {
        snake_write_text( i, 0,         "X" );
        snake_write_text( i, g_width-1, "X" );
    }
    for( i=1; i<g_width-1; i++ )
    {
        snake_write_text( 0, i,          "X" );
        snake_write_text( g_height-1, i, "X" );
    }
}

void snake_game_over( node *snake1 )
{
    int i;
    free( spaces );
    free(snake1);
    
    endwin();
    exit(0);
}

bool snake_in_bounds( node position )
{
    return position.y < g_height - 1 && position.y > 0 && position.x < g_width - 1 && position.x > 0;
}

int snake_cooridinate_to_index( node position )
{
    return g_width * position.y + position.x;
}

node snake_index_to_coordinate( int index )
{
    int x = index % g_width;
    int y = index / g_width;
    return (node) { x, y };
}

bool snake_move_enemy( node *snake1 )
{
    int i;
    int idx;

    for(i = 0; i < snake_length; i++){
        idx = snake_cooridinate_to_index( snake1[i] );
        if( spaces[idx] )
            snake_game_over(snake1);
        spaces[idx] = true;
    }
    
    // Check if we're eating the fruit
    // if( head.x == fruit.x && head.y == fruit.y )
    // {
    //     snake_draw_fruit( );
    //     g_score += 1000;
    // }
    // else
    // {
        // Handle the tail
        
    
        // node tail = snake1[snake_length];
        // spaces[snake_cooridinate_to_index( tail )] = false;
        // snake_write_text( tail.y, tail.x, " " );
        
    // }
    
    snake_write_text( snake1[0].y, snake1[0].x, "Z" );
    
}


bool snake_move_player( node *snake1 )
{
    int i;
    int idx;

    for(i = 0; i < snake_length; i++){
        idx = snake_cooridinate_to_index( snake1[i] );
        if( spaces[idx] )
            snake_game_over(snake1);
        spaces[idx] = true;
    }
    
    // Check if we're eating the fruit
    // if( head.x == fruit.x && head.y == fruit.y )
    // {
    //     snake_draw_fruit( );
    //     g_score += 1000;
    // }
    // else
    // {
        // Handle the tail
        
    
        // node tail = snake1[snake_length];
        // spaces[snake_cooridinate_to_index( tail )] = false;
        // snake_write_text( tail.y, tail.x, " " );
        
    // }
    
    snake_write_text( snake1[0].y, snake1[0].x, "S" );
    
}

void snake_get_data() {
    if(recv(sock, &x, BUFFLEN, 0) == -1) {
        printf("%s\n", "Server reply(x) failed");
    }
    if(recv(sock, &y, BUFFLEN, 0) == -1) {
        printf("%s\n", "Server reply(y) failed");
    }
    // if(recv(sock, &snake_length, BUFFLEN, 0) == -1) {
    //     printf("%s\n", "Server reply failed");
    // }
}

void recieve_event(node *snake2){
    while(1){
        if (recv(sock, &key2, sizeof(key2), 0) == -1) {
            printf("%s\n", "Server reply fail(event)");
        }else{
            switch( key2 )
            {
                case 's':
                    snake2[0].y++;
                    break;
                case 'd':
                    snake2[0].x++;
                    break;
                case 'w':
                    snake2[0].y--;
                    break;
                case 'a':
                    snake2[0].x--;
                    break;
            }
            if( !snake_in_bounds( snake2[0] ) )    
                snake_game_over( snake2 );
            else
                snake_move_enemy( snake2 );
        }
    }
    snake_game_over( snake2 );
}

int main(int argc, char *argv[]){
    
    struct sockaddr_in server;

    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    snake_get_data();

    node snake1[snake_length];
    node snake2[snake_length];

    if( ( g_mainwin = initscr() ) == NULL ) {
        perror( "error initialising ncurses" );
        exit( EXIT_FAILURE );
    }
    
    srand( time( NULL ) );
    noecho( );
    curs_set( 2 );
    halfdelay( 1 );
    keypad( g_mainwin, TRUE );
    getmaxyx( g_mainwin, g_height, g_width );
    
    g_width  = g_width  < DESIRED_WIDTH  ? g_width  : DESIRED_WIDTH;
    g_height = g_height < DESIRED_HEIGHT ? g_height : DESIRED_HEIGHT; 
    
    spaces = (bool*) malloc( sizeof( bool ) * g_height * g_width );

    snake_draw_board( );
    
    snake1[0].x = x;
    snake1[0].y = y;
    snake2[0].x = x;
    snake2[0].y = y + 5;
    // snake1[0].x = 5;
    // snake1[0].y = 5;
    // snake2[0].x = 15;
    // snake2[0].y = 15;

    snake_move_enemy(snake2);
    // recieve_event(snake2);
    while(1){
        int in = getch( );
            if( in != ERR )
                key = in;
            switch( key )
            {
                case 's':
                    snake1[0].y++;
                    break;
                case 'd':
                    snake1[0].x++;
                    break;
                case 'w':
                    snake1[0].y--;
                    break;
                case 'a':
                    snake1[0].x--;
                    break;
            }

        if( !snake_in_bounds( snake1[0] ) )    
            snake_game_over( snake1 );
        else
            snake_move_player( snake1 );
    }
    snake_game_over( snake1);
    if (send(sock, &key, sizeof(key), 0) == -1) {
	  	printf("%s\n", "Send failure");
	}

    close(sock);
    return 0;
}