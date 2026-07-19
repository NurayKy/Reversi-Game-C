//NURAY KAYA - 230101060
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

struct UserRecord {
    char username[50];       
    unsigned long pin_hash;  
    int games_won;           
    int games_lost;          
};

struct GameState { 
int gameId;  
char userName[50]; 
int board[8][8];  
int currentTurn;  
unsigned long stateHash; 
};


void printBoard(int board[8][8]);
int isValidMove(int board[8][8], int row, int column, int player); 
void makeMove(int board[8][8], int row, int column, int player);
int countFlips(int board[8][8], int row, int column, int player);
void computerMove(int board[8][8], int player);
unsigned long hash_djb2(unsigned char *str);
unsigned long calculateGameStateHash(struct GameState *state);
void saveGame(struct GameState *state);
void saveGameAs(struct GameState *state);
int loadGame(char *loggedInUser, struct GameState *activeState);
void registerUser();
int loginUser(char *loggedInUser);
void updateUserStats(char *username, int won);
void showScores(char *username);


int main(){
    int board[8][8];
    int i,j,r,c;
    int row, column;
    int player = 2;
    char loggedInUser[50];
    int choice, saveChoice, menuChoice;
    int emptyCells;
    int blackCount;
    int whiteCount;
    int canMove;
    int playerCanMove, computerCanMove;

    
    struct GameState activeState;
    
    activeState.gameId = (int)time(NULL);
    
    printf("1. Sign Up\n2. Sign In\nSelection: ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        registerUser();
        if (!loginUser(loggedInUser)) 
          return 0;
    } else {
        if (!loginUser(loggedInUser)) 
          return 0;
    }

    strcpy(activeState.userName, loggedInUser);

    printf("\n--- MAIN MENU ---\n");
    printf("1. Start New Game\n2. Load Saved Game\n3. Exit\nSelection: ");
    scanf("%d", &menuChoice);

    if (menuChoice == 3){
        showScores(loggedInUser);

    }else if (menuChoice == 2) {
            if (loadGame(loggedInUser, &activeState) == 1) { 
            
            for(i = 0; i < 8; i++){ 
                for(j = 0; j < 8; j++){
                    board[i][j] = activeState.board[i][j];
                }
            }
            player = activeState.currentTurn;
        } else {
            
            printf("\nPress Enter to return to the Main Menu...");
            getchar(); 
            getchar(); 
            return main(); 
        }
              
            
        player = activeState.currentTurn;
        
    } else if (menuChoice == 1) {
      
        for(i = 0; i < 8; i++)
            for(j = 0 ; j < 8; j++)
                board[i][j] = 0;
        
        board[3][3] = 1; 
        board[3][4] = 2;
        board[4][3] = 2; 
        board[4][4] = 1;

        player = 2; 

        activeState.gameId = (int)time(NULL); 

    } else {
        return 0;
    }
    

    while(1){
        printf("\n\n------------------------------\n\n");
        printBoard(board);
        
        emptyCells = 0;
        blackCount = 0;
        whiteCount = 0;

        for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) {
                
                if(board[i][j] == 0) 
                  emptyCells++;

                if(board[i][j] == 2) 
                  blackCount++; 

                if(board[i][j] == 1) 
                  whiteCount++; 
            }
        }

        playerCanMove = 0;
        computerCanMove = 0;
        for(i=0; i<8; i++) {
            for(j=0; j<8; j++) {
                if(isValidMove(board, i, j, 2)) 
                   playerCanMove = 1;
                if(isValidMove(board, i, j, 1)) 
                   computerCanMove = 1;
            }
        }
        if(emptyCells == 0 || (playerCanMove == 0 && computerCanMove == 0)) {
            printf("\n--- GAME OVER ---\n");
            printf("Black (B): %d | White (W): %d\n", blackCount, whiteCount);
            
            updateUserStats(loggedInUser, blackCount > whiteCount);
           
            for(r = 0; r < 8; r++) {
                for(c = 0; c < 8; c++) {
                    activeState.board[r][c] = board[r][c];
                }
            }
            activeState.currentTurn = player;
            activeState.stateHash = calculateGameStateHash(&activeState);
            saveGame(&activeState);
           
            

            if(blackCount > whiteCount) 
              printf("CONGRATULATIONS, YOU WIN!\n");
            
            else if(whiteCount > blackCount) 
              printf("COMPUTER WINS!\n");
            
            else 
              printf("TIE!\n");
            
              break; 
        }

        if(player == 2) { 

            canMove = 0;

            for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) {
                if(isValidMove(board, i, j, 2)) { 
                    canMove = 1; 
                    break; }
            }
            if(canMove) 
              break;
        }

        if(!canMove) {
            printf("No valid moves available. Switching turn to the computer.\n");
            player = 1;
            continue;
        }

            printf("\nYour Turn (Black - B)\n");
            printf("Enter Row and Column (Exit: 0 0): ");
            
            if(scanf("%d %d", &row, &column) != 2) 
              break;

            if(row == 0 && column == 0) {
   
            printf("\nDo you want to save the game?\n");
            printf("1. Save (Overwrite)\n2. Save As (New ID)\n3. Exit Without Saving\nSelection: ");
            scanf("%d", &saveChoice);

            if (saveChoice == 1 || saveChoice == 2) {
        
               for(int r = 0; r < 8; r++) {
                  for(int c = 0; c < 8; c++) {
                    activeState.board[r][c] = board[r][c];
                  }
                }
                activeState.currentTurn = player;

                if(saveChoice == 1) {
            
                  activeState.stateHash = calculateGameStateHash(&activeState);
                  saveGame(&activeState);
                  }else{
           
                  activeState.gameId = (int)time(NULL); 
                  activeState.stateHash = calculateGameStateHash(&activeState);
                  saveGameAs(&activeState);
                  }
                }
                break;
                }

                row--; 
                column--;

        if(row >= 0 && row < 8 && column >= 0 && column < 8 && isValidMove(board, row, column, player)) {
            makeMove(board, row, column, player);
            player = 1; 
            }else {
                printf("İngilizce: Invalid move! Please try again.\n");
             }
        } 
        else{ 
            printf("\nComputer (W) is making a move...\n");
            computerMove(board, 1);
            player = 2; 
        }
    }

    return 0;
}



void printBoard(int board[8][8]){
    int i,j;
    
    printf("  1 2 3 4 5 6 7 8\n");

    for(i = 0; i < 8; i++){   
        printf("%d ", i + 1);

        for(j = 0; j < 8 ; j++){   
            if(board[i][j] == 0)   
                printf(". ");    
            else if(board[i][j] == 1)
                printf("W ");
            else if(board[i][j] == 2)
                printf("B ");
        }
    
    printf("\n");     
    }
          
}



int isValidMove(int board[8][8], int row, int column, int player){
    int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1}; 
    int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};    
    int opponent;
    int x, y, d;

    if(player == 1)
        opponent = 2;

    else 
        opponent = 1;

    if(board[row][column] != 0)
        return 0;
   
    for(d = 0; d < 8; d++){
        x = row + dx[d];
        y = column + dy[d];

        if(x < 0 || x >= 8 || y < 0 || y >= 8) 
           continue;

        if(board[x][y] != opponent)
         continue;
    
       while(1){  
        x += dx[d];
        y += dy[d];
        
        if(x < 0 || x >= 8 || y < 0 || y >= 8) 
           break;

        if(board[x][y] == 0)  
           break;

        if(board[x][y] == player)
           return 1;
        }
    }

    return 0;  
    
}



void makeMove(int board[8][8], int row, int column, int player){
    int opponent;
    int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1}; 
    int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    int x, y, d, i;
    int path[8][2]; 
    int len;

    if(player == 1)
        opponent = 2;

    else 
        opponent = 1;

    board[row][column] = player;

    for(d = 0; d < 8; d++){
        x = row + dx[d];
        y = column + dy[d];
        len = 0;

        while(x >= 0 && x < 8 && y>= 0 && y < 8 && board[x][y] == opponent){
            path[len][0] = x;
            path[len][1] = y;
            len++;

            x += dx[d];
            y += dy[d];
        }

        if(x >= 0 && x < 8 && y >= 0 && y < 8 && board[x][y] == player){
            for(i = 0; i < len; i++){
                board[path[i][0]][path[i][1]] = player;
            }
        }
        
    }
}



int countFlips(int board[8][8], int row, int column, int player){
    int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1}; 
    int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    int opponent, x, y, d;
    int totalFlips = 0;
    int tempCount = 0;

    if(player == 1)
        opponent = 2;

    else 
        opponent = 1;

    if(board[row][column] != 0)
        return 0;

    for(d = 0; d < 8; d++){
        tempCount = 0;

        x = row + dx[d];
        y = column + dy[d];
        
        if(x < 0 || x >= 8 || y < 0 || y >= 8)
           continue;
        
        if(board[x][y] != opponent)
           continue;

        while(1){
           x += dx[d];
           y += dy[d];
        
           if(x < 0 || x >= 8 || y < 0 || y >= 8){
             tempCount = 0;
             break;
           }

           if(board[x][y] == 0){
              tempCount = 0;
              break;
           }

           if(board[x][y] == opponent)
              tempCount++;
        
           if(board[x][y] == player){
              totalFlips += tempCount + 1;
              break;
           }
        }
    }

    return totalFlips;
}
    
    
void computerMove(int board[8][8], int player){
     int maxFlips = 0;
     int bestRow = -1;
     int bestCol = -1;
     int i, j;
     int flips;

     for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){

            if(isValidMove(board, i, j, player)){
                flips = countFlips(board, i, j, player);

                if(flips > maxFlips){
                    maxFlips = flips;
                    bestRow = i;
                    bestCol = j;
                }

            }
           
        }
     }

     if(bestRow != -1){
        makeMove(board, bestRow, bestCol, player);
        printf("Computer played: %d %d\n", bestRow + 1, bestCol + 1);
     }else{
        printf("Computer has no valid move!\n");
     }
}


unsigned long hash_djb2(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; 
    return hash;
}


unsigned long calculateGameStateHash(struct GameState *state) {
    char buffer[4096] = ""; 
    char temp[128];

    
    sprintf(temp, "%d%s%d", state->gameId, state->userName, state->currentTurn);
    strcat(buffer, temp);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            sprintf(temp, "%d", state->board[i][j]);
            strcat(buffer, temp);
        }
    }
    
    return hash_djb2((unsigned char*)buffer);
}


void saveGame(struct GameState *state) {

    FILE *file = fopen("saved_games.dat", "rb+"); 
    if (file == NULL) {
   
        file = fopen("saved_games.dat", "wb+"); 
    }
    
    struct GameState temp;
    int found = 0;
    
    while (fread(&temp, sizeof(struct GameState), 1, file)) {
        if (temp.gameId == state->gameId) {
            fseek(file, -1 * (long)sizeof(struct GameState), SEEK_CUR); 
            fwrite(state, sizeof(struct GameState), 1, file); 
            found = 1;
            break;
        }
    }
     
    if (!found) {
        fseek(file, 0, SEEK_END);
        fwrite(state, sizeof(struct GameState), 1, file); 
    }
    
    fclose(file);
    printf("Game saved successfully!\n");
}



void saveGameAs(struct GameState *state) {

    state->gameId = time(NULL); 
    
    
    FILE *file = fopen("saved_games.dat", "ab"); 
    if (file != NULL) {
       
        fwrite(state, sizeof(struct GameState), 1, file); 
        fclose(file);
        printf("Game saved as new record with ID: %d\n", state->gameId);
    }
}


int loadGame(char *loggedInUser, struct GameState *activeState) {
    int selectedId;
    struct GameState temp;
    int foundGames = 0;

    FILE *file = fopen("saved_games.dat", "rb"); 
    if (file == NULL) {
        printf("No saved games found.\n");
        return 0;
    }
    
    
    
    printf("\n--- Your Saved Games ---\n");
    
    while (fread(&temp, sizeof(struct GameState), 1, file)) {
        if (strcmp(temp.userName, loggedInUser) == 0) {
            printf("GameId: %d, Turn: %s\n", temp.gameId, temp.currentTurn == 1 ? "Black" : "White"); 
            foundGames++;
        }
    }
    
    if (foundGames == 0) {
        printf("No saved games found for this user.\n");
        fclose(file);
        return 0;
    }
    
  
    printf("\nEnter the Game ID you want to load: "); 
    scanf("%d", &selectedId);
    rewind(file); 
    
    while (fread(&temp, sizeof(struct GameState), 1, file)) {
       
        if (temp.gameId == selectedId && strcmp(temp.userName, loggedInUser) == 0) { 
           
            unsigned long calculatedHash = calculateGameStateHash(&temp); 
            
        if (calculatedHash == temp.stateHash) { 
                *activeState = temp;
                fclose(file);
                printf("Game loaded successfully!\n");
                return 1;
            } else {
                
                printf("Error: Save file corrupted or tampered with!\n");
                fclose(file);
                return 0;
            }
            fclose(file);
            break;
        }   
    }
}


void registerUser() {
    struct UserRecord newUser;
    char pin[5]; 

    printf("\n--- Create New Account ---\n");
    printf("Username: ");
    scanf("%s", newUser.username);
    printf("Enter 4-digit PIN: ");
    scanf("%s", pin);

    newUser.pin_hash = hash_djb2((unsigned char*)pin);
    newUser.games_won = 0; 
    newUser.games_lost = 0;

    FILE *file = fopen("accounts.dat", "ab"); 
    if (file != NULL) {
        fwrite(&newUser, sizeof(struct UserRecord), 1, file);
        fclose(file);
        printf("Account created successfully! You can now log in.\n");
    }
}


int loginUser(char *loggedInUser) {
    struct UserRecord temp;
    char inputUser[50], inputPin[5];
    int attempts = 0;

    while (attempts < 3) { 
        printf("\n--- Sign In ---\n");
        printf("Username: ");
        scanf("%s", inputUser);
        printf("PIN: ");
        scanf("%s", inputPin);

        unsigned long inputHash = hash_djb2((unsigned char*)inputPin);
        FILE *file = fopen("accounts.dat", "rb"); 
        
        if (file == NULL) {
            printf("No registered users found! Please register first.\n");
            return 0;
        }

        int found = 0;
        while (fread(&temp, sizeof(struct UserRecord), 1, file)) {
           
            if (strcmp(temp.username, inputUser) == 0 && temp.pin_hash == inputHash) {
                strcpy(loggedInUser, temp.username);
                found = 1;
                break;
            }
        }
        fclose(file);

        if (found) {
            printf("Login successful! Welcome %s.\n", loggedInUser);
            return 1;
        } else {
            attempts++;
            printf("Invalid username or PIN! (Remaining attempts: %d)\n", 3 - attempts);
        }
    }

    printf("You have entered an incorrect PIN 3 times. The program is closing.\n"); 
    exit(0); 
}


void updateUserStats(char *username, int won){
    FILE *file = fopen("accounts.dat", "rb+"); 
    if (file == NULL) return;

    struct UserRecord temp;
    while (fread(&temp, sizeof(struct UserRecord), 1, file)) {
        if (strcmp(temp.username, username) == 0) {
            if (won) 
               temp.games_won++;
            else 
               temp.games_lost++;

            fseek(file, -(long)sizeof(struct UserRecord), SEEK_CUR);
            fwrite(&temp, sizeof(struct UserRecord), 1, file);
            break;
        }
    }
    fclose(file);
}


void showScores(char *username) {
    
    struct UserRecord temp;
    int found = 0;
    
    FILE *file = fopen("accounts.dat", "rb"); 
    if (file == NULL) {
        printf("System Error: Account file not found.\n");
        return;
    }


    while (fread(&temp, sizeof(struct UserRecord), 1, file)) {
        if (strcmp(temp.username, username) == 0) {
            printf("\n=== Statistics for User: %s ===\n", username);
            printf("Total Wins: %d\n", temp.games_won); 
            printf("Total Losses: %d\n", temp.games_lost);  
            printf("========================================\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("User statistics not found.\n");
    }

    fclose(file);
}