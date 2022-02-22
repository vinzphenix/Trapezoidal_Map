#include "glTpz.h"


int main(int argc, char *argv[]) {
    
#if ROBUST
	exactinit();
#endif
    
    int i, j, anim, loaded;
    GLsizei nx, ny;
    char *endptr;
    TrapezoidalMap *tpzMap;
    
    anim = 1;
    loaded = 0;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // HANDLE FLAGS (--> inspired from Gilles Poncelet / Louis Devillez project in 2020-2021)
    
    for (i = 1, j = 0 ; i < argc ; i++) {
    
        // HELP MESSAGE
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0) {
            printf("\nTrapezoidal Map algorithm by Vincent Degrooff and Loic Davreux\n \n");
            printf("Flags: \n");
            printf("-d           : disable (!) the interactive application with BOV\n");
            printf("-f --random  : construct the trapezoidal map using the file with random segments\n");
            printf("-f --galton  : construct the trapezoidal map using the file with the galton board\n");
            printf("-f --saved   : construct the trapezoidal map using the file saved from a previous simulation\n");
            printf("-h, -help    : Print this help\n");
            printf("-w -- galton : save in a file a set of segments that looks like the galton board\n");
            printf("-w -- random [nx ny] : save in a file a set of non crossing segments by dividing the plane in (nx, ny) boxes\n");
            printf("\nRemarks:\n");
            printf("1. The animation has many options that are listed by pressing [h] inside the BOV window,\n");
            printf("   but I particularly recommend to generate particules that bounce on the segments with [x].\n");
            printf("2. When two segments meet at their endpoints, we create trapezoids with 0 area as if the points were not touching.\n");
            printf("   It works well but take a bit more space in memory. Since they have 0 width, they can't be pointed by the mouse (green).\n");
            printf("   Therefore, we can't see them colored green nor red/yellow when they are neighbors of the trapezoid in green.\n");
            printf("\n\n");
            return EXIT_SUCCESS;
        }
    
        // GENERATE SEGMENTS
        if (strcmp(argv[i], "-w") == 0) {

            if (argc <= i + 1) {
                printf("You should precise the type of file you want to write with [--galton] or [--random nx ny]\n");
                return EXIT_FAILURE;
            }

            if (strcmp(argv[i + 1], "--galton") == 0) {
                generateGaltonBoard(3., 5, 4);
                i++;
            }

            else if (strcmp(argv[i + 1], "--random") == 0) {
                j = 0;

                if (argc >= i+4) nx = (GLsizei) strtol(argv[i+2], &endptr, 10);
                if (argc >= i+4 && *endptr == '\0') j++;
                else nx = 10;

                endptr = NULL;
                if (argc >= i+4) ny = (GLsizei) strtol(argv[i+3], &endptr, 10);
                if (argc >= i+4 && *endptr == '\0') j++;
                else {nx = 10; ny = 10;}
                
                genNonCrossingSegments(160., 90., nx, ny);
                i += 1 + j;
            }
 
            else {
                printf("This is not any of the modes [--galton] or [--random]\n");
                return EXIT_FAILURE;
            }
            
            printf("File generated in the ./data folder\n");
            return EXIT_SUCCESS;
	    }
		
        // OPEN A FILE
        else if (strcmp(argv[i], "-f") == 0) {
            
            if (argc <= i + 1) {
                printf("You should precise the file you want to open [--galton] or [--random] or [--saved]\n");
                return EXIT_FAILURE;
            }

            else if (strcmp(argv[i+1], "--galton") == 0) tpzMap = loadFromTxt("../data/segments_galton.txt");
            else if (strcmp(argv[i+1], "--random") == 0) tpzMap = loadFromTxt("../data/segments_random.txt");
            else if (strcmp(argv[i+1], "--saved") == 0) tpzMap = loadFromTxt("../data/segments_interactive.txt");
            else if (strcmp(argv[i+1], "--basic") == 0) tpzMap = loadFromTxt("../data/segments_basic.txt");
            else {
                printf("This is not any of the modes [--galton] or [--random] or [--saved]\n");
                return EXIT_FAILURE;
            }
            
            loaded = 1;
            i++;
        }
        
        if (strcmp(argv[i], "-d") == 0) anim = 0;
        
    }

    // allocated space for 100 pts (automatic increase)
    if (!loaded) tpzMap = createEmptyTpzMap(100); 

    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // COMPUTE THE TRAPEZOIDAL MAP
    float time_build;
    clock_t start, end;
    
    initTpzMap(tpzMap);
    DataSet *data = createDataSet(tpzMap);

    start = clock();
    buildTpzlMap(tpzMap, data);
    end = clock();
    
    infoTpzMap(tpzMap, data, 0, (float)(end - start) / CLOCKS_PER_SEC); // 0 tpz printed
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY THE MAP WITH BOV
    
    if (anim) {
        
        glTpzWindowCreate("LMECA2170 : Trapezoidal Map", 960, 540, tpzMap->bounds);
        DisplayMap *map = createDisplayMap(tpzMap);
        
        do {
            glTpzWindowUpdate(map);
            char action = glTpzGetAction();
            
            if (action == 'R') resetStructures(tpzMap, data, map);
            
            if (action == 'I') callDisplayStep(tpzMap, data, map);
            if (map->display_step != 0) showStep(map, tpzMap, data, 20.);
            else if (action == 'A') addNewPoint(map, tpzMap, data, 1); // the "1" checks if an existing point is close to the new one
            if (map->drawing_line == 2) addNewSegment(map, tpzMap, data);

            if (action == 'D') map->display_tpz = 1 - map->display_tpz;
            if (action == 'E') map->apply_force = 1 - map->apply_force;
            if (action == 'F') map->display_fast = 1 - map->display_fast;
            if (action == 'G') map->apply_gravity = 1 - map->apply_gravity;
            if (action == 'S') saveSegments("../data/segments_interactive.txt", tpzMap);
            
            if (action == 'X') addParticle(map, tpzMap, data, 20);
            updateParticles(map, tpzMap, data);
            
            drawTrapezoids(map, tpzMap);
            searchAndColorTrapezoid(map, tpzMap, data);
            
        } while(!glTpzWindowShouldClose());
        
        if (map->display_step) printf("\n"); fflush(stdout);
        
        freeDisplayMap(map);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // FREE MEMORY
    
    freeTpzMap(tpzMap);
    freeDataSet(data);
    
    return EXIT_SUCCESS;
}


