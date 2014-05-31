    float zMin = 0;
    float zMax = 0;
    ...

    FILE *gp = popen(GNUPLOT,"w");
    if (gp==NULL) {
        printf("Error opening pipe to GNU plot\n");
        return  EXIT_FAILURE;
    }

    fprintf(gp, "unset key\n");
    fprintf(gp, "set contour base\n");
    fprintf(gp, "set terminal png enhanced size 1280,1024\n");
    ...
    fprintf(gp, "set datafile separator \",\"\n");
    fprintf(gp, "set zrange [%f:%f]\n",zMin,zMax);
    ...
    fprintf(gp, "splot \"%s\" matrix  \n",sourceName);
    pclose(gp);

