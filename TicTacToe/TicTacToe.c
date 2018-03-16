#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char currentPlayer = 'X';
GtkWidget **square_buttons;
GtkWidget *label;
GtkWidget *reset_button;
int gameOver = 0;
int moveCount = 0;

static void set_win() {
  char buf[8];
  sprintf(buf, "%c Wins!", currentPlayer);
  gtk_label_set_text(GTK_LABEL (label), buf);
  gameOver = 1;
}

//This method feels a little dirty, but I'm not certain how I can do it less explicitly
static void check_win() {
  char *currentSymbol;
  if(currentPlayer == 'X') currentSymbol = "X";
  else currentSymbol = "O";
  //Rows
  for(int i=0; i<3; i++) {
    int isWon = 1;
    for(int j=0; j<3; j++) {
      if(strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[(3*i)+j])), currentSymbol)) isWon = 0;
    }
    if(isWon) {
      set_win();
      return;
    }
  }

  //Columns
  for(int i=0; i<3; i++) {
    int isWon = 1;
    for(int j=0; j<3; j++) {
      if(strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[(3*j)+i])), currentSymbol)) isWon = 0;
    }
    if(isWon) {
      set_win();
      return;
    }
  }

  //Left Diagonal
  if(!strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[0])), currentSymbol) &&
      !strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[4])), currentSymbol) &&
      !strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[8])), currentSymbol)) {
    set_win();
    return;
  }

  //Right Diagonal
  if(!strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[2])), currentSymbol) &&
      !strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[4])), currentSymbol) &&
      !strcmp(gtk_button_get_label(GTK_BUTTON (square_buttons[6])), currentSymbol)) {
    set_win();
    return;
  }

  //If the board is full and nobody has won, end game as tie
  if(moveCount >= 9) {
    gtk_label_set_text(GTK_LABEL (label), "Tie Game :(");
    gameOver = 1;
  }
}



static void reset(GtkWidget *widget, gpointer data) {
  for(int i=0; i<9; i++) {
    gtk_button_set_label(GTK_BUTTON (square_buttons[i]), " ");  
  }
  currentPlayer = 'X';
  gtk_label_set_text(GTK_LABEL (label), "Current Player: X");
  gameOver = 0;
  moveCount = 0;
}

//Called when one of the game board buttons is clicked
static void clicked(GtkWidget *widget, gpointer data) {
  //Ensure that clicked square isn't already filled and that the game is still going
  if(!strcmp(gtk_button_get_label(GTK_BUTTON (widget)), " ") && !gameOver) {
    
    //Set Button Text
    char playerStr [2];
    sprintf(playerStr, "%c", currentPlayer);
    gtk_button_set_label(GTK_BUTTON (widget), playerStr);
    
    //Update label to reflect current player
    char labelStr [18];
    sprintf(labelStr, "Current Player: %c", currentPlayer);
    gtk_label_set_text(GTK_LABEL (label), labelStr);
    
    moveCount++;
    check_win();
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
  }
}

static void activate(GtkApplication *app, gpointer userdata) {
  GtkWidget *window;
  window = gtk_application_window_new(app);
  
  GtkWidget *grid;
  grid = gtk_grid_new();
  
  square_buttons = (GtkWidget **)malloc(9*sizeof(GtkWidget *));
  
  //Set up all the game board buttons
  for(int i=0; i<9; i++) {
    square_buttons[i] = gtk_button_new();

    //If left empty, will return NULL causing errors in checking for game win
    gtk_button_set_label(GTK_BUTTON (square_buttons[i]), " ");

    //Set up function to run when button clicked
    g_signal_connect(square_buttons[i], "clicked", G_CALLBACK (clicked), NULL);
    
    //Make buttons expand to fill the window
    gtk_widget_set_vexpand(square_buttons[i], TRUE);
    gtk_widget_set_hexpand(square_buttons[i], TRUE);
    
    //Place button in grid
    gtk_grid_attach(GTK_GRID (grid), square_buttons[i], i/3, i%3, 1, 1);
  }

  //X always goes first...
  label = gtk_label_new("Current Player: X");
  gtk_grid_attach(GTK_GRID (grid), label, 0, 3, 2, 1);

  //Set up reset button
  reset_button = gtk_button_new_with_label("Reset");
  g_signal_connect(reset_button, "clicked", G_CALLBACK (reset), NULL);
  gtk_widget_set_vexpand(reset_button, TRUE);
  gtk_widget_set_hexpand(reset_button, TRUE);
  gtk_grid_attach(GTK_GRID (grid), reset_button, 2, 3, 1, 1);

  //Attach grid to window
  gtk_container_add(GTK_CONTAINER (window), grid);
  gtk_widget_set_vexpand(grid, TRUE);
  gtk_widget_set_hexpand(grid, TRUE);

  //Prepare and show window
  gtk_window_set_title(GTK_WINDOW (window), "Tic Tac Toe");
  gtk_window_set_default_size(GTK_WINDOW (window), 300, 400);
  gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("com.anzanama.tictactoe", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run(G_APPLICATION (app), argc, argv);
  g_object_unref(app);

  return status;
}
