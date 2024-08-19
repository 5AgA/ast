#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"


int main(void)
{   
    // *******************************JSON Reader*******************************
    // json 파일 오픈
    FILE *fp = fopen("target.json", "r");
    if (fp == NULL){
        printf("파일열기 실패\n");
        return 0;
    } else {
        printf("파일열기 성공\n");
    }

    long size;      // 파일 사이즈
    char *buffer;   // 문자 저장하는 곳

    // 파일 크기 구하기
    fseek(fp, 0, SEEK_END);  // 마지막으로 이동해서
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  // 다시 포인터를 처음으로 이동

    // 파일 크기 + NULL 공간만큼 메모리를 할당하고 0으로 초기화
    buffer = malloc(size + 1);
    memset(buffer, 0, size + 1);

    // 파일 내용 읽기
    while(1) {
        char temp[2];
        char *tStr = fgets(temp, 2, fp);
        strcat(buffer, temp);
        if(tStr == NULL) break;
    }

    fclose(fp);    // 파일 포인터 닫기

    // *******************************AST Parser*******************************
    json_value json = json_create(buffer);

    //json_print(json); putchar('\n'); //전체 출력

    int funcNum = 0;                    // 함수의 개수

    json_value ext = json_get(json, "ext");

    for (int i = 0; i < json_len(ext); i++) {
        json_value node = json_get(ext, i);
        char *nodeType = json_get_string(node, "_nodetype");

        if (strcmp(nodeType, "FuncDef") == 0) {
            json_value func = json_get(node, "decl");
            funcNum++;

            // 반환 타입
            char *funcRet;
            if (!json_is_null(json_get(json_get(func, "type"), "args"))) {
                if (strcmp(json_get_string(json_get(json_get(func, "type"), "type"), "_nodetype"), "PtrDecl") == 0) {
                    funcRet = json_get_string(json_get(json_get(json_get(json_get(json_get(func, "type"), "type"), "type"), "type"), "names"), 0);
                    strcat(funcRet, "*");
                } else {
                    funcRet = json_get_string(json_get(json_get(json_get(json_get(func, "type"), "type"), "type"), "names"), 0);
                }
            } else {
                funcRet = json_get_string(json_get(json_get(json_get(json_get(func, "type"), "type"), "type"), "names"), 0);
            }
                
            // 함수명
            char *funcName = json_get_string(func, "name");
            printf("%d) %s %s(", funcNum, funcRet, funcName);

            // 매개 변수
            if (!json_is_null(json_get(json_get(func, "type"), "args"))) {
                json_value params = json_get(json_get(json_get(func, "type"), "args"), "params");
                for (int j = 0; j < json_len(params); j++) {
                    json_value param = json_get(params, j);

                    // 타입
                    char *pType;
                    if (strcmp(json_get_string(json_get(param, "type"), "_nodetype"), "PtrDecl") == 0) {
                        pType = json_get_string(json_get(json_get(json_get(json_get(param, "type"), "type"), "type"), "names"), 0);
                        strcat(pType, "*");
                    } else {
                        pType = json_get_string(json_get(json_get(json_get(param, "type"), "type"), "names"), 0);
                    }
                    if (j != 0) 
                        printf(", ");

                    // 이름
                    if (json_is_null(json_get(param, "name"))) {
                        printf("%s", pType);

                    } else {
                        char *pName = json_get_string(param, "name");
                        printf("%s %s", pType, pName);
                    }
                }
            }
            printf(") ");

            // if문의 개수
            int cntIf = 0; 
            json_value blockItems = json_get(json_get(node, "body"), "block_items");
            for (int k = 0; k < json_len(blockItems); k++) {
                json_value item = json_get(blockItems, k);
                if (strcmp(json_get_string(json_get(item, "_nodetype")), "If") == 0)
                    cntIf++;
            }
            printf("number of if: %d\n", cntIf);

        } else if (strcmp(json_get_string(json_get(node, "type"), "_nodetype"), "FuncDecl") == 0) {
            funcNum++;

            // 반환 타입
            char *funcRet;
            if (strcmp(json_get_string(json_get(json_get(node, "type"), "type"), "_nodetype"), "PtrDecl") == 0) {
                funcRet = json_get_string(json_get(json_get(json_get(json_get(json_get(node, "type"), "type"), "type"), "type"), "names"), 0);
                strcat(funcRet, "*");
            } else {
                funcRet = json_get_string(json_get(json_get(json_get(json_get(node, "type"), "type"), "type"), "names"), 0);
            }

            // 함수명
            char *funcName = json_get_string(node, "name");
            printf("%d) %s %s(", funcNum, funcRet, funcName);
            

            // 매개 변수
            if (json_is_null(json_get(json_get(node, "type"), "args")) == 0) {
                json_value params = json_get(json_get(json_get(node, "type"), "args"), "params");
                for (int j = 0; j < json_len(params); j++) {
                    json_value param = json_get(params, j);

                    // 타입
                    char *pType = json_get_string(json_get(json_get(json_get(param, "type"), "type"), "names"), 0);
                    if (j != 0) 
                        printf(", ");

                    // 이름
                    if (json_is_null(json_get(param, "name"))) {
                        printf("%s", pType);

                    } else {
                        char *pName = json_get_string(param, "name");
                        printf("%s %s", pType, pName);
                    }
                }
            }
            printf(")\n");
        } else continue;
    }

    printf("Number of Functions: %d\n", funcNum);
    json_free(json);
   return 0;
}
