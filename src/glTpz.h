#include "tpzMap.h"

static float GLTPZ_BLACK[4]  = {0.0,0.0,0.0,1.0};
static float GLTPZ_WHITE[4]  = {1.0,1.0,1.0,0.75};
static float GLTPZ_GREY[4]   = {0.0,0.0,0.0,0.35};
static float GLTPZ_BLUE[4]   = {0.0,0.0,1.0,1.0};
static float GLTPZ_RED[4]    = {1.0,0.0,0.0,1.0};
static float GLTPZ_GREEN[4]  = {0.0,1.0,0.0,1.0};

static float GLTPZ_GREEN_TSP[4] = {0.0, 1.0, 0.0, 0.35};
static float GLTPZ_BLUE_TSP[4]  = {0.0, 0.0, 1.0, 0.50};
static float GLTPZ_RED_TSP[4]   = {1.0, 0.0, 0.0, 0.15};
static float GLTPZ_YEL_TSP[4]   = {1.0, 1.0, 0.0, 0.30};


#define GRAVITY 5.
#define DRAG 1.     // drag constant (force \propto speeed^2)
#define TAU 20.      // time constant
#define FRICTION 0.9 // [0, 1] : [lose, keep] all tangential speed
#define LOSS 0.9     // [0, 1] : [lose, keep] all normal speed

#define LARGE_DATA 100000

typedef struct {
    GLfloat (*coord_pts)[2];
    GLfloat (*coord_dots)[2];
    GLfloat (*coord_tri)[2];
    GLsizei size_points;
    GLsizei size_dots;
    GLsizei size_tri;
    
    int drawing_line;
    GLfloat mouse[2];
    int display_tpz;
    int apply_force;
    int apply_gravity;
    int display_fast;
    
    int display_step;
    GLfloat (*coord_pts_anim)[2];
    GLsizei size_points_anim;
    
    bov_points_t *points;
    bov_points_t *dots;
    bov_points_t *triangles;
    
    GLsizei nb_part;
    GLsizei *tpz_part;
    GLfloat (*pos)[2];
    GLfloat (*spd)[2];
    GLfloat time;
    bov_points_t *particles;
    
    bov_text_t *text_tpz;
    bov_text_t *text_pos;
    bov_text_t *text_neigh;
    bov_text_t *text_part;
} DisplayMap;


void glTpzSetColor(float color[4]);
void glTpzSetTextColor(float color[4]);
void glTpzSetLineWidth(float width);
void glTpzSetColorLine(float color[4]);
int glTpzGetKey(char theKey);
char glTpzGetAction(void);
static void glTpzKeyCallback(GLFWwindow* self, int key, int scancode, int action,int mods);
void glTpzWindowCreate(const char *windowName,int w, int h, float *bounds);
void glTpzWindowCreate(const char *windowName,int w, int h, float *bounds);
void glTpzWindowSetHelpMessage(const char *message);
void glTpzWindowResetSize();
void glTpzWindowUpdate();
void glTpzWindowUpdateAndWait();
void glTpzWindowFree();
int glTpzWindowShouldClose();

DisplayMap* createDisplayMap(TrapezoidalMap *tpzMap);
void createTexts(DisplayMap *map);
void freeDisplayMap(DisplayMap *map);
void fillCoordDots(DisplayMap *map, TrapezoidalMap *tpzMap);

void drawTrapezoids(DisplayMap *map, TrapezoidalMap *tpzMap);
void searchAndColorTrapezoid(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data);
void displayInfo(DisplayMap *map, TrapezoidalMap *tpzMap, GLsizei i);

GLsizei *followSegmentRobust(TrapezoidalMap *tpzMap, DataSet *data, Segment *s, int *size, GLfloat location[3]);
int alertCrossing(TrapezoidalMap *tpzMap, float o[2], float d[2], Trapezoid *this_tpz, GLfloat location[3]);
void addNewPoint(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data, int merge_close);
void addNewSegment(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data);

void resetStructures(TrapezoidalMap *tpzMap, DataSet *data, DisplayMap *map);
void callDisplayStep(TrapezoidalMap *tpzMap, DataSet *data, DisplayMap *map);
void showStep(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data, float time_anim);

void addParticle(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data, GLsizei nb);
void updateParticles(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data);
int followPathParticle(TrapezoidalMap *tpzMap, DataSet *data, float p[2], float q[2], GLsizei *idx, GLfloat location[3]);

