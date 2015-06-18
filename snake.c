/*-------------------------------------------------------------------------
 Include files:
 --------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*Rebecca's add*/
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/semaphore.h>
#include <linux/sched.h>
#include <asm/ioctl.h>

/*=========================================================================
 Constants and definitions:
 ==========================================================================*/
#define N (4) /* the size of the board TODO: can it change? */
#define M (3)  /* the initial size of the snake */
#define K (5)  /* the number of turns a snake can survive without eating */

typedef char Player;
/* PAY ATTENTION! i will use the fact that white is positive one and black is negative
 one to describe the segments of the snake. for example, if the white snake is 2 segments
 long and the black snake is 3 segments long
 white snake is  1   2
 black snake is -1  -2  -3 */
#define WHITE ( 1) /* id to the white player */
#define BLACK (-1) /* id to the black player */
#define EMPTY ( 0) /* to describe an empty point */
/* to describe a point with food. having the value this big guarantees that there will be no
 overlapping between the snake segments' numbers and the food id */
#define FOOD  (N*N)

typedef char bool;
#define FALSE (0)
#define TRUE  (1)

typedef int Direction;
#define DOWN  (2)
#define LEFT  (4)
#define RIGHT (6)
#define UP    (8)

/* a point in 2d space */
typedef struct {
	int x, y;
} Point;

typedef int Matrix[N][N];

typedef int ErrorCode;
#define ERR_OK      			((ErrorCode) 0)
#define ERR_BOARD_FULL			((ErrorCode)-1)
#define ERR_SNAKE_IS_TOO_HUNGRY ((ErrorCode)-2)

typedef int WinnerData;
#define WHITE_IS_WINNER (4)
#define BLACK_IS_WINNER (2)
#define A_TIE (5)
//Anna's add
#define NOT_FINISHED (-1)

typedef int ColorData;
#define SNAKE_IS_WHITE (4)
#define SNAKE_IS_BLACK (2)

// Anna's add: structs and fields
typedef struct {
	Matrix board;
	Player currentPlayer;
	WinnerData winner;
	bool isFinished;
	bool isRealesed;
	int openCount;
	struct semaphore countLock; // lock when check the openCount variable
	struct semaphore openLock; // lock when the first player waits for the second one
	struct semaphore readWriteLock; // lock when read or write (can't be both on the same time)
	struct semaphore whiteLock; // blocks the white player when it isn't his turn
	struct semaphore blackLock; // blocks the black player when it isn't his turn
	struct semaphore currentLock; //lock when checking the currentPlayer
	struct semaphore isFinishedLock; //lock when checking if the game has finished
	struct semaphore isReleasedLock; //lock when checking if one of the players released the game
	struct semaphore winnerLock; // lock when check or change the winner
} Game;

typedef struct {
	Player color;
	Game* myGame;
} PlayerS;

//Rebecca's change:
static int maxGames;
static Game* games = NULL;
static int majorNumber; ///< Stores the device number -- determined automatically
//TODO: do we need it?:
static char message[256] = { 0 }; ///< Memory for the string that is passed from userspace
static short size_of_message; ///< Used to remember the size of the string stored

/* magic number for SCMD IOCTL operations */
#define SNAKE_MAGIC 's'

#define SNAKE_GET_WINNER  _IOR(SNAKE_MAGIC, 1 , char *) //get driver data
#define SNAKE_GET_COLOR  _IOR(SNAKE_MAGIC, 2 , char *) //set driver data
// The prototype functions for the character driver -- must come before the struct definition
int open_snake(struct inode *, struct file *);
int release_snake(struct inode *, struct file *);
ssize_t read_snake(struct file *, char *, size_t, loff_t *);
ssize_t write_snake(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = { .open = open_snake, .read = read_snake,
		.write = write_snake, .release = release_snake, .llseek=llseek_snake};

// end of Rebecca's change here

bool Init(Matrix*); /* initialize the board. return false if the board is illegal (should not occur, affected by N, M parameters) */
bool Update(Matrix*, Player, char);/* handle all updating to this player. returns whether to continue or not. */
void Print(Matrix*);/* prints the state of the board */
Point GetInputLoc(Matrix*, Player);/* calculates the location that the player wants to go to */
bool CheckTarget(Matrix*, Player, Point);/* checks if the player can move to the specified location */
Point GetSegment(Matrix*, int);/* gets the location of a segment which is numbered by the value */
bool IsAvailable(Matrix*, Point);/* returns if the point wanted is in bounds and not occupied by any snake */
ErrorCode CheckFoodAndMove(Matrix*, Player, Point);/* handle food and advance the snake accordingly */
ErrorCode RandFoodLocation(Matrix*);/* randomize a location for food. return ERR_BOARD_FULL if the board is full */
void AdvancePlayer(Matrix*, Player, Point);/* advance the snake */
void IncSizePlayer(Matrix*, Player, Point);/* advance the snake and increase it's size */
bool IsMatrixFull(Matrix*);/* check if the matrix is full */
int GetSize(Matrix*, Player);/* gets the size of the snake */

/*-------------------------------------------------------------------------
 The main program. The program implements a snake game
 -------------------------------------------------------------------------*/

//Anna's add start
int open_snake(struct inode *n, struct file *f) {
	if (!n || !f) {
		return -EINVAL;
	}
	int count;
	int minor = MINOR(n->i_rdev);
	down(&(games[minor].countLock));
	if (games[minor].openCount == 2) {
		up(&(games[minor].countLock));
		return -EINVAL; //TODO:check what error code whould be returned
	}
	games[minor].openCount++;
	count = games[minor].openCount;
	up(&(games[minor].countLock));
	if (count == 1) {
        //why is malloc needed? -Rebecca
		PlayerS* specWhite = kmalloc(sizeof(PlayerS), GFP_KERNEL);
		specWhite->color = WHITE;
		specWhite->myGame = &(games[minor]);
		f->private_data = (void*) specWhite;
		down(&(games[minor].openLock));
	} else { // has to be 2
		PlayerS* specBlack = kmalloc(sizeof(PlayerS), GFP_KERNEL);
		specBlack->color = BLACK;
		specBlack->myGame = &(games[minor]);
		f->private_data = (void*) specBlack;
		up(&(games[minor].openLock));
	}
	return 0;
}

ssize_t write_snake(struct file *filp, const char *buff, size_t count,
		loff_t *offp) {
	// check parameters
	if (!filp || !offp || count < 0 || (!buff && count != 0)) {
		return -EFAULT;
	}
	if (count == 0) {
		return 0;
	}
	Game* currentGame = ((PlayerS*) (filp->private_data))->myGame;
	// check if game finished normally
	down(&(currentGame->isFinishedLock));
	if (currentGame->isFinished == TRUE) {
		up(&(currentGame->isFinishedLock));
		return -10;
	}
	up(&(currentGame->isFinishedLock));
	// check if the other player released the game
	down(&(currentGame->isReleasedLock));
	if (currentGame->isReleasedLock == TRUE) {
		up(&(currentGame->isReleasedLock));
		return -10;
	}
	up(&(currentGame->isReleasedLock));
	//start of function
	char* myBuff = kmalloc(count, GFP_KERNEL);
	int size = count;
	if (myBuff == NULL) {
		return -EFAULT;
	}
	int result = copy_from_user(myBuff, buff, count);
	if (result == count) {
		return -EFAULT;
	}
	size -= result;
	PlayerS* currentP = (PlayerS*) (filp->private_data);
	for (int i = 0; i < size; i++) {
		down(&(currentGame->currentLock));
		if (currentP->color == currentGame->currentPlayer) {
			up(&(currentGame->currentLock));
		} else {
			up(&(currentGame->currentLock));
			if (currentP->color == WHITE) {
				down(&(currentGame->whiteLock));
				down(&(currentGame->whiteLock));
			} else {
				down(&(currentGame->blackLock));
				down(&(currentGame->blackLock));
			}
		}
		down(&(currentGame->readWriteLock));
		bool upResult = Update(&(currentGame->board), currentP, myBuff[i]);
		up(&(currentGame->readWriteLock));
		down(&(currentGame->currentLock));
		if (currentP->color == WHITE) {
			currentGame->currentPlayer = BLACK;
		} else {
			currentGame->currentPlayer = WHITE;
		}
		up(&(currentGame->currentLock));
		if (currentP->color == WHITE) {
			up(&(currentGame->blackLock));
		} else {
			up(&(currentGame->whiteLock));
		}
		if (upResult == FALSE) {
			break;
		}
	}
	return size;
}

// Anna's add end
bool Init(Matrix *matrix) {
	int i;
	/* initialize the snakes location */
	for (i = 0; i < M; ++i) {
		(*matrix)[0][i] = WHITE * (i + 1);
		(*matrix)[N - 1][i] = BLACK * (i + 1);
	}
	/* initialize the food location */
	/*Rebecca's change - srand*/
	if (RandFoodLocation(matrix) != ERR_OK)
		return FALSE;
	printf(
			"instructions: white player is represented by positive numbers, \nblack player is represented by negative numbers\n");
	Print(matrix);

	return TRUE;
}

bool Update(Matrix *matrix, PlayerS* player, char move) {
	ErrorCode e;
	Point p = GetInputLoc(matrix, player->color, atoi(move));
	Game* currentGame = player->myGame;

	//TODO: check why doesn't recognize PlayerS fields
	if (!CheckTarget(matrix, player->color, p)) {
		// if the move is illegal, out of bounds, or trying to eat a snake
		down(&(currentGame->isFinishedLock));
		currentGame->isFinished = TRUE;
		up(&(currentGame->isFinishedLock));
		//set winner
		down(&(currentGame->winnerLock));
		if (player->color == WHITE) {
			currentGame->winner = BLACK_IS_WINNER;
		} else {
			currentGame->winner = WHITE_IS_WINNER;
		}
		up(&(currentGame->winnerLock));
		return FALSE;
	}
	e = CheckFoodAndMove(matrix, player, p);
	if (e == ERR_BOARD_FULL) {
//		printf("the board is full, tie");
		down(&(currentGame->winnerLock));
		currentGame->winner = A_TIE;
		up(&(currentGame->winnerLock));
		return FALSE;
	}
    //Rebeca's change
	if (e == ERR_SNAKE_IS_TOO_HUNGRY) {
        down(&(currentGame->winnerLock));
        if (player->color == WHITE) {
            currentGame->winner = BLACK_IS_WINNER;
        } else {
            currentGame->winner = WHITE_IS_WINNER;
        }
        up(&(currentGame->winnerLock));
		return FALSE;
	}
// only option is that e == ERR_OK
    //probabely checks if board is full after the snake has moved
	if (IsMatrixFull(matrix)) {
        down(&(currentGame->winnerLock));
        currentGame->winner = A_TIE;
        up(&(currentGame->winnerLock));
		return FALSE;
	}

	return TRUE;
}

Point GetInputLoc(Matrix *matrix, Player player, Direction dir) {
	Point p;

	if (dir != UP && dir != DOWN && dir != LEFT && dir != RIGHT) {
		p.x = -1;
		p.y = -1;
		return p;
	}

	p = GetSegment(matrix, player); //gets the point of the player's head

	switch (dir) {
		case UP:
			--p.y;
			break;
		case DOWN:
			++p.y;
			break;
		case LEFT:
			--p.x;
			break;
		case RIGHT:
			++p.x;
			break;
	}
	return p;
}

Point GetSegment(Matrix *matrix, int segment) {
	/* just run through all the matrix */
	Point p;
	for (p.x = 0; p.x < N; ++p.x) {
		for (p.y = 0; p.y < N; ++p.y) {
			if ((*matrix)[p.y][p.x] == segment)
				return p;
		}
	}
	p.x = p.y = -1;
	return p;
}

int GetSize(Matrix *matrix, Player player) {
	/* check one by one the size */
	Point p, next_p;
	int segment = 0;
	while (TRUE) {
		next_p = GetSegment(matrix, segment += player);
		if (next_p.x == -1)
			break;

		p = next_p;
	}

	return (*matrix)[p.y][p.x] * player;
}

bool CheckTarget(Matrix *matrix, Player player, Point p) {
	/* is empty or is the tail of the snake (so it will move the next
	 to make place) */
	return IsAvailable(matrix, p)
			|| ((*matrix)[p.y][p.x] == player * GetSize(matrix, player));
}

bool IsAvailable(Matrix *matrix, Point p) {
	return
	/* is out of bounds */
	!(p.x < 0 || p.x > (N - 1) || p.y < 0 || p.y > (N - 1) ||
	/* is empty */
	((*matrix)[p.y][p.x] != EMPTY && (*matrix)[p.y][p.x] != FOOD));
}

ErrorCode CheckFoodAndMove(Matrix *matrix, Player player, Point p) {
	static int white_counter = K;
	static int black_counter = K;
	/* if the player did come to the place where there is food */
	if ((*matrix)[p.y][p.x] == FOOD) {
		if (player == BLACK)
			black_counter = K;
		if (player == WHITE)
			white_counter = K;

		IncSizePlayer(matrix, player, p);

		if (RandFoodLocation(matrix) != ERR_OK)
			return ERR_BOARD_FULL;
	} else /* check hunger */
	{
		if (player == BLACK && --black_counter == 0)
			return ERR_SNAKE_IS_TOO_HUNGRY;
		if (player == WHITE && --white_counter == 0)
			return ERR_SNAKE_IS_TOO_HUNGRY;

		AdvancePlayer(matrix, player, p);
	}
	return ERR_OK;
}

void AdvancePlayer(Matrix *matrix, Player player, Point p) {
	/* go from last to first so the identifier is always unique */
	Point p_tmp, p_tail = GetSegment(matrix, GetSize(matrix, player) * player);
	int segment = GetSize(matrix, player) * player;
	while (TRUE) {
		p_tmp = GetSegment(matrix, segment);
		(*matrix)[p_tmp.y][p_tmp.x] += player;
		segment -= player;
		if (segment == 0)
			break;
	}
	(*matrix)[p_tail.y][p_tail.x] = EMPTY;
	(*matrix)[p.y][p.x] = player;
}

void IncSizePlayer(Matrix *matrix, Player player, Point p) {
	/* go from last to first so the identifier is always unique */
	Point p_tmp;
	int segment = GetSize(matrix, player) * player;
	while (TRUE) {
		p_tmp = GetSegment(matrix, segment);
		(*matrix)[p_tmp.y][p_tmp.x] += player;
		segment -= player;
		if (segment == 0)
			break;
	}
	(*matrix)[p.y][p.x] = player;
}

ErrorCode RandFoodLocation(Matrix *matrix) {
	Point p;
	do {
		/*Rebecca's change*/
		get_random_bytes(&p.x, sizeof(int));
		p.x = p.x % N;
		get_random_bytes(&p.y, sizeof(int));
		p.y = p.y % N;
	} while (!(IsAvailable(matrix, p) || IsMatrixFull(matrix))); //fixed by Piazza

	if (IsMatrixFull(matrix))
		return ERR_BOARD_FULL;
	(*matrix)[p.y][p.x] = FOOD;
	return ERR_OK;
}

bool IsMatrixFull(Matrix *matrix) {
	Point p;
	for (p.x = 0; p.x < N; ++p.x) {
		for (p.y = 0; p.y < N; ++p.y) {
			if ((*matrix)[p.y][p.x] == EMPTY || (*matrix)[p.y][p.x] == FOOD)
				return FALSE;
		}
	}
	return TRUE;
}

// Rebecca's add
ssize_t read_snake(struct file *filp, char *buff, size_t count, loff_t *offp) {
    if (!filp || !offp || count < 0 || (!buff && count != 0)) {
        return -EFAULT;
    }
    if (count == 0) {
        return 0;
    }
	char *local_buff = kmalloc(count, GFP_KERNEL);
	Print(&(((PlayerS*) (flip->private_data))->myGame->board), local_buff,
			count, &(((PlayerS*) (flip->private_data))->myGame->readWriteLock));
	if (copy_to_user(buff, local_buff, count))
		// TODO: type of error
		return ERROR;
	kfree(local_buff);
    return 0;
}

//Rebecca's change
//TODO: case count not long enough
void Print(Matrix *matrix, char *buff, size_t count, struct semaphore *sem) {
	int i;
	int current = 0;
	Point p;
	for (i = 0; i < N + 1; ++i) {
		buff[current++] = '-';
		buff[current++] = '-';
		buff[current++] = '-';
	}
	buff[current++] = '\n';
//TODO: check semaphore location
	down(sem);
	for (p.y = 0; p.y < N; ++p.y) {
		buff[current++] = '|';
		for (p.x = 0; p.x < N; ++p.x) {
			switch ((*matrix)[p.y][p.x]) {
				case FOOD:
					buff[current++] = ' ';
					buff[current++] = ' ';
					buff[current++] = '*';
					break;
				case EMPTY:
					buff[current++] = ' ';
					buff[current++] = ' ';
					buff[current++] = '.;
					break;
				default:
					buff[current++] = ' ';
					//TODO: check if correct
					buff[current++] = (*matrix)[p.y][p.x] + '0';
					buff[current++] = ' ';
			}
		}
		buff[current++] = ' ';
		buff[current++] = '|';
		buff[current++] = '\n';
	}
//TODO: check semaphore location
	up(sem);
	for (i = 0; i < N + 1; ++i) {
		buff[current++] = '-';
		buff[current++] = '-';
		buff[current++] = '-';
	}
	buff[current++] = '\n';
	while (current <= count)
		buff[current++] = '\0';
}

int init_module(int max_games) {
	maxGames = max_games;
	games = kmalloc(sizeof(Game) * max_games, GFP_KERNEL);
	MODULE_PARM(maxGames, "i");
	for (int i = 0; i < max_games; i++) {
		games[i].currentPlayer = WHITE;
		games[i].openCount = 0;
//			games[i].
	}

	major=register_chrdev(0, "snake", const struct file_operations * fops);
	MODULE_PARM(games, "i");
	MODULE_PARM(major, "i");
}
// Rrebecca's adds:

int ioctl_snake(int fd, int cmd) {
	switch () {
		case SNAKE_GET_WINNER:
        Game* currentGame = ((PlayerS*) (filp->private_data))->myGame;
        down(&(currentGame->isFinishedLock));
        if (currentGame->isFinished == TRUE) {
        up(&(currentGame->isFinishedLock));
            return currentGame->winner;
        }
		case SNAKE_GET_COLOR;
		if (((PlayerS*) (flip->private_data))->color == 1)
			return SNAKE_IS_WHITE;
		if (((PlayerS*) (flip->private_data))->color == -1)
			return SNAKE_IS_BLACK;
		//TODO: add error
		return ERROR;
	}
}

int release_snake(struct inode *n, struct file *f){
    if (!n || !f) {
        return -EINVAL;
    }
    Game* currentGame = ((PlayerS*) (filp->private_data))->myGame;
    
}

//TODO: finish
void cleanup_module(){
    kfree(games);
}
