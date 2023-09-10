#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
	float x, y;
} Plot;

FILE *file;
char* color;

const float r = 20;

typedef struct TreeNode {
	char* name;
	int val;
	struct TreeNode** child;
	int childCount;
} TreeNode;

TreeNode* treeNodeInit(char* name, int val) {
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	node->name = strdup(name);
	node->val = val;
	node->child = NULL;
	node->childCount = 0;
	return node;
}

void treeNodeAddChild(TreeNode* parent, TreeNode* child) {
	parent->childCount++;
	parent->child = (TreeNode**)realloc(parent->child, sizeof(TreeNode*) * parent->childCount);
	parent->child[parent->childCount - 1] = child;
}

void treeNodeFree(TreeNode* node) {
	if (node == NULL) {
		return;
	}
	for (int i = 0; i < node->childCount; i++) {
		treeNodeFree(node->child[i]);
	}
	free(node->child);
	free(node->name);
	free(node);
}

typedef struct {
	Plot** data;
	int* lengthArray;
	int size;
	int capacity;
} PlotVector;

PlotVector* plotVector;

void plotVectorInit() {
	plotVector = (PlotVector*)malloc(sizeof(PlotVector));
	plotVector->data = NULL;
	plotVector->lengthArray = NULL;
	plotVector->size = 0;
	plotVector->capacity = 0;
}

void plotVectorFree() {
	free(plotVector->lengthArray);
	free(plotVector->data);
	free(plotVector);
}

void plotVectorPushBack(Plot* plot, int sizeofPlotVector) {
	if (plotVector->size >= plotVector->capacity) {
		int newCapacity = (plotVector->capacity == 0) ? 1 : plotVector->capacity * 2;
		Plot** newData = (Plot**)realloc(plotVector->data, newCapacity * sizeofPlotVector);
		int* newLengthArray = (int*)realloc(plotVector->lengthArray, newCapacity * sizeof(int));
		if (newData == NULL) {
			printf("Memory allocation failed.\n");
			return;
		}
		plotVector->data = newData;
		plotVector->capacity = newCapacity;
		plotVector->lengthArray = newLengthArray;
	}
	plotVector->data[plotVector->size] = plot;
	plotVector->lengthArray[plotVector->size] = sizeofPlotVector;
	plotVector->size++;
}

void QBcurvePrint(int index) {
	if (index < 0 || index >= plotVector->size) {
		printf("Invalid index.\n");
		return;
	}
	Plot* plots = plotVector->data[index];

	int i = 0;
	float theta0, theta1 = atanf((plots[i + 1].y - plots[i].y) / (plots[i + 1].x - plots[i].x));
	fprintf(file, "<path d=\"M %f %f ", plots[0].x, plots[0].y);
	while (i < plotVector->lengthArray[index] / sizeof(Plot) - 2) {
		i++;
		theta0 = theta1;
		theta1 = atanf((plots[i + 1].y - plots[i].y) / (plots[i + 1].x - plots[i].x));
		fprintf(file, "L %f %f Q %f %f %f %f ",
		        plots[i].x + r * cosf(theta0 + M_PI), plots[i].y + r * sinf(theta0 + M_PI),
		        plots[i].x, plots[i].y,
		        plots[i].x + r * cosf(theta1), plots[i].y + r * sinf(theta1));
	}
	fprintf(file, "L %f %f\" stroke=\"%s\" stroke-width=\"2\" fill=\"none\" />\n", plots[i + 1].x, plots[i + 1].y, color);
}



int main() {
	plotVectorInit();
	file = fopen("output.svg", "w");
	if (file == NULL) {
		printf("Can not open SVG file.\n");
		return 1;
	}
	fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"300\" height=\"300\">\n");
	color = "black";

	Plot a[] = {{0, 10}, {40, 10}, {70, 80}, {100, 100}, {140, 80}, {180, 20}};
	Plot plot1[] = {{10, 20}, {20, 40}, {40, 20}};
	Plot plot2[] = {200, 200};
	plotVectorPushBack(plot1, sizeof(plot1));
	plotVectorPushBack(plot2, sizeof(plot2));
	plotVectorPushBack(a, sizeof(a));
	QBcurvePrint(0);



	fprintf(file, "</svg>\n");
	fclose(file);
	plotVectorFree();
	return 0;
}
