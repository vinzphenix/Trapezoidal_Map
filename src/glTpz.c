#include "glTpz.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////// - UTILS - ///////////////////////////////////////////////////////////

double   zoom_init, w_init, h_init;
double   translate_init[2];

float    current_text_color[4] = {1.0,0.0,0.0,1.0};
float    current_color[4]      = {0.0,0.0,0.0,1.0};
float    current_line_color[4] = {0.0,0.0,0.0,1.0};
float 	 current_line_width = 0.005;
float 	 thin_line_width = 0.0025;
float 	 wide_line_width = 0.0075;

GLsizei  prev_size, pts_created = 0;

typedef bov_window_t glTpzWindow;
glTpzWindow* theCurrentWindow = NULL;
float w;
float h;


void glTpzSetColor(float color[4]) {
    for(int i = 0; i < 4; ++i) {
        current_color[i] = color[i];
    }
}

void glTpzSetTextColor(float color[4]) {
    for(int i = 0; i < 4; ++i) {
        current_text_color[i] = color[i];
    }
}

void glTpzSetLineWidth(float width) {
    current_line_width = width;
}

void glTpzSetColorLine(float color[4]) {
    for(int i = 0; i < 4; ++i) {
        current_line_color[i] = color[i];
    }
}

int glTpzGetKey(char theKey) {
    return (glfwGetKey(theCurrentWindow->self,theKey) == GLFW_PRESS) ;
}


static char theCurrentAction = '0';

char glTpzGetAction(void) {
    if (theCurrentAction != '0') {
        char theLastCurrentAction = theCurrentAction;
        theCurrentAction = '0';
        return theLastCurrentAction;
    }
    else {
        return theCurrentAction;
    }
}

static void glTpzKeyCallback(GLFWwindow* self, int key, int scancode, int action, int mods) {
    bov_window_t* window = (bov_window_t*) glfwGetWindowUserPointer(self);
    if (action==GLFW_PRESS || action==GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_ESCAPE :
                glfwSetWindowShouldClose(self, GL_TRUE);
                break;
            case GLFW_KEY_H :
                if(window->help_needed == 0) window->help_needed = 1;
                else                         window->help_needed = 0;
                break;
            case GLFW_KEY_W :
                window->param.zoom = zoom_init;
                window->param.translate[0] = translate_init[0];
                window->param.translate[1] = translate_init[1];
                printf("Reset zoom\n");
                break;
        }
    }
    
    if      (key == GLFW_KEY_X && action == GLFW_PRESS)  {theCurrentAction = 'X';}
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)  {theCurrentAction = 'A';}
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)  {theCurrentAction = 'D';}
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)  {theCurrentAction = 'E';}
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)  {theCurrentAction = 'F';}
    else if (key == GLFW_KEY_G && action == GLFW_PRESS)  {theCurrentAction = 'G';}
    else if (key == GLFW_KEY_I && action == GLFW_PRESS)  {theCurrentAction = 'I';}
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)  {theCurrentAction = 'R';}
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)  {theCurrentAction = 'S';}
    else if (key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(self,GL_TRUE);
    
}

void glTpzWindowCreate(const char *windowName, int w, int h, float *bounds) {
    bov_window_t *window = bov_window_new(w, h, windowName);
    theCurrentWindow = window;
    bov_window_set_color(window, (GLfloat[4]){0.9, 0.9, 0.8, 0.0});
    
    //
    // Defining the current viewport and stores it as the reference
    //
  
    double minX  = bounds[0];
    double maxX  = bounds[1];
    double minY  = bounds[2];
    double maxY  = bounds[3];
    double sizeX = (maxX - minX) / 2.00;
    double meanX = (maxX + minX) / 2.00; 
    double sizeY = (maxY - minY) / 2.00;
    double meanY = (maxY + minY) / 2.00;
    
    double ratio = (GLfloat) h / (GLfloat) w;
    double size = fmax(sizeX,sizeY);
    double left, right, top, bottom;
    left = bounds[0];
    right = bounds[1];
    bottom = bounds[2];
    top = bounds[3];

    window->param.zoom = 1 / (0.55 * (fabs(top - bottom)));  //-5 -3 4 1
  
    window->param.translate[0] = -(left + right) / 2.0;
    window->param.translate[1] = -(bottom + top) / 2.0;
  
    zoom_init = window->param.zoom;
    translate_init[0] = window->param.translate[0];
    translate_init[1] = window->param.translate[1];
    w_init = window->size[0];
    h_init = window->size[1];
    w = w_init;
    h = h_init;
    
    theCurrentWindow->cursorPos[0] = bounds[0] - 1.;
    theCurrentWindow->cursorPos[1] = bounds[2] - 1.;
    
    //
    // Default call back and help message
    //
    glfwSetKeyCallback(window->self, glTpzKeyCallback);
    glTpzWindowSetHelpMessage((const char[]) {
    "   [esc]   Exit\n"
    "    A      Add a point to create segments\n"
    "    D      Display / hide trapezoids\n"
    "    E      Enable / disable repusive force of the mouse on the particles\n"
    "    F      Enable / disable fast display of BOV\n"
    "    G      Enable / disable gravity\n"
    "    H      Display/hide keyboard shortcuts\n"
    "    I      Show the steps of the algorithm (should not be used with segments that touch each other)\n"
    "    S      Save the segments in a txt file\n"
    "    R      Reset the Trapezoidal Map\n"
    "    X      Add particles that bounce on segments\n"
    "    Z      Reset zoom and translation\n"
    });
}

void glTpzWindowSetHelpMessage(const char *message) {    
        
    bov_text_delete(theCurrentWindow->help);
    theCurrentWindow->help = bov_text_new((const GLubyte*)(message),GL_STATIC_DRAW);

    bov_text_set_space_type(theCurrentWindow->help, PIXEL_SPACE);
    bov_text_set_fontsize(theCurrentWindow->help, 30.0f); 
    bov_text_set_boldness(theCurrentWindow->help, 0.1f);
    bov_text_set_outline_width(theCurrentWindow->help, 0.5f);
    bov_text_set_color(theCurrentWindow->help, GLTPZ_BLACK);
}

void glTpzWindowResetSize() {
		theCurrentWindow->param.zoom = zoom_init;
		theCurrentWindow->param.translate[0] = translate_init[0];
		theCurrentWindow->param.translate[1] = translate_init[1];
}

void glTpzWindowUpdate(DisplayMap *map) {
    float w_ = theCurrentWindow->param.res[0];
    float w2 = theCurrentWindow->size[0];
	float ratio = w_/w2;
	
	w = theCurrentWindow->size[0];
	h = theCurrentWindow->size[1];
    
    map->mouse[0] = theCurrentWindow->cursorPos[0] - theCurrentWindow->param.translate[0];
    map->mouse[1] = theCurrentWindow->cursorPos[1] - theCurrentWindow->param.translate[1]; 
	  
    bov_text_set_fontsize(theCurrentWindow->help, ratio*25.0f);
    bov_text_set_pos(theCurrentWindow->help, (GLfloat[2]){0.02*w, h*0.85} );
    bov_window_update(theCurrentWindow);
}

void glTpzWindowUpdateAndWait() {
    bov_text_set_pos(theCurrentWindow->help, (GLfloat[2]){-20.0f, theCurrentWindow->size[1] + 30.0f});
    bov_window_update_and_wait_events(theCurrentWindow);
}

void glTpzWindowFree() {
    bov_window_delete(theCurrentWindow);
}

int glTpzWindowShouldClose() {
    return bov_window_should_close(theCurrentWindow);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////// - CREATE STRUCTURES - ////////////////////////////////////////////////////

DisplayMap* createDisplayMap(TrapezoidalMap *tpzMap) {
    DisplayMap *map = malloc(sizeof(DisplayMap));
    map->size_points = (GLsizei) 2 * tpzMap->n_seg;
    map->size_dots = (GLsizei) 2 * tpzMap->n_tpz + 2; // 2 by trapezoid + the left segment
    map->size_tri = (GLsizei) 3*2*1;  // 3*2 * 1 // ONLY 5 TPZ COLORED
    
    map->coord_pts = malloc(sizeof(map->coord_pts[0]) * map->size_points);
    map->coord_dots = malloc(sizeof(map->coord_dots[0]) * map->size_dots);
    map->coord_tri = malloc(sizeof(map->coord_tri[0]) * map->size_tri);

    map->drawing_line = 0;
    map->display_tpz = 1;
    map->apply_force = 0;
    map->apply_gravity = 0;
    map->display_fast = (tpzMap->n_tpz < LARGE_DATA) ? 0 : 1;
    map->display_step = 0;
    current_line_width = (map->display_fast) ? 0.002 : 0.005;
    
    GLsizei i;
    for (i = 0 ; i < tpzMap->n_seg ; i++) {
        map->coord_pts[2*i][0]   = tpzMap->points[tpzMap->segments[i].orig][0];
        map->coord_pts[2*i][1]   = tpzMap->points[tpzMap->segments[i].orig][1];
        map->coord_pts[2*i+1][0] = tpzMap->points[tpzMap->segments[i].dest][0];
        map->coord_pts[2*i+1][1] = tpzMap->points[tpzMap->segments[i].dest][1];
    }
    //printf("%3d coord_pts added \t allocated size was %3d\n", 2*i, map->size_points);
    
    GLsizei idx = 0;
    // Add the right vertical line 
    map->coord_dots[idx][0] = tpzMap->bounds[1];
    map->coord_dots[idx++][1] = tpzMap->bounds[2];
    map->coord_dots[idx][0] = tpzMap->bounds[1];
    map->coord_dots[idx++][1] = tpzMap->bounds[3];
    
    // Add the left vertical line of every trapezoid
    fillCoordDots(map, tpzMap);
    
    // Add triangles coord (default: outside box)
    for (i = 0 ; i < map->size_tri ; i++) {
        map->coord_tri[i][0] = 0.;
        map->coord_tri[i][1] = 0.;
    }
    
    map->points = bov_points_new(map->coord_pts, map->size_points, GL_STATIC_DRAW);
    map->dots = bov_points_new(map->coord_dots, map->size_dots, GL_STATIC_DRAW);
    map->triangles = bov_points_new(map->coord_tri, map->size_tri, GL_DYNAMIC_DRAW);
    
    // Add particles
    map->nb_part = 0;
    map->time = glfwGetTime();
    map->pos = malloc(sizeof(map->pos[0]));
    map->spd = malloc(sizeof(map->spd[0]));
    map->tpz_part = malloc(sizeof(GLsizei));
    map->particles = bov_points_new(map->pos, map->nb_part, GL_DYNAMIC_DRAW);
    
    createTexts(map);
    
    return map;
}

void createTexts(DisplayMap *map) {
    char msg[50] = "";
	map->text_tpz = bov_text_new((const GLubyte *)msg, GL_STATIC_DRAW);
    bov_text_set_space_type(map->text_tpz, PIXEL_SPACE);
    bov_text_set_fontsize(map->text_tpz, 20);
    bov_text_set_outline_width(map->text_tpz, 0.f);
    bov_text_set_color(map->text_tpz, GLTPZ_BLACK);
    
	map->text_pos = bov_text_new((const GLubyte *)msg, GL_STATIC_DRAW);
    bov_text_set_space_type(map->text_pos, PIXEL_SPACE);
    bov_text_set_fontsize(map->text_pos, 20);
    bov_text_set_outline_width(map->text_pos, 0.f);
    bov_text_set_color(map->text_pos, GLTPZ_BLACK);
    
	map->text_neigh = bov_text_new((const GLubyte *)msg, GL_STATIC_DRAW);
    bov_text_set_space_type(map->text_neigh, PIXEL_SPACE);
    bov_text_set_fontsize(map->text_neigh, 20);
    bov_text_set_outline_width(map->text_neigh, 0);
    bov_text_set_outline_color(map->text_neigh, GLTPZ_GREY);
    bov_text_set_color(map->text_neigh, GLTPZ_GREY);
    
	map->text_part = bov_text_new((const GLubyte *)msg, GL_STATIC_DRAW);
    bov_text_set_space_type(map->text_part, PIXEL_SPACE);
    bov_text_set_fontsize(map->text_part, 20);
    bov_text_set_outline_width(map->text_part, 0);
    bov_text_set_color(map->text_part, GLTPZ_BLUE);
}


void freeDisplayMap(DisplayMap *map) {
    bov_points_delete(map->points);
    bov_points_delete(map->dots);
    bov_points_delete(map->triangles);
    bov_points_delete(map->particles);
    bov_text_delete(map->text_tpz);
    bov_text_delete(map->text_pos);
    bov_text_delete(map->text_neigh);
    bov_text_delete(map->text_part);
    free(map->coord_pts);
    free(map->coord_dots);
    free(map->coord_tri);
    free(map->tpz_part);
    free(map->pos);
    free(map->spd);
    free(map);
}


void fillCoordDots(DisplayMap *map, TrapezoidalMap *tpzMap) {
    float y_intersect;
    float *p, *q, *r;
    GLsizei i, idx;
    Trapezoid *this_tpz;
    
    for (i = 0, idx = 2 ; i < tpzMap->n_tpz ; i++) {

        this_tpz = &tpzMap->trapezoids[i];
        r = tpzMap->points[this_tpz->left_pt];

        p = tpzMap->points[tpzMap->segments[this_tpz->bottom].orig];
        q = tpzMap->points[tpzMap->segments[this_tpz->bottom].dest];
        y_intersect = (q[0] == p[0]) ? (q[1]) : (p[1] + (q[1] - p[1]) * (r[0] - p[0]) / (q[0] - p[0]));
        map->coord_dots[idx][0] = r[0];
        map->coord_dots[idx++][1] = y_intersect;

        p = tpzMap->points[tpzMap->segments[this_tpz->top].orig];
        q = tpzMap->points[tpzMap->segments[this_tpz->top].dest];
        y_intersect = (q[0] == p[0]) ? (q[1]) : (p[1] + (q[1] - p[1]) * (r[0] - p[0]) / (q[0] - p[0]));
        map->coord_dots[idx][0] = r[0];
        map->coord_dots[idx++][1] = y_intersect;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////// - DRAWING FUNCTIONS - ////////////////////////////////////////////////////

void drawTrapezoids(DisplayMap *map, TrapezoidalMap *tpzMap) {
    if (theCurrentWindow->help_needed) return;
    
    if (map->display_fast) {
        bov_points_set_color(map->points, GLTPZ_RED);
        bov_fast_lines_draw(theCurrentWindow, map->points, 0, 2*tpzMap->n_seg);
        if (map->display_tpz == 0) return;
        
        bov_points_set_color(map->points, GLTPZ_GREY);
        bov_fast_lines_draw(theCurrentWindow, map->dots, 0, 2 * tpzMap->n_tpz + 2);
        return;
    }
    
    bov_points_set_color(map->points, GLTPZ_BLACK);
    bov_points_set_width(map->points, current_line_width/theCurrentWindow->param.zoom);
    bov_lines_draw(theCurrentWindow, map->points, 0, 2*tpzMap->n_seg);
    
    if (map->display_tpz == 1) {
        bov_points_set_color(map->dots, GLTPZ_GREY);
        bov_points_set_width(map->dots, current_line_width/theCurrentWindow->param.zoom);
        bov_lines_draw(theCurrentWindow, map->dots, 0, 2 * tpzMap->n_tpz + 2);
    
        bov_points_set_color(map->dots, GLTPZ_BLACK);
        bov_points_set_width(map->dots, 2*current_line_width/theCurrentWindow->param.zoom);
        bov_points_draw(theCurrentWindow, map->dots, 0, 2 * tpzMap->n_tpz + 2);
    }
    
    bov_points_set_color(map->points, GLTPZ_RED);
    bov_points_set_width(map->points, 3*current_line_width/theCurrentWindow->param.zoom);
    bov_points_draw(theCurrentWindow, map->points, 0, 2*tpzMap->n_seg + map->drawing_line);
    
    bov_points_set_color(map->particles, GLTPZ_BLUE_TSP);
    bov_points_set_width(map->particles, 4*current_line_width/theCurrentWindow->param.zoom);
    bov_points_draw(theCurrentWindow, map->particles, 0, map->nb_part);
}


void fillCoordTriangles(DisplayMap *map, TrapezoidalMap *tpzMap, GLsizei idx_tpz, GLsizei start) {
    Trapezoid *tpz = &tpzMap->trapezoids[idx_tpz];
    Segment *top = &tpzMap->segments[tpz->top];
    Segment *bottom = &tpzMap->segments[tpz->bottom];
    float *p, *q, *r;

    // Lower triangle of trapezoid
    r = tpzMap->points[tpz->left_pt]; p = tpzMap->points[top->orig]; q = tpzMap->points[top->dest];
    map->coord_tri[start][0] = r[0];
    map->coord_tri[start][1] = (p[0] == q[0]) ? (q[1]) : (p[1] + (q[1] - p[1]) * (r[0] - p[0]) / (q[0] - p[0]));
    r = tpzMap->points[tpz->left_pt]; p = tpzMap->points[bottom->orig]; q = tpzMap->points[bottom->dest];
    map->coord_tri[start+1][0] = r[0];
    map->coord_tri[start+1][1] = (q[0] == p[0]) ? (q[1]) : (p[1] + (q[1] - p[1]) * (r[0] - p[0]) / (q[0] - p[0]));
    r = tpzMap->points[tpz->right_pt]; p = tpzMap->points[bottom->orig]; q = tpzMap->points[bottom->dest];
    map->coord_tri[start+2][0] = r[0];
    map->coord_tri[start+2][1] = (q[0] == p[0]) ? (q[1]) : (p[1] + (q[1] - p[1]) * (r[0] - p[0]) / (q[0] - p[0]));
    
    // Upper triangle of trapezoid
    map->coord_tri[start+3][0] = map->coord_tri[start+2][0];
    map->coord_tri[start+3][1] = map->coord_tri[start+2][1];
    r = tpzMap->points[tpz->right_pt]; p = tpzMap->points[top->orig]; q = tpzMap->points[top->dest];
    map->coord_tri[start+4][0] = r[0];
    map->coord_tri[start+4][1] = (q[0] == p[0]) ? (q[1]) : (p[1] + (q[1] - p[1]) * (r[0] - p[0]) / (q[0] - p[0]));
    map->coord_tri[start+5][0] = map->coord_tri[start+0][0];
    map->coord_tri[start+5][1] = map->coord_tri[start+0][1];
}


void searchAndColorTrapezoid(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data) {
    if (theCurrentWindow->help_needed) return;
    if (((tpzMap->bounds[0] < map->mouse[0]) && (map->mouse[0] < tpzMap->bounds[1])) && ((tpzMap->bounds[2] < map->mouse[1]) && (map->mouse[1] < tpzMap->bounds[3]))) {
        float helper[2] = {0., 0.};

        GLsizei idx_tpz = searchTpz(tpzMap, data, map->mouse, helper);
        GLsizei idx_tri = 0;
        
        fillCoordTriangles(map, tpzMap, idx_tpz, 0);
        map->triangles = bov_points_partial_update(map->triangles, map->coord_tri, 0, 6, map->size_tri);
        bov_points_set_color(map->triangles, GLTPZ_GREEN_TSP);
        bov_points_set_width(map->triangles, 0.);
        bov_triangles_draw(theCurrentWindow, map->triangles, 0, 6);
        
        displayInfo(map, tpzMap, idx_tpz);
        
        if (map->display_tpz == 0) return;
        
        if (tpzMap->trapezoids[idx_tpz].low_left != -1) {
            fillCoordTriangles(map, tpzMap, tpzMap->trapezoids[idx_tpz].low_left, 0);
            map->triangles = bov_points_partial_update(map->triangles, map->coord_tri, 0, 6, map->size_tri);
            bov_points_set_color(map->triangles, GLTPZ_RED_TSP);
            bov_points_set_width(map->triangles, 0.);
            bov_triangles_draw(theCurrentWindow, map->triangles, 0, 6);
        }
        if (tpzMap->trapezoids[idx_tpz].up_left != -1) {
            fillCoordTriangles(map, tpzMap, tpzMap->trapezoids[idx_tpz].up_left, 0);
            map->triangles = bov_points_partial_update(map->triangles, map->coord_tri, 0, 6, map->size_tri);
            bov_points_set_color(map->triangles, GLTPZ_YEL_TSP);
            bov_points_set_width(map->triangles, 0.);
            bov_triangles_draw(theCurrentWindow, map->triangles, 0, 6);
        }
        if (tpzMap->trapezoids[idx_tpz].low_right != -1) {
            fillCoordTriangles(map, tpzMap, tpzMap->trapezoids[idx_tpz].low_right, 0);
            map->triangles = bov_points_partial_update(map->triangles, map->coord_tri, 0, 6, map->size_tri);
            bov_points_set_color(map->triangles, GLTPZ_RED_TSP);
            bov_points_set_width(map->triangles, 0.);
            bov_triangles_draw(theCurrentWindow, map->triangles, 0, 6);
        }
        if (tpzMap->trapezoids[idx_tpz].up_right != -1) {
            fillCoordTriangles(map, tpzMap, tpzMap->trapezoids[idx_tpz].up_right, 0);
            map->triangles = bov_points_partial_update(map->triangles, map->coord_tri, 0, 6, map->size_tri);
            bov_points_set_color(map->triangles, GLTPZ_YEL_TSP);
            bov_points_set_width(map->triangles, 0.);
            bov_triangles_draw(theCurrentWindow, map->triangles, 0, 6);
        }
        
        /*printf("1 %p (%6.2f, %6.2f) -> (%6.2f, %6.2f) -> (%6.2f, %6.2f)\n", tpz, 
               map->coord_tri[0][0], map->coord_tri[0][1], map->coord_tri[1][0], map->coord_tri[1][1],
               map->coord_tri[2][0], map->coord_tri[2][1]); fflush(stdout);
          printf("%6.2f < %6.2f < %6.2f, %6.2f < %6.2f < %6.2f\n", tpzMap->bounds[0], mouse.x,
                 tpzMap->bounds[1], tpzMap->bounds[2], mouse.y, tpzMap->bounds[3]);*/
    }
}


void displayInfo(DisplayMap *map, TrapezoidalMap *tpzMap, GLsizei i) {
    char msg[50] = "";
    sprintf(msg, "Tpz %3d / %3d\n", i+1, tpzMap->n_tpz);
    map->text_tpz = bov_text_update(map->text_tpz, msg);
    bov_text_set_pos(map->text_tpz, (GLfloat[2]) {0.80 * w, 0.97*h});
    bov_text_draw(theCurrentWindow, map->text_tpz);
    
    char msg2[50] = "";
    sprintf(msg2, "x:%6.2f\t y:%6.2f\n", map->mouse[0], map->mouse[1]);
    map->text_pos = bov_text_update(map->text_pos, msg2);
    bov_text_set_pos(map->text_pos, (GLfloat[2]) {0.05 * w, 0.97*h});
    bov_text_draw(theCurrentWindow, map->text_pos);
    
    char msg3[80] = "";
    if (tpzMap->trapezoids[i].low_left != -1) sprintf(msg3, "%6s:%4d\t\t", "lo-lft", tpzMap->trapezoids[i].low_left+1);
    else sprintf(msg3, "%6s %4s\t\t", "", "");
    if (tpzMap->trapezoids[i].up_left != -1) sprintf(msg3+strlen(msg3), "%6s:%4d\t\t", "up-lft", tpzMap->trapezoids[i].up_left+1);
    else sprintf(msg3+strlen(msg3), "%6s %4s\t\t", "", "");
    if (tpzMap->trapezoids[i].low_right != -1) sprintf(msg3+strlen(msg3), "%6s:%4d\t\t", "lo-rgt", tpzMap->trapezoids[i].low_right+1);
    else sprintf(msg3+strlen(msg3), "%6s %4s\t\t", "", "");
    if (tpzMap->trapezoids[i].up_right != -1) sprintf(msg3+strlen(msg3), "%6s:%4d\t\t", "up-rgt", tpzMap->trapezoids[i].up_right+1);
    else sprintf(msg3+strlen(msg3), "%6s %4s\t\t", "", "");
    map->text_neigh = bov_text_update(map->text_neigh, msg3);
    bov_text_set_pos(map->text_neigh, (GLfloat[2]) {0.05 * w, 0.01*h});
    bov_text_draw(theCurrentWindow, map->text_neigh);
    
    char msg4[50] = "";
    sprintf(msg4, "Number of particles : %4d\n", map->nb_part);
    map->text_part = bov_text_update(map->text_part, msg4);
    bov_text_set_pos(map->text_part, (GLfloat[2]) {0.40 * w, 0.97*h});
    bov_text_draw(theCurrentWindow, map->text_part);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////// - UPDATE TRAPEZOIDAL MAP - //////////////////////////////////////////////////

GLsizei *followSegmentRobust(TrapezoidalMap *tpzMap, DataSet *data, Segment *s, int *size, GLfloat location[3]) {
    // direction must be 1 (s going to the right) or -1 (s going to the left)
    
    GLsizei p, q, right_pt, j = 0;
    p = s->orig; q = s->dest;
    
    GLsizei *tpz_crossed = malloc(sizeof(GLsizei) * 1); // list of indexes
    tpz_crossed[0] = searchTpz(tpzMap, data, tpzMap->points[p], tpzMap->points[q]);

    right_pt = tpzMap->trapezoids[tpz_crossed[0]].right_pt;
    
    if (alertCrossing(tpzMap, tpzMap->points[p], tpzMap->points[q], &tpzMap->trapezoids[tpz_crossed[0]], location) != 0) {
        printf("crossing at (%6.2f, %6.2f)\n", location[0], location[1]);
        *size = 0;
        return NULL; // a segment was crossed --> abort
    }
    
    // while right_pt on the left of q
    while (tpzMap->points[right_pt][0] < tpzMap->points[q][0]) {
        tpz_crossed = realloc(tpz_crossed, sizeof(GLsizei) * (j + 2));
        
        if (isAbove(tpzMap->points[p], tpzMap->points[q], tpzMap->points[right_pt])) {
            tpz_crossed[j+1] = tpzMap->trapezoids[tpz_crossed[j]].low_right;
        }
        else {
            tpz_crossed[j+1] = tpzMap->trapezoids[tpz_crossed[j]].up_right;
        }
        
        if (alertCrossing(tpzMap, tpzMap->points[p], tpzMap->points[q], &tpzMap->trapezoids[tpz_crossed[j+1]], location) != 0) {
            printf("crossing at (%6.2f, %6.2f)\n", location[0], location[1]);
            *size = -(j + 1);
            return NULL; // a segment was crossed --> abort
        } 
        
        right_pt = tpzMap->trapezoids[tpz_crossed[j+1]].right_pt;
        j++;
    }
    
    *size = j + 1;
    return tpz_crossed;
}


int alertCrossing(TrapezoidalMap *tpzMap, float o[2], float d[2], Trapezoid *this_tpz, GLfloat location[3]) {
    GLsizei j, k, p, q;
    float det_1, det_2, denom;
    
    //o = s->orig; d = s->dest;
    
    for (k = 0 ; k < 2 ; k++) {
        p = (k == 0) ? tpzMap->segments[this_tpz->bottom].orig : tpzMap->segments[this_tpz->top].orig;
        q = (k == 0) ? tpzMap->segments[this_tpz->bottom].dest : tpzMap->segments[this_tpz->top].dest;
        
#if ROBUST
        det_1 = orient2d(tpzMap->points[p], o, d);
        det_2 = orient2d(o, tpzMap->points[q], tpzMap->points[p]);
#else
        det_1 = (tpzMap->points[p][0] - d[0]) * (o[1] - d[1]) - (tpzMap->points[p][1] - d[1]) * (o[0] - d[0]);
        det_2 = (tpzMap->points[q][0] - o[0]) * (tpzMap->points[p][1] - o[1]) - (tpzMap->points[q][1] - o[1]) * (tpzMap->points[p][0] - o[0]);
#endif
        denom = (tpzMap->points[q][0] - tpzMap->points[p][0]) * (d[1] - o[1])
              - (tpzMap->points[q][1] - tpzMap->points[p][1]) * (d[0] - o[0]);        
        //printf("denom=%6.2f det_1=%6.2f det_2=%6.2f\n", denom, det_1, det_2);
        
        if (denom == 0.) { // parallel (almost never happens)

            // check if origin same as destination
            if (d[0] == o[0] && d[1] == o[1]) {
                location[0] = o[0]; location[1] = o[1]; location[2] = 0.;
                return 2*k-1;
            }
            
            det_1 = (tpzMap->points[q][0] - tpzMap->points[p][0]) * (o[0] - tpzMap->points[p][0])
                  + (tpzMap->points[q][1] - tpzMap->points[p][1]) * (o[1] - tpzMap->points[p][1]);
            det_2 = (tpzMap->points[q][0] - tpzMap->points[p][0]) * (d[0] - tpzMap->points[p][0])
                  + (tpzMap->points[q][1] - tpzMap->points[p][1]) * (d[1] - tpzMap->points[p][1]);
            denom = (tpzMap->points[q][0] - tpzMap->points[p][0]) * (tpzMap->points[q][0] - tpzMap->points[p][0])
                  + (tpzMap->points[q][1] - tpzMap->points[p][1]) * (tpzMap->points[q][1] - tpzMap->points[p][1]);
            
            if ((det_1 == 0) && (0 < det_1 && det_1 < denom || 0 < det_2 && det_2 < denom)) {
                location[0] = o[0]; location[1] = o[1]; location[2] = 0.;
                return 2 * k - 1; // colinear and overlaping
            }
        }
        else if (denom > 0. && 0. < det_1 && det_1 < denom && 0. < det_2 && det_2 < denom) { 
            location[0] = tpzMap->points[p][0] + det_1/denom * (tpzMap->points[q][0] - tpzMap->points[p][0]);
            location[1] = tpzMap->points[p][1] + det_1/denom * (tpzMap->points[q][1] - tpzMap->points[p][1]);
            location[2] = det_1 / denom;
            return 2*k-1; // crossing
        }
        else if (denom < 0. && denom < det_1 && det_1 < 0. && denom < det_2 && det_2 < 0.) {  // crossing
            location[0] = tpzMap->points[p][0] + det_1/denom * (tpzMap->points[q][0] - tpzMap->points[p][0]);
            location[1] = tpzMap->points[p][1] + det_1/denom * (tpzMap->points[q][1] - tpzMap->points[p][1]);
            location[2] = det_1 / denom;
            return 2*k-1;
        }
    }
    return 0;
}


void addNewPoint(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data, int merge_close) {
    
    if (map->mouse[0] < tpzMap->bounds[0] || tpzMap->bounds[1] < map->mouse[0] 
     || map->mouse[1] < tpzMap->bounds[2] || tpzMap->bounds[3] < map->mouse[1])
        return; // OUT OF THE BOX
    
    GLsizei idx_lft, idx_rgt, idx_tpz, idx_new_pt;
    idx_tpz = searchTpz(tpzMap, data, map->mouse, map->mouse);
    idx_lft = tpzMap->trapezoids[idx_tpz].left_pt;
    idx_rgt = tpzMap->trapezoids[idx_tpz].right_pt;
    
    float tol = 0.05;
    int close_lft = hypot(map->mouse[0] - tpzMap->points[idx_lft][0], map->mouse[1] - tpzMap->points[idx_lft][1]) < tol / theCurrentWindow->param.zoom;
    int close_rgt = hypot(map->mouse[0] - tpzMap->points[idx_rgt][0], map->mouse[1] - tpzMap->points[idx_rgt][1]) < tol / theCurrentWindow->param.zoom;

    if (!merge_close) {
        close_rgt = 0;
        close_lft = 0;
    }
    //printf("dst to left = %6.4f\t dst to right = %6.4f and w = %f\n", dist_to_lft_pt, dist_to_rgt_pt, theCurrentWindow->param.zoom);

    ///////////////////////   -   Check memory allocation   -   ///////////////////////
    if (tpzMap->n_pts == tpzMap->size_pts && close_lft + close_rgt == 0) { // if mouse is close -> don't need create point
        prev_size = tpzMap->size_pts;
        tpzMap->size_pts *= 2;
        tpzMap->points = realloc(tpzMap->points, sizeof(float[2]) * (tpzMap->size_pts));
        printf("%-20s Increasing the size of %12s : %4d -> %4d\n", "[Trapezoidal Map]", "[points]", prev_size, tpzMap->size_pts);
    }
    if (tpzMap->n_seg == tpzMap->size_seg) {
        prev_size = tpzMap->size_seg;
        tpzMap->size_seg *= 2;
        tpzMap->segments = realloc(tpzMap->segments, sizeof(Segment) * tpzMap->size_seg);
        printf("%-20s Increasing the size of %12s : %4d -> %4d\n", "[Trapezoidal Map]", "[segments]", prev_size, tpzMap->size_seg);
    }
    if (2 * tpzMap->n_seg == map->size_points) {
        prev_size = map->size_points;
        map->size_points *= 2;
        map->coord_pts = realloc(map->coord_pts, sizeof(map->coord_pts[0]) * map->size_points);
        printf("%-20s Increasing the size of %12s : %4d -> %4d\n", "[Display Map]", "[coord_pts]", prev_size, map->size_points);
    }
    ///////////////////////////////////////////////////////////////////////////////////
    
    if (close_lft == 1) idx_new_pt = idx_lft;
    else if (close_rgt == 1) idx_new_pt = idx_rgt;
    else {
        idx_new_pt = tpzMap->n_pts;
        tpzMap->points[idx_new_pt][0] = map->mouse[0];
        tpzMap->points[idx_new_pt][1] = map->mouse[1];
        tpzMap->n_pts++;
        pts_created++;
    }
    
    map->coord_pts[2 * tpzMap->n_seg + map->drawing_line][0] = tpzMap->points[idx_new_pt][0];
    map->coord_pts[2 * tpzMap->n_seg + map->drawing_line][1] = tpzMap->points[idx_new_pt][1];
    bov_points_update(map->points, map->coord_pts, 2 * tpzMap->n_seg + 1 + map->drawing_line);
    
    if (map->drawing_line == 0) tpzMap->segments[tpzMap->n_seg].orig = idx_new_pt;
    else tpzMap->segments[tpzMap->n_seg].dest = idx_new_pt;
    map->drawing_line++;
}


void addNewSegment(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data) {
    
    GLsizei temp, idx = tpzMap->n_seg;
    if (isLeft(tpzMap->points[tpzMap->segments[idx].orig], tpzMap->points[tpzMap->segments[idx].dest]) == 0) {
        temp = tpzMap->segments[idx].orig;
        tpzMap->segments[idx].orig = tpzMap->segments[idx].dest;
        tpzMap->segments[idx].dest = temp;
    }

    GLfloat location[3];
    GLsizei nb_tpz_crossed;
    GLsizei *tpz_crossed = followSegmentRobust(tpzMap, data, &tpzMap->segments[tpzMap->n_seg], &nb_tpz_crossed, location);
    
    if (nb_tpz_crossed <= 0) { // a segment was crossed
        tpzMap->n_pts -= pts_created; // remove the point(s) created (0, 1, 2)
        bov_points_update(map->points, map->coord_pts, 2 * tpzMap->n_seg);
    }
    else { // update tpzMap, DAG, displayMap
        tpzMap->n_seg++;
        incrementTpzMap(tpzMap, data, tpzMap->n_seg-1, tpz_crossed, nb_tpz_crossed);
        
        if (map->size_dots < 2 * tpzMap->n_tpz + 2) {
            prev_size = map->size_dots;
            map->size_dots = (2 * tpzMap->n_tpz + 2) * 2;
            map->coord_dots = realloc(map->coord_dots, sizeof(map->coord_dots[0]) * map->size_dots);
            printf("%-20s Increasing the size of %12s : %4d -> %4d\n", "[Display Map]", "[coord_dots]", prev_size, map->size_dots);
        }
        
        fillCoordDots(map, tpzMap);
        map->dots = bov_points_update(map->dots, map->coord_dots, 2 * tpzMap->n_tpz + 2);
    }
    
    map->drawing_line = 0;
    pts_created = 0; // global variable
}


void resetStructures(TrapezoidalMap *tpzMap, DataSet *data, DisplayMap *map) {
    printf("Reset program\n");
    tpzMap->n_pts = 4;
    tpzMap->n_seg = 2;
    
    tpzMap->trapezoids[0].top = 0; tpzMap->trapezoids[0].bottom = 1;
    tpzMap->trapezoids[0].left_pt = 2; tpzMap->trapezoids[0].right_pt = 1;
    tpzMap->trapezoids[0].up_left = -1;  tpzMap->trapezoids[0].up_right = -1;
    tpzMap->trapezoids[0].low_left = -1; tpzMap->trapezoids[0].low_right = -1;
    tpzMap->n_tpz = 1;
    
    data->n_nodes = 1;
    data->nodes[0].type = 't';
    data->nodes[0].idx = 0;
    tpzMap->trapezoids[0].leaf = 0;
    
    fillCoordDots(map, tpzMap);
    bov_points_update(map->dots, map->coord_dots, 4);
    bov_points_update(map->points, map->coord_pts, 4);

    map->drawing_line = 0;
    map->apply_force = 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////// - DISPLAY THE STEPS OF THE ALGO - //////////////////////////////////////////////

void callDisplayStep(TrapezoidalMap *tpzMap, DataSet *data, DisplayMap *map) {  
    if (map->display_step == 0 && tpzMap->n_tpz > 1) {
        map->display_step = 4;
        map->coord_pts_anim = malloc(map->size_points * sizeof(GLfloat[2]));
        memcpy(map->coord_pts_anim, map->coord_pts, map->size_points * sizeof(GLfloat[2]));
        map->size_points_anim = map->size_points;
        resetStructures(tpzMap, data, map);
        //printf("Animation ongoing... ");
    }
}


void showStep(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data, float time_anim) {
    
    if (map->display_step == map->size_points_anim) {
        //printf("\rAnimation ongoing... step %4d  terminated\n", map->display_step/2-1); fflush(stdout);
        map->display_step = 0;
        free(map->coord_pts_anim);
        return;
    }
    //printf("\rAnimation ongoing... step %4d", map->display_step/2-1); fflush(stdout);
    
    map->mouse[0] = map->coord_pts_anim[map->display_step][0];
    map->mouse[1] = map->coord_pts_anim[map->display_step][1];
    addNewPoint(map, tpzMap, data, 0);
    map->display_step++;

    /*map->mouse[0] = map->coord_pts_anim[map->display_step][0];
    map->mouse[1] = map->coord_pts_anim[map->display_step][1];
    addNewPoint(map, tpzMap, data, 0);
    map->display_step++;*/
    
    if (map->display_step % 2 == 0) addNewSegment(map, tpzMap, data);
    
    usleep(1e6 * time_anim / map->size_points_anim);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////// - PARTICLES SIMULATION - ///////////////////////////////////////////////////

void addParticle(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data, GLsizei nb) {
    if (map->mouse[0] < tpzMap->bounds[0] || tpzMap->bounds[1] < map->mouse[0] ||
        map->mouse[1] < tpzMap->bounds[2] || tpzMap->bounds[3] < map->mouse[1])
        return;
    
    map->nb_part += nb;
    map->pos = realloc(map->pos, sizeof(map->pos[0]) * map->nb_part);
    map->spd = realloc(map->spd, sizeof(map->spd[0]) * map->nb_part);
    map->tpz_part = realloc(map->tpz_part, sizeof(GLsizei) * map->nb_part);
    
    for (GLsizei i = 0 ; i < nb ; i++) {
        map->pos[map->nb_part-nb+i][0] = map->mouse[0];
        map->pos[map->nb_part-nb+i][1] = map->mouse[1];
        
        GLfloat ref_speed = fmax(tpzMap->bounds[1] - tpzMap->bounds[0], tpzMap->bounds[3] - tpzMap->bounds[2]) / (10 * sqrt(2));
        map->spd[map->nb_part-nb+i][0] = 2 * ((GLfloat) rand() / (GLfloat) RAND_MAX - 0.5) * ref_speed;
        map->spd[map->nb_part-nb+i][1] = 2 * ((GLfloat) rand() / (GLfloat) RAND_MAX - 0.5) * ref_speed;
        
        float now = glfwGetTime();
        float helper[2] = {map->pos[map->nb_part-nb+i][0] + (now - map->time) * map->spd[map->nb_part-nb+i][0],
                           map->pos[map->nb_part-nb+i][1] + (now - map->time) * map->spd[map->nb_part-nb+i][1]};
        map->tpz_part[map->nb_part-nb+i] = searchTpz(tpzMap, data, map->pos[map->nb_part-nb+i], helper);
    }
    map->particles = bov_points_update(map->particles, map->pos, map->nb_part);
    map->apply_force = 0;
}


int followPathParticle(TrapezoidalMap *tpzMap, DataSet *data, float from[2], float to[2], GLsizei *idx, GLfloat location[3]) {
    // direction must be 1 (s going to the right) or -1 (s going to the left)
    
    GLsizei left_pt, right_pt;
    GLsizei idx_tpz = searchTpz(tpzMap, data, from, to);
    int res;
    if (isLeft(from, to)) { // segment headed to the right

        right_pt = tpzMap->trapezoids[idx_tpz].right_pt;
        if ((res = alertCrossing(tpzMap, from, to, &tpzMap->trapezoids[idx_tpz], location)) != 0) {
            *idx = idx_tpz;
            return res; // a segment was crossed --> abort
        }

        while (tpzMap->points[right_pt][0] < to[0]) { // while right_pt on the left of "to"
            if (isAbove(from, to, tpzMap->points[right_pt])) { // right_pt above of [from-to]
                idx_tpz = tpzMap->trapezoids[idx_tpz].low_right;
            }
            else { // right_pt below of [from-to]
                idx_tpz = tpzMap->trapezoids[idx_tpz].up_right;
            }
            if (idx_tpz == -1) return 0; // out of box by its side
            if ((res = alertCrossing(tpzMap, from, to, &tpzMap->trapezoids[idx_tpz], location)) != 0) {
                *idx = idx_tpz;
                return res; // a segment was crossed --> abort
            }
            right_pt = tpzMap->trapezoids[idx_tpz].right_pt;
        }
    }
    else { // segment headed to the left

        left_pt = tpzMap->trapezoids[idx_tpz].left_pt;
        if ((res = alertCrossing(tpzMap, from, to, &tpzMap->trapezoids[idx_tpz], location)) != 0) {
            *idx = idx_tpz;
            return res; // a segment was crossed --> abort
        }

        while (to[0] < tpzMap->points[left_pt][0]) { // while left_pt on the left of to
            if (isAbove(from, to, tpzMap->points[left_pt])) { // left_pt above of [from-to]
                idx_tpz = tpzMap->trapezoids[idx_tpz].low_left;
            }
            else { // left_pt below of [from-to]
                idx_tpz = tpzMap->trapezoids[idx_tpz].up_left;
            }
            if (idx_tpz == -1) return 0; // out of box by its side
            if ((res = alertCrossing(tpzMap, from, to, &tpzMap->trapezoids[idx_tpz], location)) != 0) {
                *idx = idx_tpz;
                return res; // a segment was crossed --> abort
            }
            left_pt = tpzMap->trapezoids[idx_tpz].left_pt;
        }
    }    
    return 0;
}


void updateParticles(DisplayMap *map, TrapezoidalMap *tpzMap, DataSet *data) {
    
    GLsizei i, k, idx_tpz, idx_p, idx_q, idx_tpz_cross;
    int res;
    Trapezoid *this_tpz;
    
    float now = glfwGetTime();
    float dt = now - map->time;
    float length = fmax(tpzMap->bounds[1] - tpzMap->bounds[0], tpzMap->bounds[3] - tpzMap->bounds[2]);

    float cst_drag = 1. / length;                              // unit [kg / m]
    float cst_rpls = powf(length, 3.) / powf(TAU, 2);          // unit [kg m^3 / s^2]
    float gravity = (map->apply_gravity == 1) ? GRAVITY : 0.;  // unit [m / s^2]
    
    float dist_x, dist_y, dist, drag, rpls, dot_prod, norm2;
    float p[2], q[2], u[2], w[2], next_pos[2], crossing[3];
    // crossing_x, crossing_y, crossing_time
    
    for (i = 0 ; i < map->nb_part ; i++) {
        
        // get the tpz of this particle
        float helper[2] = {map->pos[i][0] + dt * map->spd[i][0], map->pos[i][1] + dt * map->spd[i][1]};
        idx_tpz = searchTpz(tpzMap, data, map->pos[i], helper);
        this_tpz = &tpzMap->trapezoids[idx_tpz];
        
        // check if particule touches a segment at next step
        next_pos[0] = map->pos[i][0] + map->spd[i][0] * dt;
        next_pos[1] = map->pos[i][1] + map->spd[i][1] * dt;
        
        // left - right limits
        if (next_pos[0] < tpzMap->bounds[0] || next_pos[0] > tpzMap->bounds[1]) map->spd[i][0] *= -1;
        if (next_pos[1] < tpzMap->bounds[2] || next_pos[1] > tpzMap->bounds[3]) {map->spd[i][1] *= -1; continue;}
        
        // check for intersections
        res = followPathParticle(tpzMap, data, map->pos[i], next_pos, &idx_tpz_cross, crossing);
        if (res == -1) { // bottom touched
            //printf("bottom tpz %d touched (%6.2f, %6.2f)\n", idx_tpz_cross, crossing[0], crossing[1]);
            this_tpz = &tpzMap->trapezoids[idx_tpz_cross];
            idx_p = tpzMap->segments[this_tpz->bottom].orig;
            idx_q = tpzMap->segments[this_tpz->bottom].dest;
        }
        if (res == 1) {// top touched
            //printf("top    tpz %d touched (%6.2f, %6.2f)\n", idx_tpz_cross, crossing[0], crossing[1]);
            this_tpz = &tpzMap->trapezoids[idx_tpz_cross];
            idx_p = tpzMap->segments[this_tpz->top].orig;
            idx_q = tpzMap->segments[this_tpz->top].dest;
        }
        
        if (res != 0) {
            p[0] = tpzMap->points[idx_p][0]; p[1] = tpzMap->points[idx_p][1];
            q[0] = tpzMap->points[idx_q][0]; q[1] = tpzMap->points[idx_q][1];
            
            dot_prod = map->spd[i][0] * (q[0] - p[0]) + map->spd[i][1] * (q[1] - p[1]);
            norm2 = (q[0] - p[0]) * (q[0] - p[0]) + (q[1] - p[1]) * (q[1] - p[1]);
            w[0] = dot_prod / norm2 * (q[0] - p[0]);
            w[1] = dot_prod / norm2 * (q[1] - p[1]);
            
            u[0] = map->spd[i][0] - w[0];
            u[1] = map->spd[i][1] - w[1];
            
            map->spd[i][0] = FRICTION * w[0] - LOSS * u[0];
            map->spd[i][1] = FRICTION * w[1] - LOSS * u[1];
            
            map->pos[i][0] = crossing[0] + (1 - crossing[2]) * map->spd[i][0] * dt;
            map->pos[i][1] = crossing[1] + (1 - crossing[2]) * map->spd[i][1] * dt;

            // some particles sometimes pass through the verticies as if the edges were not closed
            // (they bounce on one wall and land outside of the face (TO BE IMPROVED)            
        }
        
        // update position
        map->pos[i][0] += map->spd[i][0] * dt;
        map->pos[i][1] += map->spd[i][1] * dt;
        
        // update velocity
        dist_x = map->pos[i][0] - map->mouse[0];
        dist_y = map->pos[i][1] - map->mouse[1];
        dist = fmax(length/50., hypot(dist_x, dist_y));
        
        rpls = (map->apply_force == 0) ? 0. : cst_rpls * dist_x / powf(dist, 3); 
        drag = cst_drag * hypot(map->spd[i][0], map->spd[i][1]) * map->spd[i][0];
        map->spd[i][0] += (rpls - drag) * dt;
        
        rpls = (map->apply_force == 0) ? 0. : cst_rpls * dist_y / powf(dist, 3);
        drag = cst_drag * hypot(map->spd[i][0], map->spd[i][1]) * map->spd[i][1];
        map->spd[i][1] += (rpls - drag - gravity) * dt;
    }
    map->particles = bov_points_partial_update(map->particles, map->pos, 0, map->nb_part, map->nb_part);
    map->time = now;
}

