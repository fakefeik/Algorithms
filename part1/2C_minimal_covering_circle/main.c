#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    int isValid;
    double a;
    double b;
} Segment;

int compare(const void *a, const void *b)
{
    double cmp = (*(double *)a - *(double *)b);
    return cmp < 0 ? -1 : cmp == 0 ? 0 : 1;
}

Segment *getSegments(Point *points, int pointsCount, double radius)
{
    Segment *segments = (Segment *)malloc(sizeof(Segment) * pointsCount);
    for (size_t i = 0; i < pointsCount; i++)
    {
        double height = radius - abs(points[i].y);
        if (height < 0)
        {
            segments[i].isValid = 0;
            continue;
        }

        double segmentLength = 2 * sqrt(2 * radius * height - height * height);
        segments[i].a = points[i].x - segmentLength / 2;
        segments[i].b = points[i].x + segmentLength / 2;
        segments[i].isValid = 1;
    }
    return segments;
}

int canBuildCircle(Point *points, int pointsCount, double radius, int neededCount)
{
    Segment *segments = getSegments(points, pointsCount, radius);
    double *leftBounds = (double *)malloc(sizeof(double) * pointsCount);
    double *rightBounds = (double *)malloc(sizeof(double) * pointsCount);
    int boundsCount = 0;
    for (size_t i = 0; i < pointsCount; i++)
    {
        if (!segments[i].isValid)
            continue;
        leftBounds[boundsCount] = segments[i].a;
        rightBounds[boundsCount++] = segments[i].b;
    }

    qsort(leftBounds, boundsCount, sizeof(double), compare);
    qsort(rightBounds, boundsCount, sizeof(double), compare);
    
    int maxIntersectionsCount = 0;
    int currentCount = 0;
    int i = 0;
    int j = 0;
    while (i < boundsCount || j < boundsCount)
    {
        if (i < boundsCount && leftBounds[i] <= rightBounds[j])
        {
            i++;
            currentCount++;
        }
        else if (j < boundsCount)
        {
            j++;
            currentCount--;
        }
        if (maxIntersectionsCount < currentCount)
            maxIntersectionsCount = currentCount;
    }
    return maxIntersectionsCount >= neededCount;
}

double binarySearch(Point *points, int pointsCount, double minRadius, double maxRadius, int neededCount, double eps)
{
    double mid = (minRadius + maxRadius) / 2;
    if (maxRadius - minRadius < eps)
        return mid;
    if (canBuildCircle(points, pointsCount, mid, neededCount))
        return binarySearch(points, pointsCount, minRadius, mid, neededCount, eps);
    else
        return binarySearch(points, pointsCount, mid, maxRadius, neededCount, eps);
}

int main(int argc, char *argv[])
{
    int n;
    int k;
    scanf("%d %d", &n, &k);
    Point *points = (Point *)malloc(sizeof(Point) * n);
    for (int i = 0; i < n; i++)
    {
        Point p;
        scanf("%d %d", &p.x, &p.y);
        points[i] = p;
    }
    double radius = binarySearch(points, n, 0, 1500, k, 1e-3);
    printf("%f", radius);
}
