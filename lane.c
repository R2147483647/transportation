#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void drawLane(char* code);
void laneMain(char* code, int length, int pos);
void l(float shift);
void s(float shift);
void r(float shift);
void u(float shift, float yshift);

float i = 0.25;
char* color;
int val;
FILE* file;

const float LanePx = 60;
const float k = 4;
const float dBan = 20;
const float rx = 10; // x轴半径
const float ry = 20; // y轴半径
const float startAngle = 0; // 起始角度
const float endAngle = M_PI; // 终止角度（半圆的角度）
const int largeArcFlag = 0; // 是否使用大弧标志位
const int sweepFlag = 0; // 是否顺时针标志位


int main() {
	char line[50];
	FILE* inFile = fopen("lane.in", "r");
	char* match = ">>>";

	if (inFile == NULL) {
		printf("无法打开文件lane.in\n");
		return 1;
	}
	while (fgets(line, sizeof(line), inFile) != NULL) {
		if (line[0] == match[0] && strstr(line, match) != NULL) {
			int j = 0, k = 1;
			while (line[++j] != '\0') {
				if (line[j] == '-') {
					++k;
				}
			}
			line[strcspn(line, "\n")] = '\0'; // 移除输入字符串的换行符

			file = fopen("lane.svg", "w");
			if (file == NULL) {
				printf("无法打开文件lane.svg\n");
				return 1;
			}

			fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%f\" height=\"300\">\n", (k + 0.5) * LanePx);
			char* token = strtok(line, " ");
			token = strtok(NULL, "-");
			while (token != NULL) {
				drawLane(token);
				token = strtok(NULL, "-");
				i++;
			}
			fprintf(file, "  <line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"300\" stroke=\"black\" stroke-width=\"2\" />\n", LanePx * i, LanePx * i);
			fprintf(file, "</svg>\n");

			fclose(file);
			printf("已将输出保存为output.svg文件。\n");
			break;
		}
	}
	fclose(inFile);
	return 0;
}

void drawLane(char* code) {
	int length = strlen(code);
	int startPos = 0;
	if (code[0] == 'b') {
		while (code[++startPos] != '/');
		color = "yellow";
		laneMain(code, startPos, 1);
		fprintf(file, "  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"yellow\" stroke-width=\"2\" />", LanePx * i + dBan, 200 + dBan, LanePx * (i + 1) - dBan, 300 - dBan);
		fprintf(file, "  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"yellow\" stroke-width=\"2\" />", LanePx * i + dBan, 300 - dBan, LanePx * (i + 1) - dBan, 200 + dBan);
		startPos++;
	}
	color = "black";
	laneMain(code, length, startPos);
}

void laneMain(char* code, int length, int pos) {
	val = (color[0] == 'b') ? 0 : 100;
	fprintf(file, "  <line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"300\" stroke=\"black\" stroke-width=\"2\" />\n", LanePx * i, LanePx * i);
	if (length - pos == 1) {	//显示优化
		if (code[pos] == 'u') {
			u(10, 40);
			return;
		}
		if (code[pos] == 'l') {
			l(10);
			return;
		}
		if (code[pos] == 'r') {
			r(-10);
			return;
		}
	}
	if (length - pos == 2) {
		if ((code[pos] == 'l' && code[pos + 1] == 'u' ) || (code[pos] == 'u' && code[pos + 1] == 'l' )) {
			u(10, 60);
			l(10);
			return;
		} else if ((code[pos] == 'l' && code[pos + 1] == 's' ) || (code[pos] == 's' && code[pos + 1] == 'l' )) {
			l(5);
			s(5);
			return;
		} else if ((code[pos] == 'r' && code[pos + 1] == 's' ) || (code[pos] == 's' && code[pos + 1] == 'r' )) {
			r(-5);
			s(-5);
			return;
		}
	}
	for (int j = pos; j < length; j++) {
		if (code[j] == 'v') {
			break;
		} else if (code[j] == 'l') {
			l(0);
		} else if (code[j] == 's') {
			s(0);
		} else if (code[j] == 'r') {
			r(0);
		} else if (code[j] == 'p') {
			fprintf(file, "  <line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"300\" stroke=\"yellow\" stroke-width=\"3\" />\n", LanePx * i + 3.5, LanePx * i + 3.5);
			fprintf(file, "  <line x1=\"%f\" y1=\"0\" x2=\"%f\" y2=\"300\" stroke=\"yellow\" stroke-width=\"3\" />\n", LanePx * (i + 1) - 3.5, LanePx * (i + 1) - 3.5);
		} else if (code[j] == 'u') {
			u(0, 60);
		}
	}
}

void u(float shift, float yshift) {
	float cx = LanePx * i + LanePx / 2 - rx + shift; // 椭圆中心的x坐标
	int cy = val + yshift;	// 椭圆中心的y坐标
	fprintf(file, "  <line x1=\"%f\" y1=\"%d\" x2=\"%f\" y2=\"%d\" stroke=\"%s\" stroke-width=\"2\" />\n", LanePx * i + LanePx / 2 + shift, cy, LanePx * i + LanePx / 2 + shift, val + 90, color);
	fprintf(file, "  <line x1=\"%f\" y1=\"%d\" x2=\"%f\" y2=\"%d\" stroke=\"%s\" stroke-width=\"2\" />\n", LanePx * i + LanePx / 2 + shift - 2 * rx, cy, LanePx * i + LanePx / 2 + shift - 2 * rx, val + 70, color);
	fprintf(file, "  <path d=\"M%f,%f A%f,%f 0 %d,%d %f,%f\" fill=\"none\" stroke=\"%s\" stroke-width=\"2\" />\n",
	        cx + rx * cos(startAngle), cy + ry * sin(startAngle), rx, ry, largeArcFlag, sweepFlag,
	        cx + rx * cos(endAngle), cy + ry * sin(endAngle), color);
	fprintf(file, "  <polygon points=\"%f,%d %f,%d %f,%d\" fill=\"%s\" />\n", LanePx * i + LanePx / 2 - 7.5 - 2 * rx + shift, val + 70, LanePx * i + LanePx / 2 - 2 * rx + shift, val + 90, LanePx * i + LanePx / 2 + 7.5 - 2 * rx + shift, val + 70, color);
}

void l(float shift) {
	fprintf(file, "  <path d=\"M %f %d L %f %d Q %f %d %f %d L %f %f\" stroke=\"%s\" stroke-width=\"2\" fill=\"none\" />\n",
	        LanePx * i + LanePx / 2 + shift, val + 90, LanePx * i + LanePx / 2 + shift, val + 50,
	        LanePx * i + LanePx / 2 + shift, val + 50, LanePx * i + LanePx / 2 + shift, val + 50,
	        LanePx * i + LanePx / 2 - 7.5 - 10 / sqrtf(3) + shift, val + 40 - 7.5 * sqrtf(3), color);
	fprintf(file, "  <polygon points=\"%f,%f %f,%f %f,%f\" fill=\"%s\" />\n",
	        LanePx * i + LanePx / 2 - 7.5 - 20 / sqrtf(3) + shift, val + 40 - 7.5 * sqrtf(3) + 10 / 3,
	        LanePx * i + LanePx / 2 - 7.5 - 20 / sqrtf(3) - k + shift, val + 30 - 7.5 * sqrtf(3) - k * sqrtf(3),
	        LanePx * i + LanePx / 2 - 7.5 + shift, val + 40 - 7.5 * sqrtf(3) - 10 / 3, color);
}

void s(float shift) {
	fprintf(file, "  <line x1=\"%f\" y1=\"%d\" x2=\"%f\" y2=\"%d\" stroke=\"%s\" stroke-width=\"2\" />\n", LanePx * i + LanePx / 2 + shift, val + 20, LanePx * i + LanePx / 2 + shift, val + 90, color);
	fprintf(file, "  <polygon points=\"%f,%d %f,%d %f,%d\" fill=\"%s\" />\n", LanePx * i + LanePx / 2 - 7.5 + shift, val + 25, LanePx * i + LanePx / 2 + shift, val + 5, LanePx * i + LanePx / 2 + 7.5 + shift, val + 25, color);
}

void r(float shift) {
	fprintf(file, "  <path d=\"M %f %d L %f %d Q %f %d %f %d L %f %f\" stroke=\"%s\" stroke-width=\"2\" fill=\"none\" />\n",
	        LanePx * i + LanePx / 2 + shift, val + 90, LanePx * i + LanePx / 2 + shift, val + 50,
	        LanePx * i + LanePx / 2 + shift, val + 50, LanePx * i + LanePx / 2 + shift, val + 50,
	        LanePx * i + LanePx / 2 + 7.5 + 10 / sqrtf(3) + shift, val + 40 - 7.5 * sqrtf(3), color);
	fprintf(file, "  <polygon points=\"%f,%f %f,%f %f,%f\" fill=\"%s\" />\n",
	        LanePx * i + LanePx / 2 + 7.5 + 20 / sqrtf(3) + shift, val + 40 - 7.5 * sqrtf(3) + 10 / 3,
	        LanePx * i + LanePx / 2 + 7.5 + 20 / sqrtf(3) + k + shift, val + 30 - 7.5 * sqrtf(3) - k * sqrtf(3),
	        LanePx * i + LanePx / 2 + 7.5 + shift, val + 40 - 7.5 * sqrtf(3) - 10 / 3, color);
}
