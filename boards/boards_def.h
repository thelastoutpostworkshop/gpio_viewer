
typedef struct BoardGPIO
{
    int gpio;
    float topPosition;
    float leftPosition;
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
        boardDef.board_image = image;
    }
    String getImage(void) {
        return(boardDef.board_image);
    }

    BoardGPIO* getGPIOs(void) {
        return(boardDef.gpios);
    }

    int getGPIOsCount(void) {
        return boardDef.numberOfGpios;
    }
};