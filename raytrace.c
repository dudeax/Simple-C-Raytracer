#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define PI 3.14159265

typedef struct Vector3 {
    double x, y, z;
} vector3;

typedef struct Ray {
    vector3 position;
    vector3 direction;
} ray;

typedef struct Collision {
    vector3 position;
    vector3 normal;
} collision;

typedef struct Sphere {
    vector3 center;
    double radius, reflectance;
} sphere;

typedef struct LightSource {
    sphere collision;
    double brightness;
} lightSource;

void newVector3(double x, double y, double z, vector3 *returnVector) {
    returnVector->x = x;
    returnVector->y = y;
    returnVector->z = z;
}

void setVector3(vector3 *one, vector3 *returnVector) {
    returnVector->x = one->x;
    returnVector->y = one->y;
    returnVector->z = one->z;
}

void addVector3(vector3 *one, vector3 *two, vector3 *returnVector) {
    returnVector->x = (one->x) + (two->x);
    returnVector->y = (one->y) + (two->y);
    returnVector->z = (one->z) + (two->z);
}

void subtractVector3(vector3 *one, vector3 *two, vector3 *returnVector) {
    returnVector->x = (one->x) - (two->x);
    returnVector->y = (one->y) - (two->y);
    returnVector->z = (one->z) - (two->z);
}

void scaleVector3(double scale, vector3 *one, vector3 *returnVector) {
    returnVector->x = (one->x)*scale;
    returnVector->y = (one->y)*scale;
    returnVector->z = (one->z)*scale;
}

void divideVector3(double scale, vector3 *one, vector3 *returnVector) {
    returnVector->x = (one->x)/scale;
    returnVector->y = (one->y)/scale;
    returnVector->z = (one->z)/scale;
}

double lengthVector3(vector3 *vector) {
    return sqrt((vector->x)*(vector->x) + (vector->y)*(vector->y) + (vector->z)*(vector->z));
}

double dotVector3(vector3 *one, vector3 *two) {
    return (one->x)*(two->x) + (one->y)*(two->y) + (one->z)*(two->z);
}

void unitVector3(vector3 *one, vector3 *returnVector) {
    divideVector3(lengthVector3(one), one, returnVector);
}

void rayCollidingWithSphere(ray *incoming, sphere *colliding, vector3 *normal, vector3 *position, double *distance) {
    vector3 tempVector;
    subtractVector3(&(colliding->center), &(incoming->position), &tempVector);
    double rayLengthToCenter = dotVector3(&tempVector, &(incoming->direction));
    setVector3(&(incoming->direction), &tempVector);
    scaleVector3(rayLengthToCenter, &tempVector, &tempVector);
    addVector3(&tempVector, &(incoming->position), &tempVector);
    subtractVector3(&tempVector, &(colliding->center), &tempVector);
    double distanceFromCenter = lengthVector3(&tempVector);
    if (distanceFromCenter <= (colliding->radius)) {
        double distanceToEdge = sqrt(((colliding->radius) * (colliding->radius)) - (distanceFromCenter * distanceFromCenter));
        *distance = rayLengthToCenter-distanceToEdge;
        scaleVector3((rayLengthToCenter-distanceToEdge), &(incoming->direction), position);
        addVector3(position, &(incoming->position), position);
        subtractVector3(position, &(colliding->center), normal);
        divideVector3((colliding->radius), normal, normal);
    }
}

char toChar(double value) {
    if (value > 250.0) { return '#';
	} else if (value > 225.0) { return '@';
	} else if (value > 200.0) { return '*';
	} else if (value > 175.0) { return 'o';
	} else if (value > 150.0) { return '=';
	} else if (value > 125.0) { return 'x';
	} else if (value > 100.0) { return '+';
	} else if (value > 75.0) { return 'c';
	} else if (value > 50.0) { return '^';
	} else if (value > 25.0) { return '~';
	} else if (value > 10.0) { return '-';
    } else { return ' '; }
    return '?';
}

void drawScreen(double *pixels, int sizex, int sizey) {
	char *printValue = (char*) malloc(sizex * sizey + sizey + 1);
	int lastChar = 0;
    for (int y = 0; y < sizey; y++) {
        for (int x = 0; x < sizex; x++) {
			printValue[lastChar] = toChar(pixels[y*sizex + x]);
			lastChar++;
		}
        printValue[lastChar] = '\n';
		lastChar++;
    }
	printf(printValue);
	free(printValue);
}

double getRand(){
	return ((rand()%20000)-10000)/10000.0;
}

int main(int argc, char **argv){
    // User Variables
    int sizex = 158;
    int sizey = 42;
    int samples = 500;
    int bounces = 5;
    double verticalFov = 90.0;
	double horizontalFov = 169.0;

    // System Variables
    double *pixels;

    int numberOfSpheres = 1;
    sphere *spheres;
    int numberOfLightSources = 3;
    lightSource *lightSources;

    vector3 cameraPosition;
    vector3 cameraDirection;
    newVector3(0.0, 1.0, -3.0, &cameraPosition);
    newVector3(0.0, 0.0, 1.0, &cameraDirection);
    unitVector3(&cameraDirection,&cameraDirection);


    // Setup
    pixels = (double*) malloc(sizex * sizey * sizeof(double));
    for (int value = 0; value < (sizex * sizey); value++) pixels[value] = 0.0;

    spheres = (sphere*) malloc(numberOfSpheres * sizeof(sphere));
	for (int sphereId = 0; sphereId < numberOfSpheres; sphereId++) {
		newVector3((sphereId - (numberOfSpheres/2)) * 10, 0, 10.0, &spheres[sphereId].center);
		spheres[sphereId].radius = 4.0;
		spheres[sphereId].reflectance = 0.0;
	}

    lightSources = (lightSource*) malloc(numberOfLightSources * sizeof(lightSource));
	newVector3(0.0, 0.0, 0.0, &lightSources[0].collision.center);
    lightSources[0].collision.radius = 5.0;
    lightSources[0].brightness = 255.0;

	newVector3(0.0, -1200.0, 0.0, &lightSources[1].collision.center);
    lightSources[1].collision.radius = 1000.0;
    lightSources[1].brightness = 100.0;

	newVector3(0.0, 0.0, 0.0, &lightSources[2].collision.center);
    lightSources[2].collision.radius = 5.0;
    lightSources[2].brightness = 255.0;

    // Render
    // rand()'s ur new best friend
    ray currentRay;
    vector3 collisionNormal;
	vector3 tempCollisionNormal;
    vector3 collisionPosition;
	vector3 tempCollisionPosition;
	vector3 tempVector;
    double distance;
	double tempDistance;
	int isALightSource;
	double time = 0.0;
	double reflectance;

	while (time < 20.0) {
		newVector3(20.0 * sin(time), 5.0+ 10.0 * cos(time), 5.0, &lightSources[0].collision.center);
		newVector3(0.0, 20.0 * cos(time * 0.77), 10.0 + 20.0 * sin(time * 0.77), &lightSources[2].collision.center);
		for (int y = 0; y < sizey; y++) {
			for (int x = 0; x < sizex; x++) {
				double pixelValue = 0.0;
				int samplesTaken = 0;
				for (int sample = 0; sample < samples; sample++) {
					samplesTaken++;
					setVector3(&cameraPosition, &currentRay.position);
					double directionHorizontal = atan2(cameraDirection.z, cameraDirection.x) + ((x - (sizex / 2)) * 1.0 / sizex) * ((horizontalFov / 360.0) * (2 * PI));
					double directionVertical = atan2(cameraDirection.y, sqrt((cameraDirection.z * cameraDirection.z) + (cameraDirection.x * cameraDirection.x))) + ((y - (sizey / 2)) * 1.0 / sizey) * ((verticalFov / 360.0) * (2 * PI));
					currentRay.direction.x = cos(directionHorizontal)*cos(directionVertical);
					currentRay.direction.z = sin(directionHorizontal)*cos(directionVertical);
					currentRay.direction.y = sin(directionVertical);
					int bounce = 0;
					while (bounce < bounces) {
						bounce++;
						isALightSource = -1;
						reflectance = 0;
						distance = -1;
						for (int sphereId = 0; sphereId < numberOfSpheres; sphereId++) {
							tempDistance = -1;
							rayCollidingWithSphere(&currentRay, &spheres[sphereId], &tempCollisionNormal, &tempCollisionPosition, &tempDistance);
							if (((tempDistance > -1)&&(tempDistance<distance))||((tempDistance > -1)&&(distance == -1)))  {
								distance = tempDistance;
								setVector3(&tempCollisionNormal, &collisionNormal);
								setVector3(&tempCollisionPosition, &collisionPosition);
								reflectance = spheres[sphereId].reflectance;
							}
						}
						for (int lightSourceId = 0; lightSourceId < numberOfLightSources; lightSourceId++) {
							tempDistance = -1;
							rayCollidingWithSphere(&currentRay, &(lightSources[lightSourceId].collision), &tempCollisionNormal, &tempCollisionPosition, &tempDistance);
							if (((tempDistance > -1)&&(tempDistance<distance))||((tempDistance > -1)&&(distance == -1)))  {
								distance = tempDistance;
								setVector3(&tempCollisionNormal, &collisionNormal);
								setVector3(&tempCollisionPosition, &collisionPosition);
								isALightSource = lightSourceId;
							}
						}
						if (isALightSource != -1) { // Hit a light source
							pixelValue += lightSources[isALightSource].brightness;
							break;
						} else if (distance == -1){ // Didn't hit anything
							break;
						} else { // Hit some other object, calculate continued light path
							scaleVector3(-1.0, &currentRay.direction, &tempVector);
							double bounceDistance = dotVector3(&tempVector, &collisionNormal);
							scaleVector3((bounceDistance * 2), &collisionNormal, &tempVector);
							addVector3(&currentRay.direction, &tempVector, &currentRay.direction);
							setVector3(&collisionPosition, &currentRay.position);
							newVector3(((rand() & 2000) - 1000) * 1.0 / 1000, ((rand() & 2000) - 1000) * 1.0 / 1000, ((rand() & 2000) - 1000) * 1.0 / 1000, &tempVector);
							unitVector3(&tempVector, &tempVector);
							scaleVector3(1.0/(1+reflectance), &tempVector, &tempVector);
							addVector3(&currentRay.direction, &tempVector, &currentRay.direction);
							unitVector3(&currentRay.direction, &currentRay.direction);
						}
					}
					if (bounce == 1) break;
				}
				pixels[y * sizex + x] = pixelValue/samplesTaken;
			}
		}

		// Draw
		//printf("%f\n",time);
		drawScreen(pixels, sizex, sizey);
		usleep(1000);
		time += 0.1;
	}
}
