#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

/// la langeur et la largeur de la feunetre
int width = 800;
int height = 600;

/// La structure des points qui contient les coordonnées x et y
typedef struct Point
{
    double x,y;

}Point;

/// la structure des elements de la listes des points
typedef struct Element
{
    Point pt;
    struct Element *next;

}PointElemt;

/// La stucrure de la liste chainée des points
typedef struct list
{
    PointElemt *First;
    PointElemt *Last;
}List;

/// La stucrure du model de la droite contient a la pente et b = y(0)
typedef struct model
{
    double a,b;
}Model;

/// la création d'un point
PointElemt *CreatePt(int x, int y)
{
    PointElemt *p;
    p=(PointElemt*)malloc(sizeof(PointElemt));
    p->next = NULL;
    p->pt.x = x;
    p->pt.y = y;
    return p;

}

/// la création d'une liste des points vide
List *CreateList()
{
    List *L;
    L=(List*)malloc(sizeof(List));
    L->First = NULL;
    L->Last = NULL;
    return L;
}

///la fonction calculant la Moyenne de X
double MoyenneX(List *L,int n)
{
    double M=0;
    PointElemt *p =L->First;
    while(p!=NULL)
    {
        M=M+ p->pt.x;
        p=p->next;
    }
    M=M/n;

    return M;
}
/// la fonction calculant la Moyenne de Y
double MoyenneY(List *L,int n)
{
    double M=0;
    PointElemt *p =L->First;
    while(p!=NULL)
    {
        M=M+ p->pt.y;
        p=p->next;
    }
    M=M/n;
    return M;
}

/// la fonction calculant la Variance de X
double VARX(List *L, int n)
{
    double var =0;
    PointElemt *p = L->First;
    while(p!=NULL)
    {
        var = var + pow(p->pt.x - MoyenneX(L,n),2);
        p=p->next;
    }
    var = var/n;
    return var;
}
/// la fonction calculant la Variance de Y
double VARY(List *L,int n)
{
    double var =0;
    PointElemt *p = L->First;
    while(p!=NULL)
    {
        var = var + pow(p->pt.y - MoyenneX(L,n),2);
        p=p->next;
    }
    var = var/n;
    return var;
}

/// la fonction calculant la covariance de X,Y
double CovXY(List *L, int n)
{
    double cov=0;
    PointElemt *p = L->First;
    while(p!=NULL)
    {
        cov = cov + p->pt.x * p->pt.y;
        p=p->next;
    }
    cov = cov/n;
    cov = cov - MoyenneX(L,n)*MoyenneY(L,n);
    return cov;
}


/// Affichage d'un élément de la liste
void affichePointElemt(PointElemt *p)
{
    printf("(%f,%f) -",p->pt.x,p->pt.y);
}

/// Affichage de la liste
void afficheList(List *L)
{
    PointElemt *p;
    p=L->First;
    while(p->next!=NULL)
    {
        affichePointElemt(p);
        p=p->next;
    }
}

/// La réservation de la mémoire d'un model vide
Model *CreateModel()
{
    Model *model =(Model*)malloc(sizeof(Model));
    model->a=1;
    model->b=1;
    return model;
}

/// Ajouter un point à la fin de la liste
void AddPointElemt_list(List **L, PointElemt *p)
{
    if((*L)->First ==NULL)
    {
        (*L)->First = p;
        (*L)->Last = p;
    }
    else
    {
        (*L)->Last->next = p;
        (*L)->Last = p;
    }
}

/// Trouver le model convenable à la liste des point avec la méthode des moindres carrés
Model *findModel(List *L,int n)
{
    double error,a=1,b=1,x,real_y,predicted_y;
    PointElemt *p;
    Model *model;
    model = CreateModel();

    /**Methode des moindres carres*/
    model->a=CovXY(L,n)/VARX(L,n);
    model->b=MoyenneY(L,n)- model->a * MoyenneX(L,n);

    return model;
}

/// La fonction qui trace le modè sur la feunetre
void TraceModel(SDL_Renderer *rederer, double a, double b)
{
    SDL_SetRenderDrawColor(rederer, 0, 200, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(rederer,0,height - b,width,height - (a * width + b));
}

/// La fonction qui décide qui à faire lors le click dans la feunetre
/** si on click on cree un point qu'on ajoute a la liste pour trouver le nouveau model */
void Onclick(SDL_Renderer *rederer,double x, double y, Model **model, List **L, int n)
{
    PointElemt *p;
    /// Creation du point
    p= CreatePt(x,y);
    /// Ajout du point a la liste
    AddPointElemt_list(L,p);
    /// Nouveau model
    (*model) = findModel(*L,n);
    TraceModel(rederer,(*model)->a,(*model)->b);
}
/// La fonction principale

int main(int c,char *T[])
{
    /// tester si la SDL ouvre correctement
    if(SDL_Init(SDL_INIT_EVERYTHING)<0) {printf("%s\n",SDL_GetError()); exit(-1);}

    /// creation d'une feunetre
    SDL_Window *window;
    window=SDL_CreateWindow("Mohamed KOUHOU",500,100,width,height,SDL_WINDOW_OPENGL);

    /// une liste qui va representer les rectangle dans la SDL
    SDL_Rect *Liste;
    Liste = (SDL_Rect*)malloc(sizeof(SDL_Rect));

    int X,Y;
    List *L = CreateList();
    Model *model =CreateModel();
    if(window==NULL){printf("%s\n",SDL_GetError());exit(-1);}

    SDL_Event windowEvent;
    SDL_Renderer *rederer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    int i =0; /// conteur du nombre des PointElemts

    /// la boucle va empecher la fermeture immediat de la feunetre
    while(1)
    {
        if(SDL_PollEvent(&windowEvent))
        {
            /// Quiter la feunetre
            if(windowEvent.type == SDL_QUIT)
                {break;}
            /// gerer les click sur la feunetre
            if(windowEvent.type ==SDL_MOUSEBUTTONDOWN)
            {
                /// On reserve de la memoire pour tracer le rectangle dans l'emplacemnt du click
                Liste=(SDL_Rect*)realloc(Liste,(i+1)*sizeof(SDL_Rect));
                Liste[i].x=windowEvent.button.x;
                Liste[i].y=windowEvent.button.y;
                Liste[i].h=5;
                Liste[i++].w=5;

                X=windowEvent.button.x ;
                Y=600-windowEvent.button.y;
                /**
                Apres chaque tracage de la droite on la donne la couleur de
                l'arriere plan pour tracer la nouvellle droite
                */
                SDL_SetRenderDrawColor(rederer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(rederer,0,height - model->b,width,height - ((model->a) * width +(model->b)));
                Onclick(rederer,X,Y,&model,&L,i);

                printf("Le nombre de PointElemts est %d\n",i);
                printf("a = %f et b =%f\n",model->a,model->b);
                printf("----------------------------------------------\n");
            }
        }

        /** Colorer les rectangle representant les PointElemts */
        SDL_SetRenderDrawColor(rederer,255,255,0,0);
        for(int j=0;j<i+1;j++)
        {
            SDL_RenderFillRect(rederer,Liste + j);
        }
        /// On reserve de la memoire pour tracer le rectangle qui suit
        Liste=(SDL_Rect*)realloc(Liste,(i+1)*sizeof(SDL_Rect));
        SDL_RenderPresent(rederer);
    }

    /// detriure la feunetre
    SDL_DestroyRenderer(rederer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
