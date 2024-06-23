#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#pragma warning(disable:4996)

// 연결 리스트의 노드 정의
typedef struct Node {
    char data[100];
    struct Node* next;
} Node;

// 카테고리 구조체 정의 (포인터 배열 사용)
typedef struct Category {
    char name[100];
    Node* locations[3];
    struct Category* next;
} Category;

// 여행 계획 구조체 정의
typedef struct TravelPlan {
    Node* head;
} TravelPlan;

// gotoxy 함수 정의
void gotoxy(int x, int y) {
    COORD coord = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 리스트에 노드를 추가하는 함수 (중복 확인 포함)
void addNode(Node** head, const char* data) {
    Node* temp = *head;
    while (temp != NULL) {
        if (strcmp(temp->data, data) == 0) {
            // 중복된 데이터가 존재하면 추가하지 않음
            return;
        }
        temp = temp->next;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->data, data);
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    }
    else {
        temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// 파일에서 데이터를 읽어와 카테고리에 추가하는 함수
void readFileToCategory(const char* fileName, Node** head, int startLine, int endLine) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다: %s\n", fileName);
        return;
    }

    char line[100];
    int lineCount = 1;
    while (fgets(line, sizeof(line), file)) {
        if (lineCount >= startLine && lineCount <= endLine) {
            line[strcspn(line, "\n")] = '\0';  // 개행 문자 제거
            addNode(head, line);
        }
        lineCount++;
        if (lineCount > endLine) {
            break;
        }
    }

    fclose(file);
}

// 리스트를 출력하는 함수
void printList(Node* head) {
    Node* temp = head;
    int index = 1;
    while (temp != NULL) {
        printf("%d. %s\n", index, temp->data);
        temp = temp->next;
        index++;
    }
}

// 리스트를 해제하는 함수
void freeList(Node** head) {
    while (*head != NULL) {
        Node* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

// 카테고리를 초기화하는 함수
void initCategory(Category* category, const char* name) {
    strcpy(category->name, name);
    for (int i = 0; i < 3; i++) {
        category->locations[i] = NULL;
    }
    category->next = NULL;
}

void addCategory(Category** head, const char* name) {
    Category* newCategory = (Category*)malloc(sizeof(Category));
    initCategory(newCategory, name);

    if (*head == NULL) {
        *head = newCategory;
    }
    else {
        Category* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newCategory;
    }
}

void selectAndAddToPlan(Node* head, TravelPlan* plan) {
    int selection;
    printf("출력된 목록에서 선택할 항목의 번호를 입력하세요. ");
    scanf("%d", &selection);

    if (selection == 0) {
        return;
    }

    Node* temp = head;
    int currentIndex = 1;
    while (temp != NULL && currentIndex < selection) {
        temp = temp->next;
        currentIndex++;
    }

    if (temp != NULL) {
        addNode(&plan->head, temp->data);
        printf("'%s'가 여행 계획에 추가되었습니다.\n", temp->data);
    }
    else {
        printf("잘못된 선택입니다.\n");
    }
}

void displayMenu() {
    gotoxy(0, 0);
    printf("##########################\n");
    printf("1. 바다\n");
    printf("2. 도시\n");
    printf("3. 계곡\n");
    printf("4. 여행 계획 보기\n");
    printf("###########################\n");
    printf("번호를 입력하세요: ");
}

void displayCategoryMenu(const char* category) {
    gotoxy(0, 0);
    printf("###%s 관련 놀거리를 선택하세요###\n", category);
    printf("1. 카페\n");
    printf("2. 식당\n");
    printf("3. 관광지\n");
    printf("============================\n");
    printf("번호를 입력하세요: (돌아가려면 0 입력): ");
}

int processCategorySelection(int selection, int idx, Category* category, const char* filenames[], int ranges[][2]) {
    if (selection < 1 || selection > 3 || idx < 1 || idx > 3) {
        printf("잘못된 선택입니다.\n");
        return 0;
    }
    Node** head = &category->locations[idx - 1];
    *head = NULL;
    readFileToCategory(filenames[selection - 1], head, ranges[idx - 1][0], ranges[idx - 1][1]);
    printList(*head);
    return 1;
}

void processNextCategory(Category* category, const char* filenames[], int ranges[][2], int selection, TravelPlan* plan) {
    while (1) {
        system("cls");
        displayCategoryMenu(category->name);
        int idx;
        scanf("%d", &idx);
        if (idx == 0) {
            break;
        }
        if (processCategorySelection(selection, idx, category, filenames, ranges)) {
            selectAndAddToPlan(category->locations[idx - 1], plan);
        }
        printf("계속하려면 아무 키나 누르세요...\n");
        getchar(); getchar(); // 입력 버퍼를 비우고 대기
    }
}

void GUI(Category* categories, int ranges[][2], TravelPlan* plan) {
    int keyword = 0;
    int selection = 0;

    const char* seaFilenames[] = { "busan.txt", "gangneung.txt", "yeosu.txt" };
    const char* cityFilenames[] = { "seoul.txt", "gyeongju.txt", "daejeon.txt" };
    const char* valleyFilenames[] = { "goesan.txt", "gapyeong.txt" };

    while (1) {
        system("cls");
        displayMenu();
        scanf("%d", &keyword);

        if (keyword == 4) {
            system("cls");
            printf("여행 계획:\n");
            printList(plan->head);

            printf("계속하려면 아무 키나 한번 더 누르세요...\n");
            getchar(); getchar(); // 입력 버퍼를 비우고 대기
            continue;
        }

        switch (keyword) {
        case 1:
            system("cls");
            printf("###바다 관련 지역 선택하세요###\n");
            printf("1. 부산\n");
            printf("2. 강릉\n");
            printf("3. 여수\n");
            printf("============================\n\n");
            printf("번호를 입력하세요: ");
            scanf("%d", &selection);
            if (selection == 0 || selection < 1 || selection > 3) {
                printf("잘못된 선택입니다.\n");
                Sleep(1000); // 1초 대기 후 다시 메뉴 출력
                break;
            }
            processNextCategory(categories, seaFilenames, ranges, selection, plan);
            break;
        case 2:
            system("cls");
            printf("###도시 관련 지역을 선택하세요###\n");
            printf("1. 서울\n");
            printf("2. 경주\n");
            printf("3. 대전\n");
            printf("============================\n");
            printf("번호를 입력하세요: ");
            scanf("%d", &selection);
            if (selection == 0 || selection < 1 || selection > 3) {
                printf("잘못된 선택입니다.\n");
                Sleep(1000); // 1초 대기 후 다시 메뉴 출력
                break;
            }
            processNextCategory(categories->next, cityFilenames, ranges, selection, plan);
            break;
        case 3:
            system("cls");
            printf("###계곡 관련 지역를 선택하세요###\n");
            printf("1. 괴산\n");
            printf("2. 가평\n");
            printf("============================");
            printf("번호를 입력하세요: ");
            scanf("%d", &selection);
            if (selection == 0 || selection < 1 || selection > 2) {
                printf("잘못된 선택입니다.\n");
                Sleep(1000); // 1초 대기 후 다시 메뉴 출력
                break;
            }
            processNextCategory(categories->next->next, valleyFilenames, ranges, selection, plan);
            break;
        default:
            printf("잘못된 키워드입니다.\n");
            Sleep(1000); // 1초 대기 후 다시 메뉴 출력
        }
    }
}

int main() {
    Category* categories = NULL;
    addCategory(&categories, "바다");
    addCategory(&categories, "도시");
    addCategory(&categories, "계곡");

    int ranges[3][2] = { {2, 6}, {9, 13}, {16, 20} }; // 범위 저장
    TravelPlan plan;
    plan.head = NULL; // 최종 여행 계획 리스트 초기화

    GUI(categories, ranges, &plan);

    // 메모리 해제
    Category* tempCategory = categories;
    while (tempCategory != NULL) {
        for (int i = 0; i < 3; i++) {
            freeList(&tempCategory->locations[i]);
        }
        Category* toFree = tempCategory;
        tempCategory = tempCategory->next;
        free(toFree);
    }
    freeList(&plan.head);

    return 0;
}