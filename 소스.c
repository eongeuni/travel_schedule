#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#pragma warning(disable:4996)

// ���� ����Ʈ�� ��� ����
typedef struct Node {
    char data[100];
    struct Node* next;
} Node;

// ī�װ� ����ü ���� (������ �迭 ���)
typedef struct Category {
    char name[100];
    Node* locations[3];
    struct Category* next;
} Category;

// ���� ��ȹ ����ü ����
typedef struct TravelPlan {
    Node* head;
} TravelPlan;

// gotoxy �Լ� ����
void gotoxy(int x, int y) {
    COORD coord = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ����Ʈ�� ��带 �߰��ϴ� �Լ� (�ߺ� Ȯ�� ����)
void addNode(Node** head, const char* data) {
    Node* temp = *head;
    while (temp != NULL) {
        if (strcmp(temp->data, data) == 0) {
            // �ߺ��� �����Ͱ� �����ϸ� �߰����� ����
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

// ���Ͽ��� �����͸� �о�� ī�װ��� �߰��ϴ� �Լ�
void readFileToCategory(const char* fileName, Node** head, int startLine, int endLine) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("������ �� �� �����ϴ�: %s\n", fileName);
        return;
    }

    char line[100];
    int lineCount = 1;
    while (fgets(line, sizeof(line), file)) {
        if (lineCount >= startLine && lineCount <= endLine) {
            line[strcspn(line, "\n")] = '\0';  // ���� ���� ����
            addNode(head, line);
        }
        lineCount++;
        if (lineCount > endLine) {
            break;
        }
    }

    fclose(file);
}

// ����Ʈ�� ����ϴ� �Լ�
void printList(Node* head) {
    Node* temp = head;
    int index = 1;
    while (temp != NULL) {
        printf("%d. %s\n", index, temp->data);
        temp = temp->next;
        index++;
    }
}

// ����Ʈ�� �����ϴ� �Լ�
void freeList(Node** head) {
    while (*head != NULL) {
        Node* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

// ī�װ��� �ʱ�ȭ�ϴ� �Լ�
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
    printf("��µ� ��Ͽ��� ������ �׸��� ��ȣ�� �Է��ϼ���. ");
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
        printf("'%s'�� ���� ��ȹ�� �߰��Ǿ����ϴ�.\n", temp->data);
    }
    else {
        printf("�߸��� �����Դϴ�.\n");
    }
}

void displayMenu() {
    gotoxy(0, 0);
    printf("##########################\n");
    printf("1. �ٴ�\n");
    printf("2. ����\n");
    printf("3. ���\n");
    printf("4. ���� ��ȹ ����\n");
    printf("###########################\n");
    printf("��ȣ�� �Է��ϼ���: ");
}

void displayCategoryMenu(const char* category) {
    gotoxy(0, 0);
    printf("###%s ���� ��Ÿ��� �����ϼ���###\n", category);
    printf("1. ī��\n");
    printf("2. �Ĵ�\n");
    printf("3. ������\n");
    printf("============================\n");
    printf("��ȣ�� �Է��ϼ���: (���ư����� 0 �Է�): ");
}

int processCategorySelection(int selection, int idx, Category* category, const char* filenames[], int ranges[][2]) {
    if (selection < 1 || selection > 3 || idx < 1 || idx > 3) {
        printf("�߸��� �����Դϴ�.\n");
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
        printf("����Ϸ��� �ƹ� Ű�� ��������...\n");
        getchar(); getchar(); // �Է� ���۸� ���� ���
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
            printf("���� ��ȹ:\n");
            printList(plan->head);

            printf("����Ϸ��� �ƹ� Ű�� �ѹ� �� ��������...\n");
            getchar(); getchar(); // �Է� ���۸� ���� ���
            continue;
        }

        switch (keyword) {
        case 1:
            system("cls");
            printf("###�ٴ� ���� ���� �����ϼ���###\n");
            printf("1. �λ�\n");
            printf("2. ����\n");
            printf("3. ����\n");
            printf("============================\n\n");
            printf("��ȣ�� �Է��ϼ���: ");
            scanf("%d", &selection);
            if (selection == 0 || selection < 1 || selection > 3) {
                printf("�߸��� �����Դϴ�.\n");
                Sleep(1000); // 1�� ��� �� �ٽ� �޴� ���
                break;
            }
            processNextCategory(categories, seaFilenames, ranges, selection, plan);
            break;
        case 2:
            system("cls");
            printf("###���� ���� ������ �����ϼ���###\n");
            printf("1. ����\n");
            printf("2. ����\n");
            printf("3. ����\n");
            printf("============================\n");
            printf("��ȣ�� �Է��ϼ���: ");
            scanf("%d", &selection);
            if (selection == 0 || selection < 1 || selection > 3) {
                printf("�߸��� �����Դϴ�.\n");
                Sleep(1000); // 1�� ��� �� �ٽ� �޴� ���
                break;
            }
            processNextCategory(categories->next, cityFilenames, ranges, selection, plan);
            break;
        case 3:
            system("cls");
            printf("###��� ���� ������ �����ϼ���###\n");
            printf("1. ����\n");
            printf("2. ����\n");
            printf("============================");
            printf("��ȣ�� �Է��ϼ���: ");
            scanf("%d", &selection);
            if (selection == 0 || selection < 1 || selection > 2) {
                printf("�߸��� �����Դϴ�.\n");
                Sleep(1000); // 1�� ��� �� �ٽ� �޴� ���
                break;
            }
            processNextCategory(categories->next->next, valleyFilenames, ranges, selection, plan);
            break;
        default:
            printf("�߸��� Ű�����Դϴ�.\n");
            Sleep(1000); // 1�� ��� �� �ٽ� �޴� ���
        }
    }
}

int main() {
    Category* categories = NULL;
    addCategory(&categories, "�ٴ�");
    addCategory(&categories, "����");
    addCategory(&categories, "���");

    int ranges[3][2] = { {2, 6}, {9, 13}, {16, 20} }; // ���� ����
    TravelPlan plan;
    plan.head = NULL; // ���� ���� ��ȹ ����Ʈ �ʱ�ȭ

    GUI(categories, ranges, &plan);

    // �޸� ����
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