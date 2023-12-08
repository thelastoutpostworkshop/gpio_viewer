
typedef struct BoardGPIO
{
    int gpio;
    int topPosition;
    int leftPosition;
};

typedef struct BoardDetail
{
    BoardGPIO *gpios;
    int numberOfGpios;
    String board_image;
};

class ESPBoard
{
private:
    BoardDetail boardDef;

public:
    ESPBoard(BoardGPIO *gpioArray, int numGpios,String image) {
        boardDef.gpios = gpioArray;
        boardDef.numberOfGpios = numGpios;
    }
    String getImage(void) {
        return(boardDef.board_image);
    }
};