#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "predicates.h"
#include "BOV.h"

#define ErrorScan(a)   tpzErrorScan(a,__LINE__,__FILE__)
#define Error(a)       tpzError(a,__LINE__,__FILE__)
#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y))
#define BOLD  "\033[1m"
#define RESET "\x1B[0m"

#define ROBUST 1

//#define isLeft(pa, pb)  (((pa[0]) < (pb[0])) || (((pa[0]) == (pb[0])) && ((pa[1]) < (pb[1]))))

typedef struct {
    GLsizei orig; // index in list of point
    GLsizei dest; // index in list of point
} Segment;

typedef struct Trapezoid_tag {
    GLsizei top; // index in list of segments
    GLsizei bottom; // index in list of segments
    
    GLsizei left_pt; // index in list of points
    GLsizei right_pt; // index in list of points
    
    int up_left; // index in list of tpz (if none: -1)
    int low_left; // index in list of tpz (if none: -1)
    int up_right; // index in list of tpz (if none: -1)
    int low_right; // index in list of tpz (if none: -1)
    
    GLsizei leaf; // index in list of nodes
} Trapezoid;

typedef  struct {
    float (*points)[2];
    Segment *segments;
    Trapezoid *trapezoids;
    float bounds[4]; // x_min, x_max, y_min, y_max
    int n_pts;
    int n_seg;
    int n_tpz;
    int size_pts;
    int size_seg;
    int size_tpz;
} TrapezoidalMap;

typedef struct Node_tag {
    char type;  
    GLsizei idx; // index in list of point / segments / tpz
    GLsizei left; // index in list of nodes
    GLsizei right; // index in list of nodes
} Node;

typedef struct {
    Node *nodes; // whole list (realloc when size too big)
    int n_nodes;
    int size_nodes;
    int depth;
} DataSet;


void tpzError(char *text, int line, char *file);
void tpzErrorScan(int test, int line, char *file);

void genNonCrossingSegments(float w, float h, GLsizei n_box_x, GLsizei n_box_y);
void generateGaltonBoard(float step, GLsizei splits, GLsizei n_rows);

TrapezoidalMap *createEmptyTpzMap (GLsizei size_pts);
TrapezoidalMap *loadFromTxt (char *filename);
void adjust_ratio(TrapezoidalMap *tpzMap);

void randomPermutation(TrapezoidalMap *tpzMap);
void initTpzMap(TrapezoidalMap *tpzMap);
void infoTpzMap(TrapezoidalMap *tpzMap, DataSet *data, int max_display, float time_build);
DataSet *createDataSet(TrapezoidalMap *tpzMap);
void saveSegments(char *filename, TrapezoidalMap *tpzMap);

void initNodesList(Node *nodes, int idx_st, int idx_fn);

int isLeft(float *pa, float *pb); // pa < pb ?
int isAbove(float *pa, float *pb, float *pc); // c above [a->b] ?
int isAboveHelper(float *pa, float *pb, float *pc, float *pd); // [c->d] above [a->b] ?

GLsizei searchTpz(TrapezoidalMap *tpzMap, DataSet *data, float p[2], float helper[2]);
GLsizei *followSegment(TrapezoidalMap *tpzMap, DataSet *data, Segment *s, GLsizei *size);

void buildTpzlMapManual(TrapezoidalMap *tpzMap, DataSet *data);
void incrementTpzMap(TrapezoidalMap *tpzMap, DataSet *data, GLsizei i, GLsizei *tpz_crossed, GLsizei nb_tpz_crossed);
void buildTpzlMap(TrapezoidalMap *tpzMap, DataSet *data);

void freeTpzMap(TrapezoidalMap *tpzMap);
void freeDataSet(DataSet *data);

