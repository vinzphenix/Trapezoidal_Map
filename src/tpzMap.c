#include "tpzMap.h"


void tpzError(char *text, int line, char *file) { 
    printf("\n-------------------------------------------------------------------------------- ");
    printf("\n  Error in %s at line %d : \n  %s\n", file, line, text);
    printf("-------------------------------------------------------------------------------!! \n\n");
    exit(69);                                                 
}


void tpzErrorScan(int test, int line, char *file) {
    if (test > 0)  return;
    printf("\n-------------------------------------------------------------------------------- ");
    printf("\n  Error in fscanf or fgets in %s at line %d : \n", file, line);
    printf("------------------------------------------------------------------------------- !! \n\n");
    exit(69);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////// - GENERATE FUNCTIONS - ////////////////////////////////////////////////////

void generateGaltonBoard(float step, GLsizei splits, GLsizei n_rows) {
    FILE *file = fopen("../data/segments_galton.txt", "w");
    GLsizei i, j, idx = 0;
    
    float w = 2 * splits * step;
    n_rows += (n_rows % 2); // always even
    GLsizei n_pts = 8 + 4*splits*n_rows + n_rows + 4*(splits+1);
    
    fprintf(file, "Number of points %d\n", n_pts);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, -w, w);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, -step/5, step/5);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, w, w);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, step/5, step/5);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, -w/2, w/2+2*step);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, -step, 3*step);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, w/2, w/2+2*step);
    fprintf(file, "%5d %15.8e %15.8e\n", idx++, step, 3*step);

    float start_x, start_y = -step/2.;
    float shift = 1.;
    
    for (i = 0 ; i < n_rows;  i++) {
        start_x = -w/2. + shift * step/4.;
        
        for (j = 0 ; j < 2 * splits + (i % 2) ; j++) {
            fprintf(file, "%5d %15.8e %15.8e\n", idx++, start_x, start_y);
            fprintf(file, "%5d %15.8e %15.8e\n", idx++, start_x + step/2., start_y);
            start_x += step;
        }
        start_y -= step/2.;
        shift *= -1;
    }
    
    start_x = -w/2. - step/2.;
    for (j = 0 ; j < 2 * (splits + 1) ; j++) {
        fprintf(file, "%5d %15.8e %15.8e\n", idx++, start_x, start_y);
        fprintf(file, "%5d %15.8e %15.8e\n", idx++, start_x, start_y-5*step);
        start_x += step;
    }
    
    fprintf(file, "Number of segments %d\n", n_pts/2 + 2*splits+1);
    for (i = 0 ; i < n_pts/2; i++) {
        fprintf(file, "%5d %7d %7d\n", i, 2*i, 2*i+1);
    }
    
    for (idx = 8 + 4*splits*n_rows + n_rows+1 ; idx < n_pts ; idx+=2) {
        fprintf(file, "%5d %7d %7d\n", i++, idx, idx+2);
    }

    fclose(file);
}


void genNonCrossingSegments(float w, float h, GLsizei n_box_x, GLsizei n_box_y) {

    if (n_box_x < 1 || n_box_y < 1) return;
    
    FILE *file = fopen("../data/segments_random.txt", "w");
    GLsizei i, j, this_size_x, this_size_y, max_size_x, max_size_y, tot_box;
    float Dx, Dy, dx, dy, start_x, start_y, mean_Dy;
    
    tot_box = 0;
    max_size_x = max_size_y = 0;
    Dx = w / n_box_x;
    Dy = h / n_box_y;
    mean_Dy = 0.;
    srand(time(NULL));
    
    GLsizei *nb_rect_row = malloc(sizeof(GLsizei) * n_box_y);
    GLsizei *local_size_y = malloc(sizeof(GLsizei) * n_box_y);
    
    for (i = 0 ; i < n_box_y ; i++) {
        nb_rect_row[i] = 1 + (rand() % n_box_x);
        tot_box += nb_rect_row[i];
    }
    
    for (i = 0 ; i < n_box_y ; i++) {
        local_size_y[i] = 1 + rand() % 1;
        mean_Dy += local_size_y[i];
        max_size_y = MAX(local_size_y[i], max_size_y);
    }
    mean_Dy /= n_box_y;
    
    fprintf(file, "Number of points %d\n", 2*tot_box);
    for (i = 0 ; i < n_box_y ; i++) {
        //start_x = (float) rand() / (float) (RAND_MAX) * Dx * max_size_x;
        start_x = -w/2.;
        start_y = -h/2. + Dy * max_size_y * i;
        
        for (j = 0 ; j < nb_rect_row[i] ; j++) {
            Dx = w / nb_rect_row[i];
            this_size_y = local_size_y[i];
            
            dx = rand() / (float) RAND_MAX * Dx;
            dy = (rand() / (float) RAND_MAX) * Dy * this_size_y;
            fprintf(file, "%5d %15.8e %15.8e\n", 2*(i * n_box_x + j), start_x + dx, start_y + dy);
        
            dx = (rand() / (float) RAND_MAX) * Dx;
            dy = (rand() / (float) RAND_MAX) * Dy * this_size_y;
            fprintf(file, "%5d %15.8e %15.8e\n", 2*(i * n_box_x + j)+1, start_x + dx, start_y + dy);
            
            start_x += Dx;
        }
    }
    
    fprintf(file, "Number of segments %d\n", tot_box);
    for (i = 0 ; i < n_box_x*n_box_y; i++) {
        fprintf(file, "%5d %7d %7d\n", i, 2*i, 2*i+1);
    }

    fclose(file);
    free(nb_rect_row);
    free(local_size_y);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////// - PREPROCESSING - ///////////////////////////////////////////////////////

TrapezoidalMap *createEmptyTpzMap(GLsizei size_pts) {
    TrapezoidalMap *tpzMap = malloc(sizeof(TrapezoidalMap));
    tpzMap->n_pts = 4; // the points of the bounding box
    tpzMap->size_pts = 4 + size_pts;
    tpzMap->points = malloc(sizeof(float[2]) * (tpzMap->size_pts));
    
    tpzMap->n_seg = 2; // the segments of the bounding box
    tpzMap->size_seg = 2*tpzMap->size_pts;
    tpzMap->segments = malloc(sizeof(Segment) * (tpzMap->size_seg));
    
    tpzMap->n_tpz = 0;
    tpzMap->size_tpz = 3 * (tpzMap->size_seg) + 1;
    tpzMap->trapezoids = malloc(sizeof(Trapezoid) * tpzMap->size_tpz);
    tpzMap->bounds[0] = tpzMap->bounds[2] = -10.;
    tpzMap->bounds[1] = tpzMap->bounds[3] = +10.;
    adjust_ratio(tpzMap);  // easier to handle it now -> 16/9 ratio
    return tpzMap;
}

TrapezoidalMap *loadFromTxt(char *filename) {
    
    FILE* file = fopen(filename,"r");
    if (file == NULL) Error("No such file !");
    int i, j, idx, trash, idx_pt_1, idx_pt_2;
    float val;
    
    TrapezoidalMap *tpzMap = malloc(sizeof(TrapezoidalMap));
    
    // Scan points
    ErrorScan(fscanf(file,"Number of points %d\n", &tpzMap->n_pts));
    tpzMap->n_pts += 4; // the points of the bounding box
    tpzMap->size_pts = tpzMap->n_pts;
    tpzMap->points = malloc(sizeof(float[2]) * (tpzMap->size_pts));

    for (i = 4 ; i < tpzMap->n_pts ; i++) {
        ErrorScan(fscanf(file,"%d %e %e\n", &trash, &tpzMap->points[i][0], &tpzMap->points[i][1]));
        //printf("%d (%6.2f, %6.2f)\n", i, tpzMap->points[i][0], tpzMap->points[i][1]);
    }
    
    // Scan segments
    ErrorScan(fscanf(file,"Number of segments %d\n", &tpzMap->n_seg));
    tpzMap->n_seg += 2; // the segments of the bounding box
    tpzMap->size_seg = tpzMap->n_seg;
    tpzMap->segments = malloc(sizeof(Segment) * (tpzMap->size_seg));

    tpzMap->n_tpz = 0;
    tpzMap->size_tpz = 3 * (tpzMap->size_seg) + 1;
    tpzMap->trapezoids = malloc(sizeof(Trapezoid) * tpzMap->size_tpz);
    tpzMap->bounds[0] = tpzMap->bounds[2] = +FLT_MAX;
    tpzMap->bounds[1] = tpzMap->bounds[3] = -FLT_MAX;
    
    for (i = 2 ; i < tpzMap->n_seg ; i++) {
        ErrorScan(fscanf(file,"%d %d %d\n", &trash, &idx_pt_1, &idx_pt_2));
        idx_pt_1 += 4; // shift because of the bounding box
        idx_pt_2 += 4;
        
        if (isLeft(tpzMap->points[idx_pt_1], tpzMap->points[idx_pt_2])) {
            tpzMap->segments[i].orig = idx_pt_1;
            tpzMap->segments[i].dest = idx_pt_2;
        }
        else {
            tpzMap->segments[i].orig = idx_pt_2;
            tpzMap->segments[i].dest = idx_pt_1;
        }
        
        if ((val = tpzMap->points[tpzMap->segments[i].orig][0]) < tpzMap->bounds[0]) tpzMap->bounds[0] = val;
        if ((val = tpzMap->points[tpzMap->segments[i].dest][0]) > tpzMap->bounds[1]) tpzMap->bounds[1] = val;
        if ((val = fmin(tpzMap->points[tpzMap->segments[i].orig][1], tpzMap->points[tpzMap->segments[i].dest][1])) < tpzMap->bounds[2]) tpzMap->bounds[2] = val;
        if ((val = fmax(tpzMap->points[tpzMap->segments[i].orig][1], tpzMap->points[tpzMap->segments[i].dest][1])) > tpzMap->bounds[3]) tpzMap->bounds[3] = val;
    }

    adjust_ratio(tpzMap);  // easier to handle it now -> 16/9 ratio
    fclose(file);
    return tpzMap;
}


void saveSegments(char *filename, TrapezoidalMap *tpzMap) {
    if (tpzMap->n_tpz <= 1) return;
    
    GLsizei i;
    FILE *ptr = fopen(filename, "w");
    
    fprintf(ptr, "Number of points %d\n", tpzMap->n_pts - 4); // box doesn't count
    for (i = 4 ; i < tpzMap->n_pts ; i++) {
        fprintf(ptr, "%5d %15.8e %15.8e\n", i-4, tpzMap->points[i][0], tpzMap->points[i][1]);
    }
    
    fprintf(ptr, "Number of segments %d\n", tpzMap->n_seg - 2); // box doesn't count
    for (i = 2 ; i < tpzMap->n_seg ; i++) {
        fprintf(ptr, "%5d %7d %7d\n", i-2, tpzMap->segments[i].orig-4, tpzMap->segments[i].dest-4);
    }
    fclose(ptr);
}


void adjust_ratio(TrapezoidalMap *tpzMap) {
    double val_min, val_max;
    float ratio = (tpzMap->bounds[1] - tpzMap->bounds[0]) / (tpzMap->bounds[3] - tpzMap->bounds[2]);
    if (ratio >= 16./9.) {
        val_min = tpzMap->bounds[2]; val_max = tpzMap->bounds[3];
        tpzMap->bounds[2] = (val_min + val_max)/2. - 9./32. * (tpzMap->bounds[1] - tpzMap->bounds[0]);
        tpzMap->bounds[3] = (val_min + val_max)/2. + 9./32. * (tpzMap->bounds[1] - tpzMap->bounds[0]);
    }
    else {
        val_min = tpzMap->bounds[0]; val_max = tpzMap->bounds[1];
        tpzMap->bounds[0] = (val_min + val_max)/2. - 8./9. * (tpzMap->bounds[3] - tpzMap->bounds[2]);
        tpzMap->bounds[1] = (val_min + val_max)/2. + 8./9. * (tpzMap->bounds[3] - tpzMap->bounds[2]);
    }
}


void freeTpzMap(TrapezoidalMap *tpzMap) {
    free(tpzMap->points);
    free(tpzMap->segments);
    free(tpzMap->trapezoids);
    free(tpzMap);
}


void randomPermutation(TrapezoidalMap *tpzMap) {
    int i, rand_idx, idx;
    Segment *temp = malloc(sizeof(Segment));
    srand(time(0));
    
    for (i = tpzMap->n_seg - 3 ; i > 0 ; i--) { // -2 because of 2 segments of bounding box (actually -3 since its 0 indexed)
        idx = i + 2;
        rand_idx = rand() % (i + 1) + 2;
        
        //printf("%p %p %p %p\n", tpzMap->segments[idx].orig, tpzMap->segments[idx].dest, tpzMap->segments[rand_idx].orig, tpzMap->segments[rand_idx].dest);
        //printf("%6.2f %6.2f %6.2f %6.2f\n", tpzMap->segments[idx].orig->x, tpzMap->segments[rand_idx].orig->x, tpzMap->segments[idx].dest->x, tpzMap->segments[rand_idx].dest->x);
        
        memcpy(temp, &tpzMap->segments[rand_idx], sizeof(Segment));
        memcpy(&tpzMap->segments[rand_idx], &tpzMap->segments[idx], sizeof(Segment));
        memcpy(&tpzMap->segments[idx], temp, sizeof(Segment));

        //printf("%p %p %p %p\n", tpzMap->segments[idx].orig, tpzMap->segments[idx].dest, tpzMap->segments[rand_idx].orig, tpzMap->segments[rand_idx].dest);
        //printf("%6.2f %6.2f %6.2f %6.2f\n", tpzMap->segments[idx].orig->x, tpzMap->segments[rand_idx].orig->x, tpzMap->segments[idx].dest->x, tpzMap->segments[rand_idx].dest->x);
        //printf("random : i=%3d  idx=%3d  rand_idx=%3d\n\n", i, idx, rand_idx);
    }
    free(temp);
}


void checkAllDifferent(TrapezoidalMap *tpzMap) {
    int i,j, count = 0;
    for (i = 0 ; i < tpzMap->n_seg ; i++) {
        for (j = 0 ; j < tpzMap->n_seg ; j++) {
            if ((i != j) && (tpzMap->segments[i+2].orig == tpzMap->segments[j+2].orig)) {
                printf("%d %d Orig identical\n", i, j);
                if (tpzMap->segments[i+2].dest == tpzMap->segments[j+2].dest) {
                    printf("%d %d DEST ALSO !!! identical\n", i, j);
                    count ++;
                }
            }
        }
    }
    printf("%d Errors founds in the permutation\n", count);
}


void initTpzMap(TrapezoidalMap *tpzMap) {
    
    randomPermutation(tpzMap);  // DISABLED FOR TESTING but functionnal
    //checkAllDifferent(tpzMap);
    
    float x_min, x_max, y_min, y_max;
    // modify the bounds to not make them tight
    x_min = tpzMap->bounds[0] - 0.1 * (tpzMap->bounds[1] - tpzMap->bounds[0]);
    x_max = tpzMap->bounds[1] + 0.1 * (tpzMap->bounds[1] - tpzMap->bounds[0]);
    y_min = tpzMap->bounds[2] - 0.1 * (tpzMap->bounds[3] - tpzMap->bounds[2]);
    y_max = tpzMap->bounds[3] + 0.1 * (tpzMap->bounds[3] - tpzMap->bounds[2]);
    tpzMap->bounds[0] = x_min; tpzMap->bounds[1] = x_max;
    tpzMap->bounds[2] = y_min; tpzMap->bounds[3] = y_max;
    
    // add the points of the bounding box
    tpzMap->points[0][0] = x_min; tpzMap->points[0][1] = y_max;
    tpzMap->points[1][0] = x_max; tpzMap->points[1][1] = y_max;
    tpzMap->points[2][0] = x_min; tpzMap->points[2][1] = y_min;
    tpzMap->points[3][0] = x_max; tpzMap->points[3][1] = y_min;

    // add the segment of the bounding box
    tpzMap->segments[0].orig = 0; tpzMap->segments[0].dest = 1;
    tpzMap->segments[1].orig = 2; tpzMap->segments[1].dest = 3;
    
    // add the trapezoid of the bounding box
    tpzMap->trapezoids[0].top = 0; tpzMap->trapezoids[0].bottom = 1;
    tpzMap->trapezoids[0].left_pt = 2; tpzMap->trapezoids[0].right_pt = 1;
    tpzMap->trapezoids[0].up_left = -1;  tpzMap->trapezoids[0].up_right = -1;
    tpzMap->trapezoids[0].low_left = -1; tpzMap->trapezoids[0].low_right = -1;
    
    tpzMap->n_tpz += 1;  // the bounding box itself
}


void infoTpzMap(TrapezoidalMap *tpzMap, DataSet *data, int max_display, float time_build) {
    Segment *seg;
    GLsizei orig, dest;
    
    printf("\n ============================ Trapezoidal Map ============================\n");
    printf("    Time to build the DAG  : %8.3f s\n", time_build);
    printf("    X limits               :   %6.3f -> %6.3f\n", tpzMap->bounds[0], tpzMap->bounds[1]);
    printf("    Y limits               :   %6.3f -> %6.3f\n", tpzMap->bounds[2], tpzMap->bounds[3]);
    printf("    Number of points       : %4d\n", tpzMap->n_pts);
    printf("    Number of segments     : %4d\n", tpzMap->n_seg);
    printf("    Number of trapezoids   : %4d\n", tpzMap->n_tpz);
    printf("    Number of nodes in DAG : %4d\n", data->n_nodes);
    //printf("    Size for tpz list    : %4d\n", tpzMap->size_tpz);

    printf("\n");

    for (int i = 0 ; i < MIN(max_display, tpzMap->n_seg) ; i++) {
        orig = tpzMap->segments[i].orig;
        dest = tpzMap->segments[i].dest;
        printf("Segment %3d : (%7.3f, %7.3f) -> (%7.3f, %7.3f)\n", i+1, 
                tpzMap->points[orig][0], tpzMap->points[orig][1], tpzMap->points[dest][0], tpzMap->points[dest][1]);
    }
    
    for (int i = 0 ; i < MIN(max_display, tpzMap->n_tpz) ; i++) {
        printf("Trapezoid %3d\n", i+1);
        printf("\tleft_pt  : (%7.3f, %7.3f)\n", tpzMap->points[tpzMap->trapezoids[i].left_pt][0], tpzMap->points[tpzMap->trapezoids[i].left_pt][1]);
        printf("\tright_pt : (%7.3f, %7.3f)\n", tpzMap->points[tpzMap->trapezoids[i].right_pt][0], tpzMap->points[tpzMap->trapezoids[i].right_pt][1]);

        orig = tpzMap->segments[tpzMap->trapezoids[i].top].orig;
        dest = tpzMap->segments[tpzMap->trapezoids[i].top].dest;
        printf("\ttop      : (%7.3f, %7.3f) -> (%7.3f, %7.3f)\n", tpzMap->points[orig][0], tpzMap->points[orig][1], tpzMap->points[dest][0], tpzMap->points[dest][1]);

        orig = tpzMap->segments[tpzMap->trapezoids[i].bottom].orig;
        dest = tpzMap->segments[tpzMap->trapezoids[i].bottom].dest;
        printf("\tbottom   : (%7.3f, %7.3f) -> (%7.3f, %7.3f)\n", tpzMap->points[orig][0], tpzMap->points[orig][1], tpzMap->points[dest][0], tpzMap->points[dest][1]);
    }
    return;
}


DataSet *createDataSet(TrapezoidalMap *tpzMap) {
    
    DataSet *data = malloc(sizeof(DataSet));
    data->size_nodes = 2 * tpzMap->size_tpz;
    //printf("N_TPZ = %d\t SIZE_NODES = %d\n", tpzMap->n_tpz, data->size_nodes);
    
    data->nodes = malloc(sizeof(Node) * data->size_nodes);
    
    data->nodes[0].type = 't';
    data->nodes[0].idx = 0;
    tpzMap->trapezoids[0].leaf = 0;
    
    data->n_nodes = 1;
}


void freeDataSet(DataSet *data) {
    free(data->nodes);
    free(data);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////// - USEFULL FUNCTIONS - /////////////////////////////////////////////////////

int isLeft(float *pa, float *pb) {  // pa < pb ?
    return (pa[0] < pb[0]) || (pa[0] == pb[0] && pa[1] < pb[1]);
}


int isAbove(float *pa, float *pb, float *pc) {  // c above [a->b] ?
#if ROBUST
    return orient2d(pa, pb, pc) > 0.;
    
#else
    return (pb[0] - pa[0]) * (pc[1] - pa[1]) - (pb[1] - pa[1]) * (pc[0] - pa[0]) > 0.;

#endif
}


int isAboveHelper(float *pa, float *pb, float *pc, float *pd) { // [c->d] above [a->b] ?
#if ROBUST
    float res = orient2d(pa, pb, pc);
    if (res == 0.) return orient2d(pa, pb, pd) > 0.;
    else return res > 0.;
#else
    float res = (pb[0] - pa[0]) * (pc[1] - pa[1]) - (pb[1] - pa[1]) * (pc[0] - pa[0]);
    if (res == 0.) return (pb[0] - pa[0]) * (pd[1] - pa[1]) - (pb[1] - pa[1]) * (pd[0] - pa[0]) > 0.;
    else return res > 0.;
#endif
}


GLsizei searchTpz(TrapezoidalMap *tpzMap, DataSet *data, float p[2], float helper[2]) {
    Node *node = &data->nodes[0];
    GLsizei orig, dest;
    while (node->type != 't') { // not a leaf
        if (node->type == 'x') { // x-node
            if (isLeft(p, tpzMap->points[node->idx])) node = &data->nodes[node->left];
            else node = &data->nodes[node->right];
        }
        else if (node->type == 'y') { // y-node
            orig = tpzMap->segments[node->idx].orig;
            dest = tpzMap->segments[node->idx].dest;
            if (isAboveHelper(tpzMap->points[orig], tpzMap->points[dest], p, helper)) node = &data->nodes[node->left];
            else node = &data->nodes[node->right];
        }
        else {
            printf("SHOULD NOT HAPPEN: overlapping segments\n");
        }
    }
    return node->idx;
}


GLsizei *followSegment(TrapezoidalMap *tpzMap, DataSet *data, Segment *s, GLsizei *size) {
    GLsizei p, q, right_pt, j = 0;
    p = s->orig; q = s->dest;
    
    GLsizei *tpz_crossed = malloc(sizeof(GLsizei) * 1); // list of indexes
    tpz_crossed[0] = searchTpz(tpzMap, data, tpzMap->points[p], tpzMap->points[q]);

    right_pt = tpzMap->trapezoids[tpz_crossed[0]].right_pt;
    
    // while right_pt on the left of q
    while (tpzMap->points[right_pt][0] < tpzMap->points[q][0]) {
        tpz_crossed = realloc(tpz_crossed, sizeof(GLsizei) * (j + 2));

        // should not have to use the helper point since segments only meet at endpoints !
        if (isAbove(tpzMap->points[p], tpzMap->points[q], tpzMap->points[right_pt])) {
            tpz_crossed[j+1] = tpzMap->trapezoids[tpz_crossed[j]].low_right;
        }
        else {
            tpz_crossed[j+1] = tpzMap->trapezoids[tpz_crossed[j]].up_right;
        }
        
        right_pt = tpzMap->trapezoids[tpz_crossed[j+1]].right_pt;
        j++;
    }
    
    *size = j + 1;
    return tpz_crossed;
}


void setNewNode(Node *node, char c, GLsizei idx, GLsizei left, GLsizei right) {
    node->type = c;
    node->idx = idx;
    node->left = left;
    node->right = right;
}

void setNewLeaf(Node *node, GLsizei idx) {
    node->type = 't';
    node->idx = idx;
}

void setNewTpz(Trapezoid *tpz, GLsizei leaf, GLsizei top, GLsizei bottom, GLsizei left_pt, GLsizei right_pt) {
    tpz->top = top;
    tpz->bottom = bottom;
    tpz->left_pt = left_pt;
    tpz->right_pt = right_pt;
    tpz->leaf = leaf;
}

void setTpzNeighbors(Trapezoid *tpz, int low_left, int up_left, int low_right, int up_right) {
    tpz->low_left = low_left;
    tpz->up_left = up_left;
    tpz->up_right = up_right;
    tpz->low_right = low_right;
}

void tpzUpdateRight(Trapezoid *tpz, GLsizei right_pt, int low_right, int up_right) {
    tpz->right_pt = right_pt;
    tpz->low_right = low_right;
    tpz->up_right = up_right;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////// - MAIN PROCESS - ///////////////////////////////////////////////////////

void incrementTpzMap(TrapezoidalMap *tpzMap, DataSet *data, GLsizei i, GLsizei *tpz_crossed, GLsizei nb_tpz_crossed) {
    // add segment with index i in tpzMap->segments
    
    GLsizei idx_tpz, idx_nodes, j, temp;
    Segment *this_seg = &tpzMap->segments[i];
    Trapezoid *this_tpz = malloc(sizeof(Trapezoid));
    Trapezoid *tpz_below, *tpz_above;
    idx_tpz = tpzMap->n_tpz;
    idx_nodes = data->n_nodes;
    
    int prev_size;
    
    // Increase size of the list of trapezoids if needed
    if (tpzMap->size_tpz < tpzMap->n_tpz + 3) {
        prev_size = tpzMap->size_tpz;
        tpzMap->size_tpz = (tpzMap->n_tpz + 3) * 2;
        tpzMap->trapezoids = realloc(tpzMap->trapezoids, sizeof(Trapezoid) * tpzMap->size_tpz);
        printf("%-20s Increasing the size of %12s : %4d -> %4d\n", "[Trapezoidal Map]", "[trapezoids]", prev_size, tpzMap->size_tpz);
    }
    
    // Increase size of the list of node if needed
    if (data->size_nodes < data->n_nodes + nb_tpz_crossed + 5) {
        prev_size = data->size_nodes;
        data->size_nodes = (data->n_nodes + nb_tpz_crossed + 5) * 2;
        data->nodes = realloc(data->nodes, sizeof(Node) * data->size_nodes);
        printf("%-20s Increasing the size of %12s : %4d -> %4d\n", "[DAGraph]", "[nodes]", prev_size, data->size_nodes);
    }
    
    if (nb_tpz_crossed == 1) {
    // basic case (note that previous tpz replaced by A in memory)
        memcpy(this_tpz, &tpzMap->trapezoids[tpz_crossed[0]], sizeof(Trapezoid));
        
        // Search Structure
        setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->orig, idx_nodes+2, idx_nodes); // p (left: tpzA, right: q)
        setNewNode(&data->nodes[idx_nodes], 'x', this_seg->dest, idx_nodes+1, idx_nodes+3); // q (left: s, right: tpzB)
        setNewNode(&data->nodes[idx_nodes+1], 'y', i, idx_nodes+4, idx_nodes+5); // s (left: tpzC, right: tpzD)
        
        // Trapezoid A (left)
        setNewLeaf(&data->nodes[idx_nodes+2], tpz_crossed[0]);
        setNewTpz(&tpzMap->trapezoids[tpz_crossed[0]], idx_nodes+2, this_tpz->top, this_tpz->bottom, this_tpz->left_pt, this_seg->orig);
        setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[0]], this_tpz->low_left, this_tpz->up_left, idx_tpz+2, idx_tpz+1);
        setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[0]], this_tpz->low_left, this_tpz->up_left, idx_tpz, idx_tpz+1);

        // Trapezoid B (center bottom)
        setNewLeaf(&data->nodes[idx_nodes+5], idx_tpz);
        setNewTpz(&tpzMap->trapezoids[idx_tpz], idx_nodes+5, i, this_tpz->bottom, this_seg->orig, this_seg->dest);
        setTpzNeighbors(&tpzMap->trapezoids[idx_tpz], tpz_crossed[0], -1, idx_tpz+2, -1);
        
        // Trapezoid C (center top)
        setNewLeaf(&data->nodes[idx_nodes+4], idx_tpz+1);
        setNewTpz(&tpzMap->trapezoids[idx_tpz+1], idx_nodes+4, this_tpz->top, i, this_seg->orig, this_seg->dest);
        setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+1], -1, tpz_crossed[0], -1, idx_tpz+2);
                    
        // Trapezoid D (right)
        setNewLeaf(&data->nodes[idx_nodes+3], idx_tpz+2);
        setNewTpz(&tpzMap->trapezoids[idx_tpz+2], idx_nodes+3, this_tpz->top, this_tpz->bottom, this_seg->dest, this_tpz->right_pt);
        setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+2], idx_tpz, idx_tpz+1, this_tpz->low_right, this_tpz->up_right);
    }
    else if (nb_tpz_crossed == 2) { // slightly harder (not necessary since the case > 2 works also for 2, but ok !)

        memcpy(this_tpz, &tpzMap->trapezoids[tpz_crossed[0]], sizeof(Trapezoid));
        int commonBottom = tpzMap->trapezoids[tpz_crossed[0]].bottom == tpzMap->trapezoids[tpz_crossed[1]].bottom;
        
        if (commonBottom) {
            // Search Structure
            setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->orig, idx_nodes + 1, idx_nodes); // p
            setNewNode(&data->nodes[idx_nodes], 'y', i, idx_nodes + 2, idx_nodes + 3); // s (for first tpz)
            setNewNode(&data->nodes[idx_nodes+4], 'y', i, idx_nodes + 6, idx_nodes + 3); // s (for second tpz)
            
            // Trapezoid A
            setNewLeaf(&data->nodes[idx_nodes+1], tpz_crossed[0]); // t
            setNewTpz(&tpzMap->trapezoids[tpz_crossed[0]], idx_nodes+1, this_tpz->top, this_tpz->bottom, this_tpz->left_pt, this_seg->orig);
            setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[0]], this_tpz->low_left, this_tpz->up_left, idx_tpz+1, idx_tpz);
            
            // Trapezoid B
            setNewLeaf(&data->nodes[idx_nodes+2], idx_tpz);
            setNewTpz(&tpzMap->trapezoids[idx_tpz], idx_nodes+2, this_tpz->top, i, this_seg->orig, this_tpz->right_pt);
            setTpzNeighbors(&tpzMap->trapezoids[idx_tpz], -1, tpz_crossed[0], this_tpz->low_right, this_tpz->up_right);
            if (tpzMap->trapezoids[idx_tpz].up_right != -1) {
                tpzMap->trapezoids[tpzMap->trapezoids[idx_tpz].up_right].up_left = idx_tpz;
            }
            
            // Trapezoid C (common)
            setNewLeaf(&data->nodes[idx_nodes+3], idx_tpz+1);
            setNewTpz(&tpzMap->trapezoids[idx_tpz+1], idx_nodes+3, i, this_tpz->bottom, this_seg->orig, this_seg->dest);
            setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+1], tpz_crossed[0], -1, idx_tpz+2, -1);
            
            memcpy(this_tpz, &tpzMap->trapezoids[tpz_crossed[1]], sizeof(Trapezoid));
            // Trapezoid D
            setNewLeaf(&data->nodes[idx_nodes+5], idx_tpz+2);
            setNewTpz(&tpzMap->trapezoids[idx_tpz+2], idx_nodes+5, this_tpz->top, this_tpz->bottom, this_seg->dest, this_tpz->right_pt);
            setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+2], idx_tpz+1, tpz_crossed[1], this_tpz->low_right, this_tpz->up_right);
            
            // Trapezoid B'
            setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->dest, idx_nodes + 4, idx_nodes+5); // p
            setNewLeaf(&data->nodes[idx_nodes+6], tpz_crossed[1]);
            setNewTpz(&tpzMap->trapezoids[tpz_crossed[1]], idx_nodes+6, this_tpz->top, i, this_tpz->left_pt, this_seg->dest);
            setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[1]], idx_tpz, this_tpz->up_left, -1, idx_tpz+2);
        }
        else {
            // Search Structure
            setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->orig, idx_nodes + 1, idx_nodes); // p
            setNewNode(&data->nodes[idx_nodes], 'y', i, idx_nodes + 3, idx_nodes + 2); // s (for first tpz)
            setNewNode(&data->nodes[idx_nodes+4], 'y', i, idx_nodes + 3, idx_nodes + 6); // s (for second tpz)
            
            // Trapezoid A
            setNewLeaf(&data->nodes[idx_nodes+1], tpz_crossed[0]); // t
            setNewTpz(&tpzMap->trapezoids[tpz_crossed[0]], idx_nodes+1, this_tpz->top, this_tpz->bottom, this_tpz->left_pt, this_seg->orig);
            setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[0]], this_tpz->low_left, this_tpz->up_left, idx_tpz, idx_tpz+1);
            
            // Trapezoid B
            setNewLeaf(&data->nodes[idx_nodes+2], idx_tpz);
            setNewTpz(&tpzMap->trapezoids[idx_tpz], idx_nodes+2, i, this_tpz->bottom, this_seg->orig, this_tpz->right_pt);
            setTpzNeighbors(&tpzMap->trapezoids[idx_tpz], tpz_crossed[0], -1, this_tpz->low_right, this_tpz->up_right);
            if (tpzMap->trapezoids[idx_tpz].low_right != -1) {
                tpzMap->trapezoids[tpzMap->trapezoids[idx_tpz].low_right].low_left = idx_tpz;
            }
            
            // Trapezoid C (common)
            setNewLeaf(&data->nodes[idx_nodes+3], idx_tpz+1);
            setNewTpz(&tpzMap->trapezoids[idx_tpz+1], idx_nodes+3, this_tpz->top, i, this_seg->orig, this_seg->dest);
            setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+1], -1, tpz_crossed[0], -1, idx_tpz+2);
            
            memcpy(this_tpz, &tpzMap->trapezoids[tpz_crossed[1]], sizeof(Trapezoid));
            // Trapezoid D
            setNewLeaf(&data->nodes[idx_nodes+5], idx_tpz+2);
            setNewTpz(&tpzMap->trapezoids[idx_tpz+2], idx_nodes+5, this_tpz->top, this_tpz->bottom, this_seg->dest, this_tpz->right_pt);
            setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+2], tpz_crossed[1], idx_tpz+1, this_tpz->low_right, this_tpz->up_right);
            
            // Trapezoid B'
            setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->dest, idx_nodes + 4, idx_nodes+5); // p
            setNewLeaf(&data->nodes[idx_nodes+6], tpz_crossed[1]);
            setNewTpz(&tpzMap->trapezoids[tpz_crossed[1]], idx_nodes+6, i, this_tpz->bottom, this_tpz->left_pt, this_seg->dest);
            setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[1]], this_tpz->low_left, idx_tpz, idx_tpz+2, -1);
        }
    }
    else { // hell as fuck
        
        // Search structure
        memcpy(this_tpz, &tpzMap->trapezoids[tpz_crossed[0]], sizeof(Trapezoid));
        setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->orig, idx_nodes, idx_nodes+1); //p (left: tpz, right: s)
        setNewLeaf(&data->nodes[idx_nodes], tpz_crossed[0]);
        setNewNode(&data->nodes[idx_nodes+1], 'y', i, idx_nodes+2, idx_nodes+3);
        setNewLeaf(&data->nodes[idx_nodes+2], idx_tpz+1);
        setNewLeaf(&data->nodes[idx_nodes+3], idx_tpz);
        
        // Trapezoid (top, bot, lpt, rpt)
        setNewTpz(&tpzMap->trapezoids[tpz_crossed[0]], idx_nodes, this_tpz->top, this_tpz->bottom, this_tpz->left_pt, this_seg->orig);
        setNewTpz(&tpzMap->trapezoids[idx_tpz], idx_nodes+3, i, this_tpz->bottom, this_seg->orig, this_tpz->right_pt); // tpz_below
        setNewTpz(&tpzMap->trapezoids[idx_tpz+1], idx_nodes+2, this_tpz->top, i, this_seg->orig, this_tpz->right_pt); // tpz_above
        
        GLsizei idx_below = idx_tpz;
        GLsizei idx_above = idx_tpz+1;
        
        setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[0]], this_tpz->low_left, this_tpz->up_left, idx_tpz, idx_tpz+1);
        
        setTpzNeighbors(&tpzMap->trapezoids[idx_tpz], tpz_crossed[0], -1, this_tpz->low_right, this_tpz->up_right); // entries right can be changed
        setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+1], -1, tpz_crossed[0], this_tpz->low_right, this_tpz->up_right); // entries right can be changed
        
        for (j = 1 ; j < nb_tpz_crossed ; j++) {
            memcpy(this_tpz, &tpzMap->trapezoids[tpz_crossed[j]], sizeof(Trapezoid));
            tpz_above = &tpzMap->trapezoids[idx_above];
            tpz_below = &tpzMap->trapezoids[idx_below];
            
            if (this_tpz->top == tpz_above->top) {
                // CONTINUE THE ONE ABOVE
                tpzUpdateRight(tpz_above, this_tpz->right_pt, this_tpz->low_right, this_tpz->up_right);
                
                // FINISH THE LAST BELOW (its possible neighbor)
                if ((temp = tpzMap->trapezoids[idx_below].low_right) != -1) tpzMap->trapezoids[temp].low_left = idx_below;
                
                // CREATE THE ONE BELOW
                setNewTpz(&tpzMap->trapezoids[tpz_crossed[j]], idx_nodes+3+j, i, this_tpz->bottom, this_tpz->left_pt, this_tpz->right_pt);
                setNewLeaf(&data->nodes[idx_nodes+3+j], tpz_crossed[j]);
                setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[j]], this_tpz->low_left, idx_below, this_tpz->low_right, this_tpz->up_right);
                
                // UPDATE DATASET
                setNewNode(&data->nodes[this_tpz->leaf], 'y', i, tpz_above->leaf, idx_nodes+3+j);
                setNewLeaf(&data->nodes[idx_nodes+3+j], tpz_crossed[j]);
                
                // UPDATE IDX TPZ BELOW
                idx_below = tpz_crossed[j];
            }
            else {
                // CONTINUE THE ONE BELOW
                tpzUpdateRight(tpz_below, this_tpz->right_pt, this_tpz->low_right, this_tpz->up_right);
                
                // FINISH THE LAST ABOVE (its possible neighbor)
                if ((temp = tpzMap->trapezoids[idx_above].up_right) != -1) tpzMap->trapezoids[temp].up_left = idx_above;
                
                // CREATE THE ONE ABOVE
                setNewTpz(&tpzMap->trapezoids[tpz_crossed[j]], idx_nodes+3+j, this_tpz->top, i, this_tpz->left_pt, this_tpz->right_pt);
                setNewLeaf(&data->nodes[idx_nodes+3+j], tpz_crossed[j]);
                setTpzNeighbors(&tpzMap->trapezoids[tpz_crossed[j]], idx_above, this_tpz->up_left, this_tpz->low_right, this_tpz->up_right);
                
                // UPDATE DATASET
                setNewNode(&data->nodes[this_tpz->leaf], 'y', i, idx_nodes+3+j, tpz_below->leaf);
                setNewLeaf(&data->nodes[idx_nodes+3+j], tpz_crossed[j]);
                
                // UPDATE IDX TPZ ABOVE
                idx_above = tpz_crossed[j];
            }
        }
        
        tpz_above = &tpzMap->trapezoids[idx_above];
        tpz_below = &tpzMap->trapezoids[idx_below];
        
        tpzUpdateRight(tpz_above, this_seg->dest, -1, idx_tpz+2);
        tpzUpdateRight(tpz_below, this_seg->dest, idx_tpz+2, -1);
        setNewTpz(&tpzMap->trapezoids[idx_tpz+2], idx_nodes+4+j, this_tpz->top, this_tpz->bottom, this_seg->dest, this_tpz->right_pt);
        setTpzNeighbors(&tpzMap->trapezoids[idx_tpz+2], idx_below, idx_above, this_tpz->low_right, this_tpz->up_right);
        
        // dataset was not correctly filled -> correction
        setNewNode(&data->nodes[idx_nodes+3+j], 'y', i, data->nodes[this_tpz->leaf].left, data->nodes[this_tpz->leaf].right); // s
        setNewNode(&data->nodes[this_tpz->leaf], 'x', this_seg->dest, idx_nodes+3+j, idx_nodes+4+j); // q
        setNewLeaf(&data->nodes[idx_nodes+4+j], idx_tpz+2); // t
    }
    
    // Modify right neighbors of rightmost tpz:
    memcpy(this_tpz, &tpzMap->trapezoids[idx_tpz+2], sizeof(Trapezoid));
    if (this_tpz->low_right != -1) {
        //printf("prev_val:%3d \t new_val:%3d\n", tpzMap->trapezoids[this_tpz->low_right].low_left, idx_tpz+2);
        tpzMap->trapezoids[this_tpz->low_right].low_left = idx_tpz+2;
    }
    if (this_tpz->up_right != -1) {
        //printf("prev_val:%3d \t new_val:%3d\n", tpzMap->trapezoids[this_tpz->up_right].up_left, idx_tpz+2);
        tpzMap->trapezoids[this_tpz->up_right].up_left = idx_tpz+2;
    }
    
    tpzMap->n_tpz += 3;
    data->n_nodes += nb_tpz_crossed + 5;
    free(this_tpz);
}


void buildTpzlMap(TrapezoidalMap *tpzMap, DataSet *data) {
    GLsizei nb_tpz_crossed, i;
    GLsizei *tpz_crossed;
    
    for (i = 2 ; i < tpzMap->n_seg ; i++) {
        
        tpz_crossed = followSegment(tpzMap, data, &tpzMap->segments[i], &nb_tpz_crossed);
        //for (int k = 0 ; k < nb_tpz_crossed ; k++) printf("Tpz crossed = %d\n", tpz_crossed[k]+1);
        //printf("%s %d %s \u0394 crossed by segment %d\n", BOLD, nb_tpz_crossed, RESET, i-1); fflush(stdout);
        
        incrementTpzMap(tpzMap, data, i, tpz_crossed, nb_tpz_crossed);
        
        free(tpz_crossed); // malloc made in FollowSegment
    }
    return;
}


