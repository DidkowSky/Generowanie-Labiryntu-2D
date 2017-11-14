#include <allegro.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int tabSize = 15;
int width = 800;
int height = 700;

struct Cell{
    float x;
    float y;
    bool visited;

    Cell(): x(-1),
            y(-1),
            visited(false){}
}cellTab[15][15];

struct Wall{
    bool exist;
    bool visited;
    float x1;
    float y1;
    float x2;
    float y2;

    Wall(): exist(true),
            visited(false),
            x1(-1),
            y1(-1),
            x2(-1),
            y2(-1){}
}vWall[15][15], hWall[15][15];

struct ListArgs{
    int li;
    int lj;

    ListArgs():li(0),
                lj(0){}
};
vector<ListArgs> neighbors;
vector<ListArgs> history;

void generateLab(int, int);

void fillCellTab(){
    for(int i = 1; i < width/100; i++){
        for(int j = 1; j < height/100; j++){
            cellTab[i][j].x = width/(width/100)*i;
            cellTab[i][j].y = height/(height/100)*j;
            //cout<<"("<<cellTab[i][j].x<<", "<<cellTab[i][j].y<<")";
        }
        //cout<<endl;
    }
}

void fillHWall(){
    for(int i = 1; i < tabSize; i++){
        for(int j = 1; j < tabSize-1; j++){
            if(cellTab[i][j+1].x == -1 || cellTab[i][j+1].y == -1)
                break;
            hWall[i][j].x1 = cellTab[i][j].x -width/(width/50);
            hWall[i][j].x2 = cellTab[i][j].x +width/(width/50);
            hWall[i][j].y1 = cellTab[i][j].y +height/(height/50)-2;
            hWall[i][j].y2 = cellTab[i][j].y +height/(height/50)+2;
        }
    }
}

void fillVWall(){
    for(int i = 1; i < tabSize-1; i++){
        for(int j = 1; j < tabSize; j++){
        //for(int j = 2; j < 3; j++){
            if(cellTab[i+1][j].x == -1 || cellTab[i+1][j].y == -1)
                break;
            vWall[i][j].x1 = cellTab[i][j].x +width/(width/50)-2;
            vWall[i][j].x2 = cellTab[i][j].x +width/(width/50)+2;
            vWall[i][j].y1 = cellTab[i][j].y -height/(height/50);
            vWall[i][j].y2 = cellTab[i][j].y +height/(height/50);
        }
    }
}

void printCell(BITMAP *bmp, int i, int j, int cellSize, int r, int g, int b){

    rectfill(bmp, cellTab[i][j].x -cellSize, cellTab[i][j].y -cellSize, cellTab[i][j].x +cellSize, cellTab[i][j].y +cellSize, makecol(r,g,b));
}

void printCells(BITMAP *bmp){
    for(int i = 1; i < tabSize; i++){
        for(int j = 1; j < tabSize; j++){
            if(cellTab[i][j].x == -1 || cellTab[i][j].y == -1)
                break;
            //rectfill(bmp, cellTab[i][j].x -30, cellTab[i][j].y -30, cellTab[i][j].x +30, cellTab[i][j].y +30, makecol(0,125,100));
            printCell(bmp, i, j, 50, 0, 125, 100);
        }
    }
}

void printWalls(BITMAP *bmp){
    //vWall[3][2].exist = false;
    //hWall[3][2].exist = false;
    for(int i = 1; i < tabSize; i++){
        for(int j = 1; j < tabSize; j++){
            if(cellTab[i][j].x == -1 || cellTab[i][j].y == -1)
                break;
            if(vWall[i][j].exist == true)
                rectfill(bmp, vWall[i][j].x1, vWall[i][j].y1, vWall[i][j].x2, vWall[i][j].y2, makecol(0,0,0));
            if(hWall[i][j].exist == true)
                rectfill(bmp, hWall[i][j].x1, hWall[i][j].y1, hWall[i][j].x2, hWall[i][j].y2, makecol(0,0,0));

        }
    }
    rect(bmp, 50, 50, width-50, height-50, makecol(0,0,0));
    rectfill(bmp, 48, 48, 52, height-48, makecol(0,0,0));
    rectfill(bmp, width-48, 48, width-52, height-48, makecol(0,0,0));
    rectfill(bmp, 48, height-48, width-52, height-52, makecol(0,0,0));
    rectfill(bmp, 48, 48, width-52, 52, makecol(0,0,0));
}

void checkNeighbors(int ii, int ij){
    ListArgs current;

    if(ii-1 > 0 && cellTab[ii-1][ij].x > -1){
        if(!cellTab[ii-1][ij].visited){
            current.li = ii-1;
            current.lj = ij;
            neighbors.push_back(current);
        }
    }
    if(ii+1 < tabSize && cellTab[ii+1][ij].x > -1){
        if(!cellTab[ii+1][ij].visited){
            current.li = ii+1;
            current.lj = ij;
            neighbors.push_back(current);
        }
    }
    if(ij-1 > 0 && cellTab[ii][ij-1].x > -1){
        if(!cellTab[ii][ij-1].visited){
            current.li = ii;
            current.lj = ij-1;
            neighbors.push_back(current);
        }
    }
    if(ij+1 < tabSize && cellTab[ii][ij+1].x > -1){
        if(!cellTab[ii][ij+1].visited){
            current.li = ii;
            current.lj = ij+1;
            neighbors.push_back(current);
        }
    }
    //cout<<endl<<"Neighbors"<<endl;
    if(neighbors.empty()){
        history.pop_back();
        return;
    }/*
    for(int i = 0; i < neighbors.size(); i++){
        cout<<i<<") "<<neighbors[i].li<<" "<< neighbors[i].lj<<endl;
    }*/
}

void changeCell(){
    //cout<<"List Neighbors Size: "<< neighbors.size()<<endl;
    if(!neighbors.empty()){
        int target = (int) rand()%neighbors.size();
        //cout<<"Target: "<< target<<endl;
        if(history[history.size()-1].li == neighbors[target].li){
            if(history[history.size()-1].lj < neighbors[target].lj)
                hWall[neighbors[target].li][history[history.size()-1].lj].exist = false;
            else
                hWall[neighbors[target].li][neighbors[target].lj].exist = false;

        }else if(history[history.size()-1].lj == neighbors[target].lj){
            if(history[history.size()-1].li < neighbors[target].li)
                vWall[history[history.size()-1].li][neighbors[target].lj].exist = false;
            else
                vWall[neighbors[target].li][neighbors[target].lj].exist = false;
        }
        int i = neighbors[target].li;
        int j = neighbors[target].lj;

        while(!neighbors.empty()){
            neighbors.pop_back();
        }
        //system("pause");
        generateLab(i, j);
    }else if(history.size()>1){
        history.pop_back();
        //cout<<history[history.size()-1].li<<history[history.size()-1].lj<<endl;
        //system("pause");
        generateLab(history[history.size()-1].li, history[history.size()-1].lj);
    }else{
        return;
    }
}

void generateLab(int ii, int ij){
    ListArgs current;

    current.li = ii;
    current.lj = ij;

    cellTab[current.li][current.lj].visited = true;

    history.push_back(current);
    //history.pop_back();
/*
    system("cls");
    cout<<"History"<<endl;
    for(int i = 0; i < history.size(); i++){
        cout<<i<<") "<<history[i].li<<" "<< history[i].lj<<endl;
    }
*/
    checkNeighbors(current.li, current.lj);
    changeCell();

    if(neighbors.empty())
        return;
}

//================================================================
ListArgs firstCell, lastCell;

void clearLists(){
    while(!neighbors.empty()){
        neighbors.pop_back();
    }
    while(!history.empty()){
        history.pop_back();
    }
}

void generatePath(ListArgs firstCell, ListArgs lastCell){
    ListArgs tmp;
    int r;

    if(history.empty()){
        history.push_back(firstCell);
    }else if(history[history.size()-1].li == lastCell.li && history[history.size()-1].lj == lastCell.lj){
        return;
    }
    //cout<<vWall[history[history.size()-1].li][history[history.size()-1].lj].exist<<endl;
    if(!vWall[history[history.size()-1].li][history[history.size()-1].lj].exist && !vWall[history[history.size()-1].li][history[history.size()-1].lj].visited){
        //cout<<"Nie ma prawej sciany!"<<endl;
        tmp.li = history[history.size()-1].li +1;
        tmp.lj = history[history.size()-1].lj;
        neighbors.push_back(tmp);
    }
    if(!vWall[history[history.size()-1].li -1][history[history.size()-1].lj].exist && !vWall[history[history.size()-1].li -1][history[history.size()-1].lj].visited){
        //cout<<"Nie ma lewej sciany!"<<endl;
        tmp.li = history[history.size()-1].li -1;
        tmp.lj = history[history.size()-1].lj;
        neighbors.push_back(tmp);
    }
    if(!hWall[history[history.size()-1].li][history[history.size()-1].lj].exist && !hWall[history[history.size()-1].li][history[history.size()-1].lj].visited){
        //cout<<"Nie ma dolnej sciany!"<<endl;
        tmp.li = history[history.size()-1].li;
        tmp.lj = history[history.size()-1].lj +1;
        neighbors.push_back(tmp);
    }
    if(!hWall[history[history.size()-1].li][history[history.size()-1].lj -1].exist && !hWall[history[history.size()-1].li][history[history.size()-1].lj -1].visited){
        //cout<<"Nie ma gornej sciany!"<<endl;
        tmp.li = history[history.size()-1].li;
        tmp.lj = history[history.size()-1].lj -1;
        neighbors.push_back(tmp);
    }

    if(!neighbors.empty()){
        r = rand()% (neighbors.size());
        //cout<<"Losowa: "<< r <<" Rozmiar: "<<neighbors.size() <<endl;

        //sprawdzic ktora sciana i ustawic visited na true chyba 4 warunki
        if(history[history.size()-1].li == neighbors[r].li){
            if(history[history.size()-1].lj < neighbors[r].lj)
                hWall[neighbors[r].li][history[history.size()-1].lj].visited = true;
            else
                hWall[neighbors[r].li][neighbors[r].lj].visited = true;

        }else if(history[history.size()-1].lj == neighbors[r].lj){
            if(history[history.size()-1].li < neighbors[r].li)
                vWall[history[history.size()-1].li][neighbors[r].lj].visited = true;
            else
                vWall[neighbors[r].li][neighbors[r].lj].visited = true;
        }

        history.push_back(neighbors[r]);
    }else{
        history.pop_back();
    }

    while(!neighbors.empty()){
        neighbors.pop_back();
    }

    generatePath(firstCell, lastCell);

}

void printPath(BITMAP *bmp){
    for(int i = 0; i < history.size(); i++){
        printCell(bmp, history[i].li, history[i].lj, 40, 125, 50, 0);
    }
}

//================================================================

int main(){
    srand(time(NULL));
    int depth, res;
    allegro_init();
    depth = desktop_color_depth();
    if( depth == 0 ) depth = 32;
    set_color_depth( depth );
    res = set_gfx_mode( GFX_AUTODETECT_WINDOWED, width, height, 0, 0 );
    if(res != 0){
        allegro_message( allegro_error );
        exit( - 1 );
    }
    install_keyboard();
    BITMAP *bmp=create_bitmap(SCREEN_W,SCREEN_H);
    clear_to_color(bmp, makecol(255,255,255));
    clear_to_color(screen, makecol(255,255,255));

    fillCellTab();
    fillVWall();
    fillHWall();

    generateLab(2,2);

    blit(screen, bmp, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    printCells(bmp);
    printWalls(bmp);

    firstCell.li = 1;
    firstCell.lj = 1;
    lastCell.li = 7;
    lastCell.lj = 1;

    clearLists();
    generatePath(firstCell, lastCell);
    printPath(bmp);

    blit(bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    while(1){
        if(key[KEY_ESC]) break;
    }

    allegro_exit();
    return 0;
}
END_OF_MAIN()
