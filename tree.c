#include <stdio.h>
#include <string.h>

#define MAX_LEN   1000
#define MAX_DEPTH 100

typedef struct
{
    char node_data[MAX_DEPTH];
    int node_top;
    int counter_data[MAX_DEPTH];
    int counter_top;
} TreeStack;

void tree_stack_init(TreeStack *s)
{
    s->node_top = -1;
    s->counter_top = -1;
}

int node_is_empty(TreeStack *s)
{
    return s->node_top < 0;
}

void node_push(TreeStack *s, char node)
{
    s->node_top++;
    s->node_data[s->node_top] = node;
}

char node_pop(TreeStack *s)
{
    if (node_is_empty(s))
        return '\0';
    char popped = s->node_data[s->node_top];
    s->node_top--;
    return popped;
}

char node_peek(TreeStack *s)
{
    if (node_is_empty(s))
        return '\0';
    return s->node_data[s->node_top];
}

int counter_is_empty(TreeStack *s) 
{
    return s->counter_top < 0;
}

void counter_push(TreeStack *s, int initial_count)
{
    s->counter_top++;
    s->counter_data[s->counter_top] = initial_count;
}

int counter_pop(TreeStack *s)
{
    if (counter_is_empty(s))
        return 0;
    int popped = s->counter_data[s->counter_top];
    s->counter_top--;
    return popped;
}

void counter_increment_top(TreeStack *s)
{
    if (counter_is_empty(s))
        return;
    s->counter_data[s->counter_top]++;
}

int counter_depth(TreeStack *s)
{
    return s->counter_top + 1;
}

int check_tree(const char *str)
{
    int len = strlen(str);
    if (len == 0)
        return 0;

    int bracket_count = 0;
    int expect_node = 1;
    int expect_delim_or_bracket = 0;

    for (int i = 0; i < len; i++)
    {
        char c = str[i];
        if (c == ' ' || c == '\r' || c == '\n') continue;

        if (c >= 'A' && c <= 'Z')
        {
            if (!expect_node) return 0;
            expect_node = 0;
            expect_delim_or_bracket = 1;
        }
        else if (c == '(')
        {
            if (expect_node) return 0;
            bracket_count++;
            expect_node = 1;
            expect_delim_or_bracket = 0;
        }
        else if (c == ')')
        {
            bracket_count--;
            if (bracket_count < 0) return 0;
            expect_node = 0;
            expect_delim_or_bracket = 1;
        }
        else if (c == ',')
        {
            if (!expect_delim_or_bracket) return 0;
            expect_node = 1;
            expect_delim_or_bracket = 0;
        }
        else
            return 0;
    }

    if (bracket_count == 0 && expect_delim_or_bracket)
        return 1;
    return 0;
}

void print_tree(const char *str)
{
    printf("\n=== 계층형 트리 출력 ===\n");
    int depth = 0;
    int len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        char c = str[i];
        if (c == ' ' || c == '\r' || c == '\n')
            continue;
        if (c >= 'A' && c <= 'Z')
        {
            if (depth > 0)
            {
                for (int d = 0; d < depth - 1; d++)
                    printf("    ");
                printf("+---");
            }
            printf("%c\n", c);
        }
        else if (c == '(')
            depth++;
        else if (c == ')')
            depth--;
    }
}

int main(void) {
    char input[MAX_LEN];

    printf("트리 괄호 표기법 입력: ");
    if (fgets(input, sizeof(input), stdin) != NULL)
        input[strcspn(input, "\r\n")] = '\0';

    if (!check_tree(input))
    {
        printf("오류: 올바른 트리의 괄호 표기법이 아닙니다.\n");
        return 1;
    }
    printf("유효한 트리 괄호 표기법입니다.\n");

    TreeStack tstack;
    tree_stack_init(&tstack);

    int total_nodes = 0;
    int non_leaf_nodes = 0;
    int max_height = 0;
    int max_degree = 0;

    char parent_of_C = '\0';
    char children_of_C[50];
    int  child_count_C = 0;
    int  capturing_children_of_C = 0;
    int  C_bracket_depth = -1;

    char last_node = '\0';
    int len = strlen(input);

    for (int i = 0; i < len; i++)
    {
        char c = input[i];
        if (c == ' ' || c == '\r' || c == '\n')
            continue;

        if (c >= 'A' && c <= 'Z') {
            total_nodes++;
            last_node = c;

            if (!node_is_empty(&tstack) && c == 'C')
                parent_of_C = node_peek(&tstack);
            node_push(&tstack, c);

            if (capturing_children_of_C && counter_depth(&tstack) == C_bracket_depth + 1)
                children_of_C[child_count_C++] = c;
        }
        else if (c == '(')
        {
            non_leaf_nodes++;
            counter_push(&tstack, 1);

            if (counter_depth(&tstack) > max_height)
                max_height = counter_depth(&tstack);
            
            if (last_node == 'C')
            {
                capturing_children_of_C = 1;
                C_bracket_depth = counter_depth(&tstack) - 1;
            }
            last_node = '\0';
        }
        else if (c == ',')
        {
            counter_increment_top(&tstack);
            node_pop(&tstack);   
        }
        else if (c == ')')
        {
            int current_degree = counter_pop(&tstack);
            if (current_degree > max_degree) max_degree = current_degree;

            if (capturing_children_of_C && counter_depth(&tstack) == C_bracket_depth)
                capturing_children_of_C = 0;
            node_pop(&tstack);
            last_node = '\0';
        }
    }

    int leaf_nodes = total_nodes - non_leaf_nodes;
    children_of_C[child_count_C] = '\0';

    printf("\n=== 트리 정보 출력 ===\n");
    printf("- 전체 노드의 수: %d\n", total_nodes);
    printf("- 단말 노드의 수: %d\n", leaf_nodes);
    printf("- 비단말 노드의 수: %d\n", non_leaf_nodes);
    printf("- 트리의 높이: %d\n", max_height);
    printf("- 트리의 차수: %d\n", max_degree);

    if (parent_of_C != '\0') {
        printf("- 노드 C의 부모 노드: %c\n", parent_of_C);
    } else {
        printf("- 노드 C의 부모 노드: 없음\n");
    }

    printf("- 노드 C의 자식 노드: ");
    if (child_count_C > 0) {
        for (int i = 0; i < child_count_C; i++) printf("%c ", children_of_C[i]);
        printf("\n");
    } else {
        printf("없음\n");
    }

    print_tree(input);

    return 0;
}