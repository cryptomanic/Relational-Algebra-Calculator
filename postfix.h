#include <bits/stdc++.h>

// structure of stack node
struct Stack {
    int top;
    unsigned capacity;
    int* array;
};

// create stack of size capacity
struct Stack* createStack(unsigned capacity) {

    // memory allocation
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));

    if (!stack)
        return NULL;

    stack->top = -1;
    stack->capacity = capacity;
    stack->array = (int*) malloc(stack->capacity * sizeof(int));

    if (!stack->array)
        return NULL;

    return stack;
}

// check whether stack is empty or not
int isEmpty(struct Stack* stack) {
    return stack->top == -1 ;
}

// return top element of stack
char peek(struct Stack* stack) {
    return stack->array[stack->top];
}

// delete top element of stack
char pop(struct Stack* stack) {
    if (!isEmpty(stack))
        return stack->array[stack->top--];

    return '$';
}

// push operation
void push(struct Stack* stack, char op) {
    stack->array[++stack->top] = op;
}


// A utility function to check if the given character is operand
int isOperand(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

// A utility function to return precedence of a given operator
// Higher returned value means higher precedence
int Prec(char ch) {

    switch (ch) {
        case '|': return 1;
        case '&': return 2;
        case '!': return 3;
        case '<': return 4;
        case '>' : return 4;
        case '='  : return 4;
        case '+': return 5;
        case '-': return 5;
        case '*': return 6;
    }

    return -1;
}


// The main function that converts given infix expression
// to postfix expression.
char* infixToPostfix(char* exp) {
    int i, k;

    // Create a stack of capacity equal to expression size
    struct Stack* stack = createStack(strlen(exp));
    if(!stack) { // See if stack was created successfully
		std::cerr<<"error in creating the stack";
		exit(-1);
	}

    for (i = 0, k = -1; exp[i]; ++i) {
         // If the scanned character is an operand, add it to output.
        if (isOperand(exp[i]))
            exp[++k] = exp[i];

        // If the scanned character is an ‘(‘, push it to the stack.
        else if (exp[i] == '(')
            push(stack, exp[i]);

        //  If the scanned character is an ‘)’, pop and output from the stack
        //  until an ‘(‘ is encountered.
        else if (exp[i] == ')') {
            while (!isEmpty(stack) && peek(stack) != '(')
                exp[++k] = pop(stack);

            if (!isEmpty(stack) && peek(stack) != '(') {
               std::cerr<<"invalid expression";
                exit(-1); // invalid expression
            }
            else
                pop(stack);
        }
        else { // an operator is encountered

            while (!isEmpty(stack) && Prec(exp[i]) <= Prec(peek(stack)))
                exp[++k] = pop(stack);

            push(stack, exp[i]);
        }

    }

    // pop all the operators from the stack
    while (!isEmpty(stack)) {
        exp[++k] = pop(stack);
    }

    exp[++k] = '\0';

    return exp;
}
